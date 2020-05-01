#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <bitset>
#include "parse.h"
#include "play.h"
#include <cmath>

using namespace std;

static const struct t_bpm empty_bpm;
static const measure empty_measure;

songdata::~songdata() {
	for (auto c = this->charts.begin(); c != this->charts.end(); c++) {
		for (auto m = c->notes.begin(); m != c->notes.end(); m++) {
			for (auto n = m->begin(); n != m->end(); n++) {
				delete [] *n;
			}
		}
	}
}

bool nextChar(FILE* input, int* output) {
	int c = fgetc(input);
	if (c == EOF) {return false;}
	while (c == '/') {
		int pos = ftell(input);
		int next_c = fgetc(input);
		if (next_c == EOF) {return false;}
		if (next_c == '/') {
			while (c != '\n') {
				c = fgetc(input);
				if (c == EOF) {return false;}
			}
		} else {
			fseek(input, pos, SEEK_SET);
			break;
		}
	}
	*output = c;
	return true;
}

bool parseSimFile(songdata* song, string path) {
	song->filepath = path;
	enum state {IDLE, KEY, VALUE};
	FILE *fp = fopen(path.c_str(), "r");
	int p;
	state task = IDLE;
	string buffer = "";
	string* output = NULL;
	int key_chart = 0;
	bool strip_spaces = true;
	while (nextChar(fp, &p)) {
		switch (task) {
			case (IDLE):
				if (p == '#') {task = KEY;}
				break;
			case (KEY):
				if (p == ':') {
					task = VALUE;
					strip_spaces = true;
					if (buffer == "TITLE") {output = &song->title;}
					else if (buffer == "ARTIST") {output = &song->artist;}
					else if (buffer == "BANNER") {output = &song->banner;}
					else if (buffer == "BACKGROUND") {output = &song->bg;}
					else if (buffer == "MUSIC") {output = &song->music;}
					else if (buffer == "OFFSET") {output = &song->offset;}
					else if (buffer == "BPMS") {song->bpms_offset = ftell(fp);}
					else if (buffer == "STOPS") {song->stops_offset = ftell(fp);}
					else if (buffer == "NOTES") {
						song->charts.emplace_back();
						song->charts.back().song = song;
						key_chart = 1;
					}
					buffer = "";
				} else {
					buffer.append(1, p);
				}
				break;
			case (VALUE):
				if (p == ';') {
					task = IDLE;
					output = NULL;
					key_chart = 0;
					break;
				}
				if (key_chart) {
					if (output == NULL) {
						strip_spaces = true;
						chart* new_chart = &song->charts.back();
						switch (key_chart) {
							case 1: output = &new_chart->type; break;
							case 2: output = &new_chart->description; break;
							case 3: output = &new_chart->difficulty; break;
							case 4: output = &new_chart->meter; break;
							case 5: output = &new_chart->groove; break;
						}
					}
					if (p == ':') {
						key_chart++;
						if (key_chart == 6) {
							song->charts.back().notes_offset = ftell(fp);
							key_chart = 0;
						}
						output = NULL;
					}
				}
				if (strip_spaces && p != ' ' && p != '\n' && p != 0xd) {
					strip_spaces = false;
				} 
				if (output && !strip_spaces) {
					output->append(1, p);
				}
				break;
		}
	}
	fclose(fp);
	return true;
}

bool parseBPMS(songdata* song, bool parseStops) {
	bpmdata* bpms = &song->bpms;
	if (parseStops) {bpms = &song->stops;}
	struct t_bpm bpm = empty_bpm;
	enum state {IDLE, KEY, VALUE};
	state task = IDLE;
	int c;
	string buffer = "";
	FILE* fp = fopen(song->filepath.c_str(), "r");
	if (!parseStops) {fseek(fp, song->bpms_offset, SEEK_SET);}
	else {fseek(fp, song->stops_offset, SEEK_SET);}
	while (nextChar(fp, &c)) {
		switch (task) {
			case (IDLE):
				if (isdigit(c)) {
					task = KEY;
					buffer.append(1, c);
				}
				break;
			case (KEY):
				if (isdigit(c) || c == '.') {buffer.append(1, c);}
				if (c == '=') {
					task = VALUE;
					bpm.beatf = stod(buffer) * beatfperiod;
					buffer = "";
				}
				break;
			case (VALUE):
				if (isdigit(c) || c == '.') {buffer.append(1, c);}
				if (c == ',' || c == ';') {
					task = IDLE;
					if (parseStops)
						bpm.bpmf = (stod(buffer) * (1 << MINUTEFRAC)) / 60;
					else
						bpm.bpmf = stod(buffer) * pow(2, BPMFRAC);
					buffer = "";
					bpms->push_back(bpm);
					bpm = empty_bpm;
				}
				break;
		}
		if (c == ';') {break;}
	}
	return true;
}


bool parseNotes(chart* chart) {
	int c;
	notedata* notes = &chart->notes;
	measure m = empty_measure;
	int fourcount = 0;
	int count = -1;
	FILE* fp = fopen(chart->song->filepath.c_str(), "r");
	fseek(fp, chart->notes_offset, SEEK_SET);
	while (nextChar(fp, &c)){
		if (c == ';') {break;}
		if ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') || (c == 'M') || (c == 'L') || (c == 'F') || (c == 'K')) {
			count++;
			if (count > 3) {
				count = 0;
				fourcount += 1;
				if (fourcount > 3) {
					fourcount = 0;
				}	
			}
			if ((count == 0) && (fourcount == 0)) {
				m.push_back(new u16[4]);
				for (int i = 0; i <= 3; i++) {
					m.back()[i] = 0;
				}	
			}
		}
		switch (c) {
			case '1':
				m.back()[fourcount] |= normal[count];
				break;
			case '2':
				m.back()[fourcount] |= holdhead[count];
				break;
			case '3':
				m.back()[fourcount] |= holdtail[count];
				break;
			case 'M':
				m.back()[fourcount] |= mine[count];
				break;
			case ',':
				notes->push_back(m);
				m = empty_measure;
				break;
		}
	}
	notes->push_back(m);
	fclose(fp);
	return true;
} 

bool parseChart(songdata* song, int chart) {
	parseBPMS(song, false);
	parseBPMS(song, true);
	parseNotes(&song->charts.at(chart));
	return true;
}

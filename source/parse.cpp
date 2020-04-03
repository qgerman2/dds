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

static const struct t_pair empty_tag;
static const struct t_bpm empty_bpm;
static const measure empty_measure;

songdata::~songdata() {
	int e = 0;
	for (auto m = this->notes.begin(); m != this->notes.end(); m++) {
		for (auto n = m->begin(); n != m->end(); n++) {
			e++;
			delete [] *n;
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

songdata parseSimFile(string path, bool partial) {
	songdata simfile;
	enum state {IDLE, KEY, VALUE};
	FILE *fp = fopen(path.c_str(), "r");
	int p;
	state task = IDLE;
	string buffer = "";
	string* output = NULL;
	int key_chart = 0;
	while (nextChar(fp, &p)) {
		switch (task) {
			case (IDLE):
				if (p == '#') {task = KEY;}
				break;
			case (KEY):
				if (p == ':') {
					task = VALUE;
					if (buffer == "TITLE") {output = &simfile.title;}
					else if (buffer == "ARTIST") {output = &simfile.artist;}
					else if (buffer == "BANNER") {output = &simfile.banner;}
					else if (buffer == "BACKGROUND") {output = &simfile.bg;}
					else if (buffer == "NOTES") {
						simfile.charts.emplace_back();
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
						chart* new_chart = &simfile.charts.back();
						switch (key_chart) {
							case 1: output = &new_chart->type; break;
							case 2: output = &new_chart->description; break;
							case 3: output = &new_chart->difficulty; break;
							case 4: output = &new_chart->meter; break;
							case 5: output = &new_chart->groove; break;
							case 6:
								new_chart->notes_offset = ftell(fp);
								key_chart = 0; 
								break;
						}
					}
					if (p == ':') {
						key_chart++;
						output = NULL;
					}
				}
				if (output) {output->append(1, p);}
				break;
		}
	}
	fclose(fp);
	return simfile;
}

songdata parseSong(metadata* tags) {
	songdata song;
	song.tags = tags;
	for (auto i = tags->begin(); i != tags->end(); i++) {
		if (i->key == "NOTES") {
			song.notes = parseNotes(&i->value);
			continue;
		}
		if (i->key == "BPMS") {
			song.bpms = parseBPMS(&i->value, FALSE);
			continue;
		}
		if (i->key == "STOPS") {
			song.stops = parseBPMS(&i->value, TRUE);
			continue;
		}
	}
	return song;
}

bpmdata parseBPMS(string* data, bool isStops) {
	bpmdata bpms;
	struct t_bpm bpm = empty_bpm;
	string buffer;
	enum state {IDLE, KEY, VALUE};
	state task = IDLE;
	char c;
	for (uint i = 0; i < data->size(); i++) {
		c = (*data)[i];
		switch (task) {
			case KEY:
				if (c != '=') {
					buffer.append(1, c);
				} else {
					task = VALUE;
					bpm.beatf = stod(buffer) * beatfperiod;
					buffer = "";
				}
				break;
			case VALUE:
				if (c != ',' && c != '\n') {
					buffer.append(1, c);
				} 
				if ((c == ',') || (c == '\n') || i == data->length() - 1) {
					if (i == data->length() - 1) {
						buffer.append(1, c);
					}
					task = IDLE;
					if (isStops)
						bpm.bpmf = (stod(buffer) * (1 << MINUTEFRAC)) / 60;
					else
						bpm.bpmf = stod(buffer) * pow(2, BPMFRAC);
					buffer = "";
					bpms.push_back(bpm);
					bpm = empty_bpm;
				}
				break;
			case IDLE:
				if (c != ' ' && c != '\n' && c != ',') {
					task = KEY;
					buffer.append(1, c);
				}
				break;
		}
	}
	return bpms;
}


notedata parseNotes(string* data) {
	size_t s = data->find(':');
	size_t e = data->find(';');
	for (int v = 0; v <= 3; v++) {
		s = data->find(':', s + 1);
	}
	string rawnotes = data->substr(s, e - s);
	char c;
	notedata notes;
	measure m = empty_measure;
	int fourcount = 0;
	int count = -1;
	int k = 0;
	for (uint i = 0; i < rawnotes.size(); i++){
		c = rawnotes[i];
		if ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') || (c == 'M')) {
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
				k++;
				for (int i = 0; i <= 3; i++) {
					m.back()[i] = 0;
				}	
			}
		}
		//printf("%c", c);
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
				notes.push_back(m);
				m = empty_measure;
				break;
		}
	}
	cout << "\ncreado " << k;
	notes.push_back(m);
	size_t size = 0;
	for ( auto i = notes.begin(); i != notes.end(); i++ ) {
		size = size + i->size();
	}
	return notes;
} 

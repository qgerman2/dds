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

using namespace std;

static const struct t_pair empty_tag;
static const measure empty_measure;

songdata parseSong(string path) {
	songdata song;
	enum class state {IDLE, KEY, VALUE};
	FILE *fp = fopen(path.c_str(), "r");
	int p;
	bool prevslash = FALSE;
	bool skip = FALSE;
	state task = state::IDLE;
	string buffer = "";
	struct t_pair tag = empty_tag;
	do {
		p = fgetc(fp);
		if (feof(fp))
			break;
		if (!skip) {
			if ((task == state::KEY || task == state::VALUE) && (p != ';')) {
				if (p == '/') {
					if (prevslash) {
						skip = TRUE;
						prevslash = FALSE;
					} else {
						prevslash = TRUE;
					}
				} else {
					if (prevslash) {
						prevslash = FALSE;
						buffer.append(1, '/');
					}
					if (((task == state::VALUE) && (p == ':')) || (p != ':'))
						buffer.append(1, p);
				}
			}
			switch (p) {
				case '#':
					if (task == state::IDLE)
						task = state::KEY;
					break;
				case ':':
					if (task == state::KEY) {
						task = state::VALUE;
						tag.key = buffer;
						buffer = "";
					}
					break;
				case ';':
					if (task == state::VALUE) {
						task = state::IDLE;
						tag.value = buffer;
						song.tags.push_back(tag);
						tag = empty_tag;
						buffer = "";
					}
					break;
			}
		} else {
			if (p == '\n') {
				buffer.append(1, '\n');
				skip = FALSE;
			}
		}
	} while (1);
	fclose(fp);
	for (auto i = song.tags.begin(); i != song.tags.end(); i++) {
		if (i->key == "NOTES") {
			song.notes = parseNotes(i->value);
		}
	}
	return song;
}

notedata parseNotes(string data) {
	size_t s = data.find(':');
	size_t e = data.find(';');
	for (int v = 0; v <= 3; v++){
		s = data.find(':', s + 1);
	}
	string rawnotes = data.substr(s, e - s);
	char c;
	notedata notes;
	measure m = empty_measure;
	int fourcount = 0;
	int count = -1;
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
			case 'M':
				m.back()[fourcount] |= mine[count];
				break;
			case ',':
				notes.push_back(m);
				m = empty_measure;
				break;
		}
	}
	notes.push_back(m);
	//debug print all
	/*int size;
	u16* set;
	int cc = 0; 
	for (auto m = notes.begin(); m != notes.end(); m++) {
		cout << "\n" << "measure " << cc;
		cc++;
		size = m->size();
		for (int s = 0; s < size; s++) {
			cout << "\n" << "set " << s;
			set = m->at(s);
			for (int i = 0; i < 4; i++) {
				cout << '\n' << bitset<16>(set[i]);
			}
		}
	}*/
	cout << "tamo";
	return notes;
} 

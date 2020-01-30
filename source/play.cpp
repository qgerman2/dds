#include <nds.h>
#include <iostream>
#include "main.h"
#include "parse.h"
#include "play.h"
#include <hold.h>
#include <bitset>
#include <cmath>
#include <maxmod9.h>
#include <vector>
#include "play_render.h"

using namespace std;

static u32 beatfperiod = (1 << (BPMFRAC + MINUTEFRAC));
songdata song;
vector<step> steps;
vector<hold> holds;
u8 parseaheadbeats = 12;
u32 time;
u32 bpmf = 0;
int bpmindex = -1;
u32 minutef;
u32 beatf;
int beat;					//beat global
int firstbeat = -1;			//primer beat del measure global
int count = 0; 				//beat relativo al primer beat de measure global
int sets;					//cantidad de sets en measure
int cursor = 0;
int measurecursor = -1;
int stepbeatf = 0;			//beat preciso en el cursor
int relbeatf = 0;			//beat preciso relativo al cursor a x set

u16 *set;
measure m;

void setup(songdata s){
	pr_setup();
	TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024;
	TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;
	song = s;
}

void loop(){
	while (1) {
		scanKeys();
		mmStreamUpdate();
		swiWaitForVBlank();
		updateBeat();
		updateSteps();
		renderSteps();
		oamUpdate(&oamMain);
	}
}

u32 minutefbpm;	//tiempo que dura ese bpm
u32 minutefsum;
bool lostbeatsbpm;
void updateBeat() {
	time = millis();
	minutef = (time * (1 << MINUTEFRAC)) / 60000;
	beatf = 0;
	minutefsum = 0;
	for (uint i = 0; i < song.bpms.size(); i++) {
		lostbeatsbpm = FALSE;
		if (i < song.bpms.size() - 1) {
			minutefbpm = (song.bpms[i + 1].beatf - song.bpms[i].beatf) / song.bpms[i].bpmf;
			for (uint s = 0; s < song.stops.size(); s++) {
				if ((song.stops[s].beatf > song.bpms[i].beatf) && (song.stops[s].beatf < song.bpms[i + 1].beatf)) {
					minutefbpm = minutefbpm + song.stops[s].bpmf;
					lostbeatsbpm = TRUE;
				}
			}
		} else {
			minutefbpm = 0;
		}
		if ((minutefbpm > 0) && ((minutefsum + minutefbpm) < minutef)) {
			beatf = song.bpms[i + 1].beatf - song.bpms[i].beatf + beatf;
			minutefsum = minutefsum + minutefbpm;
		} else {
			beatf = beatf + ((minutef - minutefsum) * song.bpms[i].bpmf);
			if (lostbeatsbpm) {
				for (uint s = 0; s < song.stops.size(); s++) {
					if ((song.stops[s].beatf > song.bpms[i].beatf) && (song.stops[s].beatf < song.bpms[i + 1].beatf)) {
						if (beatf > song.stops[s].beatf) {
							u32 offset = song.stops[s].bpmf * song.bpms[i].bpmf;
							if (beatf - song.stops[s].beatf < offset){
								beatf = song.stops[s].beatf;
							} else {
								beatf = beatf - offset;
							}
						}
					}
				}
			}
			break;
		}
	}
	beat = beatf >> (MINUTEFRAC + BPMFRAC);
}

void updateSteps() {
	//crear nuevos steps
	for (int i = cursor; i < beat + parseaheadbeats; i++) {
		if ((i / 4) > measurecursor) {
			cout << "\nmeasure " << i / 4;
			firstbeat = i;
			m = getMeasureAtBeat(i);
			sets = m.size();
			measurecursor = i / 4;
		}
		count = i - firstbeat;
		stepbeatf = i * beatfperiod;
		switch (sets) {
			case 1: //1 set, 1 linea por beat
				set = m.at(0);
				newSteps(set[count], stepbeatf);
				break;
			case 2: //2 sets, 2 lineas por beat
				if ((count == 0) || (count == 1)) {
					set = m.at(0);
					newSteps(set[count * 2], stepbeatf);
					newSteps(set[count * 2 + 1], stepbeatf + (beatfperiod / 2));
				} else {
					set = m.at(1);
					newSteps(set[(count - 2) * 2], stepbeatf);
					newSteps(set[(count - 2) * 2 + 1], stepbeatf + ((beatfperiod) / 2));
				}
				break;
			default: //sets sets, sets / 4 lineas por beat
				for (int k = 0; k < sets / 4; k++) {
					set = m.at(count * (sets / 4) + k);
					relbeatf = ((k * beatfperiod) / (sets / 4));
					for (int ii = 0; ii < 4; ii++) {
						newSteps(set[ii], stepbeatf + relbeatf + (ii * (beatfperiod / sets)));
					}
				}
				break;
		}
		cursor = i + 1;
	}
	//crear steps por holds
	int ystart;
	int yend;
	u32 height;
	step s;
	for (auto h = holds.begin(); h != holds.end(); h++) {
		ystart = ((h->startbeatf >> BEATFSCREENYFRAC) - (beatf >> BEATFSCREENYFRAC)) + 16;
		if (ystart > NDSHEIGHT) {
			continue;
		}
		height = NDSHEIGHT - ystart;
		if (h->endbeatf > 0) {
			yend = ((h->endbeatf >> BEATFSCREENYFRAC) - (beatf >> BEATFSCREENYFRAC)) + 16;
			height = yend - ystart;
		}
		while (((height + 32 - 1) / 32) > h->stepcount) {
			h->stepcount = h->stepcount + 1;
			s.type = 5;
			s.x = (10 + 30 * h->col);
			s.y = 0;
			s.col = h->col;
			s.sprite = popSprite();
			s.beatf = h->startbeatf;
			s.stepcount = h->stepcount - 1;
			steps.push_back(s);
			h->laststep = &steps.back();
		}
		//cortar sprite de ultimo hold
		if (h->endbeatf > 0) {
			u8 mod = height % 32;
			if (mod > 0) {
				h->laststep->gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
				dmaCopyHalfWords(3, holdBitmap, h->laststep->gfx, holdBitmapLen / 32 * mod);
			}
			holds.erase(h--);
		}
	}
	//actualizar posicion
	for (auto i = steps.begin(); i != steps.end(); i++) {
		i->y = ((i->beatf >> BEATFSCREENYFRAC) - (beatf >> BEATFSCREENYFRAC));
		if (i->type == 5) { //holds
			i->y += 16 + 32 * i->stepcount;
		}
		if (i->y < -32) {
			pushSprite(i->sprite);
			if (i->gfx != NULL) {
				oamFreeGfx(&oamMain, i->gfx);
			} 
			steps.erase(i--);
		}
	}
}

void newSteps(u16 data, u32 beatf) {
	if (data == 0)
		return;
	bool newstep = false;
	//normal steps
	step s;
	for (int i = 0; i < 4; i++) {
		newstep = false;
		if (data & normal[i]) {
			s.type = 1;
			newstep = true;
		}
		if (data & holdhead[i]) {
			s.type = 2;
			newstep = true;
			hold h;
			h.col = i;
			h.startbeatf = beatf;
			h.endbeatf = 0;
			h.stepcount = 0;
			holds.push_back(h);
		}
		if (data & holdtail[i]) {
			s.type = 3;
			newstep = true;
			for (auto k = holds.begin(); k != holds.end(); k++) {
				if ((k->col == i) && (k->endbeatf == 0)) {
					k->endbeatf = beatf;
					break;
				}
			}
		}
		if (newstep) {
			s.x = (10 + 30 * i);
			s.y = 100;
			s.col = i;
			s.sprite = popSprite();
			s.beatf = beatf;
			steps.push_back(s);
		}
	}
}

measure getMeasureAtBeat(u32 beat) {
	if (beat / 4 > song.notes.size() - 1) {
		sassert(0, "attempted to get nonexistant measure");
	}
	return song.notes.at(beat / 4);
}

u32 millis() {
	return (timerTick(0) + (timerTick(1) << 16)) / NDSFREQ;
}
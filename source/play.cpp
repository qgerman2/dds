#include <nds.h>
#include <tap.h>
#include <iostream>
#include "main.h"
#include "parse.h"
#include "play.h"
#include <bitset>
#include <cmath>
#include <maxmod9.h>

//indice memoria
#define sprites ((spriteEntry*) OAM)
#define tiles_tap 0
#define pal_tap 0

//macros para acceso a memoria
#define tile2objram(t) (SPRITE_GFX + (t) * 16)
#define pal2objram(p) (SPRITE_PALETTE + (p) * 16)

using namespace std;

static u8 notetype[] = {4, 8, 12, 16, 24, 32, 48, 64, 192};
bool freesprites[128];
static u32 beatfperiod = (1 << (BPMFRAC + MINUTEFRAC));
songdata song;
vector<step> steps;
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
	dmaCopyHalfWords(3, tapTiles, tile2objram(tiles_tap), tapTilesLen);
	dmaCopyHalfWords(3, tapPal, pal2objram(pal_tap), tapPalLen);
	setRotData();
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
	}
	TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024;
	TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;
	song = s;
}

void loop(){
	while (1) {
		updateBeat();
		updateSteps();
		mmStreamUpdate();
		swiWaitForVBlank();
		renderSteps();
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
	for (int i = cursor; i < beat + 8; i++) {
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
				newSteps(set[count], stepbeatf, 0);
				break;
			case 2: //2 sets, 2 lineas por beat
				if ((count == 0) || (count == 1)) {
					set = m.at(0);
					newSteps(set[count * 2], stepbeatf, 0);
					newSteps(set[count * 2 + 1], stepbeatf + (beatfperiod / 2), 1);
				} else {
					set = m.at(1);
					newSteps(set[(count - 2) * 2], stepbeatf, 0);
					newSteps(set[(count - 2) * 2 + 1], stepbeatf + ((beatfperiod) / 2) , 1);
				}
				break;
			default: //sets sets, sets / 4 lineas por beat
				for (int k = 0; k < sets / 4; k++) {
					set = m.at(count * (sets / 4) + k);
					relbeatf = ((k * beatfperiod) / (sets / 4));
					for (int ii = 0; ii < 4; ii++) {
						newSteps(set[ii], stepbeatf + relbeatf + (ii * (beatfperiod / sets)), 0);
					}
				}
				break;
		}
		cursor = i + 1;
	}
	//actualizar steps existentes
	for (auto i = steps.begin(); i != steps.end(); i++) {
		i->y = ((i->beatf >> 14) - (beatf >> 14));
		if (i->beatf < beatf) {
			pushSprite(i->sprite);
			steps.erase(i--);
		}
	}
}

void newSteps(u16 data, u32 beatf, u8 type) {
	if (data == 0)
		return;
	//normal steps
	for (int i = 0; i < 4; i++) {
		if (data & normal[i]) {
			step s;
			s.x = (10 + 30 * i);
			s.y = 100;
			s.type = 0;
			s.beatf = beatf;
			s.sprite = popSprite();
			s.col = i;
			steps.push_back(s);
		}
	}
}

void renderSteps() {
	for (auto i = steps.begin(); i != steps.end(); i++) {
		if (i->y < 160) {
			sprites[i->sprite].attr0 = i->y | ATTR0_ROTSCALE_DOUBLE;
			sprites[i->sprite].attr1 = i->x | ATTR1_SIZE_32 | ATTR1_ROTDATA(i->col);
			sprites[i->sprite].attr2 = tiles_tap + (pal_tap << 12);
		} else {
			sprites[i->sprite].attr0 = ATTR0_DISABLED;
		}
	}
}

u8 popSprite() {
	for (u8 i = 0; i < 128; i++) {
		if (freesprites[i]) {
			freesprites[i] = FALSE;
			return i;
		}
	}
	sassert(0, "out of sprites");
	return 0;
}

void pushSprite(u8 i) {
	freesprites[i] = TRUE;
	sprites[i].attr0 = ATTR0_DISABLED;
}

measure getMeasureAtBeat(u32 beat) {
	if (beat / 4 > song.notes.size() - 1) {
		sassert(0, "attempted to get nonexistant measure");
	}
	return song.notes.at(beat / 4);
}

void setRotData() {
	s16 s;
	s16 c;
	u16* affine;
	//left
	s = sinLerp(degreesToAngle(270)) >> 4;
	c = cosLerp(degreesToAngle(270)) >> 4;
	affine = OAM + 3;
	affine[0] = c;
	affine[4] = s;
	affine[8] = -s;
	affine[12] = c;
	//up
	s = sinLerp(degreesToAngle(0)) >> 4;
	c = cosLerp(degreesToAngle(0)) >> 4;
	affine = OAM + 16 + 3;
	affine[0] = c;
	affine[4] = s;
	affine[8] = -s;
	affine[12] = c;
	//down
	s = sinLerp(degreesToAngle(180)) >> 4;
	c = cosLerp(degreesToAngle(180)) >> 4;
	affine = OAM + 32 + 3;
	affine[0] = c;
	affine[4] = s;
	affine[8] = -s;
	affine[12] = c;
	//right
	s = sinLerp(degreesToAngle(90)) >> 4;
	c = cosLerp(degreesToAngle(90)) >> 4;
	affine = OAM + 48 + 3;
	affine[0] = c;
	affine[4] = s;
	affine[8] = -s;
	affine[12] = c;
}

u32 millis() {
	return (timerTick(0) + (timerTick(1) << 16)) / NDSFREQ;
}
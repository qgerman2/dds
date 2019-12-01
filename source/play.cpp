#include <nds.h>
#include <tap.h>
#include <iostream>
#include "main.h"
#include "parse.h"
#include "play.h"
#include <bitset>
#include <cmath>

//indice memoria
#define sprites ((spriteEntry*) OAM)
#define tiles_tap 0
#define pal_tap 0

//macros para acceso a memoria
#define tile2objram(t) (SPRITE_GFX + (t) * 16)
#define pal2objram(p) (SPRITE_PALETTE + (p) * 16)

#define NDSFREQ 32.7284 //khz
#define BPMFRAC 6 //bits
#define MINUTEFRAC 12

using namespace std;

songdata song;
vector<step> steps;
bool freesprites[128];

void setup(songdata s){
	dmaCopyHalfWords(3, tapTiles, tile2objram(tiles_tap), tapTilesLen);
	dmaCopyHalfWords(3, tapPal, pal2objram(pal_tap), tapPalLen);
	for (int i = 0; i < 128; i++)
		freesprites[i] = TRUE;
	TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024;
	TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;
	song = s;
    /*int n;
    for( n = 0; n < 50; n++ )
    {
        // attribute0: set vertical position 0->screen_height-sprite_height, 
        // other default options will be okay (default == zeroed)
        sprites[n].attr0 = rand() % (192 - 16) | ATTR0_ROTSCALE_DOUBLE;

        // attribute1: set horizontal position 0->screen_width-sprite_width
        // also set 16x16 size mode
        sprites[n].attr1 = (rand() % (256 - 16)) | ATTR1_SIZE_32 | ATTR1_ROTDATA(0);

        // attribute0: select tile number and palette number
        sprites[n].attr2 = tiles_tap + (pal_tap << 12);

		s16 s = sinLerp(10000) >> 4;
		s16 c = cosLerp(10000) >> 4;

		u16* affine;
		affine = OAM + 3;
		affine[0] = c;
		affine[4] = s;
		affine[8] = -s;
		affine[12] = c;
    }*/
}

u32 millis() {
	return (timerTick(0) + (timerTick(1) << 16)) / NDSFREQ;
}

void loop(){
	while (1) {
		updateSteps();
		swiWaitForVBlank();
	}
}

u32 time;
u32 bpmf = 100.999999 * pow(2, BPMFRAC);
u32 minutef;
u32 beatf;
int beat;					//beat global
int firstbeat = -1;			//primer beat del measure global
int count = 0; 				//beat relativo al primer beat de measure global
int sets;					//cantidad de sets en measure
int cursor = 0;
int measurecursor = -1;
u16 *set;
measure m;
void updateSteps() {
	time = millis();
	minutef = (time * (1 << MINUTEFRAC)) / 60000;
	beatf = (bpmf * minutef);
	beat = beatf >> (MINUTEFRAC + BPMFRAC);
	//cursor puede ir delante del beat global
	for (int i = cursor; i < beat + 10; i++) {
		if ((i / 4) > measurecursor) {
			cout << "\nmeasure " << i / 4;
			firstbeat = i;
			m = getMeasure(i);
			sets = m.size();
			measurecursor = i / 4;
		}
		count = i - firstbeat;
		switch (sets) {
			case 1: //1 set, 1 linea por beat
				cout << "\n" << "set ";
				set = m.at(0);
				cout << '\n' << bitset<16>(set[count]);
				break;
			case 2: //2 sets, 2 lineas por beat
				cout << "\n" << "set ";
				if ((count == 0) || (count == 1)) {
					set = m.at(0);
					cout << '\n' << bitset<16>(set[count * 2]) << " " << count * 2;
					cout << '\n' << bitset<16>(set[count * 2 + 1]) << " " << count * 2 + 1;
				} else {
					set = m.at(1);
					cout << '\n' << bitset<16>(set[(count - 2) * 2]) << " " << (count - 2) * 2;
					cout << '\n' << bitset<16>(set[(count - 2) * 2 + 1]) << " " << (count - 2) * 2 + 1;
				}
				break;
			default: //sets sets, sets / 4 sets por beat
				for (int k = 0; k < sets / 4; k++) {
					cout << "\n" << "set " << count * (sets / 4) + k;
					set = m.at(count * (sets / 4) + k);
					for (int ii = 0; ii < 4; ii++) {
						cout << '\n' << bitset<16>(set[ii]);
					}
				}
				break;
		}
		cursor = i + 1;
	}
}

int popSprite() {
	for (int i = 0; i < 128; i++) {
		if (freesprites[i]) {
			freesprites[i] = FALSE;
			return i;
		}
	}
}

void pushSprite(int i) {
	freesprites[i] = TRUE;
}

measure getMeasure(int beat) {
	return song.notes.at(beat / 4);
}
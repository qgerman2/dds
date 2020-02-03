#include <nds.h>
#include <iostream>
#include <bitset>
#include "parse.h"
#include "play.h"
#include "play_input.h"
#include "play_render.h"

using namespace std;

u32 keysPressed;
u32 keysReleased;
u32 keysHeldd;
u32 msperbeat;
u32 beatfdiff;
u32 judgesWindow[5];

u32 colToKeys[4] = {
	bitset<32> ("101000100000").to_ulong(), //left (Y, L BUMPER, DPAD LEFT)
	bitset<32> ("10000010").to_ulong(), 	//down (DPAD DOWN, B)
	bitset<32> ("10001000000").to_ulong(), 	//up (X, DPAD UP)
	bitset<32> ("100010001").to_ulong(), 	//right (R BUMPER, DPAD RIGHT, A)
};
u32 stepKeys = bitset<32> ("110011111111").to_ulong();
u8 stateCol[4] = {0}; //0 aun no pasa, 1 buscar hold, 2 ta listo
vector<step>::iterator holdCol[4];
bool pressed = false;	//se apreto/solto teclas durante el frame
bool released = false;
bool held = false;

void updateInput() {
	for (int i = 0; i < 4; i++) {
		stateCol[i] = 0;
	}
	keysPressed = keysDown();
	keysReleased = keysUp();
	keysHeldd = keysHeld();

	if (((keysPressed & stepKeys) > 0) || ((keysReleased & stepKeys) > 0) || ((keysHeldd & stepKeys) > 0)) {
		for (auto s = steps.begin(); s != steps.end(); s++) {
			if (stateCol[s->col] == 2) {continue;}
			pressed = (keysPressed & colToKeys[s->col]) > 0;
			released = (keysReleased & colToKeys[s->col]) > 0;
			held = (keysHeldd & colToKeys[s->col]) > 0;
			if (pressed || released || held) {
				beatfdiff = s->beatf > beatf ? s->beatf - beatf : beatf - s->beatf;
				if ((beatfdiff <= judgesWindow[4]) || (s->type == 5)) {
					if (stateCol[s->col] == 1) {
						if (s->type == 5) {
							holdCol[s->col] = next(s, 0);
							stateCol[s->col] = 2;
						}
					}
					if (pressed) {
						if (s->type == 2) { //hold
							stateCol[s->col] = 1;
							removeStep(&s);
						}
						else if ((s->type != 3) && (s->type != 5)) {
							stateCol[s->col] = 2;
							removeStep(&s);
						}
					}
					if (held && !pressed) {
						if (holdCol[s->col] != steps.end()) {
							if (s->type == 5) {
								if (s->y < (HITYOFFSET - 16)) {
									holdCol[s->col] = steps.end();
									stateCol[s->col] = 1;
									removeStep(&s);
								}
							}
						}
						if ((holdCol[s->col] != steps.end()) || (stateCol[s->col] == 1)) {
							if (s->type == 3) {
								if (beatf >= s->beatf) {
									if (holdCol[s->col] != steps.end()) {
										removeStep(&holdCol[s->col]);
										s--;
									}
									holdCol[s->col] = steps.end();
									stateCol[s->col] = 2;
									removeStep(&s);
								}
							}
						}
					}
					if (released) {
						if (holdCol[s->col] != steps.end()) {
							if (s->type == 3) {
								removeStep(&holdCol[s->col]);
								s--;
								holdCol[s->col] = steps.end();
								stateCol[s->col] = 2;
								removeStep(&s);
							}
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < 4; i++) {
		if (holdCol[i] != steps.end()) {
			if ((keysHeldd & colToKeys[i]) == 0) {
				holdCol[i] = steps.end();
			}
		}
	}
}

void updateJudgesWindow() {
	msperbeat = (60000 << BPMFRAC) / bpmf;
	judgesWindow[0] = beatfperiod * MARVELOUSJUDGE / msperbeat;
	judgesWindow[1] = beatfperiod * PERFECTJUDGE / msperbeat;
	judgesWindow[2] = beatfperiod * GREATJUDGE / msperbeat;
	judgesWindow[3] = beatfperiod * GOODJUDGE / msperbeat;
	judgesWindow[4] = beatfperiod * BOOJUDGE / msperbeat;
}
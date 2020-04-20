#include <nds.h>
#include <iostream>
#include <bitset>
#include <utility>
#include "main.h"
#include "play.h"
#include "play_score.h"
#include "play_input.h"
#include "play_render.h"
using namespace std;

const u32 colToKeys[4] = {
	bitset<32> ("101000100000").to_ulong(), //left (Y, L BUMPER, DPAD LEFT)
	bitset<32> ("10000010").to_ulong(), 	//down (DPAD DOWN, B)
	bitset<32> ("10001000000").to_ulong(), 	//up (X, DPAD UP)
	bitset<32> ("100010001").to_ulong(), 	//right (R BUMPER, DPAD RIGHT, A)
};
const u32 stepKeys = bitset<32> ("110011111111").to_ulong();

PlayInput::PlayInput(Play* play) {
	this->play = play;
	for (int c = 0; c < 4; c++) {
		holdCol[c].first = false;
	}
}

PlayInput::~PlayInput() {

}

void PlayInput::update() {
	for (int i = 0; i < 4; i++) {
		stateCol[i] = 0;
	}
	keysPressed = keysDown();
	keysReleased = keysUp();
	keysHeldd = keysHeld();
	if (((keysPressed & stepKeys) > 0) || ((keysReleased & stepKeys) > 0) || ((keysHeldd & stepKeys) > 0)) {
		for (auto s = play->steps.begin(); s != play->steps.end();) {
			if (stateCol[s->col] == 2) {++s; continue;}
			pressed = (keysPressed & colToKeys[s->col]) > 0;
			released = (keysReleased & colToKeys[s->col]) > 0;
			held = (keysHeldd & colToKeys[s->col]) > 0;
			if (pressed || released || held) {
				beatfdiff = s->beatf > play->beatf ? s->beatf - play->beatf : play->beatf - s->beatf;
				if ((beatfdiff <= play->score->judgesWindow[4]) || (s->type == 5)) {
					if (stateCol[s->col] == 1) {
						if (s->type == 5) {
							holdCol[s->col].first = true;
							holdCol[s->col].second = s;
							stateCol[s->col] = 2;
						}
					}
					if (pressed) {
						if (s->type == 2) { //hold
							stateCol[s->col] = 1;
							play->score->add(&(*s), beatfdiff);
							s = play->removeStep(s);
							continue;
						}
						else if ((s->type != 3) && (s->type != 5)) {
							stateCol[s->col] = 2;
							play->score->add(&(*s), beatfdiff);
							s = play->removeStep(s);
							continue;
						}
					}
					if (held && !pressed) {
						if (holdCol[s->col].first) {
							if (s->type == 5) {
								if (s->y < (HITYOFFSET - 16 + s->height)) {
									holdCol[s->col].first = false;
									stateCol[s->col] = 1;
									s = play->removeStep(s);
									continue;
								}
							}
						}
						if (stateCol[s->col] == 1) {
							if (s->type == 3) {
								if (play->beatf >= s->beatf) {
									stateCol[s->col] = 2;
									play->score->add(&(*s), beatfdiff);
									s = play->removeStep(s);
									continue;
								}
							}
						}
					}
					if (released) {
						if (holdCol[s->col].first) {
							if (s->type == 3) {
								for (auto k = s; k != holdCol[s->col].second;) {
									if (k->type == 5 && k->col == s->col) {
										k = play->removeStep(k);
									};
									k--;
								}
								holdCol[s->col].first = false;
								play->score->add(&(*s), beatfdiff);
								play->removeStep(holdCol[s->col].second);
								stateCol[s->col] = 2;
								s = play->removeStep(s);
								continue;
							}
						}
					}
				}
			}
			++s;
		}
	}
	for (int i = 0; i < 4; i++) {
		if (holdCol[i].first) {
			if ((keysHeldd & colToKeys[i]) == 0) {
				holdCol[i].first = false;
			}
		}
	}
}
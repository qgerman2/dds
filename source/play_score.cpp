#include <nds.h>
#include <iostream>
#include "parse.h"
#include "play.h"
#include "play_render.h"
#include "play_score.h"

u32 msperbeat;
u32 judgesWindow[5];

u32 score;
u32 combo;

u32 dptotal = 0;
int dpscore = 0;

using namespace std;

void ps_setup() {
	combo = 0;
	score = 0;
}

void addScore(step* s, u32 beatfdiff) {
	int worth;
	for (int i = 0; i < 5; i++) {
		if (beatfdiff <= judgesWindow[i]) {
			if (i < 3) {
				combo++;
				if (i == 0) {worth = 777; dpscore = dpscore + 2;} //marvelous
				else if (i == 1) {worth = 555; dpscore = dpscore + 2;} //perfect
				else if (i == 2) {worth = 333; dpscore++;} //great
				prevscore = score;
				score = score + worth * combo;
				playScoreAnim();
			}
			else {
				dropCombo();
			}
			u8 anim = i * 2;
			if (s->beatf < beatf) {
				anim++;
			}
			if (s->type != 3) {
				playJudgmentAnim(anim);
			}
			return;
		}
	}
	if (score > 999999999) {score = 999999999;}
}

void addDPTotal() {
	dptotal = dptotal + 2;
}

void dropCombo() {
	combo = 0;
}

void updateJudgesWindow() {
	msperbeat = (60000 << BPMFRAC) / bpmf;
	judgesWindow[0] = beatfperiod * MARVELOUSJUDGE / msperbeat;
	judgesWindow[1] = beatfperiod * PERFECTJUDGE / msperbeat;
	judgesWindow[2] = beatfperiod * GREATJUDGE / msperbeat;
	judgesWindow[3] = beatfperiod * GOODJUDGE / msperbeat;
	judgesWindow[4] = beatfperiod * BOOJUDGE / msperbeat;
}
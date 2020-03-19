#include <nds.h>
#include <iostream>
#include "play.h"
#include "play_render.h"
#include "play_score.h"

using namespace std;

PlayScore::PlayScore(Play* play) {
	this->play = play;
}

void PlayScore::add(step* s, u32 beatfdiff) {
	int worth;
	for (int i = 0; i < 5; i++) {
		if (beatfdiff <= judgesWindow[i]) {
			if (i < 3) {
				combo++;
				if (i == 0) {worth = 777; dpscore = dpscore + 2;} //marvelous
				else if (i == 1) {worth = 555; dpscore = dpscore + 2;} //perfect
				else if (i == 2) {worth = 333; dpscore++;} //great
				play->render->prevscore = score;
				score = score + worth + 111 * combo;
				play->render->playScoreAnim();
			}
			else {
				dropCombo();
			}
			u8 anim = i * 2;
			if (s->beatf < play->beatf) {
				anim++;
			}
			if (s->type != 3) {
				play->render->playJudgmentAnim(anim);
			}
			return;
		}
	}
	if (score > 999999999) {score = 999999999;}
}

void PlayScore::addDPTotal() {
	dptotal = dptotal + 2;
}

void PlayScore::dropCombo() {
	combo = 0;
}

void PlayScore::updateJudgesWindow() {
	msperbeat = (60000 << BPMFRAC) / play->bpmf;
	judgesWindow[0] = beatfperiod * MARVELOUSJUDGE / msperbeat;
	judgesWindow[1] = beatfperiod * PERFECTJUDGE / msperbeat;
	judgesWindow[2] = beatfperiod * GREATJUDGE / msperbeat;
	judgesWindow[3] = beatfperiod * GOODJUDGE / msperbeat;
	judgesWindow[4] = beatfperiod * BOOJUDGE / msperbeat;
}
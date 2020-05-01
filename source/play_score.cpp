#include <nds.h>
#include <iostream>
#include <vector>
#include "globals.h"
#include "buffer.h"
#include "parse.h"
#include "play.h"
#include "play_render.h"
#include "play_score.h"

using namespace std;

enum JUDGE {MARVELOUS, PERFECT, GREAT, GOOD, BAD, MISS};

const int worth[6] {
	500, 250, 50, 0, -50, -100
};

const int window[5] {
	23, 45, 90, 135, 180
};

PlayScore::PlayScore(Play* play) {
	this->play = play;
	score = new score_t;
	score->chart = songchart;
}

PlayScore::~PlayScore() {
	if (state == 0) {
		ScoreSave(songpath, score);
	}
	delete score;
}

void PlayScore::add(step* s, u32 beatfdiff) {
	for (int i = 0; i < 5; i++) {
		if (beatfdiff <= judgesWindow[i]) {
			//add points to score_t
			score->points[i]++;
			//update total and display
			updateScoreDisplay(i);
			//combo
			switch (i) {
				case MARVELOUS:
				case PERFECT:
				case GREAT:
					combo++;
					if (combo > score->max_combo) {score->max_combo = combo;}
					break;
				case GOOD:
				case BAD:
					dropCombo();
					break;
			}
			u8 anim = i * 2;
			if (s->beatf < play->beatf) {
				anim++;
			}
			if (s->type != 3) { //dont play anim on hold tail
				play->render->playJudgmentAnim(anim);
			}
			return;
		}
	}
}

void PlayScore::miss(step *s) {
	play->render->playJudgmentAnim(11);
	score->points[MISS]++;
	updateScoreDisplay(MISS);
	dropCombo();
}

void PlayScore::mine() {
	dropCombo();
}

void PlayScore::dropCombo() {
	combo = 0;
}

void PlayScore::updateJudgesWindow() {
	msperbeat = (60000 << BPMFRAC) / play->bpmf;
	for (int i = 0; i < 5; i++) {
		judgesWindow[i] = beatfperiod * window[i] / msperbeat;
	}
}

void PlayScore::updateScoreDisplay(int judge) {
	//previous score
	if (score_total < 0) {play->render->prevscore = 0;
	} else {play->render->prevscore = score_total;}
	//update score
	score_total += worth[judge];
	//new score
	if (score_total > 999999999) {score_total = 999999999;}
	if (score_total < 0) {play->render->newscore = 0;
	} else {play->render->newscore = score_total;}
	play->render->playScoreAnim();
}

void ScoreSave(string path, score_t* score) {
	string filepath = path + "/highscore";
	bool newfile = false;
	bool pb = false;
	int pos = 0;
	//check if file exists
	FILE* file;
	file = fopen(filepath.c_str(), "rb");
	if (file) {
		//check if entry exists
		score_t best;
		score_t last;
		while (1) {
			pos = ftell(file);
			if (ScoreRead(file, &best, &last)) {
				if (best.chart == score->chart) {
					pb = ScoreIsPB(score, &best);
					break;
				}
			} else {
				pb = true;
				break;
			}
		}
		fclose(file);
	} else {
		newfile = true;
	}
	if (newfile) {
		file = fopen(filepath.c_str(), "wb");
	} else {
		file = fopen(filepath.c_str(), "rb+");
	}
	if (file) {
		fseek(file, pos, SEEK_SET);
		fwrite(&score->chart, 4, 1, file);
		ScoreWrite(file, score);
		if (pb || newfile) {
			ScoreWrite(file, score);
		}
		fclose(file);
	}
}

void ScoreLoad(string path, vector<score_p>* scores, songdata* song) {
	string filepath = path + "/highscore";
	FILE* file = fopen(filepath.c_str(), "rb");
	if (!file) {
		scores->emplace_back();
	} else {
		while (1) {
			score_p entry;
			if (!ScoreRead(file, &entry.best, &entry.last)) {break;}
			int chart;
			chart = entry.best.chart;
			if (chart > -1 && chart < int(song->charts.size())) {
				entry.dif = song->charts[chart].difficulty;
			}
			scores->push_back(entry);
		}
		fclose(file);
	}
}

bool ScoreRead(FILE* infile, score_t* best, score_t* last) {
	int chart;
	if (!fread(&chart, 4, 1, infile)) {
		return false;
	}
	best->chart = chart;
	last->chart = chart;
	score_t* out;
	for (int c = 0; c < 2; c++) {
		if (c == 0) {out = last;}
		else if (c == 1) {out = best;}
		//score
		for (int i = 0; i < 6; i++) {
			if (!fread(&(out->points[i]), 4, 1, infile)) {
				return false; 
			}
		}
		if (!fread(&(out->max_combo), 4, 1, infile)) {
			return false;
		}
	}
	return true;
}

bool ScoreWrite(FILE* outfile, score_t* score) {
	for (int i = 0; i < 6; i++) {
		fwrite(&score->points[i], 4, 1, outfile);
	}
	fwrite(&score->max_combo, 4, 1, outfile);
	return true;
}

bool ScoreIsPB(score_t* current, score_t* best) {
	int current_score = 0;
	int best_score = 0;
	for (int i = 0; i < 6; i++) {
		current_score += current->points[i] * worth[i];
		best_score += best->points[i] * worth[i];
	}
	if (current_score > best_score) {
		return true;
	}
	return false;
}
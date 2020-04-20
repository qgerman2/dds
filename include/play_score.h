#ifndef PLAYSCORE
#define PLAYSCORE
#include <string>
#include "play.h"
struct score_t {
	u32 chart;
	u32 points[6] = {0};
	u32 max_combo = 0;
};
class Play;
class PlayRender;
class PlayScore {
private:
	u32 msperbeat;
	Play* play;
	void updateScoreDisplay(int judge);
	void dropCombo();
public:
	score_t* score;
	int score_total = 0;
	u32 combo = 0;
	u32 judgesWindow[5];
	PlayScore(Play* play);
	~PlayScore();
	void add(step* s, u32 beatfdiff);
	void miss(step* s);
	void updateJudgesWindow();
};
void ScoreSave(std::string, score_t* score);
bool ScoreRead(FILE* infile, score_t* best, score_t* last);
bool ScoreWrite(FILE* infile, score_t* score);
bool ScoreIsPB(score_t* current, score_t* best);
#endif
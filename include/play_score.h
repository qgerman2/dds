#ifndef PLAYSCORE
#define PLAYSCORE
#include <string>
#include "play.h"
#include "parse.h"
typedef struct score_t {
	int chart = -1;
	u32 points[6] = {0};
	u32 max_combo = 0;
} score_t;
typedef struct score_p{
	score_t last;
	score_t best;
	std::string dif = "No records";
} score_p;
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
extern const int worth[6];
void ScoreSave(std::string path, score_t* score);
void ScoreLoad(std::string path, std::vector<score_p>* scores, songdata* song);
bool ScoreRead(FILE* infile, score_t* best, score_t* last);
bool ScoreWrite(FILE* infile, score_t* score);
bool ScoreIsPB(score_t* current, score_t* best);
#endif
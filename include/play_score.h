#ifndef PLAYSCORE
#define PLAYSCORE
#include "play.h"
#define MARVELOUSJUDGE 23
#define PERFECTJUDGE 45
#define GREATJUDGE 90
#define GOODJUDGE 135
#define BOOJUDGE 180
class Play;
class PlayRender;
class PlayScore {
private:
	u32 msperbeat;
	Play* play;
public:
	u32 score = 0;
	u32 combo = 0;
	u32 judgesWindow[5];
	u32 dptotal = 0;
	int dpscore = 0;
	PlayScore(Play* play);
	void add(step* s, u32 beatfdiff);
	void updateJudgesWindow();
	void addDPTotal();
	void dropCombo();
};
#endif
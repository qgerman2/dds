#ifndef SCORE
#define SCORE
#include <nds.h>
struct score_t {
	u32 chart;
	u32 points;
	u32 dp;
	u32 totaldp;
};
class Score {
private:
public:
	Score();
	~Score();
	void loop();
	void render();
};
#endif
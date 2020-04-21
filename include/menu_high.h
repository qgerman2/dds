#ifndef MENUHIGH
#define MENUHIGH
#include <vector>
#include "play_score.h"
class MenuHigh {
private:
	int anim = 0;
	int frame = 0;
	uint items = 0;
	uint count = 0;
	std::vector<score_p>* scores;
	u16* scoreGfx[11];
	u16* pointGfx[10];
	u8 scoreSprite[11];
	u8 pointSprite[3 * 6];
public:
	MenuHigh();
	~MenuHigh();
	void render();
	void update(std::vector<score_p>* scores);
	void draw();
};
#endif
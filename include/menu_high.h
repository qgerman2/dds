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
	u8 difSprite[2];
	u16* difGfx[2];
	u8 lastBestSprite;
	u16* lastBestGfx[2];
public:
	MenuHigh();
	~MenuHigh();
	void render();
	void update(std::vector<score_p>* scores);
	void draw();
};
#endif
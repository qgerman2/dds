#ifndef PAUSE
#define PAUSE
#include <nds.h>
class Pause {
private:
	int frameSprite;
	u16* frameGfx;
	int cursorSprite;
	u16* cursorGfx;
	int cursor = 0;
public:
	Pause();
	~Pause();
	void loop();
	void input();
	void render();
};
#endif
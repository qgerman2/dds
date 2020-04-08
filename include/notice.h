#ifndef NOTICE
#define NOTICE
#include "buffer.h"
class Notice {
private:
	Buffer* buffer;
	u16* bgGfx[12];
	int bgSprite[12];
	int fadeFrame = 64;
	bool transition = false;
	int transitionFrame;
	int transitionTimer = 210;
	int notice_id;
	int menu_id;
	int menu_sub_id;
public:
	Notice();
	~Notice();
	void loop();
	void fadeNoticeUpdate();
	void transitionMenu();
	void transitionMenuUpdate();
};
#endif
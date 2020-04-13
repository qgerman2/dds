#ifndef NOTICE
#define NOTICE
#include "buffer.h"
class Notice {
private:
	Config* config;
	Buffer* buffer;
	int fadeFrame = 64;
	bool transition = false;
	int transitionFrame;
	int transitionTimer = 210;
	int notice_id;
	int menu_id;
	int menu_sub_id;
	int cursorPos = 0;
	int cursorAnim = 0;
	int cursorAlpha = 0;
	int cursorPressed = false;
	int cursorSprite;
	u16* cursorGfx;
	bool ready = true;
public:
	Notice();
	~Notice();
	void loop();
	void fadeNoticeUpdate();
	void transitionMenu();
	void transitionMenuUpdate();
	void cursorUpdate();
};
#endif
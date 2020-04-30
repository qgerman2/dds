#ifndef MENU
#define MENU
#include <string>
#include "menu_dif.h"
#include "menu_wheel.h"
#include "menu_high.h"
class Menu {
private:
	int top_id;
	int sub_id;
	bool ready = false;
	void loadBanner();
	void loadBuffer();
	int waitSprite;
	u16* waitGfx;
public:
	MenuDif* dif;
	MenuWheel* wheel;
	MenuHigh* high;
	std::string bannerQueue = "";
	std::string bannerCurrent = "";
	bool bufferBlock = false;
	std::string bufferFocus = "";
	Menu();
	~Menu();
	void loop();
	void frame();
	void input();
	void render();
};
#endif
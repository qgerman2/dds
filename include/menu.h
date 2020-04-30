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
public:
	MenuDif* dif;
	MenuWheel* wheel;
	MenuHigh* high;
	std::string bannerQueue = "";
	std::string bannerCurrent = "";
	Menu();
	~Menu();
	void loop();
	void frame();
	void input();
	void render();
	void loadBanner();
};
#endif
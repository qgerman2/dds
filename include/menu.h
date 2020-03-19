#ifndef MENU
#define MENU
#include "menu_wheel.h"
class Menu {
private:
	MenuWheel* wheel;
public:
	Menu();
	void loop();
	void input();
	void render();
};
#endif
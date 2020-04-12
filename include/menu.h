#ifndef MENU
#define MENU
#include "menu_dif.h"
#include "menu_wheel.h"
class Menu {
private:
public:
	MenuDif* dif;
	MenuWheel* wheel;
	Menu();
	~Menu();
	void loop();
	void input();
	void render();
};
#endif
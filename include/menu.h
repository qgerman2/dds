#ifndef MENU
#define MENU
#include "menu_dif.h"
#include "menu_wheel.h"
class Menu {
private:
	int top_id;
	int sub_id;
	bool ready = false;
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
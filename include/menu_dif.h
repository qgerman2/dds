#ifndef MENUDIF
#define MENUDIF
#include <nds.h>
#include "buffer.h"
#define DIFVIEW 4
class MenuWheel;
class MenuDif {
private:
	bufferitem* item;
	int frameSprite;
	int sprite[4];
	u16* frameGfx;
	u16* gfx[4];
	const int x = 32;
	const int y = 32;
	int cursor = 0;
	int view = 0;
	int size;
	int arrowSprite[2];
	u16* arrowGfx;
	int cursorSprite;
	u16* cursorGfx;
	void update();
public:
	bool active = false;
	MenuDif();
	~MenuDif();
	void show(bufferitem* item);
	void hide();
	void input();
	void next();
	void prev();
};
#endif
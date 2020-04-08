#ifndef MENUWHEEL
#define MENUWHEEL
#include <string>
#include "buffer.h"
#define ASCIIOFFSET 32
#define CHARWIDTH 10
#define CHAROFFSET 3
#define CHARSPRITES 3
#define WHEELANGLE 6
#define WHEELVIEW 9
#define WHEELVIEWCHAR 7
class MenuWheel {
private:
	Buffer* buffer;
	const int tilesYOffset[5] = {-1, 3, 9, 13, 15};
	bufferitem wheelitems[WHEELVIEW];
	u16* songFontGfx[CHARSPRITES * WHEELVIEWCHAR];
	u8 songFontSprite[CHARSPRITES * WHEELVIEWCHAR];
	u8 songFrameColor[WHEELVIEW];
	int tilesLen[5];
	int tilesTotalLen = 0;
	int bg1;
	int bg2;
	int anim = 0;
	void loadSongFontGfx();
	void loadFrameBg();
	void prev();
	void next();
	void renderChar(int angle);
	void printToBitmap(u8 gfx, std::string str);
	void updateColor();
	void updateFrameBg();
public:
	MenuWheel();
	~MenuWheel();
	void playAnim(int anim);
	void render();
	int frame = 0;
};
#endif
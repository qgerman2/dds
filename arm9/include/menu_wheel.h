#ifndef MENUWHEEL
#define MENUWHEEL
#include <string>
#define ASCIIOFFSET 32
#define CHARWIDTH 10
#define CHAROFFSET 3
#define CHARSPRITES 3
#define WHEELANGLE 6
#define WHEELVIEW 9
#define WHEELVIEWCHAR 7
#define BUFFERSIZE 49
typedef struct wheelitem{
	int type = -1;
	std::string name;
	std::string path;
	std::string smpath;
} wheelitem;
class MenuWheel {
private:
	const int tilesYOffset[5] = {-1, 3, 9, 13, 15};
	int buffercursor = BUFFERSIZE / 2;
	wheelitem bufferitems[BUFFERSIZE];
	wheelitem wheelitems[WHEELVIEW];
	u16* songFontGfx[CHARSPRITES * WHEELVIEWCHAR];
	u8 songFontSprite[CHARSPRITES * WHEELVIEWCHAR];
	u8 songFrameColor[WHEELVIEW];
	int size = -1;
	int tilesLen[5];
	int tilesTotalLen = 0;
	int bg1;
	int bg2;
	int anim = 0;
	int cursor = 0;
	void loadSongFontGfx();
	void loadFrameBg();
	void fillBuffer();
	int bufferToFile(int i);
	int dircountToBuffer(int ic);
	void prev();
	void next();
	void renderChar(int angle);
	void printToBitmap(u8 gfx, std::string str);
	void updateColor();
	void updateFrameBg();
	std::string fileext;
public:
	MenuWheel();
	~MenuWheel();
	void playAnim(int anim);
	void render();
	int frame = 0;
};
#endif
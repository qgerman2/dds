#define ASCIIOFFSET 32
#define CHARWIDTH 10
#define CHAROFFSET 3
#define CHARSPRITES 3
#define WHEELANGLE 6
typedef struct wheelitem{
	int type = -1;
	std::string name;
	std::string path;
	std::string smpath;
} wheelitem;
void mw_setup();
void loadSongFontGfx();
void loadFrameBg();
void wheelNext();
void wheelPrev();
void printToBitmap(u8 gfx, std::string str);
void fillBuffer();
void updateWheelColor();
int bufferToFile(int i);
int dircountToBuffer(int i);
void renderWheel();
void playWheelAnim(int anim);
void renderWheelChar(int angle);
void updateFrameBg();
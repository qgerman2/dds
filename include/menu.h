#define ASCIIOFFSET 32
#define CHARWIDTH 10
#define CHAROFFSET 3
#define CHARSPRITES 3
#define WHEELANGLE 5
typedef struct wheelitem{
	int type = -1;
	std::string name;
	std::string path;
} wheelitem;
void m_setup();
int nearWheelCursor(int i);
void wheelNext();
void wheelPrev();
void loadSongFrameGfx();
void loadSongFontGfx();
void printToBitmap(u8 gfx, std::string str);
bool parseDir(std::string dir, int index, int dest);
void fillWheel();
void menuLoop();
void renderMenu();
void renderWheel();
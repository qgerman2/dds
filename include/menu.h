#define ASCIIOFFSET 32
#define CHARWIDTH 10
#define CHAROFFSET 3
#define CHARSPRITES 3
#define WHEELANGLE 5
typedef struct group {
	std::vector<group*> groups;
	std::vector<metadata> songs;
} group;
void m_setup();
void wheelNext();
void wheelPrev();
void loadSongFrameGfx();
void loadSongFontGfx();
void printToBitmap(u8 gfx, std::string str);
void fillGroup(std::string dir, group* parent);
void menuLoop();
void renderMenu();
void renderWheel();
#define NDSFREQ 32.7284	//khz
#define BPMFRAC 8
#define MINUTEFRAC 12
#define BEATFSCREENYFRAC 14
#define HITY 22 //posicion y donde achuntarle a las flechas
#define NDSHEIGHT 192

void setup(songdata song); 
void loop();
void updateBeat();
void updateSteps();
typedef struct step {
	int x;
	int y;
	u8 type;
	u32 beatf;
	u8 sprite;
	u8 col;
	u8 stepcount;
	u16* gfx = NULL;
} step;
typedef struct hold {
	int col;
	u32 startbeatf;
	u32 endbeatf;
	u32 stepcount;
	step* laststep;
} hold;

void newSteps(u16 data, u32 beatf);

measure getMeasureAtBeat(u32 beat);
u32 millis();

extern u32 beatf;
extern std::vector<step> steps;
extern std::vector<hold> holds;
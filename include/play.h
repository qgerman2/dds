#define NDSFREQ 32.7284	//khz
#define BPMFRAC 8
#define MINUTEFRAC 12
#define BEATFSCREENYFRAC 14
#define HITY 22 //posicion y donde achuntarle a las flechas
#define NDSHEIGHT 192

void p_setup(); 
void playLoop();
u8 getNoteType(u32 row);
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
	u8 notetype;
	u16* gfx = NULL;
	bool disabled = false;
} step;
typedef struct hold {
	int col;
	u32 startbeatf;
	u32 endbeatf;
	u8 stepcount;
} hold;

void newSteps(u16 data, u32 beatf, u8 notetype);
void removeStep(std::vector<step>::iterator* s);

measure getMeasureAtBeat(u32 beat);
u32 millis();

extern std::vector<step>::iterator holdCol[4];
extern u32 beatf;
extern u32 bpmf;
extern u32 beatfperiod;
extern std::vector<step> steps;
extern std::vector<hold> holds;
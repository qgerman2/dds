#ifndef PLAY
#define PLAY
#include <list>
#include <vector>
#include "parse.h"
#define NDSFREQ 32.7284	//khz
#define BPMFRAC 8
#define MINUTEFRAC 14
#define BEATFSCREENYFRAC 16
#define HITY 22 //posicion y donde achuntarle a las flechas
#define NDSHEIGHT 192
typedef struct step {
	int x;
	int y;
	u8 type;
	u32 beatf;
	int sprite;
	u8 col;
	u8 stepcount;
	u8 notetype;
	u16* gfx = NULL;
	bool disabled = false;
	int height = 0;
} step;
typedef struct hold {
	int col;
	u32 startbeatf;
	u32 endbeatf;
	u8 stepcount;
} hold;
class PlayRender;
class PlayScore;
class PlayInput;
class Play {
private:
	u32 time;
	int bpmindex = -1;
	u32 minutef;
	int beat;					//beat global
	int firstbeat = -1;			//primer beat del measure global
	int count = 0; 				//beat relativo al primer beat de measure global
	int sets;					//cantidad de sets en measure
	int cursor = 0;
	measure* m;
	int measurecursor = -1;
	int stepbeatf = 0;			//beat preciso en el cursor
	int relbeatf = 0;			//beat preciso relativo al cursor a x set
	u32 rowspermeasure = 0;
	u16 *set;
	u32 minutefbpm;
	u32 minutefsum;
	bool lostbeatsbpm;
	int offset;
	bool cursor_end = false;
	int beat_end = 0;
public:
	PlayRender* render;
	PlayScore* score;
	PlayInput* input;
	songdata* song;
	u32 beatf;
	u32 bpmf = 0;
	std::list<step> steps;
	std::vector<hold> holds;
	Play();
	~Play();
	void loop();
	void frame();
	void updateSteps();
	void updateBeat();
	void newSteps(u16 data, u32 beatf, u8 notetype);
	std::list<step>::iterator removeStep(std::list<step>::iterator s);
	int getNoteType(u32 row);
	bool getMeasureAtBeat(u32 beat);
	u32 millis();
};
extern u32 beatfperiod;
#endif
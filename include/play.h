#ifndef PLAY
#define PLAY
#include <vector>
#include "parse.h"
#define NDSFREQ 32.7284	//khz
#define BPMFRAC 8
#define MINUTEFRAC 12
#define BEATFSCREENYFRAC 14
#define HITY 22 //posicion y donde achuntarle a las flechas
#define NDSHEIGHT 192
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
class PlayRender;
class PlayScore;
class PlayInput;
class Play {
private:
	u8 parseaheadbeats = 12;
	u32 time;
	int bpmindex = -1;
	u32 minutef;
	int beat;					//beat global
	int firstbeat = -1;			//primer beat del measure global
	int count = 0; 				//beat relativo al primer beat de measure global
	int sets;					//cantidad de sets en measure
	int cursor = 0;
	int measurecursor = -1;
	int stepbeatf = 0;			//beat preciso en el cursor
	int relbeatf = 0;			//beat preciso relativo al cursor a x set
	u32 rowspermeasure = 0;
	u16 *set;
	measure m;
	u32 minutefbpm;
	u32 minutefsum;
	bool lostbeatsbpm;
public:
	PlayRender* render;
	PlayScore* score;
	PlayInput* input;
	u32 beatf;
	u32 bpmf = 0;
	std::vector<step> steps;
	std::vector<hold> holds;
	Play();
	void loop();
	void updateSteps();
	void updateBeat();
	void newSteps(u16 data, u32 beatf, u8 notetype);
	void removeStep(std::vector<step>::iterator* s);
	int getNoteType(u32 row);
	measure getMeasureAtBeat(u32 beat);
	u32 millis();
};
extern u32 beatfperiod;
#endif
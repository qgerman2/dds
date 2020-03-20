#ifndef PLAYINPUT
#define PLAYINPUT
#include <vector>
#include "play.h"
class Play;
class PlayScore;
class PlayInput {
private:
	u32 keysPressed;
	u32 keysReleased;
	u32 keysHeldd;
	u32 beatfdiff;
	u8 stateCol[4] = {0}; //0 aun no pasa, 1 buscar hold, 2 ta listo
	bool pressed = false;	//se apreto/solto teclas durante el frame
	bool released = false;
	bool held = false;
	Play* play;
public:
	std::vector<step>::iterator holdCol[4];
	PlayInput(Play* play);
	~PlayInput();
	void update();
};
#endif
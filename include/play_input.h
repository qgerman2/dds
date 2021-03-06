#ifndef PLAYINPUT
#define PLAYINPUT
#include <vector>
#include <utility>
#include <list>
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
	std::pair<bool, std::list<step>::iterator> mineCol[4];
	bool pressed = false;	//se apreto/solto teclas durante el frame
	bool released = false;
	bool held = false;
	Play* play;
public:
	std::pair<bool, std::list<step>::iterator> holdCol[4];
	PlayInput(Play* play);
	~PlayInput();
	void update();
};
#endif
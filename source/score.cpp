#include <nds.h>
#include <iostream>
#include "globals.h"
#include "score.h"
#include "render.h"

using namespace std;

Score::Score() {
	vramSetBankF(VRAM_F_BG_EXT_PALETTE_SLOT01);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
	fadeIn(3);
	cout << "\nscore screen";
}

Score::~Score() {

}

void Score::loop() {
	while (1) {
		input();
		render();
		swiWaitForVBlank();
		if (state != 4) {return;}
	}
}

void Score::input() {

}

void Score::render() {

}
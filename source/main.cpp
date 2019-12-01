#include <iostream>
#include <nds.h>
#include <fat.h>
#include <string>
#include "main.h"
#include "parse.h"
#include "play.h"

using namespace std;

int main(){
	songdata song;
	consoleDemoInit();
	if (fatInitDefault()) {
		song = parseSong("/ddr/Inkh 5 - dream's destination/G.O.A.T/GOAT.SM");
	} else {
		iprintf("fatInitDefault failure: terminating\n");
	}

	vramSetBankA(VRAM_A_MAIN_SPRITE);
	videoSetMode( MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT );

	setup(song);
	loop();
	
	return 0;
}


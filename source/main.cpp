#include <nds.h>
#include <fat.h>
#include <string>
#include "play.h"
#include "main.h"
#include "parse.h"

using namespace std;

int main(){
	consoleDemoInit();
	if (fatInitDefault()) {
		parseSong("/ddr/Inkh 5 - dream's destination/G.O.A.T/GOAT.SM");
	} else {
		iprintf("fatInitDefault failure: terminating\n");
	}
	irqInit();
	irqEnable(IRQ_VBLANK);

	vramSetBankA(VRAM_A_MAIN_SPRITE);
	videoSetMode( MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT );

	setup();
	
	
	return 0;
}


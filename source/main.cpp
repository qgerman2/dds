#include <iostream>
#include <nds.h>
#include <fat.h>
#include <string>
#include "main.h"
#include "parse.h"
#include "sound.h"
#include "play.h"
#include "lodepng.h"

using namespace std;
u16* bg = (u16*)0x026C8000;

#include <iostream>
#include <vector>
int idd;
int main(){
	videoSetMode(MODE_3_2D | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT);
	videoSetModeSub(MODE_0_2D | DISPLAY_SPR_ACTIVE);
	bgExtPaletteEnable();
	bgExtPaletteEnableSub();
	vramSetBankA(VRAM_A_MAIN_BG_0x06040000);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	vramSetBankD(VRAM_D_SUB_SPRITE);
	vramSetBankE(VRAM_E_MAIN_BG);
	vramSetBankF(VRAM_F_LCD); //bg ext palette
	vramSetBankH(VRAM_H_LCD); //bg ext palette sub
	songdata song;
	
	idd = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 16, 0);

	if (fatInitDefault()) {
		song = parseSong("/ddr/song.sm");
	} else {
		iprintf("fatInitDefault failure: terminating\n");
	}

	imagetobg("/ddr/bg.png");
	setup(song);
	vramSetBankF(VRAM_F_BG_EXT_PALETTE_SLOT01);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
	for (int i = 0; i < 60; i++) {
		swiWaitForVBlank();
	}
	playSong();
	TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024;
	TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;
	loop();
	return 0;
}

void imagetobg(string path) {
	vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, path);
	for(unsigned i = 0; i < image.size() / 4; i++) {
  		bg[i] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
	}
	dmaCopyWordsAsynch(0, bg, bgGetGfxPtr(idd), 0x18000);
}
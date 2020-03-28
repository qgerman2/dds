#include <nds.h>
#include <fat.h>
#include <maxmod9.h>
#include <iostream>
#include <string>
#include <vector>
#include "main.h"
#include "menu.h"
#include "play.h"
#include "parse.h"
#include "sound.h"
#include "artwork.h"

using namespace std;

int state = 2;
string songpath;
int bgid;

int main(){
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);
	bgExtPaletteEnable();
	bgExtPaletteEnableSub();
	vramSetBankA(VRAM_A_MAIN_BG_0x06040000);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	vramSetBankD(VRAM_D_SUB_SPRITE);
	vramSetBankE(VRAM_E_MAIN_BG);
	vramSetBankF(VRAM_F_LCD); //bg ext palette
	vramSetBankH(VRAM_H_LCD); //bg ext palette sub
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);

	
	if (!fatInitDefault()) {
		sassert(0, "failed to load libfat");
	}
	consoleDemoInit();
	bgid = bgInit(2, BgType_Bmp8, BgSize_B16_256x256, 16, 0);
	
	processArtwork("mono.jpeg", 0);

	s_play();

	while (1) {
		switch (state) {
			case (0): {
				Menu* menu = new Menu();
				menu->loop();
				delete menu;
			}
			break;
			case (1): {
				metadata tags = parseSimFile(songpath, false);
				songdata song = parseSong(&tags);
				Play* play = new Play(&song);
				play->loop();
				delete play;
			}
			break;
			case (2): {
				while (1) {
					mmStreamUpdate();
					swiWaitForVBlank();
				}
			}
		}

	}
	return 0;
}
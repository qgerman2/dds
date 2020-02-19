#include <iostream>
#include <vector>
#include <nds.h>
#include <fat.h>
#include <string>
#include "parse.h"
#include "main.h"
#include "menu.h"
#include "play.h"
#include "lodepng.h"

using namespace std;

songdata song;
GameState gameState = MENU;

int main(){
	videoSetMode(MODE_3_2D);
	videoSetModeSub(MODE_0_2D);
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

	song = parseSimFile("/ddr/song.sm");

	while (1) {
		switch (gameState) {
			case (MENU):
				m_setup();
				menuLoop();
				break;
			case (PLAY):
				p_setup();
				playLoop();
				break;
		}

	}
	return 0;
}

void imagetobg(string path) {
	vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, path);
	//for(unsigned i = 0; i < image.size() / 4; i++) {
  	//	bg[i] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
	//}
	//dmaCopyWordsAsynch(0, bg, bgGetGfxPtr(idd), 0x18000);
}
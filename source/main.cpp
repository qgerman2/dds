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

int main(){
	songdata song;
	consoleDemoInit();
	videoSetMode(MODE_3_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT);
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	consoleDemoInit();
	bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	if (fatInitDefault()) {
		song = parseSong("/ddr/song.sm");
		playSong();
	} else {
		iprintf("fatInitDefault failure: terminating\n");
	}
	if (isDSiMode()) {
		cout << "dsimode";
	}
	else 
	{
		cout << "nodsimode";
	}

	imagetobg("/ddr/bg5.png");

	setup(song);
	loop();

	return 0;
}

void imagetobg(string path) {
	vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, path);
	cout << width << "\n" << height;
	for(unsigned i = 0; i < image.size() / 4; i++) {
  		bg[i] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
	}
	dmaCopyWordsAsynch(0, bg, BG_GFX, 0x18000);
}
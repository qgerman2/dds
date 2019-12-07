#include <iostream>
#include <nds.h>
#include <fat.h>
#include <string>
#include "main.h"
#include "parse.h"
#include "play.h"
#include "lodepng.h"

using namespace std;
u16* bg = (u16*)0x026C8000;

int main(){
	songdata song;
	consoleDemoInit();
	videoSetMode( MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT);
	vramSetBankA(VRAM_A_MAIN_BG);
	consoleDemoInit();
	bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	if (fatInitDefault()) {
		song = parseSong("/ddr/Inkh 5 - dream's destination/G.O.A.T/GOAT.SM");
	} else {
		iprintf("fatInitDefault failure: terminating\n");
	}

	vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, "/ddr/bg.png");
	if (error)
		printf("fug");
	cout << image.size();
	cout << "\n" << BIT(15);
	for(unsigned i = 0; i < image.size() / 4; i++) {
  		bg[i] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
	}
	dmaCopyWordsAsynch(0, bg, BG_GFX, 0x18000);

	setup(song);
	loop();

	return 0;
}


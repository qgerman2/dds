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

#include <iostream>
#include <vector>
int id;
int main(){
	videoSetMode(MODE_3_2D | DISPLAY_BG3_ACTIVE | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT);
	vramSetBankA(VRAM_A_MAIN_BG_0x06040000);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankE(VRAM_E_MAIN_BG);
	songdata song;
	
	id = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 16, 0);

	if (fatInitDefault()) {
		song = parseSong("/ddr/song.sm");
		playSong();
	} else {
		iprintf("fatInitDefault failure: terminating\n");
	}

	consoleDemoInit();
	setup(song);
	imagetobg("/ddr/bg5.png");
	loop();
	return 0;
}

void imagetobg(string path) {
	vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, path);
	for(unsigned i = 0; i < image.size() / 4; i++) {
  		bgGetGfxPtr(id)[i] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
	}
}
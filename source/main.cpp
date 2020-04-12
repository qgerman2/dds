#include <nds.h>
#include <fat.h>
#include <maxmod9.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "main.h"
#include "menu.h"
#include "menu_wheel.h"
#include "play.h"
#include "parse.h"
#include "sound.h"
#include "artwork.h"
#include "notice.h"
#include "config.h"
#include <font.h>

using namespace std;

struct settings_t settings;
int state = 2;
string simpath;
string songpath;
int bgid;
Buffer* shared_buffer = NULL;
bool nocash = false;

int main(){
	srand(time(NULL));
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

	bgid = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 16, 0);

	//set up debug console
	PrintConsole *console = consoleInit(0, 0, BgType_Text4bpp, BgSize_T_256x256, 0, 1, false, false);
	ConsoleFont font;
	font.gfx = (u16*)fontTiles;
	font.pal = (u16*)fontPal;
	font.numChars = 95;
	font.numColors = fontPalLen / 2;
	font.bpp = 4;
	font.asciiOffset = 32;
	font.convertSingleColor = false;
	consoleSetFont(console, &font);
	bgSetPriority(console->bgId, 0);

	//check if running on no$gba
	if (strncmp((char*)0x4FFFA00, "no$gba", 6) == 0) {
		nocash = true;
		cout << "\nRunning on no$gba";
	}

	while (1) {
		switch (state) {
			case (0): {
				Menu menu;
				menu.loop();
			}
			break;
			case (1): {
				songdata song;
				parseSimFile(&song, simpath);
				parseChart(&song);
				loadAudio(songpath + "/" + song.music);
				loadArtwork(songpath + "/" + song.bg, bgGetGfxPtr(bgid), 256, 192);
				playAudio();
				Play play(&song);
				play.loop();
			}
			break;
			case (2): {
				Notice notice;
				notice.loop();
			}
			break;
		}
	}
	return 0;
}
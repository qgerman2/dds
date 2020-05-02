#include <nds.h>
#include <fat.h>
#include <sys/dir.h>
#include <unistd.h>
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
#include "render.h"
#include "notice.h"
#include "config.h"
#include "pause.h"
#include "globals.h"
#include <font.h>

using namespace std;

void vblank_interrupt() {
	if (shared_play) {
		shared_play->frame();
	} else if (shared_menu) {
		shared_menu->frame();
	}
}

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
	consoleid = console->bgId;

	if (!fatInitDefault()) {
		cout << "\nFailed to load libfat";
		cout << "\nIf you're using a flashcard, try to patch dds.nds with a DLDI patch";
		error();
	}

	if (!ddsCheck()) {
		cout << "\nTry placing dds.nds at root of your sdcard and create a dds folder";
		cout << "\nAlso check if your sd card is write protected";
		error();
	}

	setBackdropColor(ARGB16(1, 29, 29, 29));
	setBackdropColorSub(ARGB16(1, 29, 29, 29));
	bgid = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 16, 0);
	bgSetPriority(bgid, 2);

	ConfigLoad();

	if (settings.debug) {
		bgShow(consoleid);
	} else {
		bgHide(consoleid);
	}

	//check if running on no$gba
	if (strncmp((char*)0x4FFFA00, "no$gba", 6) == 0) {
		nocash = true;
		cout << "\nRunning on no$gba";
	}

	//play frame
	irqSet(IRQ_VBLANK, vblank_interrupt);


	while (1) {
		switch (state) {
			case (0): {
				Menu menu;
				menu.loop();
			}
			break;
			case (1): {
				Play play;
				play.loop();
			}
			break;
			case (2): {
				Notice notice;
				notice.loop();
			}
			break;
			case (3): {
				Pause pause;
				pause.loop();
			}
			break;
		}
	}
	return 0;
}

void error() {
	while (1) {
		swiWaitForVBlank();
	}
}

bool ddsCheck() {
	DIR* dir = opendir("/dds");
	if (!dir) {
		cout << "\nCan't find dds folder";
		cout << "\nTrying to create it";
		mkdir("/dds",777);
		dir = opendir("/dds"); 
		if (!dir) {
			cout << "\nCouldn't create dds folder";
			return false;
		} 
	}
	if (dir) {
		closedir(dir);
	}
	return true;
}
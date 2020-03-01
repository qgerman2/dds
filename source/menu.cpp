#include <vector>
#include <iostream>
#include <string>
#include <nds.h>
#include <fat.h>
#include <string>
#include <sys/dir.h>
#include <unistd.h>
#include <stdio.h>
#include "parse.h"
#include "menu.h"
#include "render.h"
#include <font.h>
#include "menu_wheel.h"

using namespace std;

void m_setup() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
		pushSpriteSub(i);
	}
	PrintConsole *console = consoleInit(0, 2, BgType_Text4bpp, BgSize_T_256x256, 4, 1, true, false);
	ConsoleFont font;
	font.gfx = (u16*)fontTiles;
	font.pal = (u16*)fontPal;
	font.numChars = 95;
	font.numColors = fontPalLen / 2;
	font.bpp = 4;
	font.asciiOffset = 32;
	font.convertSingleColor = false;
	consoleSetFont(console, &font);
	mw_setup();
	vramSetBankF(VRAM_F_BG_EXT_PALETTE_SLOT01);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
}

void menuLoop() {
	while (1) {
		swiWaitForVBlank();
		renderMenu();
		bgUpdate();
		oamUpdate(&oamSub);
	}
}

void renderMenu() {
	renderWheelChar();
}
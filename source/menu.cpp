#include <nds.h>
#include <iostream>
#include "main.h"
#include "menu.h"
#include "menu_wheel.h"
#include "render.h"
#include <font.h>

using namespace std;

Menu::Menu() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
		pushSpriteSub(i);
	}
	wheel = new MenuWheel();
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
	vramSetBankF(VRAM_F_BG_EXT_PALETTE_SLOT01);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
}

Menu::~Menu() {
	delete wheel;
	vramSetBankF(VRAM_F_LCD); //bg ext palette
	vramSetBankH(VRAM_H_LCD); //bg ext palette sub
}

void Menu::loop() {
	while (1) {
		scanKeys();
		input();
		swiWaitForVBlank();
		render();
		oamUpdate(&oamSub);
		if (state != 0) {
			return;
		}
	}
}

void Menu::input() {
	if (wheel->frame == 0) {
		if (keysHeld() & KEY_UP) {
			wheel->playAnim(-1);
		}
		else if (keysHeld() & KEY_DOWN) {
			wheel->playAnim(1);
		}
		else if (keysHeld() & KEY_A) {
			state = 1;
		}
	}
}

void Menu::render() {
	wheel->render();
}
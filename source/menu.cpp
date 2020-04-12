#include <nds.h>
#include <iostream>
#include "main.h"
#include "menu.h"
#include "menu_dif.h"
#include "menu_wheel.h"
#include "render.h"
#include <maxmod9.h>
#include <font.h>

using namespace std;

Menu::Menu() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
		pushSpriteSub(i);
	}
	dif = new MenuDif();
	wheel = new MenuWheel(this);
	vramSetBankF(VRAM_F_BG_EXT_PALETTE_SLOT01);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
}

Menu::~Menu() {
	vramSetBankF(VRAM_F_LCD);
	vramSetBankH(VRAM_H_LCD);
	delete dif;
	delete wheel;
}

void Menu::loop() {
	while (1) {
		scanKeys();
		input();
		mmStreamUpdate();
		swiWaitForVBlank();
		render();
		oamUpdate(&oamSub);
		if (state != 0) {
			return;
		}
	}
}

void Menu::input() {
	wheel->input();
}

void Menu::render() {
	wheel->render();
}
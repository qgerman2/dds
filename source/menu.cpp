#include <nds.h>
#include <iostream>
#include "globals.h"
#include "menu.h"
#include "menu_dif.h"
#include "menu_wheel.h"
#include "menu_high.h"
#include "render.h"
#include "sound.h"
#include <maxmod9.h>
#include <font.h>
#include <wheel_bg.h>
#include <wheel_sub_bg.h>

using namespace std;

Menu::Menu() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
		pushSpriteSub(i);
	}
	clearBitmapBg(bgid);
	dif = new MenuDif();
	high = new MenuHigh();
	wheel = new MenuWheel(this);

	int top_id = bgInit(0, BgType_Text8bpp, BgSize_ER_256x256, 0, 1);
	dmaCopy(wheel_bgTiles, bgGetGfxPtr(top_id), wheel_bgTilesLen);
	dmaCopy(wheel_bgMap, bgGetMapPtr(top_id), wheel_bgMapLen);
	dmaCopy(wheel_bgPal, &VRAM_F[0*16*256], wheel_bgPalLen);
	bgSetPriority(top_id, 3);

	int sub_id = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 1, 2);
	dmaCopy(wheel_sub_bgTiles, bgGetGfxPtr(sub_id), wheel_sub_bgTilesLen);
	dmaCopy(wheel_sub_bgMap, bgGetMapPtr(sub_id), wheel_sub_bgMapLen);
	dmaCopy(wheel_sub_bgPal, &VRAM_H[1*16*256], wheel_sub_bgPalLen);
	bgSetPriority(sub_id, 3);

	vramSetBankF(VRAM_F_BG_EXT_PALETTE_SLOT01);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
}

Menu::~Menu() {
	if (state == 1) {
		stopAudio();
	}
	fadeOut(3);
	vramSetBankF(VRAM_F_LCD);
	vramSetBankH(VRAM_H_LCD);
	delete dif;
	delete wheel;
	delete high;
}

void Menu::loop() {
	while (1) {
		scanKeys();
		input();
		if (!idleAudio()) {mmStreamUpdate();}
		swiWaitForVBlank();
		render();
		oamUpdate(&oamMain);
		oamUpdate(&oamSub);
		if (state != 0) {
			return;
		}
		if (!ready) {
			ready = true;
			fadeIn(3);
		}
	}
}

void Menu::input() {
	if (dif->active) {
		dif->input();
	} else {
		if (keysDown() & KEY_B) {
			state = 2;
			return;
		}
		wheel->input();
	}
}

void Menu::render() {
	wheel->render();
	high->render();
	bgUpdate();
}
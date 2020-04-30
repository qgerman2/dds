#include <nds.h>
#include <iostream>
#include "globals.h"
#include "render.h"
#include "pause.h"
#include <play_quit.h>
#include <dif_cursor.h>

using namespace std;

Pause::Pause() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
	}
	cursorSprite = popSprite();
	cursorGfx = oamAllocateGfx(&oamMain, SpriteSize_8x8, SpriteColorFormat_16Color);
	dmaCopy(dif_cursorTiles, cursorGfx, dif_cursorTilesLen);
	dmaCopy(dif_cursorPal, SPRITE_PALETTE + 16, dif_cursorPalLen);
	oamSet(&oamMain, cursorSprite, 90, 84, 0, 1, SpriteSize_8x8, SpriteColorFormat_16Color, cursorGfx, 2, false, false, false, false, false);
	frameSprite = popSprite();
	frameGfx = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_16Color);
	dmaCopy(play_quitTiles, frameGfx, play_quitTilesLen);
	dmaCopy(play_quitPal, SPRITE_PALETTE, play_quitPalLen);
	oamSet(&oamMain, frameSprite, 96, 80, 0, 0, SpriteSize_64x32, SpriteColorFormat_16Color, frameGfx, 2, false, false, false, false, false);
}

Pause::~Pause() {
	fadeOut(3, true);
	oamFreeGfx(&oamMain, cursorGfx);
	oamFreeGfx(&oamMain, frameGfx);
}

void Pause::loop() {
	while(1) {
		scanKeys();
		input();
		render();
		oamUpdate(&oamMain);
		swiWaitForVBlank();
		if (state != 3) {return;}
	}
}

void Pause::input() {
	if (keysDown() & KEY_UP) {
		cursor--;
		if (cursor < 0) {cursor = 1;}
	} else if (keysDown() & KEY_DOWN) {
		cursor++;
		if (cursor > 1) {cursor = 0;}
	} else if (keysDown() & KEY_A) {
		switch (cursor) {
			case 0:
				keep_artwork = true;
				state = 1;
				break;
			case 1:
				state = 0;
				break;
		}
	}
}

void Pause::render() {
	oamSet(&oamMain, cursorSprite, 90, 84 + 16 * cursor, 0, 1, SpriteSize_8x8, SpriteColorFormat_16Color, cursorGfx, 2, false, false, false, false, false);
}
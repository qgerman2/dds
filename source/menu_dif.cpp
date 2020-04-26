#include <nds.h>
#include "globals.h"
#include "menu_dif.h"
#include "render.h"
#include "buffer.h"
#include <dif_frame.h>
#include <dif_arrow.h>
#include <dif_cursor.h>

using namespace std;

MenuDif::MenuDif() {
	cursorSprite = popSpriteSub();
	cursorGfx = oamAllocateGfx(&oamSub, SpriteSize_8x8, SpriteColorFormat_16Color);
	dmaCopy(dif_cursorTiles, cursorGfx, dif_cursorTilesLen);
	dmaCopy(dif_cursorPal, SPRITE_PALETTE_SUB + 32, dif_cursorPalLen);
	for (int i = 0; i < 2; i++) {
		arrowSprite[i] = popSpriteSub();
	}
	arrowGfx = oamAllocateGfx(&oamSub, SpriteSize_8x8, SpriteColorFormat_16Color);
	dmaCopy(dif_arrowTiles, arrowGfx, dif_arrowTilesLen);
	dmaCopy(dif_arrowPal, SPRITE_PALETTE_SUB + 16, dif_arrowPalLen);
	oamRotateScale(&oamSub, 1, 0, 1 << 8, -1 << 8);
	for (int i = 0; i < 4; i++) {
		sprite[i] = popSpriteSub();
		gfx[i] = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_Bmp);
	}
	frameSprite = popSpriteSub();
	frameGfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	dmaCopy(dif_frameTiles, frameGfx, dif_frameTilesLen);
	dmaCopy(dif_framePal, SPRITE_PALETTE_SUB, dif_framePalLen);
	oamRotateScale(&oamSub, 2, 0, 1 << 7, 1 << 7);
}

MenuDif::~MenuDif() {
	oamFreeGfx(&oamSub, cursorGfx);
	oamFreeGfx(&oamSub, arrowGfx);
	oamFreeGfx(&oamSub, frameGfx);
	for (int i = 0; i < 4; i++) {
		oamFreeGfx(&oamSub, gfx[i]);
	}
}

void MenuDif::update() {
	int c = 0;
	auto i = item->song.charts.begin();
	for (advance(i, view); i < item->song.charts.end(); i++) {
		printToBitmap(&gfx[(c / 2) * 2], 2, (c % 2) * 27, i->meter + " " + i->difficulty);
		c++;
		if (c > DIFVIEW) {break;}
	}
	if (view < size - 4) {
		oamSet(&oamSub, arrowSprite[1], x + 60, y + 115, 0, 1, SpriteSize_8x8, SpriteColorFormat_16Color, arrowGfx, 0, false, false, false, false, false);
	} else {
		oamClearSprite(&oamSub, arrowSprite[1]);
	}
	if (view > 0) {
		oamSet(&oamSub, arrowSprite[0], x + 60, y + 5, 0, 1, SpriteSize_8x8, SpriteColorFormat_16Color, arrowGfx, 1, false, false, false, false, false);
	} else {
		oamClearSprite(&oamSub, arrowSprite[0]);
	}
}

void MenuDif::show(bufferitem* item) {
	this->item = item;
	active = true;
	size = item->song.charts.size();
	cursor = 0;
	view = 0;
	update();
	for (int i = 0; i < 4; i++) {
		oamSet(&oamSub, sprite[i], x + 4 + (i % 2) * 64, y + 16 + (i / 2) * 54, 0, 15, SpriteSize_64x64, SpriteColorFormat_Bmp, gfx[i], 0, false, false, false, false, false);
	}
	oamSet(&oamSub, cursorSprite, x - 5, y + 20, 0, 2, SpriteSize_8x8, SpriteColorFormat_16Color, cursorGfx, 0, false, false, false, false, false);
	oamSet(&oamSub, frameSprite, x, y, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color, frameGfx, 2, true, false, false, false, false);
}

void MenuDif::hide() {
	active = false;
	for (int i = 0; i < 4; i++) {
		oamClearSprite(&oamSub, sprite[i]);
		dmaFillHalfWords(0, gfx[i], 128 * 64);
	}
	for (int i = 0; i < 2; i++) {
		oamClearSprite(&oamSub, arrowSprite[i]);
	}
	oamClearSprite(&oamSub, cursorSprite);
	oamClearSprite(&oamSub, frameSprite);
}

void MenuDif::input() {
	if (keysDown() & KEY_UP) {prev();}
	else if (keysDown() & KEY_DOWN) {next();}
	else if (keysDown() & KEY_B) {hide();}
	else if (keysDown() & KEY_A) {
		songchart = cursor + view;
		state = 1;
	}
}

void MenuDif::next() {
	cursor++;
	if (cursor > size - 1) {cursor = size - 1;}
	if (cursor > 3) {
		view++;
		if (view > size - 4) {view = size - 4;}
		cursor = 3;
	}
	oamSet(&oamSub, cursorSprite, x - 5, y + 20 + (27 * cursor), 0, 2, SpriteSize_8x8, SpriteColorFormat_16Color, cursorGfx, 0, false, false, false, false, false);
	update();
}

void MenuDif::prev() {
	cursor--;
	if (cursor < 0) {
		view--;
		if (view < 0) {view = 0;}
		cursor = 0;
	}
	oamSet(&oamSub, cursorSprite, x - 5, y + 20 + (27 * cursor), 0, 2, SpriteSize_8x8, SpriteColorFormat_16Color, cursorGfx, 0, false, false, false, false, false);
	update();
} 

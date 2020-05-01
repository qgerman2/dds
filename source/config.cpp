#include <nds.h>
#include <iostream>
#include "globals.h"
#include "config.h"
#include "render.h"
#include <config_sub.h>
//#include <score.h>
#include <mark.h>
#include <config_cursor.h>
#include <score_numbers.h>

using namespace std;

enum SETTING {SPEED, BGOPACITY, STARTUPSONG, CACHEBANNERS, CACHEBG, MINES, DEBUG};
enum STATE {IDLE, FADEIN, FADEOUT, NEXT, PREV};

//todo: reduce verbosity
Config::Config() {
	for (int i = 0; i < 10; i++) {
		numberGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
		dmaCopy(score_numbersTiles + i * 16, numberGfx[i], 64);
		dmaCopy(score_numbersTiles + 176 + i * 16, numberGfx[i] + 32, 64);
	}
	//dmaCopy(scorePal, SPRITE_PALETTE_SUB, scorePalLen);
	markGfx = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
	dmaCopy(markTiles, markGfx, markTilesLen);
	dmaCopy(markPal, SPRITE_PALETTE_SUB + 16, markPalLen);
	for (int i = 0; i < CONFIGCOUNT; i++) {
		valueSprites[i] = popSpriteSub();
	}
}

Config::~Config() {
	for (int i = 0; i < 10; i++) {
		oamFreeGfx(&oamSub, numberGfx[i]);
	}
	oamFreeGfx(&oamSub, markGfx);
}

void Config::bg() {
	vramSetBankH(VRAM_H_LCD);
	sub_bg = bgInitSub(1, BgType_Text4bpp, BgSize_T_256x512, 3, 6);
	dmaCopy(config_subTiles, bgGetGfxPtr(sub_bg), config_subTilesLen);
	dmaCopy(config_subMap, bgGetMapPtr(sub_bg), config_subMapLen);
	dmaCopy(config_subPal, &BG_PALETTE_SUB[16], config_subPalLen);
	bgSetPriority(sub_bg, 2);
	bgHide(sub_bg);
	cursor_bg = bgInitSub(3, BgType_ExRotation, BgSize_ER_256x256, 1, 2);
	dmaCopy(config_cursorTiles, bgGetGfxPtr(cursor_bg), config_cursorTilesLen);
	dmaCopy(config_cursorMap, bgGetMapPtr(cursor_bg), config_cursorMapLen);
	dmaCopy(config_cursorPal, &VRAM_H[3*16*256], config_cursorPalLen);
	bgHide(cursor_bg);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
}

void Config::show() {
	active = true;
	state = FADEIN;
	animFrame = 16;
	cursor = 0;
	bgShow(sub_bg);
	REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BG2;
	REG_BLDALPHA_SUB = 16 << 8;
}

void Config::hide() {
	state = FADEOUT;
	animFrame = 16;
	bgHide(cursor_bg);
	REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BG2;
	REG_BLDALPHA_SUB = 16;
	ConfigSave();
}

void Config::update() {
	switch (state) {
		case FADEIN: {
			animFrame--;
			y = -animFrame - 32;
			updateSprites();
			bgSetScroll(sub_bg, 0, y);
			REG_BLDALPHA_SUB = (16 - animFrame) | animFrame << 8;
			//fin del fadein
			if (animFrame == 0) {
				y = -32;
				y_f = y << 8;
				bgSetScroll(cursor_bg, 0, y - 32);
				bgShow(cursor_bg);
				state = IDLE;
				REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG3 | BLEND_DST_BG1;
				cursorFrame = 0;
				cursorAnim = 1;
				REG_BLDALPHA_SUB = 16 << 8;
			}
			break;
		}
		case FADEOUT: {
			animFrame--;
			y = y - 1;
			updateSprites();
			bgSetScroll(sub_bg, 0, y);
			REG_BLDALPHA_SUB = animFrame | (16 - animFrame) << 8;
			//fin del fadeout
			if (animFrame == 0) {
				bgHide(sub_bg);
				hideSprites();
				active = false;
				state = IDLE;
			}
			break;
		}
		case IDLE: 
		case PREV:
		case NEXT: {
			if (cursorAnim > 0) {cursorFrame++;}
			else if (cursorAnim < 0) {cursorFrame--;}
			if (cursorFrame == 0) {cursorAnim = 1;}
			else if (cursorFrame == 16) {cursorAnim = -1;}
			REG_BLDALPHA_SUB = cursorFrame | (16 - cursorFrame) << 8;
			input();
			if (state == NEXT || state == PREV) {
				animFrame--;
				y_f = y_f + (((y_dest - y) << 16) / (6 << 8));
				y = y_f >> 8;
				//updateSprites();
				if (animFrame == 0) {
					y = y_dest;
					y_f = y << 8;
					state = IDLE;
					REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG3 | BLEND_DST_BG1;
					REG_BLDALPHA_SUB = 16 << 8;
				}
				bgSetScroll(sub_bg, 0, y);
				bgSetScroll(cursor_bg, 0, y - 32 * (cursor + 1));
			}
			updateSprites();
			break;
		}
	}
}

void Config::input() {
	if (keysDown() & KEY_DOWN) {next();}
	else if (keysDown() & KEY_UP) {prev();}
	else if (keysDown() & KEY_B) {hide();}
	else if (keysDown() & KEY_A) {toggle();}
	else if (keysDown() & KEY_RIGHT) {increase();}
	else if (keysDown() & KEY_LEFT) {decrease();}
}

void Config::next() {
	cursor++;
	if (cursor > CONFIGCOUNT - 1) {
		cursor = CONFIGCOUNT - 1;
	} else {
		animFrame = 30;
		y_dest = (cursor + 1) * 32 - 80;
		if (y_dest < y_min) {y_dest = y_min;}
		if (y_dest > y_max) {y_dest = y_max;}
		state = NEXT;
		cursorFrame = 0;
		cursorAnim = 1;
	}
}

void Config::prev() {
	cursor--;
	if (cursor < 0) {
		cursor = 0;
	} else {
		animFrame = 30;
		y_dest = (cursor + 1) * 32 - 80;
		if (y_dest < y_min) {y_dest = y_min;}
		if (y_dest > y_max) {y_dest = y_max;}
		state = PREV;
		cursorFrame = 0;
		cursorAnim = 1;
	}
}

void Config::updateSprites() {
	//speed
	oamSet(&oamSub, valueSprites[0], 188, -y + 40 , 0, 4, SpriteSize_16x16, SpriteColorFormat_16Color, numberGfx[settings.speed], 0, false, false, false, false, false);
	//opacity
	oamSet(&oamSub, valueSprites[1], 188, -y + 40 + 32, 0, 4, SpriteSize_16x16, SpriteColorFormat_16Color, numberGfx[settings.opacity], 0, false, false, false, false, false);
	//startup song
	if (settings.intro) {
		oamSet(&oamSub, valueSprites[2], 188, -y + 40 + 32 * 2, 0, 1, SpriteSize_16x16, SpriteColorFormat_16Color, markGfx, 0, false, false, false, false, false);
	} else {
		oamClearSprite(&oamSub, valueSprites[2]);
	}
	//cache banners
	if (settings.cache) {
		oamSet(&oamSub, valueSprites[3], 188, -y + 40 + 32 * 3, 0, 1, SpriteSize_16x16, SpriteColorFormat_16Color, markGfx, 0, false, false, false, false, false);
	} else {
		oamClearSprite(&oamSub, valueSprites[3]);
	}
	//cache backgrounds
	if (settings.cache_bg) {
		oamSet(&oamSub, valueSprites[4], 188, -y + 40 + 32 * 4, 0, 1, SpriteSize_16x16, SpriteColorFormat_16Color, markGfx, 0, false, false, false, false, false);
	} else {
		oamClearSprite(&oamSub, valueSprites[4]);
	}
	//mines
	if (settings.mines && (-y + 40 + 32 * 5) < 192) { //fixme
		oamSet(&oamSub, valueSprites[5], 188, -y + 40 + 32 * 5, 0, 1, SpriteSize_16x16, SpriteColorFormat_16Color, markGfx, 0, false, false, false, false, false);
	} else {
		oamClearSprite(&oamSub, valueSprites[5]);
	}
	//debug console
	if (settings.debug && (-y + 40 + 32 * 5) < 192) { //fixme
		oamSet(&oamSub, valueSprites[6], 188, -y + 40 + 32 * 6, 0, 1, SpriteSize_16x16, SpriteColorFormat_16Color, markGfx, 0, false, false, false, false, false);
	} else {
		oamClearSprite(&oamSub, valueSprites[6]);
	}
}

void Config::hideSprites() {
	for (int i = 0; i < CONFIGCOUNT; i++) {
		oamClearSprite(&oamSub, valueSprites[i]);
	}
}

void Config::toggle() {
	switch (cursor) {
		case STARTUPSONG:
			settings.intro = !settings.intro;
			break;
		case CACHEBANNERS:
			settings.cache = !settings.cache;
			break;
		case CACHEBG:
			settings.cache_bg = !settings.cache_bg;
			break;
		case MINES:
			settings.mines = !settings.mines;
			break;
		case DEBUG:
			settings.debug = !settings.debug;
			if (settings.debug) {
				bgShow(consoleid);
			} else {
				bgHide(consoleid);
			}
			break;
	}
}

void Config::increase() {
	switch (cursor) {
		case SPEED:
			settings.speed++;
			break;
		case BGOPACITY:
			settings.opacity++;
			break;
	}
	ConfigCheck();
}

void Config::decrease() {
	switch (cursor) {
		case SPEED:
			settings.speed--;
			break;
		case BGOPACITY:
			settings.opacity--;
			break;
	}
	ConfigCheck();
}

void ConfigLoad() {
	FILE* file = fopen(CONFIGPATH, "rb");
	if (file) {
		cout << "\nLoading config";
		fread(&settings.speed, 4, 1, file);
		fread(&settings.opacity, 4, 1, file);
		fread(&settings.intro, 1, 1, file);
		fread(&settings.cache, 1, 1, file);
		fread(&settings.cache_bg, 1, 1, file);
		fread(&settings.debug, 1, 1, file);
		fread(&settings.mines, 1, 1, file);
		fclose(file);
		ConfigCheck();
	} else {
		cout << "\nFailed to load config file";
		ConfigSave();
	}
}

void ConfigSave() {
	FILE* file = fopen(CONFIGPATH, "wb");
	if (file) {
		cout << "\nSaving config";
		fwrite(&settings.speed, 4, 1, file);
		fwrite(&settings.opacity, 4, 1, file);
		fwrite(&settings.intro, 1, 1, file);
		fwrite(&settings.cache, 1, 1, file);
		fwrite(&settings.cache_bg, 1, 1, file);
		fwrite(&settings.debug, 1, 1, file);
		fwrite(&settings.mines, 1, 1, file);
		fclose(file);
	}
}

void ConfigCheck() {
	if (settings.speed < 1) {settings.speed = 1;}
	if (settings.speed > 9) {settings.speed = 9;}
	if (settings.opacity < 0) {settings.opacity = 0;}
	if (settings.opacity > 9) {settings.opacity = 9;}
}
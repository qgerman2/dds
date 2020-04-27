#include <nds.h>
#include <iostream>
#include <string>
#include "globals.h"
#include "sound.h"
#include "notice.h"
#include "buffer.h"
#include "render.h"
#include <notice_bg.h>
#include <menu_bg.h>
#include <menu_sub_bg.h>
#include <menu_cursor.h>
#include <config_cursor.h>

using namespace std;

Notice::Notice() {
	setBrightness(3, -16);

	for (int i = 0; i < 128; i++) {
		pushSprite(i);
		pushSpriteSub(i);
	}
	config = new Config();

	notice_id = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 1, 2);
	dmaCopy(notice_bgTiles, bgGetGfxPtr(notice_id), notice_bgTilesLen);
	dmaCopy(notice_bgMap, bgGetMapPtr(notice_id), notice_bgMapLen);
	dmaCopy(notice_bgPal, &VRAM_H[1*16*256], notice_bgPalLen);
	bgSetPriority(notice_id, 1);

	menu_sub_id = bgInitSub(2, BgType_ExRotation, BgSize_ER_256x256, 2, 4);
	dmaCopy(menu_sub_bgTiles, bgGetGfxPtr(menu_sub_id), menu_sub_bgTilesLen);
	dmaCopy(menu_sub_bgMap, bgGetMapPtr(menu_sub_id), menu_sub_bgMapLen);
	dmaCopy(menu_sub_bgPal, &VRAM_H[2*16*256], menu_sub_bgPalLen);
	bgSetPriority(menu_sub_id, 3);

	menu_id = bgInit(0, BgType_Text8bpp, BgSize_ER_256x256, 0, 1);
	dmaCopy(menu_bgTiles, bgGetGfxPtr(menu_id), menu_bgTilesLen);
	dmaCopy(menu_bgMap, bgGetMapPtr(menu_id), menu_bgMapLen);
	dmaCopy(menu_bgPal, &VRAM_F[0*16*256], menu_bgPalLen);

	cursorSprite = popSpriteSub();
	cursorGfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_Bmp);
	dmaCopy(menu_cursorBitmap, cursorGfx, menu_cursorBitmapLen);
	oamSet(&oamSub, cursorSprite, 16, 64, 0, 0, SpriteSize_64x64, SpriteColorFormat_Bmp, cursorGfx, 0, false, false, false, false, false);

	if (!shared_buffer) {
		shared_buffer = new Buffer();
		buffer = shared_buffer;
		buffer->setRandom();
		buffer->fill();
		songpath = buffer->items[BUFFERSIZE / 2].path;
		simpath = buffer->items[BUFFERSIZE / 2].smpath;
		songdata song;
		parseSimFile(&song, simpath);
		if (settings.intro) {
			loadAudio(songpath + "/" + song.music);
		}
		for (int i = 0; i < 20; i++) {
			swiWaitForVBlank();
		}
	} else {
		bgHide(notice_id);
		fadeFrame = 0;
		transitionTimer = 0;
		transitionFrame = 0;
		cursorAnim = 1;
		cursorAlpha = 0;
		transition = true;
		config->bg();
		REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG3 | BLEND_DST_BG2;
		ready = false;
	}

	vramSetBankF(VRAM_F_BG_EXT_PALETTE_SLOT01);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
}

Notice::~Notice() {
	fadeOut(3);
	vramSetBankF(VRAM_F_LCD);
	vramSetBankH(VRAM_H_LCD);
	oamFreeGfx(&oamSub, cursorGfx);
	bgHide(notice_id);
	bgHide(menu_id);
	REG_BLDCNT_SUB = 0;
	delete config;
}

void Notice::loop() {
	while (1) {
		//fade in a advertencia
		if (fadeFrame > 0) {fadeNoticeUpdate();}
		//transicion a menu
		if (transitionTimer > 0)  {
			transitionTimer--;
		} else if (!transition) {
			transitionMenu();
		}
		if (transition && transitionFrame > 0) {transitionMenuUpdate();}
		scanKeys();
		if (!config->active) {cursorUpdate();} 
		if (config->active) {config->update();}
		oamUpdate(&oamMain);
		oamUpdate(&oamSub);
		bgUpdate();
		if (!idleAudio()) {mmStreamUpdate();}
		swiWaitForVBlank();
		if (state != 2) {return;}
		if (!ready) {
			ready = true;
			fadeIn(3);
		}
	}
}

void Notice::fadeNoticeUpdate() {
	fadeFrame--;
	setBrightness(2, -fadeFrame / 4);
}

void Notice::transitionMenu() {
	if (settings.intro) {
		playAudio();
	}
	transition = true;
	transitionFrame = 30;
	REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BG2;
	REG_BLDALPHA_SUB = 16;
}

void Notice::transitionMenuUpdate() {
	transitionFrame--;
	REG_BLDALPHA_SUB = (transitionFrame / 2) | (16 - (transitionFrame / 2)) << 8;
	setBrightness(1, -(transitionFrame / 2));
	if (transitionFrame == 0) {
		cursorAnim = 1;
		cursorAlpha = 0;
		REG_BLDCNT_SUB = BLEND_DST_BG2;
		REG_BLDALPHA_SUB = 0;
		config->bg();
	}
}

void Notice::cursorUpdate() {
	//pestañeo cursor
	if (cursorAnim == 1) {
		cursorAlpha++;
		if (cursorAlpha == 15) {
			cursorAnim = 2;
		}
	} else if (cursorAnim == 2) {
		cursorAlpha--;
		if (cursorAlpha == 0) {
			cursorAnim = 1;
		}
	}
	//posicion cursor
	u32 keys = keysDown();
	if (keys & KEY_RIGHT || keys & KEY_LEFT) {
		cursorAlpha = 0;
		cursorAnim = 1; 
		cursorPressed = false;
	} 
	if (keys & KEY_RIGHT) {cursorPos++; if (cursorPos > 2) {cursorPos = 0;};}
	if (keys & KEY_LEFT)  {cursorPos--; if (cursorPos < 0) {cursorPos = 2;};}
	//activar menus
	if (keys & KEY_A) {cursorPressed = true;}
	if (keysUp() & KEY_A && cursorPressed) {
		cursorPressed = false;
		cursorAlpha = 0;
		cursorAnim = 1;
		switch (cursorPos) {
			case 0:
				state = 0;
				break;
			case 1:
				config->show();
				break;
			case 2:
				exit(0);
				break;
		}
	}
	oamSetXY(&oamSub, cursorSprite, 16 + 80 * cursorPos, 64);
	oamSetAlpha(&oamSub, cursorSprite, cursorAlpha);
}
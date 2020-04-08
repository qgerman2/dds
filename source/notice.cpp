#include <nds.h>
#include <iostream>
#include <string>
#include "main.h"
#include "sound.h"
#include "notice.h"
#include "buffer.h"
#include <notice_bg.h>
#include <menu_bg.h>
#include <menu_sub_bg.h>
#include <bitset>

Notice::Notice() {
	setBrightness(3, -16);

	shared_buffer = new Buffer();
	buffer = shared_buffer;
	buffer->setRandom();
	buffer->fill();
	songpath = buffer->items[BUFFERSIZE / 2].path;
	simpath = buffer->items[BUFFERSIZE / 2].smpath;
	songdata song;
	parseSimFile(&song, simpath);
	loadAudio(songpath + "/" + song.music);

	notice_id = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 1, 2);
	dmaCopy(notice_bgTiles, bgGetGfxPtr(notice_id), notice_bgTilesLen);
	dmaCopy(notice_bgMap, bgGetMapPtr(notice_id), notice_bgMapLen);
	dmaCopy(notice_bgPal, &VRAM_H[1*16*256], notice_bgPalLen);

	menu_sub_id = bgInitSub(2, BgType_ExRotation, BgSize_ER_256x256, 2, 4);
	dmaCopy(menu_sub_bgTiles, bgGetGfxPtr(menu_sub_id), menu_sub_bgTilesLen);
	dmaCopy(menu_sub_bgMap, bgGetMapPtr(menu_sub_id), menu_sub_bgMapLen);
	dmaCopy(menu_sub_bgPal, &VRAM_H[2*16*256], menu_sub_bgPalLen);

	menu_id = bgInit(0, BgType_Text8bpp, BgSize_ER_256x256, 0, 1);
	dmaCopy(menu_bgTiles, bgGetGfxPtr(menu_id), menu_bgTilesLen);
	dmaCopy(menu_bgMap, bgGetMapPtr(menu_id), menu_bgMapLen);
	dmaCopy(menu_bgPal, &VRAM_F[0*16*256], menu_bgPalLen);

	vramSetBankF(VRAM_F_BG_EXT_PALETTE_SLOT01);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
	for (int i = 0; i < 20; i++) {
		swiWaitForVBlank();
	}
}

Notice::~Notice() {
	vramSetBankF(VRAM_F_LCD);
	vramSetBankH(VRAM_H_LCD);
	bgHide(notice_id);
	bgHide(menu_id);
	setBrightness(3, 0);
	REG_BLDCNT_SUB = 0;
}

void Notice::loop() {
	while (1) {
		//fade in a advertencia
		if (fadeFrame > 0) {fadeNoticeUpdate();}
		//transicion a menu
		if (transitionTimer > 0) {transitionTimer--;}
		else if (!transition) {transitionMenu();}
		if (transition && transitionFrame > 0) {transitionMenuUpdate();}
		scanKeys();
		if (keysHeld() & KEY_B) {state = 0;}
		mmStreamUpdate();
		swiWaitForVBlank();
		if (state != 2) {return;}
	}
}

void Notice::fadeNoticeUpdate() {
	fadeFrame--;
	setBrightness(2, -fadeFrame / 4);
	if (fadeFrame == 0) {
		for (int i = 0; i < 10; i++) {
			swiWaitForVBlank();
		}
	}
}

void Notice::transitionMenu() {
	playAudio();
	transition = true;
	transitionFrame = 32;
	REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BG2;
	REG_BLDALPHA_SUB = (transitionFrame / 2) | (16 - (transitionFrame / 2)) << 8;
}

void Notice::transitionMenuUpdate() {
	transitionFrame--;
	REG_BLDALPHA_SUB = (transitionFrame / 2) | (16 - (transitionFrame / 2)) << 8;
	setBrightness(1, -(transitionFrame / 2));
}
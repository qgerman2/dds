#include <nds.h>
#include <iostream>
#include "main.h"
#include "config.h"
#include "render.h"
#include <config_sub.h>
#include <score.h>
#include <mark.h>

using namespace std;

Config::Config() {
	sub_bg = bgInitSub(3, BgType_ExRotation, BgSize_ER_256x256, 7, 3);
	dmaCopy(config_subTiles, bgGetGfxPtr(sub_bg), config_subTilesLen);
	dmaCopy(config_subMap, bgGetMapPtr(sub_bg), config_subMapLen);
	dmaCopy(config_subPal, &VRAM_H[3*16*256], config_subPalLen);
	bgSetPriority(sub_bg, 1);
	bgHide(sub_bg);
	for (int i = 0; i < 10; i++) {
		numberGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
		dmaCopy(scoreTiles + i * 16, numberGfx[i], 64);
		dmaCopy(scoreTiles + 176 + i * 16, numberGfx[i] + 32, 64);
	}
	//dmaCopy(scorePal, SPRITE_PALETTE_SUB, scorePalLen);
	for (int i = 0; i < CONFIGCOUNT; i++) {
		valueSprites[i] = popSpriteSub();
	}
}

void Config::show() {
	active = true;
	anim = 1;
	animFrame = 120;
	bgShow(sub_bg);
}

void Config::hide() {
	active = false;

}

void Config::update() {
	if (anim == 1) {
		animFrame--;
		dialog_y = animFrame + 32;
		updateSprites();
		bgSetScroll(sub_bg, 0, -dialog_y);
		if (animFrame == 0) {anim = 0;}
	}
}

void Config::updateSprites() {
	//speed
	oamSet(&oamSub, valueSprites[0], 188, dialog_y + 40 , 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, numberGfx[settings.speed], 0, false, false, false, false, false);
	//opacity
	oamSet(&oamSub, valueSprites[1], 188, dialog_y + 40 + 32, 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, numberGfx[settings.opacity], 0, false, false, false, false, false);
	
}
#include <nds.h>
#include <iostream>
#include <vector>
#include <tap.h>
#include <tail.h>
#include <hold.h>
#include "parse.h"
#include "play.h"
#include "play_render.h"

using namespace std;

bool sprites[128];

u16* tapMemory;
u16* tailMemory;
u16* holdMemory;

void pr_setup() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
	}
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	tapMemory = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	tailMemory = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	holdMemory = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
	dmaCopy(tapTiles, tapMemory, tapTilesLen);
	dmaCopy(tailTiles, tailMemory, tailTilesLen);
	dmaCopyHalfWords(3, holdBitmap, holdMemory, holdBitmapLen);
	dmaCopy(tapPal, SPRITE_PALETTE, 512);
	setRotData();
}

void renderSteps() {
	for (auto i = steps.begin(); i != steps.end(); i++) {
		if (i->y < 224) {
			switch (i->type) {
				case (1):
				case (2):
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, tapMemory, i->col, false, false, false, false, false);
					break;
				case (3):
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, tailMemory, 1, false, false, false, false, false);
					break;
				case (5):
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, 0, SpriteSize_32x32, SpriteColorFormat_Bmp, holdMemory, 1, false, false, false, false, false);
					break;
			}
		} else {
			oamClearSprite(&oamMain, i->sprite);
		}
	}
}

void renderHolds() {
	int ystart;
	int yend;
	u32 height;
	step s;
	for (auto h = holds.begin(); h != holds.end(); h++) {
		ystart = ((h->startbeatf >> BEATFSCREENYFRAC) - (beatf >> BEATFSCREENYFRAC)) + 16;
		if (ystart > NDSHEIGHT) {
			continue;
		}
		height = NDSHEIGHT - ystart;
		if (h->endbeatf > 0) {
			yend = ((h->endbeatf >> BEATFSCREENYFRAC) - (beatf >> BEATFSCREENYFRAC));
			if (yend < -32) {
				holds.erase(h--);
				continue;
			}
			height = yend - ystart;
		} 
		if ((height / 32) + 1 > h->stepcount) {
			h->stepcount = h->stepcount + 1;
			s.type = 5;
			s.x = (10 + 30 * h->col);
			s.y = ystart + (32 * (h->stepcount - 1));
			s.col = h->col;
			s.sprite = popSprite();
			s.beatf = h->startbeatf;
			s.stepcount = h->stepcount - 1;
			steps.push_back(s);
		}
	}

}

u8 popSprite() {
	for (u8 i = 0; i < 128; i++) {
		if (sprites[i]) {
			sprites[i] = FALSE;
			return i;
		}
	}
	sassert(0, "out of sprites");
	return 0;
}

void pushSprite(u8 i) {
	sprites[i] = TRUE;
	oamClearSprite(&oamMain, i);
}

void setRotData() {
	oamRotateScale(&oamMain, 0, degreesToAngle(90), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 1, degreesToAngle(0), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 2, degreesToAngle(180), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 3, degreesToAngle(270), intToFixed(1, 8), intToFixed(1, 8));
}
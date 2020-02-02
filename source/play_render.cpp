#include <nds.h>
#include <iostream>
#include <vector>
#include <tap.h>
#include <tail.h>
#include <hold.h>
#include <hit.h>
#include "parse.h"
#include "play.h"
#include "play_render.h"

using namespace std;

bool sprites[128];

u16* tapMemory;
u16* tailMemory;
u16* holdMemory;
u16* hitMemory;

void pr_setup() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
	}
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	tapMemory = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	tailMemory = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	holdMemory = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
	hitMemory = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);

	dmaCopy(tapTiles, tapMemory, tapTilesLen);
	dmaCopy(tailTiles, tailMemory, tailTilesLen);
	dmaCopyHalfWords(3, holdBitmap, holdMemory, holdBitmapLen);
	dmaCopy(hitTiles, hitMemory, hitTilesLen);

	dmaCopy(tapPal, SPRITE_PALETTE, 512);
	dmaCopy(hitPal, SPRITE_PALETTE + 16, 512);
	setRotData();

	u8 left = popSprite();
	u8 up = popSprite();
	u8 down = popSprite();
	u8 right = popSprite();
	oamSet(&oamMain, left, 10, HITYOFFSET, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, hitMemory, 0, false, false, false, false, false);
	oamSet(&oamMain, up, 10 + 30, HITYOFFSET, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, hitMemory, 1, false, false, false, false, false);
	oamSet(&oamMain, down, 10 + 60, HITYOFFSET, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, hitMemory, 2, false, false, false, false, false);
	oamSet(&oamMain, right, 10 + 90, HITYOFFSET, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, hitMemory, 3, false, false, false, false, false);
	oamSetPalette(&oamMain, left, 1);
	oamSetPalette(&oamMain, down, 1);
	oamSetPalette(&oamMain, up, 1);
	oamSetPalette(&oamMain, right, 1);
}

void renderPlay() {
	renderSteps();
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
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, tapMemory, i->col, false, false, false, false, false);
					break;
				case (5):
					if (i->gfx != NULL) {
						oamSet(&oamMain, i->sprite, i->x, i->y, 0, 15, SpriteSize_32x32, SpriteColorFormat_Bmp, i->gfx, 1, false, false, false, false, false);
					}
					else {
						oamSet(&oamMain, i->sprite, i->x, i->y, 0, 15, SpriteSize_32x32, SpriteColorFormat_Bmp, holdMemory, 1, false, false, false, false, false);
					}
					break;
			}
		} else {
			oamClearSprite(&oamMain, i->sprite);
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
	oamRotateScale(&oamMain, 1, degreesToAngle(180), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 2, degreesToAngle(0), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 3, degreesToAngle(270), intToFixed(1, 8), intToFixed(1, 8));
}
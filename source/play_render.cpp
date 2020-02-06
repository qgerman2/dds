#include <nds.h>
#include <iostream>
#include <vector>
#include <tap.h>
#include <tail.h>
#include <hold.h>
#include <hit.h>
#include <numbers.h>
#include "parse.h"
#include "play.h"
#include "play_render.h"
#include "play_score.h"

using namespace std;

bool sprites[128];

u16* tapMemory;
u16* tailMemory;
u16* holdMemory;
u16* hitMemory;
u16* numberGfx[10];
u8 comboSprite[3];

void pr_setup() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
	}
	for (int i = 0; i < 3; i++) {
		comboSprite[i] = popSprite();
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
	//cargar numeros
	for (int n = 0; n < 10; n++) {
		int k = n / 4;
		int p = n % 4;
		numberGfx[n] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
		dmaCopy(numbersTiles + (k * 512) + (p * 32), numberGfx[n], 128);
		dmaCopy(numbersTiles + (k * 512) + (p * 32) + 128, numberGfx[n] + 64, 128);
		dmaCopy(numbersTiles + (k * 512) + (p * 32) + 256, numberGfx[n] + 128, 128);
		dmaCopy(numbersTiles + (k * 512) + (p * 32) + 384, numberGfx[n] + 192, 128);
	}
	dmaCopy(numbersPal, SPRITE_PALETTE + 64, 64);
}

void renderPlay() {
	renderSteps();
	renderCombo();
}

u8 diff;
void renderSteps() {
	for (auto i = 0; i < 4; i++) {
		if (holdCol[i]->y < (HITYOFFSET + 16)) {
			if (holdCol[i]->gfx == NULL) {
				holdCol[i]->gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
				dmaCopyHalfWords(3, holdBitmap, holdCol[i]->gfx, holdBitmapLen);
			}
			diff = HITYOFFSET + 16 - holdCol[i]->y;
			dmaFillHalfWords(ARGB16(0,0,0,0), holdCol[i]->gfx, 32*diff*2);
		}
	}
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
						oamSet(&oamMain, i->sprite, i->x, i->y, 0, 15, SpriteSize_32x32, SpriteColorFormat_Bmp, i->gfx, 2, false, false, false, true, false);
					}
					else {
						oamSet(&oamMain, i->sprite, i->x, i->y, 0, 15, SpriteSize_32x32, SpriteColorFormat_Bmp, holdMemory, 2, false, false, false, true, false);
					}
					break;
			}
		} else {
			oamClearSprite(&oamMain, i->sprite);
		}
	}
}

void renderCombo() {
	int u;
	int d;
	int c;
	if (combo < 10) {
		oamSet(&oamMain, comboSprite[2], 90, 90, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[combo], 2, false, false, false, true, false);			
		oamClearSprite(&oamMain, comboSprite[0]);
		oamClearSprite(&oamMain, comboSprite[1]);
	}
	else if (combo < 100) {
		u = combo % 10;
		d = combo / 10;
		oamSet(&oamMain, comboSprite[2], 90, 90, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[u], 2, false, false, false, true, false);			
		oamSet(&oamMain, comboSprite[1], 90 - 20, 90, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[d], 2, false, false, false, true, false);			
		oamClearSprite(&oamMain, comboSprite[0]);
	}
	else if (combo < 1000) {
		u = (combo % 100) % 10;
		d = (combo % 100) / 10;
		c = combo / 100;
		oamSet(&oamMain, comboSprite[2], 90, 90, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[u], 2, false, false, false, true, false);			
		oamSet(&oamMain, comboSprite[1], 90 - 20, 90, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[d], 2, false, false, false, true, false);			
		oamSet(&oamMain, comboSprite[0], 90 - 40, 90, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[c], 2, false, false, false, true, false);			
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
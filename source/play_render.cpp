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
#include <math.h>

#include <marvelous.h>
#include <perfect.h>
#include <great.h>
#include <good.h>
#include <boo.h>
#include <miss.h>

using namespace std;

bool sprites[128];

u16* tapMemory;
u16* tailMemory;
u16* holdMemory;
u16* hitMemory;
u16* numberGfx[10];
u16* judgeGfx[24];
const u16* judgePal[6] = {
	marvelousPal,
	perfectPal,
	greatPal,
	goodPal,
	booPal,
	missPal,
};
u8 judgeFrame = 0;
u8 judgeAnim = 0;
u8 comboSprite[3];
u8 judgeSprite[2];

void pr_setup() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
	}
	for (int i = 0; i < 3; i++) {
		comboSprite[i] = popSprite();
	}
	for (int i = 0; i < 2; i++) {
		judgeSprite[i] = popSprite();
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
	loadJudgmentGfx();
	dmaCopy(judgePal[0], SPRITE_PALETTE + 64 + 16, 64);
}

void loadJudgmentGfx() {
	const uint* tiles[6] {
		marvelousTiles,
		perfectTiles,
		greatTiles,
		goodTiles,
		booTiles,
		missTiles,
	};
	for (int i = 0; i < 6; i++) {
		int g1 = i * 4;
		int g2 = g1 + 1;
		int g3 = g2 + 1;
		int g4 = g3 + 1;
		judgeGfx[g1] = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_16Color);
		judgeGfx[g2] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
		judgeGfx[g3] = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_16Color);
		judgeGfx[g4] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
		dmaCopy(tiles[i], judgeGfx[g1], 256);
		dmaCopy(tiles[i] + 192, judgeGfx[g1] + 128, 256);
		dmaCopy(tiles[i] + 192 * 2, judgeGfx[g1] + 256, 256);
		dmaCopy(tiles[i] + 192 * 3, judgeGfx[g1] + 384, 256);
		dmaCopy(tiles[i] + 64, judgeGfx[g2], 128);
		dmaCopy(tiles[i] + 64 + 192, judgeGfx[g2] + 64, 128);
		dmaCopy(tiles[i] + 64 + 192 * 2, judgeGfx[g2] + 128, 128);
		dmaCopy(tiles[i] + 64 + 192 * 3, judgeGfx[g2] + 192, 128);
		dmaCopy(tiles[i] + 96, judgeGfx[g3], 256);
		dmaCopy(tiles[i] + 96 + 192, judgeGfx[g3] + 128, 256);
		dmaCopy(tiles[i] + 96 + 192 * 2, judgeGfx[g3] + 256, 256);
		dmaCopy(tiles[i] + 96 + 192 * 3, judgeGfx[g3] + 384, 256);
		dmaCopy(tiles[i] + 160, judgeGfx[g4], 128);
		dmaCopy(tiles[i] + 160 + 192, judgeGfx[g4] + 64, 128);
		dmaCopy(tiles[i] + 160 + 192 * 2, judgeGfx[g4] + 128, 128);
		dmaCopy(tiles[i] + 160 + 192 * 3, judgeGfx[g4] + 192, 128);
	}
}

void renderPlay() {
	renderSteps();
	renderCombo();
	renderJudgment();
}

u8 diff;
void renderSteps() {
	for (auto i = 0; i < 4; i++) {
		if ((holdCol[i] != steps.end()) && (holdCol[i]->y < (HITYOFFSET + 16))) {
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

void renderJudgment() {
	if (judgeFrame == 0) {return;}
	int x = 48;
	int y = 50;
	u8 f = 0;
	u32 size;
	u32 offset;
	judgeFrame--;
	if (judgeFrame > 60) {
		size = 256 + (judgeFrame - 60) * 6;
	}
	else if (judgeFrame > 0) {
		size = 256;
	}
	else {
		oamClearSprite(&oamMain, judgeSprite[0]);
		oamClearSprite(&oamMain, judgeSprite[1]);
		return;
	}
	if (size == 256) {f = 1;}
	oamRotateScale(&oamMain, 5, 0, (1 << 16) / size, (1 << 16) / size);
	offset = ((size - 256) * 32) / 256;
	oamSet(&oamMain, judgeSprite[0], x - offset / 2, y, 0, 5, SpriteSize_64x32, SpriteColorFormat_16Color, judgeGfx[judgeAnim * 2], 5, true, false, false, false, false);
	oamSet(&oamMain, judgeSprite[1], x - f + 81 + offset, y, 0, 5, SpriteSize_32x32, SpriteColorFormat_16Color, judgeGfx[judgeAnim * 2 + 1], 5, true, false, false, false, false);
}

void playJudgmentAnim(u8 anim) {
	judgeFrame = 70;
	if (judgeAnim != anim) {
		dmaCopy(judgePal[anim / 2], SPRITE_PALETTE + 64 + 16, 64);
		judgeAnim = anim;
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
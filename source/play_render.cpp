#include <nds.h>
#include <iostream>
#include <vector>
#include "parse.h"
#include "play.h"
#include "play_render.h"
#include "play_score.h"
#include <math.h>

#include <step.h>
#include <tap.h>
#include <tail.h>
#include <hold.h>
#include <hit.h>
#include <numbers.h>
#include <font.h>

#include <marvelous.h>
#include <perfect.h>
#include <great.h>
#include <good.h>
#include <boo.h>
#include <miss.h>

#include <bar.h>
#include <barBot.h>
#include <barTop.h>

using namespace std;

bool sprites[128];

u16* stepGfx[8];

u16* tapMemory;
u16* tailMemory;
u16* holdMemory;
u16* hitMemory;

u16* barTopGfx;
u8 barTopSprite;
u16* barBotGfx;
u8 barBotSprite;
u8 segments = 0;
u16* barGfx;

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
	oamSet(&oamMain, left, HITXOFFSET, HITYOFFSET, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, hitMemory, 0, false, false, false, false, false);
	oamSet(&oamMain, up, HITXOFFSET + 32, HITYOFFSET, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, hitMemory, 1, false, false, false, false, false);
	oamSet(&oamMain, down, HITXOFFSET + 64, HITYOFFSET, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, hitMemory, 2, false, false, false, false, false);
	oamSet(&oamMain, right, HITXOFFSET + 96, HITYOFFSET, 0, 0, SpriteSize_32x32, SpriteColorFormat_16Color, hitMemory, 3, false, false, false, false, false);
	oamSetPalette(&oamMain, left, 1);
	oamSetPalette(&oamMain, down, 1);
	oamSetPalette(&oamMain, up, 1);
	oamSetPalette(&oamMain, right, 1);
	loadStepGfx();
	loadLifebarGfx();
	loadNumberGfx();
	loadJudgmentGfx();
	//loadFontGfx();
}

void loadStepGfx() {
	for (int i = 0; i < 8; i++) {
		stepGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
		dmaCopy(stepTiles, stepGfx[i], 128);
		dmaCopy(stepTiles + i * 32 + 256, stepGfx[i] + 64, 128);
		dmaCopy(stepTiles + i * 32 + 512, stepGfx[i] + 128, 128);
		dmaCopy(stepTiles + i * 32 + 768, stepGfx[i] + 192, 128);
	}
	dmaCopy(stepPal, SPRITE_PALETTE + 16 * 15, stepPalLen);
}

void loadLifebarGfx() {
	barTopGfx = oamAllocateGfx(&oamMain, SpriteSize_16x8, SpriteColorFormat_16Color);
	dmaCopy(barTopTiles, barTopGfx, barTopTilesLen);
	dmaCopy(barTopPal, SPRITE_PALETTE + 32, barTopPalLen);
	barTopSprite = popSprite();
	oamSet(&oamMain, barTopSprite, 16, 19, 0, 2, SpriteSize_16x8, SpriteColorFormat_16Color, barTopGfx, 2, false, false, false, false, false);
	
	barBotGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	dmaCopy(barBotTiles, barBotGfx, barBotTilesLen);
	dmaCopy(barBotPal, SPRITE_PALETTE + 32 + 16, barBotPalLen);
	barBotSprite = popSprite();
	oamSet(&oamMain, barBotSprite, 8, 152, 0, 3, SpriteSize_32x32, SpriteColorFormat_16Color, barBotGfx, 2, false, false, false, false, false);

	int id = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 3, 0);
	dmaCopy(barTiles, bgGetGfxPtr(id), barTilesLen);
	dmaCopy(barPal, &VRAM_F[1*16*256], barPalLen);
	barGfx = bgGetMapPtr(id);
}

void loadNumberGfx() {
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

void loadFontGfx() {
	PrintConsole *console = consoleInit(0, 2, BgType_Text4bpp, BgSize_T_256x256, 4, 1, true, false);
	ConsoleFont font;
	font.gfx = (u16*)fontTiles;
	font.pal = (u16*)fontPal;
	font.numChars = 95;
	font.numColors = fontPalLen / 2;
	font.bpp = 4;
	font.asciiOffset = 32;
	font.convertSingleColor = false;
	consoleSetFont(console, &font);
}

void renderLifebar() {
	u8 t;
	segments++;
	if (segments > 28) {
		segments = 0;
	}
	for (int y = 0; y < 9; y++) {
		if (y > 8 - ((segments - 1) / 3)) {
			barGfx[2 + (y*2+3)*32] = 1;
			barGfx[3 + (y*2+3)*32] = 2;
			barGfx[2 + (y*2+4)*32] = 3;
			barGfx[3 + (y*2+4)*32] = 4;
		}
		else if ((y == 8 - ((segments - 1) / 3)) && (segments != 0)) {
			t = 2 - (segments+2)%3;
			barGfx[2 + (y*2+3)*32] = 5 + t*4;
			barGfx[3 + (y*2+3)*32] = 6 + t*4;
			barGfx[2 + (y*2+4)*32] = 7 + t*4;
			barGfx[3 + (y*2+4)*32] = 8 + t*4;
		}
		else {
			barGfx[2 + (y*2+3)*32] = 17;
			barGfx[3 + (y*2+3)*32] = 18;
			barGfx[2 + (y*2+4)*32] = 19;
			barGfx[3 + (y*2+4)*32] = 20;
		}
	}
}

void renderPlay() {
	renderLifebar();
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
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, 15, SpriteSize_32x32, SpriteColorFormat_16Color, stepGfx[3], i->col, false, false, false, false, false);
					break;
				case (3):
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, 15, SpriteSize_32x32, SpriteColorFormat_16Color, stepGfx[3], i->col, false, false, false, false, false);
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
	int x = 100;
	int y = 150;
	int u;
	int d;
	int c;
	if (combo < 10) {
		oamSet(&oamMain, comboSprite[2], x, y, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[combo], 2, false, false, false, true, false);			
		oamClearSprite(&oamMain, comboSprite[0]);
		oamClearSprite(&oamMain, comboSprite[1]);
	}
	else if (combo < 100) {
		u = combo % 10;
		d = combo / 10;
		oamSet(&oamMain, comboSprite[2], x, y, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[u], 2, false, false, false, true, false);			
		oamSet(&oamMain, comboSprite[1], x - 20, y, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[d], 2, false, false, false, true, false);			
		oamClearSprite(&oamMain, comboSprite[0]);
	}
	else if (combo < 1000) {
		u = (combo % 100) % 10;
		d = (combo % 100) / 10;
		c = combo / 100;
		oamSet(&oamMain, comboSprite[2], x, y, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[u], 2, false, false, false, true, false);			
		oamSet(&oamMain, comboSprite[1], x - 20, y, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[d], 2, false, false, false, true, false);			
		oamSet(&oamMain, comboSprite[0], x - 40, y, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[c], 2, false, false, false, true, false);			
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
	if (judgeFrame == 0) {
		oamClearSprite(&oamMain, judgeSprite[0]);
		oamClearSprite(&oamMain, judgeSprite[1]);
		return;
	}
	if (judgeAnim < 11) {
		if (judgeFrame > 60) {
			size = 256 + (judgeFrame - 60) * 6;
		}
		else if (judgeFrame > 0) {
			size = 256;
		}
	}
	else {
		size = 256;
		switch (judgeFrame) {
			case 69:
				y = y - 10;
				break;
			case 68:
				y = y - 9;
				break;
			case 67:
				y = y - 8;
				break;
			case 66:
				y = y - 7;
				break;
			case 65:
				y = y - 6;
				break;
			case 64:
				y = y - 5;
				break;
			case 63:
				y = y - 4;
				break;
			case 62:
				y = y - 3;
				break;
			case 61:
				y = y - 2;
				break;
			case 60:
				y = y - 1;
				break;
			case 59:
			case 58:
				y = y;
				break;
			case 57:
			case 56:
				y = y + 1;
				break;
			case 55:
			case 54:
				y = y + 2;
				break;
			case 53:
			case 52:
				y = y + 3;
				break;
			case 51:
			case 50:
				y = y + 4;
				break;
			case 49:
			case 48:
			case 47:
				y = y + 5;
				break;
			case 46:
			case 45:
			case 44:
				y = y + 6;
				break;
			default:
				y = y + 7;
				break;
		}
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
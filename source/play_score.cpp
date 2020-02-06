#include <nds.h>
#include <iostream>
#include "parse.h"
#include "play.h"
#include "play_render.h"
#include "play_score.h"
#include <tail.h>
#include <bar.h>
#include <barBot.h>
#include <barTop.h>
u32 msperbeat;
u32 judgesWindow[5];

u16* barTopGfx;
u8 barTopSprite;
u16* barBotGfx;
u8 barBotSprite;

u32 score;
u32 combo;

using namespace std;

u8 segments = 0; //27 total
u16* barGfx;
void ps_setup() {
	combo = 0;
	barTopGfx = oamAllocateGfx(&oamMain, SpriteSize_16x8, SpriteColorFormat_16Color);
	dmaCopy(barTopTiles, barTopGfx, barTopTilesLen);
	dmaCopy(barTopPal, SPRITE_PALETTE + 32, barTopPalLen);
	barTopSprite = popSprite();
	oamSet(&oamMain, barTopSprite, 232, 19, 0, 2, SpriteSize_16x8, SpriteColorFormat_16Color, barTopGfx, 2, false, false, false, false, false);
	
	barBotGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	dmaCopy(barBotTiles, barBotGfx, barBotTilesLen);
	dmaCopy(barBotPal, SPRITE_PALETTE + 32 + 16, barBotPalLen);
	barBotSprite = popSprite();
	oamSet(&oamMain, barBotSprite, 224, 152, 0, 3, SpriteSize_32x32, SpriteColorFormat_16Color, barBotGfx, 2, false, false, false, false, false);

	int id = bgInit(2, BgType_Text8bpp, BgSize_T_256x256, 3, 0);
	dmaCopy(barTiles, bgGetGfxPtr(id), barTilesLen);
	dmaCopy(barPal, BG_PALETTE, barPalLen);
	barGfx = bgGetMapPtr(id);
}

void renderScore() {
	u8 t;
	segments++;
	if (segments > 28) {
		segments = 0;
	}
	for (int y = 0; y < 9; y++) {
		if (y > 8 - ((segments - 1) / 3)) {
			barGfx[29 + (y*2+3)*32] = 1;
			barGfx[30 + (y*2+3)*32] = 2;
			barGfx[29 + (y*2+4)*32] = 3;
			barGfx[30 + (y*2+4)*32] = 4;
		}
		else if ((y == 8 - ((segments - 1) / 3)) && (segments != 0)) {
			t = 2 - (segments+2)%3;
			barGfx[29 + (y*2+3)*32] = 5 + t*4;
			barGfx[30 + (y*2+3)*32] = 6 + t*4;
			barGfx[29 + (y*2+4)*32] = 7 + t*4;
			barGfx[30 + (y*2+4)*32] = 8 + t*4;
		}
		else {
			barGfx[29 + (y*2+3)*32] = 17;
			barGfx[30 + (y*2+3)*32] = 18;
			barGfx[29 + (y*2+4)*32] = 19;
			barGfx[30 + (y*2+4)*32] = 20;
		}
	}
}

void addScore(step* s) {
	combo++;
}

void dropCombo() {
	combo = 0;
}

void updateJudgesWindow() {
	msperbeat = (60000 << BPMFRAC) / bpmf;
	judgesWindow[0] = beatfperiod * MARVELOUSJUDGE / msperbeat;
	judgesWindow[1] = beatfperiod * PERFECTJUDGE / msperbeat;
	judgesWindow[2] = beatfperiod * GREATJUDGE / msperbeat;
	judgesWindow[3] = beatfperiod * GOODJUDGE / msperbeat;
	judgesWindow[4] = beatfperiod * BOOJUDGE / msperbeat;
}
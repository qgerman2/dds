#include <nds.h>
#include <iostream>
#include "parse.h"
#include "play.h"
#include "play_render.h"
#include "play_score.h"
#include <bar0.h>
#include <bar1.h>
#include <bar2.h>
#include <bar3.h>
#include <bar4.h>
#include <tail.h>
u32 msperbeat;
u32 judgesWindow[5];
u16* barGfx[5];

using namespace std;

int life;
u8 barSprite[5];
void ps_setup() {
	for (int i = 0; i < 5; i++) {
		barSprite[i] = popSprite();
		barGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	}
	dmaCopy(bar0Tiles, barGfx[0], bar0TilesLen);
	dmaCopy(bar1Tiles, barGfx[1], bar1TilesLen);
	dmaCopy(bar2Tiles, barGfx[2], bar2TilesLen);
	dmaCopy(bar3Tiles, barGfx[3], bar3TilesLen);
	dmaCopy(bar4Tiles, barGfx[4], bar4TilesLen);
	dmaCopy(bar0Pal, SPRITE_PALETTE + 16 * 2, bar0PalLen);
	dmaCopy(bar1Pal, SPRITE_PALETTE + 16 * 3, bar1PalLen);
	dmaCopy(bar2Pal, SPRITE_PALETTE + 16 * 4, bar2PalLen);
	dmaCopy(bar3Pal, SPRITE_PALETTE + 16 * 5, bar3PalLen);
	dmaCopy(bar4Pal, SPRITE_PALETTE + 16 * 6, bar4PalLen);
	life = 50;
}

int d = 10;
void renderScore() {
	d++;
	if (d > 19) {
		d = 0;
	}
	int i = d / 4;
	int y = 16 + ((4 - i) * 32);
	for (int x = 4; x >= 0; x--) {
		if (x < i) {
			oamSet(&oamMain, barSprite[x], 150, 16 + (x * 32), 0, 2, SpriteSize_32x32, SpriteColorFormat_16Color, barGfx[0], 2, false, false, false, false, false);
		}
		else if (x == i) {
			oamSet(&oamMain, barSprite[x], 150, 16 + (x * 32), 0, (4 - d%4 + 2), SpriteSize_32x32, SpriteColorFormat_16Color, barGfx[4 - d%4], 2, false, false, false, false, false);
		}
		else {
			oamSet(&oamMain, barSprite[x], 150, 16 + (x * 32), 0, 6, SpriteSize_32x32, SpriteColorFormat_16Color, barGfx[4], 2, false, false, false, false, false);
		}
	}
}

void updateJudgesWindow() {
	msperbeat = (60000 << BPMFRAC) / bpmf;
	judgesWindow[0] = beatfperiod * MARVELOUSJUDGE / msperbeat;
	judgesWindow[1] = beatfperiod * PERFECTJUDGE / msperbeat;
	judgesWindow[2] = beatfperiod * GREATJUDGE / msperbeat;
	judgesWindow[3] = beatfperiod * GOODJUDGE / msperbeat;
	judgesWindow[4] = beatfperiod * BOOJUDGE / msperbeat;
}
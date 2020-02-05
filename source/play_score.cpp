#include <nds.h>
#include <iostream>
#include "parse.h"
#include "play.h"
#include "play_render.h"
#include "play_score.h"
#include <bar1.h>
#include <bar2.h>
#include <bar3.h>
#include <bar4.h>
#include <bar5.h>
#include <tail.h>
u32 msperbeat;
u32 judgesWindow[5];
u16* barGfx[5];

using namespace std;

int life;
u8 barSprite;
void ps_setup() {
	barSprite = popSprite();
	for (int i = 0; i < 5; i++) {
		barGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	}
	dmaCopy(bar1Tiles, barGfx[0], bar1TilesLen);
	dmaCopy(bar2Tiles, barGfx[1], bar2TilesLen);
	dmaCopy(bar3Tiles, barGfx[2], bar3TilesLen);
	dmaCopy(bar4Tiles, barGfx[3], bar4TilesLen);
	dmaCopy(bar5Tiles, barGfx[4], bar5TilesLen);
	dmaCopy(bar1Pal, SPRITE_PALETTE + 16 * 2, bar1PalLen);
	dmaCopy(bar2Pal, SPRITE_PALETTE + 16 * 3, bar2PalLen);
	dmaCopy(bar3Pal, SPRITE_PALETTE + 16 * 4, bar3PalLen);
	dmaCopy(bar4Pal, SPRITE_PALETTE + 16 * 5, bar4PalLen);
	dmaCopy(bar5Pal, SPRITE_PALETTE + 16 * 6, bar5PalLen);
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
	cout << "\n" << d << " " << i << " " << d%4;
	oamSet(&oamMain, barSprite, 150, y, 0, i, SpriteSize_32x32, SpriteColorFormat_16Color, barGfx[d%4 + 1], 2, false, false, false, false, false);
}

void updateJudgesWindow() {
	msperbeat = (60000 << BPMFRAC) / bpmf;
	judgesWindow[0] = beatfperiod * MARVELOUSJUDGE / msperbeat;
	judgesWindow[1] = beatfperiod * PERFECTJUDGE / msperbeat;
	judgesWindow[2] = beatfperiod * GREATJUDGE / msperbeat;
	judgesWindow[3] = beatfperiod * GOODJUDGE / msperbeat;
	judgesWindow[4] = beatfperiod * BOOJUDGE / msperbeat;
}
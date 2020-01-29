#include <nds.h>
#include <iostream>
#include <vector>
#include <tap.h>
#include <tail.h>
#include <hold.h>
#include "parse.h"
#include "play.h"
#include "play_render.h"

#define sprites ((spriteEntry*) OAM)
#define tiles_tap 0
#define pal_tap 0
#define tiles_hold 10
#define pal_hold 10
#define tiles_tail 15
#define pal_tail 15

//macros para acceso a memoria
#define tile2objram(t) (SPRITE_GFX + (t) * 16)
#define pal2objram(p) (SPRITE_PALETTE + (p) * 16)

bool freesprites[127];

using namespace std;

void pr_setup() {
	setRotData();
	for (int i = 0; i < 127; i++) {
		pushSprite(i);
	}
	dmaCopyHalfWords(3, tapTiles, tile2objram(tiles_tap), tapTilesLen);
	dmaCopyHalfWords(3, tapPal, pal2objram(pal_tap), tapPalLen);
	cout << "\npal len: " <<tapPalLen << "\ntiles len: " << tapTilesLen << "\n";
	dmaCopyHalfWords(3, tailTiles, tile2objram(tiles_tail), tailTilesLen);
	dmaCopyHalfWords(3, tailPal, pal2objram(pal_tail), tailPalLen);
	dmaCopyHalfWords(3, tailTiles, tile2objram(tiles_hold), holdTilesLen);
	dmaCopyHalfWords(3, tailPal, pal2objram(pal_hold), holdPalLen);
}

void renderSteps(vector<step> steps) {
	for (auto i = steps.begin(); i != steps.end(); i++) {
		if (i->y < 160) {
			sprites[i->sprite].attr0 = i->y | ATTR0_ROTSCALE_DOUBLE;
			switch (i->type) {
				case (1):
				case (2):
					sprites[i->sprite].attr1 = i->x | ATTR1_SIZE_32 | ATTR1_ROTDATA(i->col);
					sprites[i->sprite].attr2 = tiles_tap + (pal_tap << 12);
					break;
				case (3):
					sprites[i->sprite].attr1 = i->x | ATTR1_SIZE_32 | ATTR1_ROTDATA(2);
					sprites[i->sprite].attr2 = tiles_tail + (pal_tail << 12);
					break;
			}
		} else {
			sprites[i->sprite].attr0 = ATTR0_DISABLED;
		}
	}
}

void renderHolds(vector<hold> holds) {
	sprites[127].attr0 = 100 | ATTR0_ROTSCALE_DOUBLE;
	sprites[127].attr1 = 50 | ATTR1_SIZE_32 | ATTR1_ROTDATA(2);
	sprites[127].attr2 = tiles_hold + (pal_hold << 12);
}

u8 popSprite() {
	for (u8 i = 0; i < 127; i++) {
		if (freesprites[i]) {
			freesprites[i] = FALSE;
			return i;
		}
	}
	sassert(0, "out of sprites");
	return 0;
}

void pushSprite(u8 i) {
	freesprites[i] = TRUE;
	sprites[i].attr0 = ATTR0_DISABLED;
}

void setRotData() {
	s16 s;
	s16 c;
	u16* affine;
	//left
	s = sinLerp(degreesToAngle(270)) >> 4;
	c = cosLerp(degreesToAngle(270)) >> 4;
	affine = OAM + 3;
	affine[0] = c;
	affine[4] = s;
	affine[8] = -s;
	affine[12] = c;
	//up
	s = sinLerp(degreesToAngle(180)) >> 4;
	c = cosLerp(degreesToAngle(180)) >> 4;
	affine = OAM + 16 + 3;
	affine[0] = c;
	affine[4] = s;
	affine[8] = -s;
	affine[12] = c;
	//down
	s = sinLerp(degreesToAngle(0)) >> 4;
	c = cosLerp(degreesToAngle(0)) >> 4;
	affine = OAM + 32 + 3;
	affine[0] = c;
	affine[4] = s;
	affine[8] = -s;
	affine[12] = c;
	//right
	s = sinLerp(degreesToAngle(90)) >> 4;
	c = cosLerp(degreesToAngle(90)) >> 4;
	affine = OAM + 48 + 3;
	affine[0] = c;
	affine[4] = s;
	affine[8] = -s;
	affine[12] = c;
}
#include <nds.h>
#include <fat.h>
#include <iostream>
#include <string>
#include "main.h"
#include "render.h"
#include "menu_wheel.h"
#include "sound.h"
#include <font.h>
#include <song_frame.h>
#include <group_frame.h>
#include <song_font.h>
#include <dif_frame.h>
<<<<<<< HEAD
#include <dif_arrow.h>
=======
#include <dif_font.h>
>>>>>>> ad4ef25ce7c46f6ec0fe12f208a6dafd3de94f12

using namespace std;

const int* wheelTiles[] {
	(const int[]){
		16, 17, 18, 19, 20, 21, 22,
		41, 42, 43, 44, 45,
		67, 68, -1
	},
	(const int[]){
		3, 4, 5, 6, 7, 8,
		25, 26, 27, 28, 29, 30, 31, 32, 33,
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
		71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
		93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
		116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136,
		144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
		171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183,
		197, 198, 199, 200, 201, 202, 203, 204, 205, 206,
		225, 225, 226, 227, 228, 229,
		251, 252, -1,
	},
	(const int[]){
		254, 255, 256, 257, 258, 259,
		277, 278, 279, 280, 281, 282, 283, 284, 285, 286,
		300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312,
		323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339,
		345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366,
		370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390,
		395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413,
		424, 425, 426, 427, 428, 429, 430, 431, 432, 433, 434, 435, 436,
		454, 455, 456, 457, 458, 459, -1
	},
	(const int[]){
		460, 461, 462, 463, 464,
		483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493,
		506, 507, 508, 509, 510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523,
		529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543, 544, 545, 546, 547, 548, 549, 550, 551,
		552, 553, 554, 555, 556, 557, 558, 559, 560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571, 572, 573, 574,
		575, 576, 577, 578, 579, 580, 581, 582, 583, 584, 585, 586, 587, 588, 589, 590, 591, 592, 593, 594, 595, 596, 597,
		598, 599, 600, 601, 602, 603, 604, 605, 606, 607, 608, 609, 610, 611, 612, 613, 614, 615, 616, 617, 618, 619, 620,
		637, 638, 639, 640, 641, 642, 643, -1
	},
	(const int[]){
		644, 645, 646, 647, 648, 649, 650, 651, 652, 653, 654, 655, 656, 657, 658, 659, 660,
		667, 668, 669, 670, 671, 672, 673, 674, 675, 676, 677, 678, 679, 680, 681, 682, 683, 684, 685, 686, 687, 688, 689,
		690, 691, 692, 693, 694, 695, 696, 697, 698, 699, 700, 701, 702, 703, 704, 705, 706, 707, 708, 709, 710, 711, 712, -1
	}
};

MenuWheel::MenuWheel() {
	if (shared_buffer) {
		buffer = shared_buffer;
	} else {
		buffer = new Buffer();
		buffer->fill();
	}
	loadSongFontGfx();
	int gfx = 0;
	for (int i = buffer->cursor - WHEELVIEWCHAR / 2; i <= buffer->cursor + WHEELVIEWCHAR / 2; i++) {
		printToBitmap(&songFontGfx[gfx * CHARSPRITES], CHARSPRITES, 8, buffer->items[i].name + ' ');
		gfx++;
	}
	loadFrameBg();
	updateFrameBg();
<<<<<<< HEAD
	loadDif();
=======
	for (int i = 0; i < 4; i++) {
		difSprite[i] = popSpriteSub();
		difGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_Bmp);
		dmaFillHalfWords(ARGB16(1, 31, 15, i * 6), difGfx[i], 128 * 64);
		oamSet(&oamSub, difSprite[i], (i % 2) * 64, (i / 2) * 64, 0, 15, SpriteSize_64x64, SpriteColorFormat_Bmp, difGfx[i], 0, false, false, false, false, false);	
	}
	printToBitmap(&difGfx[0], 2, 0, "02 Easy");
	printToBitmap(&difGfx[0], 2, 16, "07 Beginner");
	printToBitmap(&difGfx[2], 2, 0, "13 Normal");
	printToBitmap(&difGfx[2], 2, 16, "22 Challenge");

	difFrameSprite = popSpriteSub();
	difFrameGfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	dmaCopy(dif_frameTiles, difFrameGfx, dif_frameTilesLen);
	//dmaCopy(dif_framePal, SPRITE_PALETTE_SUB, dif_framePalLen);
	oamRotateScale(&oamSub, 1, 0, 1 << 7, 1 << 7);
	oamSet(&oamSub, difFrameSprite, 32 - 8, 32 - 8, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color, difFrameGfx, 1, true, false, false, false, false);
>>>>>>> ad4ef25ce7c46f6ec0fe12f208a6dafd3de94f12
}

MenuWheel::~MenuWheel() {
	for (int i = 0; i < CHARSPRITES * WHEELVIEWCHAR; i++) {
		oamFreeGfx(&oamSub, songFontGfx[i]);
	}
	delete buffer;
	if (shared_buffer) {shared_buffer = NULL;}
}

void MenuWheel::loadSongFontGfx() {
	for (int i = 0; i < CHARSPRITES * WHEELVIEWCHAR; i++) {
		songFontSprite[i] = popSpriteSub();
	}
	for (int i = 0; i < CHARSPRITES * WHEELVIEWCHAR; i++) {
		songFontGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_64x32, SpriteColorFormat_Bmp);
	}
}

void MenuWheel::loadFrameBg() {
	bg1 = bgInitSub(2, BgType_ExRotation, BgSize_ER_256x256, 3, 4);
	bg2 = bgInitSub(3, BgType_ExRotation, BgSize_ER_256x256, 4, 4);
	bgSetPriority(bg1, 2);
	bgSetPriority(bg2, 2);
	int g = 1;
	//rojo negro
	for (int i = 0; i < 5; i++) {
		int t = 0;
		while (wheelTiles[i][t] != -1) {
			dmaCopy(song_frameTiles + 16 * wheelTiles[i][t], bgGetGfxPtr(bg1) + 32 * g, 64);
			t++;
			g++;
		}
		tilesLen[i] = t;
		tilesTotalLen += t;
	}
	//verde
	for (int i = 0; i < 5; i++) {
		int t = 0;
		while (wheelTiles[i][t] != -1) {
			dmaCopy(group_frameTiles + 16 * wheelTiles[i][t], bgGetGfxPtr(bg1) + 32 * g, 64);
			t++;
			g++;
		}
	}
	dmaCopy(song_framePal, &VRAM_H_EXT_PALETTE[2][0], song_framePalLen);
	dmaCopy(song_framePal, &VRAM_H_EXT_PALETTE[2][1], song_framePalLen);
	dmaCopy(group_framePal, &VRAM_H_EXT_PALETTE[2][2], group_framePalLen);
	dmaCopy(song_framePal, &VRAM_H_EXT_PALETTE[3][0], song_framePalLen);
	dmaCopy(song_framePal, &VRAM_H_EXT_PALETTE[3][1], song_framePalLen);
	dmaCopy(group_framePal, &VRAM_H_EXT_PALETTE[3][2], group_framePalLen);
}

void MenuWheel::loadDif() {
	for (int i = 0; i < 2; i++) {
		difArrowSprite[i] = popSpriteSub();
	}
	difArrowGfx = oamAllocateGfx(&oamSub, SpriteSize_8x8, SpriteColorFormat_16Color);
	dmaCopy(dif_arrowTiles, difArrowGfx, dif_arrowTilesLen);
	dmaCopy(dif_arrowPal, SPRITE_PALETTE_SUB + 16, dif_arrowPalLen);
	oamRotateScale(&oamSub, 1, 0, 1 << 8, -1 << 8);
	for (int i = 0; i < 4; i++) {
		difSprite[i] = popSpriteSub();
		difGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_Bmp);
	}
	difFrameSprite = popSpriteSub();
	difFrameGfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	dmaCopy(dif_frameTiles, difFrameGfx, dif_frameTilesLen);
	dmaCopy(dif_framePal, SPRITE_PALETTE_SUB, dif_framePalLen);
	oamRotateScale(&oamSub, 2, 0, 1 << 7, 1 << 7);
}

void MenuWheel::next() {
	buffer->cursor++;
	if ((buffer->cursor - BUFFERSIZE / 2) >= (buffer->size - WHEELVIEW / 2)) {
		buffer->cursor -= buffer->size;
	}
	else if (buffer->cursor >= (BUFFERSIZE - WHEELVIEW / 2 - 1)) {
		buffer->center = buffer->center + (BUFFERSIZE / 2 - WHEELVIEW / 2);
		if (buffer->center >= buffer->size) {
			buffer->center -= buffer->size;
		}
		buffer->cursor = BUFFERSIZE / 2;
		//mover buffer->items
		for (int y = 0; y <= BUFFERSIZE / 2 + WHEELVIEW / 2; y++) {
			buffer->items[y] = buffer->items[y + BUFFERSIZE - (BUFFERSIZE / 2 + WHEELVIEW / 2) - 1];
		}
		for (int i = buffer->cursor + WHEELVIEW / 2 + 1; i < BUFFERSIZE; i++) {
			buffer->items[i].type = -1;
		}
		buffer->fill();
	}
	//mover texto
	u16* tempFontGfx[CHARSPRITES];
	for (int i = 0; i < CHARSPRITES; i++) {
		tempFontGfx[i] = songFontGfx[i];
	}
	for (int y = 0; y < WHEELVIEWCHAR - 1; y++) {
		for (int i = 0; i < CHARSPRITES; i++) {
			songFontGfx[y * CHARSPRITES + i] = songFontGfx[(y + 1) * CHARSPRITES + i];
		}
	}
	for (int i = 0; i < CHARSPRITES; i++) {
		songFontGfx[(WHEELVIEWCHAR - 1) * CHARSPRITES + i] = tempFontGfx[i];
	}
	printToBitmap(&songFontGfx[(WHEELVIEWCHAR - 1) * CHARSPRITES], 3, 8, buffer->items[buffer->cursor + WHEELVIEWCHAR / 2].name + ' ');
	updateFrameBg();
}

void MenuWheel::prev() {
	buffer->cursor--;
	if ((buffer->cursor - BUFFERSIZE / 2) <= (WHEELVIEW / 2 - buffer->size)) {
		buffer->cursor += buffer->size;
	}
	else if (buffer->cursor <= (WHEELVIEW / 2)) {
		buffer->center = buffer->center - (BUFFERSIZE / 2 - WHEELVIEW / 2);
		if (buffer->center < 0) {
			buffer->center += buffer->size;
		}
		buffer->cursor = BUFFERSIZE / 2;
		//mover buffer->items
		for (int i = BUFFERSIZE - 1; i >= BUFFERSIZE / 2 - WHEELVIEW / 2; i--) {
			buffer->items[i] = buffer->items[i - (BUFFERSIZE - (BUFFERSIZE / 2 + WHEELVIEW / 2)) + 1];
		}
		for (int i = 0; i <= (buffer->cursor - WHEELVIEW / 2 - 1); i++) {
			buffer->items[i].type = -1;
		}
		buffer->fill();
	}
	//mover texto
	u16* tempFontGfx[CHARSPRITES];
	for (int i = 0; i < CHARSPRITES; i++) {
		tempFontGfx[i] = songFontGfx[(WHEELVIEWCHAR - 1) * CHARSPRITES + i];
	}
	for (int y = WHEELVIEWCHAR - 1; y > 0; y--) {
		for (int i = 0; i < CHARSPRITES; i++) {
			songFontGfx[y * CHARSPRITES + i] = songFontGfx[(y - 1) * CHARSPRITES + i];
		}
	}
	for (int i = 0; i < CHARSPRITES; i++) {
		songFontGfx[i] = tempFontGfx[i];
	}
	printToBitmap(&songFontGfx[0], 3, 8, buffer->items[buffer->cursor - WHEELVIEWCHAR / 2].name + ' ');
	updateFrameBg();
}

void MenuWheel::printToBitmap(u16** gfx, int sprites, int y_offset, string str) {
	int c;
	int x;
	int s;
	for (uint i = 0; i < str.length(); i++) {
		c = int(str[i]) - ASCIIOFFSET;
		x = i * CHARWIDTH;
		s = x / 64;
		if ((x + CHARWIDTH) > (sprites * 64)) {
			break;
		}
		for (int y = 0; y < CHARHEIGHT; y++) {
			if ((x % 64) > (64 - CHARWIDTH)) {
				dmaCopy(song_fontBitmap + CHARWIDTH * y + (CHARWIDTH * CHARHEIGHT) * c + CHAROFFSET, gfx[s] + (y + y_offset) * 64 + i * CHARWIDTH - (s * 64), (((s + 1) * 64) - x) * 2);
				//oh no
				dmaCopy(song_fontBitmap + CHARWIDTH * y + (CHARWIDTH * CHARHEIGHT) * c + CHAROFFSET + (((s + 1) * 64) - x), gfx[s + 1] + (y + y_offset) * 64 + i * CHARWIDTH + (((s + 1) * 64) - x) - ((s + 1) * 64), (CHARWIDTH * 2) - ((((s + 1) * 64) - x) * 2));
			}
			else {
				dmaCopy(song_fontBitmap + CHARWIDTH * y + (CHARWIDTH * CHARHEIGHT) * c + CHAROFFSET, gfx[s] + (y + y_offset) * 64 + i * CHARWIDTH - (s * 64), CHARWIDTH * 2);
			}
		}
	}
}

void MenuWheel::render() {
	int angle = 0;
	if (frame > 22) {
		angle = 12 * (45 - frame) * -anim;
	}
	else if (frame > 0) {
		angle = 12 * (frame) * anim;
	}
	if (frame == 22) {
		if (anim > 0) {
			next();
		}
		else {
			prev();
		}
		simpath = buffer->items[buffer->cursor].smpath;
		songpath = buffer->items[buffer->cursor].path;
		updateDif(&buffer->items[buffer->cursor]);
		stopAudio();
		hideDif();
		//cout << "\n" << songpath;
	}
	bgSet(bg1, angle, 1 << 8, 1 << 8, 515 << 8, 128 << 8, 520 << 8, 96 << 8);
	bgSet(bg2, angle, 1 << 8, 1 << 8, 515 << 8, 128 << 8, 520 << 8, 96 << 8);
	bgUpdate();
	renderChar(angle);
	if (frame > 0) {
		frame--;
		frame--;
	}
}

void MenuWheel::playAnim(int a) {
	anim = a;
	frame = 44;
}

void MenuWheel::renderChar(int angle) {
	int scale = 256;
	int o = ((scale - 256) * 64) / 256;
	for (int i = -3; i <= 3; i++) {
		for (int c = 0; c < CHARSPRITES; c++) {
			int x = (((370 - (63 * c) - (o * (c * 2 + 1) / 2)) * cosLerp((180 + i * -WHEELANGLE) * 32768 / 360 - angle)) >> 12) + 60;
			int y = (((370 - (63 * c) - (o * (c * 2 + 1) / 2)) * sinLerp((180 + i * -WHEELANGLE) * 32768 / 360 - angle)) >> 12) + 32;
			oamSet(&oamSub, songFontSprite[CHARSPRITES * (-i + 3) + c], x - 85, y + 48, 1, 15, SpriteSize_64x32, SpriteColorFormat_Bmp, songFontGfx[CHARSPRITES * (i + 3) + c], i + 3 + 7, false, false, false, false, false);
		}
		oamRotateScale(&oamSub, i + 3 + 7, (i * WHEELANGLE) * 32768 / 360 + angle, (1 << 16) / scale, 256);
	}
}

void MenuWheel::updateColor() {
	int item = 0;
	for (int i = buffer->cursor - WHEELVIEW / 2; i <= buffer->cursor + WHEELVIEW / 2; i++) {
		switch (buffer->items[i].type) {
			case 0:
				songFrameColor[item] = 2;
				break;
			case 1:
				songFrameColor[item] = 1;
				break;
		}
		item++;
	}
}

void MenuWheel::updateFrameBg() {
	updateColor();
	int tileOffset = 1;
	int t = 0;
	u16* bgMap1 = bgGetMapPtr(bg1);
	u16* bgMap2 = bgGetMapPtr(bg2);
	u16* bgMap = bgMap1;
	for (int i = 0; i < 5; i++) {
		while (wheelTiles[i][t] != -1) {
			int pos = wheelTiles[i][t] + (wheelTiles[i][t] / 23 * 9);
			int x = pos % 32;
			int y = pos / 32;
			if (y >= tilesYOffset[i]) {
				y -= tilesYOffset[i];
				int tile1 = (t + tileOffset) | TILE_PALETTE(songFrameColor[i]);
				int tile2 = (t + tileOffset) | TILE_PALETTE(songFrameColor[8 - i]) | TILE_FLIP_V;
				if (songFrameColor[i] == 2) {tile1 += tilesTotalLen;}
				if (songFrameColor[8 - i] == 2) {tile2 += tilesTotalLen;}
				bgMap[y * 32 + x + 9] = tile1;
				bgMap[(31 - y) * 32 + x + 9] = tile2;
			}
			t++;
		}
		if (bgMap == bgMap1) {bgMap = bgMap2;}
		else {bgMap = bgMap1;}
		tileOffset += tilesLen[i];
		t = 0;
	}
}

void MenuWheel::updateDif(bufferitem* item) {
<<<<<<< HEAD
	difSize = item->song.charts.size();
	difCursor = 0;
	difView = 0;
	for (auto i = item->song.charts.begin(); i < item->song.charts.end(); i++) {
		int count = i - item->song.charts.begin();
		if (count > DIFVIEW) {break;}
		printToBitmap(&difGfx[(count / 2) * 2], 2, (count % 2) * 27, i->meter + " " + i->difficulty);
	}
}

void MenuWheel::showDif() {
	updateDif(&buffer->items[buffer->cursor]);
	for (int i = 0; i < 4; i++) {
		oamSet(&oamSub, difSprite[i], difX + 4 + (i % 2) * 64, difY + 16 + (i / 2) * 54, 0, 15, SpriteSize_64x64, SpriteColorFormat_Bmp, difGfx[i], 0, false, false, false, false, false);
	}
	oamSet(&oamSub, difArrowSprite[0], difX + 60, difY + 5, 0, 1, SpriteSize_8x8, SpriteColorFormat_16Color, difArrowGfx, 1, false, false, false, false, false);
	oamSet(&oamSub, difArrowSprite[1], difX + 60, difY + 115, 0, 1, SpriteSize_8x8, SpriteColorFormat_16Color, difArrowGfx, 0, false, false, false, false, false);
	oamSet(&oamSub, difFrameSprite, difX, difY, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color, difFrameGfx, 2, true, false, false, false, false);
}

void MenuWheel::hideDif() {
	for (int i = 0; i < 4; i++) {
		oamClearSprite(&oamSub, difSprite[i]);
	}
	for (int i = 0; i < 2; i++) {
		oamClearSprite(&oamSub, difArrowSprite[i]);
	}
	oamClearSprite(&oamSub, difFrameSprite);
=======
	for (auto i = item->song.charts.begin(); i < item->song.charts.end(); i++) {
		//printBitmap(i - item->song.charts.begin(), i->meter + " " + i->difficulty);
	}
>>>>>>> ad4ef25ce7c46f6ec0fe12f208a6dafd3de94f12
}
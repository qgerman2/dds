#include <iostream>
#include <string>
#include <nds.h>
#include <fat.h>
#include <sys/dir.h>
#include <font.h>
#include <song_frame.h>
#include <group_frame.h>
#include <song_font.h>
#include "render.h"
#include "menu_wheel.h"

using namespace std;

string fileext;
int wheelcursor;
const int wheelview = 7; //amount of files visible at a single time on wheel

int dircount; //internal count over files
int wheelsize; //internal total amount of files
int wheelcount; //internal count of files added to wheel

const int* wheelTiles[] {
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

int wheelTilesLen[4];
int wheelTilesYOffset[4] = {3, 9, 13, 15};

u8 songFontSprite[CHARSPRITES * wheelview];
u16* songFontGfx[CHARSPRITES * wheelview];
u8 songFrameColor[wheelview];
int wheelBg1;
int wheelBg2;

wheelitem wheelitems[wheelview];

void mw_setup() {
	cout << "aver";
	//fillWheel();
	loadSongFontGfx();
	//loadSongFrameGfx();
	loadFrameBg();
	for (int i = 0; i < 7; i++) {
		printToBitmap(i * 3, "aaaaaaaaaaaaaaaaaa");
	}
	updateFrameBg();
}

void loadSongFontGfx() {
	for (int i = 0; i < CHARSPRITES * 7; i++) {
		songFontSprite[i] = popSpriteSub();
	}
	for (int i = 0; i < CHARSPRITES * 7; i++) {
		songFontGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_64x32, SpriteColorFormat_Bmp);
	}
}

void loadFrameBg() {
	wheelBg1 = bgInitSub(2, BgType_ExRotation, BgSize_ER_256x256, 0, 1);
	wheelBg2 = bgInitSub(3, BgType_ExRotation, BgSize_ER_256x256, 1, 1);
	int g = 1;
	//rojo negro
	for (int i = 0; i < 4; i++) {
		int t = 0;
		while (wheelTiles[i][t] != -1) {
			dmaCopy(song_frameTiles + 16 * wheelTiles[i][t], bgGetGfxPtr(wheelBg1) + 32 * g, 64);
			t++;
			g++;
		}
		wheelTilesLen[i] = t;
	}
	//verde
	for (int i = 0; i < 4; i++) {
		int t = 0;
		while (wheelTiles[i][t] != -1) {
			dmaCopy(group_frameTiles + 16 * wheelTiles[i][t], bgGetGfxPtr(wheelBg1) + 32 * g, 64);
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

void wheelNext() {
	wheelcursor++;
	for (int y = 0; y < wheelview - 1; y++) {
		for (int i = 0; i < CHARSPRITES; i++) {
			songFontGfx[y * CHARSPRITES + i] = songFontGfx[(y + 1) * CHARSPRITES + i];
		}
		wheelitems[y] = wheelitems[y + 1];
	}
	wheelitems[wheelview - 1].type = -1;
	fillWheelEmpty();
}

void wheelPrev() {
	wheelcursor--;
	for (int y = wheelview - 1; y > 0; y--) {
		for (int i = 0; i < CHARSPRITES; i++) {
			songFontGfx[y * CHARSPRITES + i] = songFontGfx[(y - 1) * CHARSPRITES + i];
		}
		wheelitems[y] = wheelitems[y - 1];
	}
	wheelitems[0].type = -1;
	fillWheelEmpty();
}

void printToBitmap(u8 gfx, string str) {
	int c;
	int x;
	int s;
	for (uint i = 0; i < str.length(); i++) {
		c = int(str[i]) - ASCIIOFFSET;
		x = i * CHARWIDTH;
		s = x / 64;
		if ((x + CHARWIDTH) > (CHARSPRITES * 64)) {
			break;
		}
		for (int y = 0; y < 16; y++) {
			if ((x % 64) > (64 - CHARWIDTH)) {
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET, songFontGfx[gfx + s] + (y + 8) * 64 + i * CHARWIDTH - (s * 64), (((s + 1) * 64) - x) * 2);
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET + (((s + 1) * 64) - x), songFontGfx[gfx + s + 1] + (y + 8) * 64 + i * CHARWIDTH + (((s + 1) * 64) - x) - ((s + 1) * 64), (CHARWIDTH * 2) - ((((s + 1) * 64) - x) * 2));
			}
			else {
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET, songFontGfx[gfx + s] + (y + 8) * 64 + i * CHARWIDTH - (s * 64), CHARWIDTH * 2);
			}
		}
	}
}

void fillWheel() {
	//encontrar total de elementos
	wheelcursor = -1;
	dircount = -1;
	wheelcount = 0;
	parseDir("/ddr", -1, -1);
	wheelsize = dircount + 1;
	//popular rueda
	wheelcursor = 2;
	dircount = -1;
	wheelcount = 0;
	parseDir("/ddr", -1, -1);
	//llenar espacios que faltan
	fillWheelEmpty();
	for (int i = 0; i < wheelview; i++) {
		cout << "\n" << i << " " << wheelitems[i].name;
		switch (wheelitems[i].type) {
			case 0:
				songFrameColor[i] = 2;
				break;
			case 1:
				songFrameColor[i] = 1;
				break;
		}
	}
}

void fillWheelEmpty() {
	for (int i = 0; i < wheelview; i++) {
		if (wheelitems[i].type == -1) {
			int pos = indexToFile(i);
			dircount = -1;
			parseDir("/ddr", pos, i);
		}
	}
}

int indexToFile(int i) {
	int pos = wheelcursor - 3 + i;
	while (pos >= wheelsize) {
		pos = pos - wheelsize;
	}
	while (pos < 0) {
		pos = pos + wheelsize;
	}
	return pos;
}

int nearWheelCursor(int i) {
	if (abs(wheelcursor - i) <= (wheelview / 2)) {
		return (i - wheelcursor + (wheelview / 2));
	}
	else if (abs(wheelcursor + wheelsize - i) <= (wheelview / 2)) {
		return (i - wheelsize - wheelcursor + (wheelview / 2));
	}
	else if (abs(wheelcursor - wheelsize - i) <= (wheelview / 2)) {
		return (i + wheelsize - wheelcursor + (wheelview / 2));
	}
	return -1;
}

bool parseDir(string dir, int index, int dest) {
	int pos;
	DIR *pdir;
	struct dirent *pent;
	bool isgroup = false;
	pdir = opendir(dir.c_str());
	if (pdir){
		while ((pent = readdir(pdir)) != NULL) {
			fileext = "";
    		if ((strcmp(".", pent->d_name) == 0) || (strcmp("..", pent->d_name) == 0)) {
        		continue;
    		}
    		if (pent->d_type == DT_DIR) {
    			dircount++;
    			isgroup = true;
    			if (((wheelcursor > -1) && (index == -1)) || (index == dircount)) {
    				pos = nearWheelCursor(dircount);
        			if (pos != -1) {
        				wheelitem group;
        				group.type = 0;
        				group.name = pent->d_name;
        				group.path = dir + '/' + pent->d_name;
        				if (dest != -1) {
        					wheelitems[dest] = group;
        				}
        				else {
        					wheelitems[pos] = group;
        				}
        				wheelcount++;
        			}
        		}
    			if (parseDir(dir + '/' + pent->d_name, index, dest) && (index != -1)) {
    				return true;
    			}
    			else if ((wheelcount > wheelview) || (index == dircount)) {
    				return true;
    			}
    		}
    		else if (!isgroup) {
        		for (int i = 0; pent->d_name[i] != '\0'; i++) {
        			fileext += pent->d_name[i];
        			if (pent->d_name[i] == '.') {
        				fileext = "";
        			}
        		}
        		if (fileext == "sm") {
        			if (((wheelcursor > -1) && (index == -1)) || (index == dircount)) {
        				pos = nearWheelCursor(dircount);
	        			if (pos != -1) {
	        				wheelitem* song = &wheelitems[pos];
	        				song->type = 1;
	        				song->smpath = dir + '/' + pent->d_name;
	        				return true;
	        			}
        			}
        		}
    		}
		}
		closedir(pdir);
	}
	return false;
}

int scale = 256;
void renderWheelChar() {
	int o = ((scale - 256) * 64) / 256;
	scale--;
	if (scale <= 128) {
		scale = 256;
	}
	for (int i = -3; i <= 3; i++) {
		for (int c = 0; c < CHARSPRITES; c++) {
			int x = (((477 - (63 * c) - (o * (c * 2 + 1) / 2)) * cosLerp((180 + i * WHEELANGLE) * 32768 / 360)) >> 12) + 60;
			int y = (((477 - (63 * c) - (o * (c * 2 + 1) / 2)) * sinLerp((180 + i * WHEELANGLE) * 32768 / 360)) >> 12) + 32;
			oamSet(&oamSub, songFontSprite[CHARSPRITES * (i + 3) + c], x, y + 32, 0, 15, SpriteSize_64x32, SpriteColorFormat_Bmp, songFontGfx[CHARSPRITES * (i + 3) + c], i + 3 + 7, false, false, false, false, false);
		}
		oamRotateScale(&oamSub, i + 3 + 7, (-i * WHEELANGLE) * 32768 / 360, (1 << 16) / scale, 256);
	}
}

void updateFrameBg() {
	int tileOffset = 1;
	int t = 0;
	u16* bgMap1 = bgGetMapPtr(wheelBg1);
	u16* bgMap2 = bgGetMapPtr(wheelBg2);
	u16* bgMap = bgMap1;
	for (int i = 0; i < 4; i++) {
		while (wheelTiles[i][t] != -1) {
			int pos = wheelTiles[i][t] + (wheelTiles[i][t] / 23 * 9);
			int x = pos % 32;
			int y = pos / 32;
			if (y >= wheelTilesYOffset[i]) {
				y -= wheelTilesYOffset[i];
				bgMap[y * 32 + x] = t + tileOffset;
				bgMap[(31 - y) * 32 + x] = (t + tileOffset) | TILE_FLIP_V;
			}
			t++;
		}
		if (bgMap == bgMap1) {
			bgMap = bgMap2;
		}
		else {
			bgMap = bgMap1;
		}
		tileOffset += wheelTilesLen[i];
		t = 0;
	}
}
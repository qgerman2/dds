#include <vector>
#include <iostream>
#include <string>
#include <nds.h>
#include <fat.h>
#include <string>
#include <sys/dir.h>
#include <unistd.h>
#include <stdio.h>
#include "parse.h"
#include "menu.h"
#include "render.h"
#include <song_frame.h>
#include <song_font.h>
#include <font.h>

using namespace std;

string fileext;
int wheelcursor;
int wheelview = 7; //amount of files visible at a single time on wheel

int dircount; //internal count over files
int wheelsize; //internal total amount of files
int wheelcount; //internal count of files added to wheel

u8 songFrameSprite[21];
u16* songFrameGfx[9];
u8 songFontSprite[CHARSPRITES * 7];
u16* songFontGfx[CHARSPRITES * 7];
u8 songFrameColor[7];

wheelitem wheelitems[7];

void m_setup() {
	for (int i = 0; i < 128; i++) {
		pushSprite(i);
		pushSpriteSub(i);
	}
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
	fillWheel();
	loadSongFontGfx();
	loadSongFrameGfx();
}

void wheelNext() {
	wheelcursor++;
	for (int y = 6; y > 0; y--) {
		for (int i = 0; i < CHARSPRITES; i++) {
			songFontGfx[y * CHARSPRITES + i] = songFontGfx[(y - 1) * CHARSPRITES + i];
		}
	}
}

void wheelPrev() {
	wheelcursor--;
	for (int y = 0; y < 6; y++) {
		for (int i = 0; i < CHARSPRITES; i++) {
			songFontGfx[y * CHARSPRITES + i] = songFontGfx[(y + 1) * CHARSPRITES + i];
		}
	}
}

void loadSongFrameGfx() {
	for (int i = 0; i < 21; i++) {
		songFrameSprite[i] = popSpriteSub();
	}
	for (int x = 0; x < 3; x++) {
		songFrameGfx[x] = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
		for (int y = 0; y < 8; y++) {
			dmaCopy(song_frameTiles + x * 64 + y * 192, songFrameGfx[x] + y * 128, 256);
		}
		songFrameGfx[x + 3] = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
		for (int y = 0; y < 8; y++) {
			dmaCopy(song_frameTiles + 1536 + x * 64 + y * 192, songFrameGfx[x + 3] + y * 128, 256);
		}
		songFrameGfx[x + 6] = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
		for (int y = 0; y < 8; y++) {
			dmaCopy(song_frameTiles + 3072 + x * 64 + y * 192, songFrameGfx[x + 6] + y * 128, 256);
		}
	}
	dmaCopy(song_framePal, SPRITE_PALETTE_SUB + 16 * 15, song_framePalLen);
	for (int i = 0; i < 7; i++) {
		oamRotateScale(&oamSub, i, -((i - 3) * WHEELANGLE) * 32768 / 360, 1 << 8, 1 << 8);
	}
}

void loadSongFontGfx() {
	for (int i = 0; i < CHARSPRITES * 7; i++) {
		songFontSprite[i] = popSpriteSub();
	}
	for (int i = 0; i < CHARSPRITES * 7; i++) {
		songFontGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_64x32, SpriteColorFormat_Bmp);
	}
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
	dircount = 0;
	wheelcount = 0;
	parseDir("/ddr", -1, -1);
	wheelsize = dircount;
	//popular rueda
	wheelcursor = 1;
	dircount = 0;
	wheelcount = 0;
	parseDir("/ddr", -1, -1);
	//terminar de popular rueda si no se lleno
	for (int i = 0; i < wheelview; i++) {
		if (wheelitems[i].type == -1) {
			int pos = wheelcursor - 3 + i;
			while (pos >= wheelsize) {
				pos = pos - wheelsize;
			}
			while (pos < 0) {
				pos = pos + wheelsize;
			}
			dircount = 0;
			parseDir("/ddr", pos, i);
		}
	}
	for (int i = 0; i < wheelview; i++) {
		cout << "\n" << i << " " << wheelitems[i].name;
	}
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
	pdir = opendir(dir.c_str());
	if (pdir){
		while ((pent = readdir(pdir)) != NULL) {
			fileext = "";
    		if ((strcmp(".", pent->d_name) == 0) || (strcmp("..", pent->d_name) == 0)) {
        		continue;
    		}
    		if (pent->d_type == DT_DIR) {
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
        				if ((wheelcount == (wheelview - 1)) || (index != -1)) {
        					return true;
        				}
        			}
        		}
    			dircount++;
    			if (parseDir(dir + '/' + pent->d_name, index, dest)) {
    				return true;
    			}
    		}
    		else {
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
	        				wheelitem song;
	        				song.type = 1;
	        				song.name = pent->d_name;
	        				song.path = dir + '/' + pent->d_name;
	        				if (dest != -1) {
	        					wheelitems[dest] = song;
	        				}
	        				else {
	        					wheelitems[pos] = song;
	        				}
	        				wheelcount++;
	        				if ((wheelcount > wheelview) || (index != -1)) {
	        					return true;
	        				}
	        			}
        			}
        			dircount++;
        		}
    		}
		}
		closedir(pdir);
	}
	return false;
}

void menuLoop() {
	while (1) {
		swiWaitForVBlank();
		renderMenu();
		oamUpdate(&oamSub);
	}
}

void renderMenu() {
	renderWheel();
}

int scale = 256;
void renderWheel() {
	int s = 0;
	int o = ((scale - 256) * 64) / 256;
	scale--;
	if (scale <= 128) {
		scale = 256;
	}
	for (int i = -3; i <= 3; i++) {
		for (int c = 0; c < 3; c++) {
			int x = (((512 - (63 * c)) * cosLerp((180 + i * WHEELANGLE) * 32768 / 360)) >> 12) + 60;
			int y = (((512 - (63 * c)) * sinLerp((180 + i * WHEELANGLE) * 32768 / 360)) >> 12) + 32;
			oamSet(&oamSub, songFrameSprite[s + c], x, y, 0, 15, SpriteSize_64x64, SpriteColorFormat_16Color, songFrameGfx[(songFrameColor[i + 3] * 3) + c], i + 3, true, false, false, false, false);
		}
		s = s + 3;
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
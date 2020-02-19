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

group root;
string fileext;

u8 songFrameSprite[21];
u16* songFrameGfx[9];
u8 songFontSprite[CHARSPRITES * 7];
u16* songFontGfx[CHARSPRITES * 7];

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
	fillGroup("/ddr", &root);
	loadSongFontGfx();
	loadSongFrameGfx();
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
	cout << "\nwena";
	for (int i = 0; i < 7; i++) {
		printToBitmap(i * 4, "wenaaaabbbb");
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

void fillGroup(string dir, group* parent) {
	DIR *pdir;
	struct dirent *pent;
	pdir = opendir(dir.c_str());
	if (pdir){
		group* cur = new group();
		parent->groups.push_back(cur);
		while ((pent = readdir(pdir)) != NULL) {
			fileext = "";
    		if ((strcmp(".", pent->d_name) == 0) || (strcmp("..", pent->d_name) == 0)) {
        		continue;
    		}
    		if (pent->d_type == DT_DIR) {
        		fillGroup(dir + '/' + pent->d_name, cur);
    		}
    		else {
        		for (int i = 0; pent->d_name[i] != '\0'; i++) {
        			fileext += pent->d_name[i];
        			if (pent->d_name[i] == '.') {
        				fileext = "";
        			}
        		}
        		if (fileext == "sm") {
        			cur->songs.push_back(parseSimFile(dir + '/' + pent->d_name));
        			cout << "\nloaded song " << pent->d_name;
        		}
    		}
		}
		if ((cur->songs.size() == 0) && (cur->groups.size() == 0)) {
			parent->groups.pop_back();
			delete cur;
		}
		closedir(pdir);
	}
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
			oamSet(&oamSub, songFrameSprite[s + c], x, y, 0, 15, SpriteSize_64x64, SpriteColorFormat_16Color, songFrameGfx[c], i + 3, true, false, false, false, false);
		}
		s = s + 3;
	}
	for (int i = -3; i <= 3; i++) {
		for (int c = 0; c < CHARSPRITES; c++) {
			int x = (((477 - (63 * c) - (o * (c * 2 + 1) / 2)) * cosLerp((180 + i * WHEELANGLE) * 32768 / 360)) >> 12) + 60;
			int y = (((477 - (63 * c) - (o * (c * 2 + 1) / 2)) * sinLerp((180 + i * WHEELANGLE) * 32768 / 360)) >> 12) + 32;
			oamSet(&oamSub, songFontSprite[CHARSPRITES * (i + 3) + c], x, y + 32, 0, 15, SpriteSize_64x32, SpriteColorFormat_Bmp, songFontGfx[CHARSPRITES * (i + 3) + c], i + 3 + 7, true, false, false, false, false);
		}
		oamRotateScale(&oamSub, i + 3 + 7, (-i * WHEELANGLE) * 32768 / 360, (1 << 16) / scale, 256);
	}
}
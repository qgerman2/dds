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
	//loadSongFrameGfx();
	loadSongFontGfx();
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
	int s = 0;
	int a = 5;
	for (int i = -3; i <= 3; i++) {
		int x1 = (((512) * cosLerp((180 + i * a) * 32768 / 360)) >> 12) + 60;
		int x2 = (((449) * cosLerp((180 + i * a) * 32768 / 360)) >> 12) + 60;
		int x3 = (((386) * cosLerp((180 + i * a) * 32768 / 360)) >> 12) + 60;
		int y1 = (((512) * sinLerp((180 + i * a) * 32768 / 360)) >> 12) + 32;
		int y2 = (((449) * sinLerp((180 + i * a) * 32768 / 360)) >> 12) + 32;
		int y3 = (((386) * sinLerp((180 + i * a) * 32768 / 360)) >> 12) + 32;
		oamSet(&oamSub, songFrameSprite[s], x1, y1, 0, 15, SpriteSize_64x64, SpriteColorFormat_16Color, songFrameGfx[6], i + 3, true, false, false, false, false);
		oamSet(&oamSub, songFrameSprite[s + 1], x2, y2, 0, 15, SpriteSize_64x64, SpriteColorFormat_16Color, songFrameGfx[7], i + 3, true, false, false, false, false);
		oamSet(&oamSub, songFrameSprite[s + 2], x3, y3, 0, 15, SpriteSize_64x64, SpriteColorFormat_16Color, songFrameGfx[8], i + 3, true, false, false, false, false);
		s = s + 3;
	}
	for (int i = 0; i < 7; i++) {
		oamRotateScale(&oamSub, i, -((i - 3) * a) * 32768 / 360, 1 << 8, 1 << 8);
	}
}

void loadSongFontGfx() {
	for (int i = 0; i < 21; i++) {
		songFontSprite[i] = popSpriteSub();
	}
	for (int i = 0; i < 21; i++) {
		songFontGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_64x32, SpriteColorFormat_Bmp);
	}
	printToBitmap(0, "wenaaaabbbbCCctmmm wena ajajjajajajj");
	oamSet(&oamSub, songFontSprite[0], 0, 64, 0, 15, SpriteSize_64x32, SpriteColorFormat_Bmp, songFontGfx[0], 0, false, false, false, false, false);
	oamSet(&oamSub, songFontSprite[1], 64, 64, 0, 15, SpriteSize_64x32, SpriteColorFormat_Bmp, songFontGfx[1], 0, false, false, false, false, false);
	oamSet(&oamSub, songFontSprite[2], 128, 64, 0, 15, SpriteSize_64x32, SpriteColorFormat_Bmp, songFontGfx[2], 0, false, false, false, false, false);
	oamSet(&oamSub, songFontSprite[3], 128 + 64, 64, 0, 15, SpriteSize_64x32, SpriteColorFormat_Bmp, songFontGfx[3], 0, false, false, false, false, false);
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
			/*if (x > 128) {
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET, songFontGfx[2] + y * 64 + i * CHARWIDTH - 128, CHARWIDTH * 2);
			}
			else if ((x + CHARWIDTH) > 128) {
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET, songFontGfx[1] + y * 64 + i * CHARWIDTH, (128 - x) * 2);
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET + (128 - x), songFontGfx[2] + y * 64 + i * CHARWIDTH + (128 - x) - 128, (CHARWIDTH * 2) - ((128 - x) * 2));
			}
			else if (x > 64) {
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET, songFontGfx[1] + y * 64 + i * CHARWIDTH - 64, CHARWIDTH * 2);
			}
			else if ((x + CHARWIDTH) > 64) {
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET, songFontGfx[0] + y * 64 + i * CHARWIDTH, (64 - x) * 2);
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET + (64 - x), songFontGfx[1] + y * 64 + i * CHARWIDTH + (64 - x) - 64, (CHARWIDTH * 2) - ((64 - x) * 2));
			}
			else {
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET, songFontGfx[0] + y * 64 + i * CHARWIDTH, CHARWIDTH * 2);
			}*/
			if ((x % 64) > (64 - CHARWIDTH)) {
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET, songFontGfx[s] + y * 64 + i * CHARWIDTH - (s * 64), (((s + 1) * 64) - x) * 2);
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET + (((s + 1) * 64) - x), songFontGfx[s + 1] + y * 64 + i * CHARWIDTH + (((s + 1) * 64) - x) - ((s + 1) * 64), (CHARWIDTH * 2) - ((((s + 1) * 64) - x) * 2));
			}
			else {
				dmaCopy(song_fontBitmap + 16 * y + 256 * c + CHAROFFSET, songFontGfx[s] + y * 64 + i * CHARWIDTH - (s * 64), CHARWIDTH * 2);
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

}
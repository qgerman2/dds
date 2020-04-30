#include <nds.h>
#include "sound.h"
#include <iostream>
#include <maxmod9.h>
#include <string>
#include "render.h"
#include "artwork.h"
#include <song_font.h>

bool sprites[128];
bool spritesSub[128];

int popSprite() {
	for (int i = 0; i < 128; i++) {
		if (sprites[i]) {
			sprites[i] = FALSE;
			return i;
		}
	}
	sassert(0, "out of sprites");
	return 0;
}

void pushSprite(int i) {
	if (i < 0 || i > 128) {
		sassert(0, "attempt to remove out of range sprite");
	}
	sprites[i] = TRUE;
	oamClearSprite(&oamMain, i);
}

int popSpriteSub() {
	for (int i = 0; i < 128; i++) {
		if (spritesSub[i]) {
			spritesSub[i] = FALSE;
			return i;
		}
	}
	sassert(0, "out of sprites sub");
	return 0;
}

void pushSpriteSub(int i) {
	if (i < 0 || i > 128) {
		sassert(0, "attempt to remove out of range sprite");
	}
	spritesSub[i] = TRUE;
	oamClearSprite(&oamSub, i);
}

void printToBitmap(u16** gfx, int sprites, int y_offset, std::string str) {
	for (int i = 0; i < sprites; i++) {
		int row = (y_offset + 15) / 32;
		dmaFillHalfWords(0, gfx[i] + row * 64 * 32, 128 * 32);
	}
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

int fadeFrame = -1;
int fadeAnim = 0;
int fadeScreen = 0;

void fadeOut(int screen, bool blocking) {
	fadeFrame = 16;
	fadeAnim = 0;
	fadeScreen = screen;
	setBrightness(fadeScreen, 0);
	if (blocking) {
		while (fadeFrame >= 0) {
			fadeUpdate();
			swiWaitForVBlank();
		}
	}
}

void fadeIn(int screen, bool blocking) {
	fadeFrame = 16;
	fadeAnim = 1;
	fadeScreen = screen;
	setBrightness(fadeScreen, -16);
	if (blocking) {
		while (fadeFrame >= 0) {
			fadeUpdate();
			swiWaitForVBlank();
		}
	}
}

void fadeUpdate() {
	if (fadeFrame >= 0) {
		int brightness;
		if (fadeAnim == 1) {
			brightness = -fadeFrame;
		} else {
			brightness = -16 + fadeFrame;
		}
		setBrightness(fadeScreen, brightness);
		fadeFrame--;
	}
}

void clearBitmapBg(int id) {
	u16* gfx = bgGetGfxPtr(id);
	dmaFillHalfWords(0, gfx, 512 * 192);
}

void darkenBitmapBg(int id, int opacity) {
	u16* gfx = bgGetGfxPtr(id);
	for (int x = 0; x < 256; x++) {
		for (int y = 0; y < 192; y++) {
			u16 p = gfx[x + y * 256];
			u8 b = (p & BLUEMASK) >> 10;
			b = ((b << 10) / (9 << 5) * opacity) >> 5;
			u8 g = (p & GREENMASK) >> 5;
			g = ((g << 10) / (9 << 5) * opacity) >> 5;
			u8 r = p & REDMASK;
			r = ((r << 10) / (9 << 5) * opacity) >> 5;
			gfx[x + y * 256] = ARGB16(1, r, g, b);
		}
	}
}
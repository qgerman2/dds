#include <nds.h>
#include "sound.h"
#include <maxmod9.h>
#include <string>
#include "render.h"
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

void fadeOut(int screen) {
	fade(false, screen);
}

void fadeIn(int screen) {
	fade(true, screen);
}

void fade(bool in, int screen) {
	int frame = 16;
	do {
		int brightness;
		if (in) {brightness = -frame;}
		else {brightness = -16 + frame;}
		setBrightness(screen, brightness);
		if (!idleAudio()) {mmStreamUpdate();}
		swiWaitForVBlank();
		frame--;
	} while (frame > -1);
}
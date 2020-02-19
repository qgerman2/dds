#include <nds.h>

bool sprites[128];
bool spritesSub[128];

u8 popSprite() {
	for (u8 i = 0; i < 128; i++) {
		if (sprites[i]) {
			sprites[i] = FALSE;
			return i;
		}
	}
	sassert(0, "out of sprites");
	return 0;
}

void pushSprite(u8 i) {
	sprites[i] = TRUE;
	oamClearSprite(&oamMain, i);
}

u8 popSpriteSub() {
	for (u8 i = 0; i < 128; i++) {
		if (spritesSub[i]) {
			spritesSub[i] = FALSE;
			return i;
		}
	}
	sassert(0, "out of sprites sub");
	return 0;
}

void pushSpriteSub(u8 i) {
	spritesSub[i] = TRUE;
	oamClearSprite(&oamSub, i);
}
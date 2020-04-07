#include <nds.h>
#include <iostream>

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
		std::cout << "\nattempt to remove " << i;
		while (1) {
			swiWaitForVBlank();
		}
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
	spritesSub[i] = TRUE;
	oamClearSprite(&oamSub, i);
}
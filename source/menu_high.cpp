#include <nds.h>
#include <iostream>
#include <vector>
#include <string>
#include "menu_high.h"
#include "render.h"
#include <score_numbers.h>
#include <score_numbers2.h>
#include <bestlast.h>

using namespace std;

MenuHigh::MenuHigh() {
	for (int i = 0; i < 11; i++) {
		scoreSprite[i] = popSprite();
	}
	for (int i = 0; i < 11; i++) {
		scoreGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_16Color);
		dmaCopy(score_numbersTiles + i * 16, scoreGfx[i], 64);
		dmaCopy(score_numbersTiles + 176 + i * 16, scoreGfx[i] + 32, 64);
	}
	dmaFillHalfWords(ARGB16(1, 13, 13, 13), SPRITE_PALETTE + 1, 16);
	oamSet(&oamMain, scoreSprite[9], 216 - 4 * 16, 128, 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, scoreGfx[10], 0, false, false, false, false, false);			
	oamSet(&oamMain, scoreSprite[10], 216 - 8 * 16, 128, 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, scoreGfx[10], 0, false, false, false, false, false);
	for (int i = 0; i < (3 * 6); i++) {
		pointSprite[i] = popSprite();
	}
	for (int i = 0; i < 10; i++) {
		pointGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_16Color);
		dmaCopy(score_numbers2Tiles + i * 16, pointGfx[i], 64);
		dmaCopy(score_numbers2Tiles + 160 + i * 16, pointGfx[i] + 32, 64);
	}
	dmaFillHalfWords(ARGB16(1, 13, 13, 13), SPRITE_PALETTE + 16 + 1, 16);
	for (int i = 0; i < 2; i++) {
		difSprite[i] = popSprite();
		difGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_Bmp);
		oamSet(&oamMain, difSprite[i], 40 + i * 64, 112, 0, 0, SpriteSize_64x32, SpriteColorFormat_Bmp, difGfx[i], 0, false, false, false, false, false);
	}
	lastBestSprite = popSprite();
	for (int i = 0; i < 2; i++) {
		lastBestGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_16Color);
		dmaCopy(bestlastTiles + i * 128, lastBestGfx[i], 512);
	}
	oamSet(&oamMain, lastBestSprite, 174, 112, 0, 0, SpriteSize_64x32, SpriteColorFormat_16Color, lastBestGfx[1], 0, false, false, false, false, false);
}

MenuHigh::~MenuHigh() {
	for (int i = 0; i < 11; i++) {
		oamFreeGfx(&oamMain, scoreGfx[i]);
	}
	for (int i = 0; i < 10; i++) {
		oamFreeGfx(&oamMain, pointGfx[i]);
	}
	for (int i = 0; i < 2; i++) {
		oamFreeGfx(&oamMain, difGfx[i]);
		oamFreeGfx(&oamMain, lastBestGfx[i]);
	}

}

void MenuHigh::render() {
	frame++;
	enum ANIM {FADEIN, IDLE, FADEOUT};
	switch (anim) {
		case FADEIN: {
			if (frame >= 16) {
				anim = IDLE;
				frame = 0;
			}
		}
		break;
		case IDLE: {
			if (frame > 200) {
				anim = FADEOUT;
				frame = 0;
			}
		}
		break;
		case FADEOUT: {
			if (frame >= 16) {
				anim = FADEIN;
				frame = 0;
				count++;
				if (count >= items) {
					count = 0;
				}
				draw();
			}
		}
		break;
	}
}

void MenuHigh::update(vector<score_p>* scores) {
	this->scores = scores;
	anim = 0;
	frame = 0;
	items = scores->size() * 2;
	count = 0;
	draw();
}

void MenuHigh::draw() {
	if (scores->size() == 0) {return;}
	if (count >= scores->size() * 2) {return;}
	score_t* score;
	if (count % 2 == 0) {
		score = &scores->at(count / 2).last;
	} else {
		score = &scores->at(count / 2).best;
	}
	oamSetGfx(&oamMain, lastBestSprite, SpriteSize_64x32, SpriteColorFormat_16Color, lastBestGfx[1 - count % 2]);
	int rawscore = 0;
	for (int i = 0; i < 6; i++) {
		rawscore += score->points[i] * worth[i];
	}
	//score
	string n = to_string(rawscore);
	int offset = 9 - n.length();
	int x = 0;
	for (u8 i = 0; i < 9; i++) {
		if (i % 3 == 0) {x++;}
		if (i >= offset) {
			oamSet(&oamMain, scoreSprite[i], 24 + x * 16, 128, 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, scoreGfx[int(n[i - offset]) - 48], 0, false, false, false, false, false);
		}
		else {
			oamSet(&oamMain, scoreSprite[i], 24 + x * 16, 128, 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, scoreGfx[0], 0, false, false, false, false, false);
		}
		x++;
	}
	//points
	static const int pow10[3] = {1, 10, 100};
	for (int i = 0; i < 6; i++) {
		int col = i / 3;
		int row = i % 3;
		for (int x = 0; x < 3; x++) {
			int digit = (score->points[i] / pow10[x]) % 10;
			oamSet(&oamMain, pointSprite[i * 3 + x], 91 + col * 112 + (2 - x) * 8, 145 + row * 16, 0, 1, SpriteSize_16x16, SpriteColorFormat_16Color, pointGfx[digit], 0, false, false, false, false, false);
		}
	}
	//dif
	printToBitmap(&difGfx[0], 2, 0, scores->at(count / 2).dif);
}
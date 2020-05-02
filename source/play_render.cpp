#include <nds.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "globals.h"
#include "parse.h"
#include "play.h"
#include "play_render.h"
#include "play_score.h"
#include "play_input.h"
#include "render.h"

#include <step.h>
#include <tap.h>
#include <tail.h>
#include <hold.h>
#include <hold_tail_side.h>
#include <hold_tail_up.h>
#include <hold_tail_down.h>
#include <receptor.h>
#include <pulse.h>
#include <numbers.h>
#include <font.h>
#include <mine.h>

#include <marvelous.h>
#include <perfect.h>
#include <great.h>
#include <good.h>
#include <boo.h>
#include <miss.h>

#include <sub_bg.h>
#include <score_numbers.h>
#include <score_numbers2.h>

using namespace std;

const u16* judgePal[6] = {
	marvelousPal,
	perfectPal,
	greatPal,
	goodPal,
	booPal,
	missPal,
};
const u8 notetypePal[9] = {8, 9, 10, 11, 12, 13, 14, 15, 15};

PlayRender::PlayRender(Play* play) {
	this->play = play;
	prevscore = 0;

	tapGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	tailGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);

	dmaCopy(tapTiles, tapGfx, tapTilesLen);
	dmaCopy(tailTiles, tailGfx, tailTilesLen);
	dmaCopy(tapPal, SPRITE_PALETTE, tapPalLen);
	
	loadHoldGfx();
	loadReceptorGfx();
	loadStepGfx();
	loadNumberGfx();
	loadJudgmentGfx();
	loadPulseGfx();

	loadSubBackground();
	loadSubScore();

	REG_BLDCNT = BLEND_ALPHA | BLEND_DST_BG2;
	REG_BLDALPHA = 12 | 4 << 8;
}

PlayRender::~PlayRender() {
	for (int i = 0; i < 8; i++) {
		oamFreeGfx(&oamMain, stepGfx[i]);
	}
	oamFreeGfx(&oamMain, tapGfx);
	oamFreeGfx(&oamMain, tailGfx);
	oamFreeGfx(&oamMain, holdGfx);
	oamFreeGfx(&oamMain, holdSideGfx);
	oamFreeGfx(&oamMain, holdUpGfx);
	oamFreeGfx(&oamMain, holdDownGfx);
	oamFreeGfx(&oamMain, receptorGfx);
	oamFreeGfx(&oamMain, pulseGfx);
	for (int i = 0; i < 10; i++) {
		oamFreeGfx(&oamMain, numberGfx[i]);
	}
	for (int i = 0; i < 24; i++) {
		oamFreeGfx(&oamMain, judgeGfx[i]);
	}
	for (int i = 0; i < 11; i++) {
		oamFreeGfx(&oamSub, scoreGfx[i]);
	}
	for (int i = 0; i < 10; i++) {
		oamFreeGfx(&oamSub, pointGfx[i]);
	}
}

void PlayRender::loadHoldGfx() {
	for (int i = 0; i < 4; i++) {
		holdTopSprite[i] = popSprite();
		holdTop[i] = -1;
	}
	holdGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
	holdSideGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
	holdUpGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	holdDownGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	dmaCopyHalfWords(3, holdBitmap, holdGfx, holdBitmapLen);
	dmaCopy(hold_tail_sideTiles, holdSideGfx, hold_tail_sideTilesLen);
	dmaCopy(hold_tail_upTiles, holdUpGfx, hold_tail_upTilesLen);
	dmaCopy(hold_tail_downTiles, holdDownGfx, hold_tail_downTilesLen);
	dmaCopy(hold_tail_sidePal, SPRITE_PALETTE + 64, hold_tail_sidePalLen);
	dmaCopy(hold_tail_upPal, SPRITE_PALETTE + 80, hold_tail_upPalLen);
	dmaCopy(hold_tail_downPal, SPRITE_PALETTE + 96, hold_tail_downPalLen);
	oamRotateScale(&oamMain, 1, 0, -1 << 8, 1 << 8);
}

void PlayRender::loadReceptorGfx() {
	receptorGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	dmaCopy(receptorTiles, receptorGfx, receptorTilesLen);
	dmaCopy(receptorPal, SPRITE_PALETTE + 16, receptorPalLen);
	for (int i = 0; i < 4; i++) {
		receptorSprite[i] = popSprite();
		oamSet(&oamMain, receptorSprite[i], HITXOFFSET + i * 32, HITYOFFSET, 2, 1, SpriteSize_32x32, SpriteColorFormat_16Color, receptorGfx, 20 + i, false, false, false, false, false);
		oamMain.oamMemory[receptorSprite[i]].attribute[0] |= ATTR0_TYPE_BLENDED;
	}	
}

void PlayRender::loadStepGfx() {
	int colors[7][3] = {{0,0,31},{31,0,0},{0,31,0},{0,31,31},{31,0,15},{0,17,31},{24,8,24}};
	int output[3];
	u16 gray;
	for (int i = 0; i < 8; i++) {
		stepGfx[i] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
		dmaCopy(stepTiles, stepGfx[i], 128);
		dmaCopy(stepTiles + i * 32 + 256, stepGfx[i] + 64, 128);
		dmaCopy(stepTiles + i * 32 + 512, stepGfx[i] + 128, 128);
		dmaCopy(stepTiles + i * 32 + 768, stepGfx[i] + 192, 128);
	}
	for (int i = 0; i < 7; i++) {
		for (int g = 1; g < 16; g++) {
			gray = stepPal[g] & 31;
			output[0] = colors[i][0];
			output[1] = colors[i][1];
			output[2] = colors[i][2];
			for (int c = 0; c < 3; c++) {
				if (colors[i][c] != 0) {
					if (gray > 15) {
						output[c] = colors[i][c] + (((31 - colors[i][c]) * (gray - 15)) / 16);
					}
					else if (gray < 15) {
						output[c] = colors[i][c] * gray / 15;
					}
				}
				else {
					if (gray > 15) {
						output[c] = (gray - 15) * 2 - 1;
					}
				}
			}
			SPRITE_PALETTE[128 + i * 16] = 0;
			SPRITE_PALETTE[128 + i * 16 + g] = 1024 * output[0] + 32 * output[1] + output[2];
		}
	}
	dmaCopy(stepPal, SPRITE_PALETTE + 16 * 15, stepPalLen);
	oamRotateScale(&oamMain, 20, degreesToAngle(90), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 21, degreesToAngle(180), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 22, degreesToAngle(0), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 23, degreesToAngle(270), intToFixed(1, 8), intToFixed(1, 8));
	mineGfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	dmaCopy(mineTiles, mineGfx, mineTilesLen);
	dmaCopy(minePal, SPRITE_PALETTE + 112, minePalLen);
}

void PlayRender::loadNumberGfx() {
	for (int i = 0; i < 3; i++) {
		comboSprite[i] = popSprite();
	}
	for (int n = 0; n < 10; n++) {
		int k = n / 4;
		int p = n % 4;
		numberGfx[n] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
		dmaCopy(numbersTiles + (k * 512) + (p * 32), numberGfx[n], 128);
		dmaCopy(numbersTiles + (k * 512) + (p * 32) + 128, numberGfx[n] + 64, 128);
		dmaCopy(numbersTiles + (k * 512) + (p * 32) + 256, numberGfx[n] + 128, 128);
		dmaCopy(numbersTiles + (k * 512) + (p * 32) + 384, numberGfx[n] + 192, 128);
	}
	dmaCopy(numbersPal, SPRITE_PALETTE + 32, numbersPalLen);
}

void PlayRender::loadJudgmentGfx() {
	for (int i = 0; i < 2; i++) {
		judgeSprite[i] = popSprite();
	}
	const uint* tiles[6] {
		marvelousTiles,
		perfectTiles,
		greatTiles,
		goodTiles,
		booTiles,
		missTiles,
	};
	for (int i = 0; i < 6; i++) {
		int g1 = i * 4;
		int g2 = g1 + 1;
		int g3 = g2 + 1;
		int g4 = g3 + 1;
		judgeGfx[g1] = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_16Color);
		judgeGfx[g2] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
		judgeGfx[g3] = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_16Color);
		judgeGfx[g4] = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
		dmaCopy(tiles[i], judgeGfx[g1], 256);
		dmaCopy(tiles[i] + 192, judgeGfx[g1] + 128, 256);
		dmaCopy(tiles[i] + 192 * 2, judgeGfx[g1] + 256, 256);
		dmaCopy(tiles[i] + 192 * 3, judgeGfx[g1] + 384, 256);
		dmaCopy(tiles[i] + 64, judgeGfx[g2], 128);
		dmaCopy(tiles[i] + 64 + 192, judgeGfx[g2] + 64, 128);
		dmaCopy(tiles[i] + 64 + 192 * 2, judgeGfx[g2] + 128, 128);
		dmaCopy(tiles[i] + 64 + 192 * 3, judgeGfx[g2] + 192, 128);
		dmaCopy(tiles[i] + 96, judgeGfx[g3], 256);
		dmaCopy(tiles[i] + 96 + 192, judgeGfx[g3] + 128, 256);
		dmaCopy(tiles[i] + 96 + 192 * 2, judgeGfx[g3] + 256, 256);
		dmaCopy(tiles[i] + 96 + 192 * 3, judgeGfx[g3] + 384, 256);
		dmaCopy(tiles[i] + 160, judgeGfx[g4], 128);
		dmaCopy(tiles[i] + 160 + 192, judgeGfx[g4] + 64, 128);
		dmaCopy(tiles[i] + 160 + 192 * 2, judgeGfx[g4] + 128, 128);
		dmaCopy(tiles[i] + 160 + 192 * 3, judgeGfx[g4] + 192, 128);
	}
}

void PlayRender::loadPulseGfx() {
	pulseGfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_Bmp);
	dmaCopy(pulseBitmap, pulseGfx, pulseBitmapLen);
	for (int i = 0; i < 4; i++) {
		pulseSprite[i] = popSprite();
		pulseFrame[i] = 0;
		oamSet(&oamMain, pulseSprite[i], (HITXOFFSET - 16) + 32 * i, HITYOFFSET - 16, 1, 7, SpriteSize_64x64, SpriteColorFormat_Bmp, pulseGfx, i + 24, false, false, false, false, false);
	}
	oamRotateScale(&oamMain, 24, degreesToAngle(90), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 25, degreesToAngle(180), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 26, degreesToAngle(0), intToFixed(1, 8), intToFixed(1, 8));
	oamRotateScale(&oamMain, 27, degreesToAngle(270), intToFixed(1, 8), intToFixed(1, 8));
}

void PlayRender::loadSubBackground() {
	int id = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 1, 2);
	bgSetPriority(id, 1);
	dmaCopy(sub_bgTiles, bgGetGfxPtr(id), sub_bgTilesLen);
	dmaCopy(sub_bgMap, bgGetMapPtr(id), sub_bgMapLen);
	dmaCopy(sub_bgPal, &VRAM_H[1*16*256], sub_bgPalLen);
}

void PlayRender::loadSubScore() {
	for (int i = 0; i < 11; i++) {
		scoreSprite[i] = popSpriteSub();
	}
	for (int i = 0; i < 11; i++) {
		scoreGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
		dmaCopy(score_numbersTiles + i * 16, scoreGfx[i], 64);
		dmaCopy(score_numbersTiles + 176 + i * 16, scoreGfx[i] + 32, 64);
	}
	dmaCopy(score_numbersPal, SPRITE_PALETTE_SUB, score_numbersPalLen);
	oamSet(&oamSub, scoreSprite[9], 216 - 4 * 16, 0, 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, scoreGfx[10], 0, false, false, false, false, false);			
	oamSet(&oamSub, scoreSprite[10], 216 - 8 * 16, 0, 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, scoreGfx[10], 0, false, false, false, false, false);
	for (int i = 0; i < (3 * 6); i++) {
		pointSprite[i] = popSpriteSub();
	}
	for (int i = 0; i < 10; i++) {
		pointGfx[i] = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_16Color);
		dmaCopy(score_numbers2Tiles + i * 16, pointGfx[i], 64);
		dmaCopy(score_numbers2Tiles + 160 + i * 16, pointGfx[i] + 32, 64);
	}
	dmaCopy(score_numbersPal, SPRITE_PALETTE_SUB + 16, score_numbersPalLen);
}

void PlayRender::update() {
	renderSteps();
	renderCombo();
	renderJudgment();
	renderSubScore();
	renderPulse();
	renderReceptor();
}

void PlayRender::renderSteps() {
	u8 beatp = (play->beatf % beatfperiod) >> (BPMFRAC + MINUTEFRAC - 2);
	if (play->beat > lastbeat) {
		swapStepGfx = !swapStepGfx;
		lastbeat = play->beat;
	}
	for (int i = 0; i < 4; i++) {
		if (play->input->holdCol[i].first && (play->input->holdCol[i].second->y < (HITYOFFSET + 16))) {
			if (play->input->holdCol[i].second->gfx == NULL) {
				play->input->holdCol[i].second->gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_Bmp);
				dmaCopyHalfWords(3, holdBitmap, play->input->holdCol[i].second->gfx, holdBitmapLen);
			}
			u8 diff = min(HITYOFFSET + 16 - play->input->holdCol[i].second->y, 32);
			dmaFillHalfWords(ARGB16(0,0,0,0), play->input->holdCol[i].second->gfx, 32 * diff * 2);
		}
		if (holdTop[i] != -1) {
			oamSet(&oamMain, holdTopSprite[i], HITXOFFSET + i * 32, HITYOFFSET, 0, notetypePal[holdTop[i] - 1], SpriteSize_32x32, SpriteColorFormat_16Color, stepGfx[0], i + 20, false, false, false, false, false);
		} else {
			oamClearSprite(&oamMain, holdTopSprite[i]);
		}
	}
	for (auto i = play->steps.begin(); i != play->steps.end(); i++) {
		if (i->y < 224) {
			switch (i->type) {
				case (1):
				case (2):
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, notetypePal[i->notetype - 1], SpriteSize_32x32, SpriteColorFormat_16Color, stepGfx[beatp + int(swapStepGfx) * 4], i->col + 20, false, false, false, false, false);
					break;
				case (3):
					switch (i->col) {
						case 0:
						case 3:
							oamSet(&oamMain, i->sprite, i->x - int(i->col == 3), i->y - 1, 0, 4, SpriteSize_32x32, SpriteColorFormat_16Color, holdSideGfx, int(i->col == 3), false, false, false, false, false);
							break;
						case 1:
							oamSet(&oamMain, i->sprite, i->x, i->y - 1, 0, 6, SpriteSize_32x32, SpriteColorFormat_16Color, holdDownGfx, 0, false, false, false, false, false);
							break;
						case 2:
							oamSet(&oamMain, i->sprite, i->x, i->y - 1, 0, 5, SpriteSize_32x32, SpriteColorFormat_16Color, holdUpGfx, 0, false, false, false, false, false);
							break;
					}
					break;
				case (5):
					if (i->gfx != NULL) {
						oamSet(&oamMain, i->sprite, i->x, i->y, 1, 15, SpriteSize_32x32, SpriteColorFormat_Bmp, i->gfx, 2, false, false, false, true, false);
					}
					else {
						oamSet(&oamMain, i->sprite, i->x, i->y, 1, 15, SpriteSize_32x32, SpriteColorFormat_Bmp, holdGfx, 2, false, false, false, true, false);
					}
					break;
				case (6):
					oamSet(&oamMain, i->sprite, i->x, i->y, 0, 7, SpriteSize_32x32, SpriteColorFormat_16Color, mineGfx, i->col + 20, false, false, false, false, false);
					break;
			}
		} else {
			oamClearSprite(&oamMain, i->sprite);
		}
	}
}

void PlayRender::renderCombo() {
	int u;
	int d;
	int c;
	if (play->score->combo < 10) {
		oamSet(&oamMain, comboSprite[2], COMBOX, COMBOY, 2, 2, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[play->score->combo], 2, false, false, false, true, false);			
		oamClearSprite(&oamMain, comboSprite[0]);
		oamClearSprite(&oamMain, comboSprite[1]);
	}
	else if (play->score->combo < 100) {
		u = play->score->combo % 10;
		d = play->score->combo / 10;
		oamSet(&oamMain, comboSprite[2], COMBOX + 10, COMBOY, 2, 2, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[u], 2, false, false, false, true, false);			
		oamSet(&oamMain, comboSprite[1], COMBOX - 10, COMBOY, 2, 2, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[d], 2, false, false, false, true, false);			
		oamClearSprite(&oamMain, comboSprite[0]);
	}
	else if (play->score->combo < 1000) {
		u = (play->score->combo % 100) % 10;
		d = (play->score->combo % 100) / 10;
		c = play->score->combo / 100;
		oamSet(&oamMain, comboSprite[2], COMBOX + 20, COMBOY, 2, 2, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[u], 2, false, false, false, true, false);			
		oamSet(&oamMain, comboSprite[1], COMBOX, COMBOY, 2, 2, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[d], 2, false, false, false, true, false);			
		oamSet(&oamMain, comboSprite[0], COMBOX - 20, COMBOY, 2, 2, SpriteSize_32x32, SpriteColorFormat_16Color, numberGfx[c], 2, false, false, false, true, false);			
	}
}

void PlayRender::renderJudgment() {
	if (judgeFrame == 0) {return;}
	int x = 48;
	int y = 50;
	u8 f = 0;
	u32 size;
	u32 offset;
	judgeFrame--;
	if (judgeFrame == 0) {
		oamClearSprite(&oamMain, judgeSprite[0]);
		oamClearSprite(&oamMain, judgeSprite[1]);
		return;
	}
	if (judgeAnim < 11) {
		if (judgeFrame > 60) {
			size = 256 + (judgeFrame - 60) * 6;
		}
		else if (judgeFrame > 0) {
			size = 256;
		}
	}
	else {
		size = 256;
		switch (judgeFrame) {
			case 69: y = y - 10; break;
			case 68: y = y - 9; break;
			case 67: y = y - 8; break;
			case 66: y = y - 7; break;
			case 65: y = y - 6; break;
			case 64: y = y - 5; break;
			case 63: y = y - 4; break;
			case 62: y = y - 3; break;
			case 61: y = y - 2; break;
			case 60: y = y - 1; break;
			case 59:
			case 58: y = y; break;
			case 57:
			case 56: y = y + 1; break;
			case 55:
			case 54: y = y + 2; break;
			case 53:
			case 52: y = y + 3; break;
			case 51:
			case 50: y = y + 4; break;
			case 49:
			case 48:
			case 47: y = y + 5; break;
			case 46:
			case 45:
			case 44: y = y + 6; break;
			default: y = y + 7; break;
		}
	}
	if (size == 256) {f = 1;}
	oamRotateScale(&oamMain, 5, 0, (1 << 16) / size, (1 << 16) / size);
	offset = ((size - 256) * 32) / 256;
	oamSet(&oamMain, judgeSprite[0], x - offset / 2, y, 0, 3, SpriteSize_64x32, SpriteColorFormat_16Color, judgeGfx[judgeAnim * 2], 5, true, false, false, false, false);
	oamSet(&oamMain, judgeSprite[1], x - f + 81 + offset, y, 0, 3, SpriteSize_32x32, SpriteColorFormat_16Color, judgeGfx[judgeAnim * 2 + 1], 5, true, false, false, false, false);
}

void PlayRender::renderSubScore() {
	//score
	string n;
	if ((prevscore != newscore) && (scoreFrame > 0)) {
		int chunk = (newscore - prevscore) / 10;
		n = to_string(prevscore + chunk * (11 - scoreFrame));
		if (scoreFrame == 1) {
			prevscore = newscore;
		}
	}
	else {
		n = to_string(newscore);
	}
	int offset = 9 - n.length();
	int x = 0;
	for (u8 i = 0; i < 9; i++) {
		if (i % 3 == 0) {x++;}
		if (i >= offset) {
			oamSet(&oamSub, scoreSprite[i], 24 + x * 16, 0, 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, scoreGfx[int(n[i - offset]) - 48], 0, false, false, false, false, false);
		}
		else {
			oamSet(&oamSub, scoreSprite[i], 24 + x * 16, 0, 0, 0, SpriteSize_16x16, SpriteColorFormat_16Color, scoreGfx[0], 0, false, false, false, false, false);
		}
		x++;
	}
	scoreFrame--;
	//points
	static const int pow10[3] = {1, 10, 100};
	for (int i = 0; i < 6; i++) {
		int col = i / 3;
		int row = i % 3;
		for (int x = 0; x < 3; x++) {
			int digit = (play->score->score->points[i] / pow10[x]) % 10;
			oamSet(&oamSub, pointSprite[i * 3 + x], 91 + col * 112 + (2 - x) * 8, 32 + row * 16, 0, 1, SpriteSize_16x16, SpriteColorFormat_16Color, pointGfx[digit], 0, false, false, false, false, false);
		}
	}
}

void PlayRender::renderPulse() {
	for (int i = 0; i < 4; i++) {
		int deg;
		switch (i) {
			case 0: deg = 90; break;
			case 1: deg = 180; break;
			case 2: deg = 0; break;
			case 3: deg = 270; break;
		}
		if (pulseFrame[i] > 0) {
			pulseFrame[i]--;
			int size = 208 + pulseFrame[i] * 6;
			oamRotateScale(&oamMain, i + 24, degreesToAngle(deg), size, size);
			oamSetAlpha(&oamMain, pulseSprite[i], pulseFrame[i] * 2);
		} else {
			oamRotateScale(&oamMain, i + 24, degreesToAngle(deg), 1 << 8, 1 << 8);
			oamSetAlpha(&oamMain, pulseSprite[i], 0);
		}
	}
}

void PlayRender::renderReceptor() {
	u8 beatp = 15 - ((play->beatf % beatfperiod) >> (BPMFRAC + MINUTEFRAC - 4));
	for (int i = 0; i < receptorPalLen / 2; i ++) {
		u8 c = receptorPal[i] & 0b11111;
		u8 r = (c + beatp) / 2;
		if (r > 31) {r = 31;}
		SPRITE_PALETTE[16 + i] = ARGB16(1, r, r, r);
	}
}

void PlayRender::playJudgmentAnim(u8 anim) {
	judgeFrame = 70;
	if (judgeAnim != anim) {
		dmaCopy(judgePal[anim / 2], SPRITE_PALETTE + 48, 32);
		judgeAnim = anim;
	}
}

void PlayRender::playScoreAnim() {
	scoreFrame = 10;
}
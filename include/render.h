#ifndef RENDER
#define RENDER
#define ASCIIOFFSET 32
#define CHARWIDTH 10
#define CHARHEIGHT 16
#define CHAROFFSET 0
#include <nds.h>
#include <string>
extern bool sprites[128];
extern bool spritesSub[128];
int popSprite();
int popSpriteSub();
void pushSprite(int i);
void pushSpriteSub(int i);
void printToBitmap(u16** gfx, int sprites, int y_offset, std::string str);
void fadeIn(int screen);
void fadeOut(int screen);
void fade(bool in, int screen);
#endif
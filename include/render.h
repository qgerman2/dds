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
extern int fadeFrame;
int popSprite();
int popSpriteSub();
void pushSprite(int i);
void pushSpriteSub(int i);
void printToBitmap(u16** gfx, int sprites, int y_offset, std::string str);
void fadeIn(int screen, bool blocking);
void fadeOut(int screen, bool blocking);
bool fadeUpdate();
void clearBitmapBg(int id);
void darkenBitmapBg(int id, int opacity);
#endif
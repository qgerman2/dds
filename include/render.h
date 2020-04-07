#ifndef RENDER
#define RENDER
extern bool sprites[128];
extern bool spritesSub[128];
int popSprite();
int popSpriteSub();
void pushSprite(int i);
void pushSpriteSub(int i);
#endif
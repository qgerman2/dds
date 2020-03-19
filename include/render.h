#ifndef RENDER
#define RENDER
extern bool sprites[128];
extern bool spritesSub[128];
u8 popSprite();
u8 popSpriteSub();
void pushSprite(u8 i);
void pushSpriteSub(u8 i);
#endif
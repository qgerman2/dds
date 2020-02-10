#define HITXOFFSET 64
#define HITYOFFSET 15
#define COMBOX 100
#define COMBOY 150

extern u32 prevscore;

extern u16* tapMemory;
extern u16* tailMemory;
extern u16* holdMemory;

void pr_setup();
void loadStepGfx();
void loadLifebarGfx();
void loadNumberGfx();
void loadJudgmentGfx();
void loadFontGfx();
void loadSubBackground();
void loadSubScore();
void renderLifebar();
void renderPlay();
void renderSteps();
void renderCombo();
void renderSubScore();
void playJudgmentAnim(u8 anim);
void renderJudgment();
u8 popSprite();
u8 popSpriteSub();
void pushSprite(u8 i);
void pushSpriteSub(u8 i);
void setRotData();
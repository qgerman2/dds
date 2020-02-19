#define HITXOFFSET 64
#define HITYOFFSET 15
#define COMBOX 100
#define COMBOY 150

void pr_setup();
void loadStepGfx();
void loadLifebarGfx();
void loadNumberGfx();
void loadJudgmentGfx();
void loadFontGfx();
void loadSubBackground();
void loadSubScore();
void renderPlay();
void renderLifebar();
void renderSteps();
void renderCombo();
void renderJudgment();
void renderSubScore();
void playJudgmentAnim(u8 anim);
void playScoreAnim();
void setRotData();

extern u32 prevscore;
extern u16* tapMemory;
extern u16* tailMemory;
extern u16* holdMemory;
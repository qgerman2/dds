#ifndef PLAYRENDER
#define PLAYRENDER
#define HITXOFFSET 64
#define HITYOFFSET 15
#define COMBOX 100
#define COMBOY 150
class Play;
class PlayRender {
private:
	Play* play;
	u16* stepGfx[8];
	u16* tapGfx;
	u16* tailGfx;
	u16* holdGfx;
	u16* holdSideGfx;
	u16* holdUpGfx;
	u16* holdDownGfx;
	u16* receptorGfx;
	u16* numberGfx[10];
	u16* judgeGfx[24];
	u16* scoreGfx[11];
	u16* pointGfx[10];
	u16* pulseGfx;
	u8 holdTopSprite[4];
	u8 receptorSprite[4];
	u8 comboSprite[3];
	u8 judgeSprite[2];
	u8 scoreSprite[11];
	u8 pointSprite[3 * 6];
	u8 pulseSprite[4];
	u8 scoreFrame = 0;
	u8 segments = 0;
	u8 judgeFrame = 0;
	u8 judgeAnim = 254;
public:
	int holdTop[4];
	int newscore = 0;
	int prevscore = 0;
	int pulseFrame[4];
	PlayRender(Play* play);
	~PlayRender();
	void update();
	void renderSteps();
	void loadHoldGfx();
	void loadReceptorGfx();
	void loadStepGfx();
	void loadNumberGfx();
	void loadJudgmentGfx();
	void loadPulseGfx();
	void loadSubBackground();
	void loadSubScore();
	void renderCombo();
	void renderJudgment();
	void renderSubScore();
	void renderPulse();
	void renderReceptor();
	void playJudgmentAnim(u8 anim);
	void playScoreAnim();
};
#endif
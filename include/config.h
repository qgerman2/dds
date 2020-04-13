#ifndef CONFIG
#define CONFIG
#define CONFIGCOUNT 5
struct settings_t {
	int speed = 4;
	int opacity = 9;
	bool intro = true;
	bool cache = true;
	bool cache_bg = false;
	bool folder = true;
};
class Config {
private:
	int y;
	int y_f;
	int y_dest;
	const int y_min = -32;
	const int y_max = -CONFIGCOUNT * 32 + 198;
	int height;
	int sub_bg;
	int cursor_bg;
	int state = 0;
	int animFrame;
	int valueSprites[CONFIGCOUNT];
	int cursorSprite;
	int cursorAnim;
	int cursorFrame;
	int cursor = 0;
	u16* numberGfx[10];
	u16* markGfx;
	void updateSprites();
	void hideSprites();
public:
	bool active = false;
	Config();
	~Config();
	void bg();
	void show();
	void hide();
	void update();
	void input();
	void next();
	void prev();
};
#endif
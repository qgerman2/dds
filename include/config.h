#ifndef CONFIG
#define CONFIG
#define CONFIGCOUNT 6
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
	int dialog_y;
	int sub_bg;
	int anim = 0;
	int animFrame;
	int valueSprites[CONFIGCOUNT];
	int cursorSprite;
	u16* numberGfx[10];
	void updateSprites();
public:
	bool active = false;
	Config();
	void show();
	void hide();
	void update();
};
#endif
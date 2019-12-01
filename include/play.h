void setup(songdata song); 
void loop();
void updateSteps();
typedef struct step {
	int x;
	int y;
	int fourths;
	int sprite;
} step;

typedef struct t_spriteEntry {
    u16 attr0;
    u16 attr1;
    u16 attr2;
    u16 affine_data;
} spriteEntry;

int popSprite();
void pushSprite(int i);

measure getMeasure(int beat);
void setup(songdata song); 
void loop();
void setRotData();
void updateSteps();
void renderSteps();
typedef struct step {
	int x;
	int y;
	u8 type;
	u32 beatf;
	u8 sprite;
	u8 col;
} step;

typedef struct t_spriteEntry {
    u16 attr0;
    u16 attr1;
    u16 attr2;
    u16 affine_data;
} spriteEntry;

void newSteps(u16 data, u32 beatf, u8 type);
u8 popSprite();
void pushSprite(u8 i);

measure getMeasureAtBeat(u32 beat);
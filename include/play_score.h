#define MARVELOUSJUDGE 23
#define PERFECTJUDGE 45
#define GREATJUDGE 90
#define GOODJUDGE 135
#define BOOJUDGE 180
extern u32 score;
extern u32 combo;
extern u32 judgesWindow[5];
void ps_setup();
void addScore(step* s, u32 beatfdiff);
void addDPTotal();
void dropCombo();
void updateJudgesWindow();
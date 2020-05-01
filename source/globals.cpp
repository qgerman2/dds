//see globals.h for more details
#include "globals.h"
#include "config.h"
#include <nds.h>
#include <string>
#include "play.h"
#include "menu.h"
#include "buffer.h"
#include "play_score.h"
struct settings_t settings;
int state = 2;
std::string simpath;
std::string songpath;
int songchart;
int bgid;
int consoleid;
Buffer* shared_buffer = NULL;
Play* shared_play = NULL;
Menu* shared_menu = NULL;
int buffer_center = 0;
int buffer_cursor = BUFFERSIZE / 2;
bool nocash = false;
bool keep_artwork = false;
std::string bufferpath = "/dds";
u32 beatfperiod = (1 << (BPMFRAC + MINUTEFRAC));
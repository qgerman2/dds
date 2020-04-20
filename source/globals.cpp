//see globals.h for more details
#include "globals.h"
#include "config.h"
#include <string>
#include "play.h"
#include "buffer.h"
#include "play_score.h"
struct settings_t settings;
int state = 2;
std::string simpath;
std::string songpath;
int songchart;
int bgid;
Buffer* shared_buffer = NULL;
Play* shared_play = NULL;
int buffer_center = 0;
int buffer_cursor = 0;
bool nocash = false;
bool keep_artwork = false;
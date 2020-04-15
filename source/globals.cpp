#include "globals.h"
#include "config.h"
#include <string>
#include "play.h"
#include "buffer.h"
struct settings_t settings;
int state = 2;
std::string simpath;
std::string songpath;
int songchart;
int bgid;
Buffer* shared_buffer = NULL;
Play* shared_play = NULL;
bool nocash = false;
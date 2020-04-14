#ifndef MAIN
#define MAIN
#include <string>
#include "buffer.h"
#include "config.h"
#include "play.h"
#include "parse.h"
extern struct settings_t settings;
extern int state;
extern std::string simpath;
extern std::string songpath;
extern int songchart;
extern int bgid;
extern Play* shared_play;
extern Buffer* shared_buffer;
extern bool nocash;
#endif
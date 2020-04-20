#ifndef GLOBALS
#define GLOBALS
#include "config.h"
#include <string>
#include "play.h"
#include "buffer.h"
//settings
extern struct settings_t settings;
//game state 0 = wheel menu  1 = ingame  2 = main menu  3 = pause  4 = score
extern int state;
//song data for state 1
extern std::string simpath;
extern std::string songpath;
extern int songchart;
//bitmap background for artwork
extern int bgid;
//play pointer for vblank interrupt
extern Play* shared_play;
//buffer pointer shared between wheel and main menu
extern Buffer* shared_buffer;
//wheel position after exiting song
extern int buffer_center;
extern int buffer_cursor;
//true when running on no$gba
extern bool nocash;
//bool to avoid refreshing artwork on reset
extern bool keep_artwork;
#endif
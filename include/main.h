#ifndef MAIN
#define MAIN
#include <string>
#include "buffer.h"
#include "config.h"
#include "parse.h"
void imagetobg(std::string path);
void testpng();
void testjpeg();
extern struct settings_t settings;
extern int state;
extern std::string simpath;
extern std::string songpath;
extern int bgid;
extern Buffer* shared_buffer;
#endif
#ifndef MAIN
#define MAIN
#include <string>
#include "buffer.h"
#include "parse.h"
void imagetobg(std::string path);
void testpng();
void testjpeg();
extern songdata song;
extern int state;
extern std::string simpath;
extern std::string songpath;
extern int bgid;
extern songdata song;
extern Buffer* shared_buffer;
#endif
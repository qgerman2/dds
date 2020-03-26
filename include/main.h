#ifndef MAIN
#define MAIN
#include <string>
#include "parse.h"
void imagetobg(std::string path);
void testpng();
void testjpeg();
extern songdata song;
extern int state;
extern std::string songpath;
extern int bgid;
#endif
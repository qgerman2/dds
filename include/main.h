#ifndef MAIN
#define MAIN
#include <string>
#include "buffer.h"
#include "parse.h"
void imagetobg(std::string path);
void testpng();
void testjpeg();
extern int state;
extern std::string simpath;
extern std::string songpath;
extern int bgid;
extern Buffer* shared_buffer;
#endif
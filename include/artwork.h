#ifndef ARTWORK
#define ARTWORK
#define PIXELFRAC 6
#define COLORFRAC 7
#define NDSWIDTH 256
#define PNGBUFFER 2500
#include <nds.h>
#include <string>
#include <jpeglib.h>
#include <png.h>
struct transform {
	u32 source_width;
	u32 source_height;
	u32 output_width;
	u32 output_height;
	u32* output = NULL;
	bool error = false;
	std::string error_msg;
};
struct jpeg_error {
	struct jpeg_error_mgr mgr;
	struct transform* tinfo;
};
struct bmp {
	const char* bm = "BM";
	const char* dds = "!dds";
	u32 size = 0;
	u32 offset_pixel = 70;
	u32 size_header = 56;
	u16 planes = 1;
	u16 colordepth = 16;
	u32 mode = 3;
	u32 size_pixel = 99;
	u32 print_res = 2835;
	u32 zero = 0;
	u32 red_mask = 31744;	//11111 00000 00000
	u32 green_mask = 992;	//00000 11111 00000
	u32 blue_mask = 31;		//00000 00000 11111
};
bool processArtwork(std::string filepath, int type);
bool processFile(FILE** infile, std::string);
void processScanline(struct transform* tinfo, u8* scanline, uint count);
bool fromJpeg(FILE* infile, struct transform* tinfo);
void errorJpeg(j_common_ptr cinfo);
bool fromPng(FILE* infile, struct transform* tinfo);
void infoPng(png_structp png_ptr, png_infop info_ptr);
void rowPng(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass);
void endPng(png_structp png_ptr, png_infop info_ptr);
void errorPng(png_structp png_ptr, png_const_charp msg);
void warningPng(png_structp png_ptr, png_const_charp msg);
bool exportArtwork(std::string filepath, struct transform* tinfo);
#endif
#ifndef ARTWORK
#define ARTWORK
#define PIXELFRAC 6
#define COLORFRAC 7
#define NDSWIDTH 256
#define PNGBUFFER 2500
#define REDMASK 0b000000000011111
#define GREENMASK 0b000001111100000
#define BLUEMASK 0b111110000000000
#include <nds.h>
#include <string>
#include <jpeglib.h>
#include <png.h>
struct transform {
	u32 source_width;
	u32 source_height;
	u32 output_width;
	u32 output_height;
	u32* row_buffer = NULL;
	u16* output = NULL;
	bool error = false;
	std::string error_msg;
	~transform() {
		if (row_buffer) {delete[] row_buffer;}
	};
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
	u32 red_mask  = REDMASK;
	u32 green_mask= GREENMASK;
	u32 blue_mask = BLUEMASK;
};
bool loadArtwork(std::string filepath, u16* dest, uint width, uint height);
bool processFile(FILE** infile, std::string);
void processScanline(struct transform* tinfo, u8* scanline, uint count);
void pixelToOutput(uint out_x, uint row, struct transform* tinfo);
bool fromJpeg(FILE* infile, struct transform* tinfo);
void errorJpeg(j_common_ptr cinfo);
bool fromPng(FILE* infile, struct transform* tinfo);
void infoPng(png_structp png_ptr, png_infop info_ptr);
void rowPng(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass);
void endPng(png_structp png_ptr, png_infop info_ptr);
void errorPng(png_structp png_ptr, png_const_charp msg);
void warningPng(png_structp png_ptr, png_const_charp msg);
bool fromBmp(FILE* bmp, u16* dest, uint width, uint height);
bool exportArtwork(std::string filepath, u16* buffer, uint width, uint height);
#endif
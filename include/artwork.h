#ifndef ARTWORK
#define ARTWORK
#define PIXELFRAC 6
#define COLORFRAC 6
#define NDSWIDTH 256
#define PNGBUFFER 2000
#define PNG_ROWBYTES(pixel_bits, width) \
	((pixel_bits) >= 8 ? \
	((size_t)(width) * (((size_t)(pixel_bits)) >> 3)) : \
	(( ((size_t)(width) * ((size_t)(pixel_bits))) + 7) >> 3) )
#include <nds.h>
#include <string>
#include <jpeglib.h>
#include <png.h>
struct transform {
	u32 source_width;
	u32 source_height;
	u32 output_width;
	u32 output_height;
	u32 area_width;
	u32 area_height;
	u32* output;
};
bool processArtwork(std::string filepath, int type);
void processScanline(u8* scanline, int count, int width, int height);
bool fromJpeg(std::string filepath, int type);
void errorJpeg(j_common_ptr cinfo);
bool fromPng(std::string filepath, int type);
void infoPng(png_structp png_ptr, png_infop info_ptr);
void rowPng(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass);
void endPng(png_structp png_ptr, png_infop info_ptr);
void errorPng(png_structp png_ptr, png_const_charp msg);
void warningPng(png_structp png_ptr, png_const_charp msg);
#endif
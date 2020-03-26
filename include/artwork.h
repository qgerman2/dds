#ifndef ARTWORK
#define ARTWORK
#define PNGBUFFER 2000
#define PNG_ROWBYTES(pixel_bits, width) \
    ((pixel_bits) >= 8 ? \
    ((size_t)(width) * (((size_t)(pixel_bits)) >> 3)) : \
    (( ((size_t)(width) * ((size_t)(pixel_bits))) + 7) >> 3) )
#include <string>
#include <jpeglib.h>
#include <png.h>
bool processArtwork(std::string filepath, int type);
bool fromJpeg(std::string filepath, int type);
void errorJpeg(j_common_ptr cinfo);
bool fromPng(std::string filepath, int type);
void infoPng(png_structp png_ptr, png_infop info_ptr);
void rowPng(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass);
void endPng(png_structp png_ptr, png_infop info_ptr);
void warningPng(png_structp png_ptr, png_const_charp msg);
#endif
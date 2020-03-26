#include <nds.h>
#include <string>
#include <iostream>
#include <jpeglib.h>
#include <zlib.h>
#include <png.h>
#include "artwork.h"

using namespace std;

bool processArtwork(string filepath, int type) {
	string extension = filepath.substr(filepath.find_last_of(".") + 1);
	if (extension == "jpeg" || extension == "jpg") {
		return fromJpeg(filepath, type);
	} else if (extension == "png") {
		return fromPng(filepath, type);
	}
	return false;
}

bool fromJpeg(string filepath, int type) {
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jerr.error_exit = errorJpeg;
	FILE* infile = fopen(filepath.c_str(), "rb");
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	cinfo.out_color_space = JCS_RGB;
	jpeg_start_decompress(&cinfo);
	int bufsize = cinfo.output_width * cinfo.output_components;
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, bufsize, 1);
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		for (int x = 0; x < 256 * 3; x = x + 3) {
			//bg[(x / 3) + 256 * cinfo.output_scanline] = ARGB16(1, buffer[0][x] >> 3, buffer[0][x + 1] >> 3, buffer[0][x + 2] >> 3);
		}
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	return true;
}

void errorJpeg(j_common_ptr cinfo) {
    char jpegLastErrorMsg[JMSG_LENGTH_MAX];
    (*( cinfo->err->format_message )) (cinfo, jpegLastErrorMsg);
    cout << "\n" << jpegLastErrorMsg;
}

bool fromPng(string filepath, int type) {
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, warningPng);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_set_progressive_read_fn(png_ptr, NULL, infoPng, rowPng, endPng);
	u_char inbuf[PNGBUFFER];
	FILE* infile;
	if (!(infile = fopen(filepath.c_str(), "rb"))) {
		//
	} else {
		int incount = 0;
		while (1) {
			incount = fread(inbuf, 1, PNGBUFFER, infile);
			png_process_data(png_ptr, info_ptr, inbuf, incount);
			if (incount != PNGBUFFER) {
				cout << "\nfin";
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				break;
			}
	    }
	}
	return true;
}

void infoPng(png_structp png_ptr, png_infop info_ptr) {
	cout << "\ninfo callback";
	png_uint_32 channels = png_get_channels(png_ptr, info_ptr);
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
	png_read_update_info(png_ptr, info_ptr);
	uint pixel_depth = channels * bit_depth;
	cout << "\nwidth: " << width << " height: " << height << " bitdepth: " << bit_depth << " color_type: " << color_type;
	cout << "\nchannels: " << channels << " pixel_depth: " << pixel_depth;
	uint row_bytes = PNG_ROWBYTES(pixel_depth, width);
	cout << "\nrow_bytes: " << row_bytes;

}

void rowPng(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass) {
	//u16* bg = bgGetGfxPtr(bgid);
	if (!new_row) {
		cout << "\nrow vacia";
	}
	//u8* row = (u8*)new_row;
	for (int x = 0; x < 256 * 3; x = x + 3) {
		//bg[(x / 3) + 256 * row_num] = ARGB16(1, row[x] >> 3, row[x + 1] >> 3, row[x + 2] >> 3);
	}
	//swiWaitForVBlank();
}

void endPng(png_structp png_ptr, png_infop info_ptr) {
	//cout << "\nend callback";
}

void warningPng(png_structp png_ptr, png_const_charp msg) {
    fprintf(stderr, "%s\n", msg);
}
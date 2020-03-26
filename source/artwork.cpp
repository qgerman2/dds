#include <nds.h>
#include <string>
#include <iostream>
#include <jpeglib.h>
#include <zlib.h>
#include <png.h>
#include "main.h"
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

void processScanline(struct transform* tinfo, u8* scanline, int count) {
	u16* bg = bgGetGfxPtr(bgid);
	for (uint out_x = 0; out_x < tinfo->output_width; out_x++) {
		uint startpixel = (out_x * tinfo->area_width) / (1 << PIXELFRAC);
		uint endpixel = ((((out_x + 1) * tinfo->area_width) + (1 << PIXELFRAC) - 1) / (1 << PIXELFRAC)) - 1;
		uint r = 0;
		uint g = 0;
		uint b = 0;
		//cout << "\nat " << out_x << " from " << startpixel << " to " << endpixel << "\n";
		for (uint src_x = startpixel; src_x <= endpixel; src_x++) {
			int weigth = (1 << PIXELFRAC);
			if (src_x == startpixel) {
				int startweigth = (1 << PIXELFRAC) - ((out_x * tinfo->area_width) % 32);
				if (startweigth != 0) {weigth = startweigth;}
			} else if (src_x == endpixel) {
				int endweigth = ((out_x + 1) * tinfo->area_width) % 32;
				if (endweigth != 0) {weigth = endweigth;}
			}
			r += ((scanline[src_x * 3] << (COLORFRAC * 2)) / (tinfo->area_width << COLORFRAC)) * weigth;
			g += ((scanline[src_x * 3 + 1] << (COLORFRAC * 2)) / (tinfo->area_width << COLORFRAC)) * weigth;
			b += ((scanline[src_x * 3 + 2] << (COLORFRAC * 2)) / (tinfo->area_width << COLORFRAC)) * weigth;
			//cout << weigth << " ";
		}
		r = r >> COLORFRAC;
		g = g >> COLORFRAC;
		b = b >> COLORFRAC;
		bg[out_x + 256 * count] = ARGB16(1, r >> 3, g >> 3, b >> 3);
		//swiWaitForVBlank();
	}
}

bool fromJpeg(string filepath, int type) {
	struct transform tinfo;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jerr.error_exit = errorJpeg;
	FILE* infile = fopen(filepath.c_str(), "rb");
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	cinfo.out_color_space = JCS_RGB;
	tinfo.source_width = cinfo.image_width;
	tinfo.source_height = cinfo.image_height;
	tinfo.output_width = 256;
	tinfo.output_height = 192;
	tinfo.output = new u8[tinfo.output_width * tinfo.output_height * 3]();
	tinfo.area_width = (1 << PIXELFRAC) * (tinfo.source_width << PIXELFRAC) / (tinfo.output_width << PIXELFRAC);
	cout << "\n" << tinfo.area_width;
	jpeg_start_decompress(&cinfo);
	int bufsize = cinfo.output_width * cinfo.output_components;
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, bufsize, 1);
	while (cinfo.output_scanline < 200) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		processScanline(&tinfo, (u8*)buffer[0], cinfo.output_scanline);
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
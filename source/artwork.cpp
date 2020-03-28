#include <nds.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <jpeglib.h>
#include <zlib.h>
#include <png.h>
#include "main.h"
#include "artwork.h"
#define AREAWIDTH (tinfo->source_width << (PIXELFRAC * 2)) / (tinfo->output_width << PIXELFRAC)
#define AREAHEIGHT (tinfo->source_height << (PIXELFRAC * 2)) / (tinfo->output_height << PIXELFRAC)

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
	count--;
	int rowweight = (1 << PIXELFRAC);
	int nextrowweight = 0;
	int row = count * tinfo->output_height / tinfo->source_height;
	int endcount = ((((row + 1) * AREAHEIGHT) + (1 << PIXELFRAC) - 1) / (1 << PIXELFRAC)) - 1;
	if (count == endcount) {
		int endcountweight = ((row + 1) * AREAHEIGHT) % (1 << PIXELFRAC);
		if (endcountweight != 0) {
			rowweight = endcountweight;
			nextrowweight = (1 << PIXELFRAC) - rowweight;
		}
	}
	cout << "\nc: " << count << " r: " << row << " w: " << rowweight << " nw: " << nextrowweight; 
	for (uint out_x = 0; out_x < tinfo->output_width; out_x++) {
		uint startpixel = (out_x * AREAWIDTH) / (1 << PIXELFRAC);
		uint endpixel = ((((out_x + 1) * AREAWIDTH) + (1 << PIXELFRAC) - 1) / (1 << PIXELFRAC)) - 1;
		uint r = 0;
		uint g = 0;
		uint b = 0;
		for (uint src_x = startpixel; src_x <= endpixel; src_x++) {
			int weight = (1 << PIXELFRAC);
			if (src_x == startpixel) {
				int startweight = (1 << PIXELFRAC) - ((out_x * AREAWIDTH) % (1 << PIXELFRAC));
				if (startweight != 0) {weight = startweight;}
			} else if (src_x == endpixel) {
				int endweight = ((out_x + 1) * AREAWIDTH) % (1 << PIXELFRAC);
				if (endweight != 0) {weight = endweight;}
			}
			r += ((scanline[src_x * 3] << (COLORFRAC * 2)) / ((AREAWIDTH) << COLORFRAC)) * weight;
			g += ((scanline[src_x * 3 + 1] << (COLORFRAC * 2)) / ((AREAWIDTH) << COLORFRAC)) * weight;
			b += ((scanline[src_x * 3 + 2] << (COLORFRAC * 2)) / ((AREAWIDTH) << COLORFRAC)) * weight;
		}
		int out_i = out_x + row * tinfo->output_width;
		tinfo->output[out_i * 3] += (r << COLORFRAC) / ((AREAHEIGHT) << COLORFRAC) * rowweight;
		tinfo->output[out_i * 3 + 1] += (g << COLORFRAC) / ((AREAHEIGHT) << COLORFRAC) * rowweight;
		tinfo->output[out_i * 3 + 2] += (b << COLORFRAC) / ((AREAHEIGHT) << COLORFRAC) * rowweight;
		if (nextrowweight) {
			int out_ii = out_x + (row + 1) * tinfo->output_width;
			tinfo->output[out_ii * 3] += (r << COLORFRAC) / ((AREAHEIGHT) << COLORFRAC) * nextrowweight;
			tinfo->output[out_ii * 3 + 1] += (g << COLORFRAC) / ((AREAHEIGHT) << COLORFRAC) * nextrowweight;
			tinfo->output[out_ii * 3 + 2] += (b << COLORFRAC) / ((AREAHEIGHT) << COLORFRAC) * nextrowweight;
		}
	}
	//color cap
	if (count == endcount) {
		for (uint x = 0; x < tinfo->output_width; x++) {
			int out_i = x + row * tinfo->output_width;
			if (tinfo->output[out_i * 3] >= 255 << COLORFRAC) {tinfo->output[out_i * 3] = (255 << COLORFRAC) - 1;}
			if (tinfo->output[out_i * 3 + 1] >= 255 << COLORFRAC) {tinfo->output[out_i * 3 + 1] = (255 << COLORFRAC) - 1;}
			if (tinfo->output[out_i * 3 + 2] >= 255 << COLORFRAC) {tinfo->output[out_i * 3 + 2] = (255 << COLORFRAC) - 1;}
		}
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
	tinfo.output = new u32[tinfo.output_width * tinfo.output_height * 3]();
	jpeg_start_decompress(&cinfo);
	int bufsize = cinfo.output_width * cinfo.output_components;
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, bufsize, 1);
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		processScanline(&tinfo, (u8*)buffer[0], cinfo.output_scanline);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	u16* bg = bgGetGfxPtr(bgid);
	for (uint i = 0; i < tinfo.output_width * tinfo.output_height * 3; i = i + 3) {
		bg[i / 3] = ARGB16(1, tinfo.output[i] >> (COLORFRAC + 3), tinfo.output[i + 1] >> (COLORFRAC + 3), tinfo.output[i + 2] >> (COLORFRAC + 3));
	}
	return true;
}

void errorJpeg(j_common_ptr cinfo) {
    char jpegLastErrorMsg[JMSG_LENGTH_MAX];
    (*( cinfo->err->format_message )) (cinfo, jpegLastErrorMsg);
    cout << "\n" << jpegLastErrorMsg;
}

bool fromPng(string filepath, int type) {
	struct transform tinfo;
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (void*)&tinfo, errorPng, warningPng);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_set_progressive_read_fn(png_ptr, NULL, infoPng, rowPng, endPng);
	png_set_packing(png_ptr);
	png_set_palette_to_rgb(png_ptr);
	png_set_gray_to_rgb(png_ptr);
	png_set_strip_16(png_ptr);
	png_set_strip_alpha(png_ptr);
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
	png_uint_32 width;
	png_uint_32 height;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, NULL, NULL, NULL, NULL, NULL);
	struct transform* tinfo = (struct transform*)png_get_progressive_ptr(png_ptr);
	tinfo->source_width = width;
	tinfo->source_height = height;
	tinfo->output_width = 256;
	tinfo->output_height = 192;
	tinfo->output = new u32[tinfo->output_width * tinfo->output_height * 3]();
	png_read_update_info(png_ptr, info_ptr);
}

void rowPng(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass) {
	struct transform* tinfo = (struct transform*)png_get_progressive_ptr(png_ptr);
	processScanline(tinfo, new_row, row_num);
	swiWaitForVBlank();
}

void endPng(png_structp png_ptr, png_infop info_ptr) {
	struct transform* tinfo = (struct transform*)png_get_progressive_ptr(png_ptr);
	u16* bg = bgGetGfxPtr(bgid);
	for (uint i = 0; i < tinfo->output_width * tinfo->output_height * 3; i = i + 3) {
		bg[i / 3] = ARGB16(1, tinfo->output[i] >> 9, tinfo->output[i + 1] >> 9, tinfo->output[i + 2] >> 9);
	}
	cout << "\nend callback";
}

void errorPng(png_structp png_ptr, png_const_charp msg) {
	fprintf(stderr, "%s\n", msg);
}

void warningPng(png_structp png_ptr, png_const_charp msg) {
    fprintf(stderr, "%s\n", msg);
}
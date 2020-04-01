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
	FILE* infile = NULL;
	bool success = false;
	struct transform tinfo;
	tinfo.output_width = 256;
	tinfo.output_height = 192;
	string extension = filepath.substr(filepath.find_last_of(".") + 1);
	if (extension == "jpeg" || extension == "jpg") {
		if (processFile(&infile, filepath)) {
			success = fromJpeg(infile, &tinfo);
		}
	} else if (extension == "png") {
		if (processFile(&infile, filepath)) {
			success = fromPng(infile, &tinfo);
		}
	} else {
		cout << "\nNot an image file " << filepath;
		success = false;
	}
	if (infile != NULL) {
		fclose(infile);
	}
	if (tinfo.output != NULL) {
		exportArtwork(filepath, &tinfo);
		delete[] tinfo.output;
	}
	return success;
}

bool processFile(FILE** infile, string filepath) {
	*infile = fopen(filepath.c_str(), "rb");
	if (*infile == NULL) {
		cout << "\nCouldn't open image " << filepath;
		return false;
	}
	return true;
}

void processScanline(struct transform* tinfo, u8* scanline, uint count) {
	count--;
	int rowweight = (1 << PIXELFRAC);
	int nextrowweight = 0;
	int row = count * tinfo->output_height / tinfo->source_height;
	uint endcount = ((((row + 1) * AREAHEIGHT) + (1 << PIXELFRAC) - 1) / (1 << PIXELFRAC)) - 1;
	if (count == endcount) {
		int endcountweight = ((row + 1) * AREAHEIGHT) % (1 << PIXELFRAC);
		if (endcountweight != 0) {
			rowweight = endcountweight;
			nextrowweight = (1 << PIXELFRAC) - rowweight;
		}
	}
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

bool fromJpeg(FILE* infile, struct transform* tinfo) {
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error jerr;
	cinfo.err = jpeg_std_error(&jerr.mgr);
	jerr.mgr.error_exit = errorJpeg;
	jerr.tinfo = tinfo;
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	if (tinfo->error) {
		jpeg_destroy_decompress(&cinfo);
		return false;
	}
	cinfo.out_color_space = JCS_RGB;
	tinfo->source_width = cinfo.image_width;
	tinfo->source_height = cinfo.image_height;
	tinfo->output = new u32[tinfo->output_width * tinfo->output_height * 3]();
	jpeg_start_decompress(&cinfo);
	int bufsize = cinfo.output_width * cinfo.output_components;
	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, bufsize, 1);
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		processScanline(tinfo, (u8*)buffer[0], cinfo.output_scanline);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return true;
}

void errorJpeg(j_common_ptr cinfo) {
    char msg[JMSG_LENGTH_MAX];
    struct jpeg_error* err = (jpeg_error*)cinfo->err;
    (*(err->mgr.format_message)) (cinfo, msg);
	err->tinfo->error = true;
	err->tinfo->error_msg = msg;
    cout << "\nlibjpeg-turbo fatal error: " << msg;
}

bool fromPng(FILE* infile, struct transform* tinfo) {
	u_char inbuf[PNGBUFFER];
	if (fread(inbuf, 1, 8, infile) != 8) {
		cout << "\nFailed to read png header";
		return false;
	}
	if (png_sig_cmp(inbuf, 0, 8)) {
		cout << "\nImage is not a valid png";
		return false;
	}
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, errorPng, warningPng);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_set_progressive_read_fn(png_ptr, tinfo, infoPng, rowPng, endPng);
	png_set_packing(png_ptr);
	png_set_palette_to_rgb(png_ptr);
	png_set_gray_to_rgb(png_ptr);
	png_set_strip_16(png_ptr);
	png_set_strip_alpha(png_ptr);
	png_process_data(png_ptr, info_ptr, inbuf, 8);
	int incount = 0;
	while (1) {
		incount = fread(inbuf, 1, PNGBUFFER, infile);
		if (incount > 0) {
			png_process_data(png_ptr, info_ptr, inbuf, incount);
		}
		if (tinfo->error) {
			png_destroy_info_struct(png_ptr, &info_ptr);
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return false;
		}
		if (incount != PNGBUFFER) {
			break;
		}
    }
	png_destroy_info_struct(png_ptr, &info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return true;
}

void infoPng(png_structp png_ptr, png_infop info_ptr) {
	png_uint_32 width;
	png_uint_32 height;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, NULL, NULL, NULL, NULL, NULL);
	struct transform* tinfo = (struct transform*)png_get_progressive_ptr(png_ptr);
	tinfo->source_width = width;
	tinfo->source_height = height;
	tinfo->output = new u32[tinfo->output_width * tinfo->output_height * 3]();
	png_read_update_info(png_ptr, info_ptr);
}

void rowPng(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass) {
	struct transform* tinfo = (struct transform*)png_get_progressive_ptr(png_ptr);
	processScanline(tinfo, new_row, ++row_num);
}

void endPng(png_structp png_ptr, png_infop info_ptr) {

}

void errorPng(png_structp png_ptr, png_const_charp msg) {
	struct transform* tinfo = (struct transform*)png_get_progressive_ptr(png_ptr);
	tinfo->error = true;
	tinfo->error_msg = msg;
	cout << "\nlibpng fatal error: " << msg;
}

void warningPng(png_structp png_ptr, png_const_charp msg) {
	cout << "\nlibpng warning: " << msg;
}

bool exportArtwork(string filepath, struct transform* tinfo) {
	u16* bg = bgGetGfxPtr(bgid);
	for (uint i = 0; i < tinfo->output_width * tinfo->output_height * 3; i = i + 3) {
		bg[i / 3] = ARGB16(1, tinfo->output[i] >> (COLORFRAC + 3), tinfo->output[i + 1] >> (COLORFRAC + 3), tinfo->output[i + 2] >> (COLORFRAC + 3));
	}
	struct bmp info;
	info.size_pixel = tinfo->output_width * tinfo->output_height * 2;
	info.size = info.size_pixel + info.offset_pixel;
	FILE* bmp = fopen("output.bmp", "wb");
	fwrite(info.bm, 2, 1, bmp);
	fwrite(&(info.size), 4, 1, bmp);
	fwrite(info.dds, 4, 1, bmp);
	fwrite(&(info.offset_pixel), 4, 1, bmp);
	fwrite(&(info.size_header), 4, 1, bmp);
	fwrite(&(tinfo->output_width), 4, 1, bmp);
	fwrite(&(tinfo->output_height), 4, 1, bmp);
	fwrite(&(info.planes), 2, 1, bmp);
	fwrite(&(info.colordepth), 2, 1, bmp);
	fwrite(&(info.mode), 4, 1, bmp);
	fwrite(&(info.size_pixel), 4, 1, bmp);
	fwrite(&(info.print_res), 4, 1, bmp);
	fwrite(&(info.print_res), 4, 1, bmp);
	fwrite(&(info.zero), 4, 1, bmp);
	fwrite(&(info.zero), 4, 1, bmp);
	fwrite(&(info.blue_mask), 4, 1, bmp);
	fwrite(&(info.green_mask), 4, 1, bmp);
	fwrite(&(info.red_mask), 4, 1, bmp);
	fwrite(&(info.zero), 4, 1, bmp);
	for (int y = tinfo->output_height - 1; y >= 0; y--) {
		for (uint x = 0; x < tinfo->output_width; x++) {
			uint offset = (y * tinfo->output_width * 3) + (x * 3);
			u16 pixel = ARGB16(1, tinfo->output[offset] >> (COLORFRAC + 3), tinfo->output[offset + 1] >> (COLORFRAC + 3), tinfo->output[offset + 2] >> (COLORFRAC + 3));
			fwrite(&pixel, 2, 1, bmp);
		}
		fwrite(&(info.zero), 1, (tinfo->output_width * 2) % 4, bmp);
	}
	fclose(bmp);
	return false;
}
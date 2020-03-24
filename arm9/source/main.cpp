#include <nds.h>
#include <fat.h>
#include <iostream>
#include <string>
#include <vector>
#include "lodepng.h"
#include "main.h"
#include "menu.h"
#include "play.h"
#include "parse.h"
#include "sound.h"
#include <maxmod9.h>
#include <zlib.h>
#include <png.h>

#define PNG_ROWBYTES(pixel_bits, width) \
    ((pixel_bits) >= 8 ? \
    ((size_t)(width) * (((size_t)(pixel_bits)) >> 3)) : \
    (( ((size_t)(width) * ((size_t)(pixel_bits))) + 7) >> 3) )

using namespace std;

int state = 2;
string songpath;
int bgid;

int main(){
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);
	bgExtPaletteEnable();
	bgExtPaletteEnableSub();
	vramSetBankA(VRAM_A_MAIN_BG_0x06040000);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	vramSetBankD(VRAM_D_SUB_SPRITE);
	vramSetBankE(VRAM_E_MAIN_BG);
	vramSetBankF(VRAM_F_LCD); //bg ext palette
	vramSetBankH(VRAM_H_LCD); //bg ext palette sub
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
	oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);

	
	if (!fatInitDefault()) {
		sassert(0, "failed to load libfat");
	}
	consoleDemoInit();
	//consoleDebugInit(DebugDevice_NOCASH);
	cout << "\nzlib: " << zlibVersion();
	cout << "\nlibpng: " << PNG_LIBPNG_VER_STRING;
	s_play();
	bgid = bgInit(2, BgType_Bmp8, BgSize_B16_256x256, 16, 0);
	testpng();
	//imagetobg("mono.png");

	while (1) {
		switch (state) {
			case (0): {
				Menu* menu = new Menu();
				menu->loop();
				delete menu;
			}
			break;
			case (1): {
				metadata tags = parseSimFile(songpath, false);
				songdata song = parseSong(&tags);
				Play* play = new Play(&song);
				play->loop();
				delete play;
			}
			break;
			case (2): {
				while (1) {
					mmStreamUpdate();
					swiWaitForVBlank();
				}
			}
		}

	}
	return 0;
}

void imagetobg(string path) {
	u16* bg = bgGetGfxPtr(bgid);
	vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, path);
	cout << width << " " << height << " " << image.size();
	for(unsigned i = 0; i < 256; i++) {
  		bg[i] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
	}
	cout << "\nend";
}

png_structp png_ptr;
png_infop info_ptr;

uint pixel_depth;
uint row_bytes;
void info_callback(png_structp png_ptr, png_infop info_ptr) {
	cout << "\ninfo callback";
	png_uint_32 channels = png_get_channels(png_ptr, info_ptr);
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
	png_read_update_info(png_ptr, info_ptr);
	pixel_depth = channels * bit_depth;
	cout << "\nwidth: " << width << " height: " << height << " bitdepth: " << bit_depth << " color_type: " << color_type;
	cout << "\nchannels: " << channels << " pixel_depth: " << pixel_depth;
	row_bytes = PNG_ROWBYTES(pixel_depth, width);
	cout << "\nrow_bytes: " << row_bytes;

}

void row_callback(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass) {
	u16* bg = bgGetGfxPtr(bgid);
	if (!new_row) {
		cout << "\nrow vacia";
	}
	u8* row = (u8*)new_row;
	for (int x = 0; x < 256 * 3; x = x + 3) {
		bg[(x / 3) + 256 * row_num] = ARGB16(1, row[x] >> 3, row[x + 1] >> 3, row[x + 2] >> 3);
	}
	swiWaitForVBlank();
}

void end_callback(png_structp png_ptr, png_infop info_ptr) {
	cout << "\nend callback";
}

void warning_handler(png_structp png_ptr, png_const_charp msg)
{
    //fprintf(stderr, "readpng2 libpng warning: %s\n", msg);
    //fflush(stderr);
}

void testpng() {
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, warning_handler);
    info_ptr = png_create_info_struct(png_ptr);
    png_set_progressive_read_fn(png_ptr, NULL, info_callback, row_callback, end_callback);

	const int INBUFSIZE = 2000;
	u_char inbuf[INBUFSIZE];
	FILE* infile;
	if (!(infile = fopen("mono.png", "rb"))) {
		//
	} else {
		int incount = 0;
		while (1) {
			incount = fread(inbuf, 1, INBUFSIZE, infile);
			png_process_data(png_ptr, info_ptr, inbuf, incount);
			if (incount != INBUFSIZE) {
				cout << "\nfin";
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				break;
			}
	    }
	}
}
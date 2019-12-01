#include <nds.h>
#include <tap.h>
#include "main.h"
#include "play.h"

//indice memoria
#define sprites ((spriteEntry*) OAM)
#define tiles_tap 0
#define pal_tap 0

//macros para acceso a memoria
#define tile2objram(t) (SPRITE_GFX + (t) * 16)
#define pal2objram(p) (SPRITE_PALETTE + (p) * 16)


void setup(){
	// code to test out the sprite engine
	//cargar graficos
	dmaCopyHalfWords( 3, tapTiles, tile2objram(tiles_tap), tapTilesLen);
	dmaCopyHalfWords( 3, tapPal, pal2objram(pal_tap), tapPalLen);
    int n;
    for( n = 0; n < 50; n++ )
    {
        // attribute0: set vertical position 0->screen_height-sprite_height, 
        // other default options will be okay (default == zeroed)
        sprites[n].attr0 = rand() % (192 - 16) | ATTR0_ROTSCALE_DOUBLE;

        // attribute1: set horizontal position 0->screen_width-sprite_width
        // also set 16x16 size mode
        sprites[n].attr1 = (rand() % (256 - 16)) | ATTR1_SIZE_32 | ATTR1_ROTDATA(0);

        // attribute0: select tile number and palette number
        sprites[n].attr2 = tiles_tap + (pal_tap << 12);

		s16 s = sinLerp(10000) >> 4;
		s16 c = cosLerp(10000) >> 4;

		u16* affine;
		affine = OAM + 3;
		affine[0] = c;
		affine[4] = s;
		affine[8] = -s;
		affine[12] = c;
    }
}

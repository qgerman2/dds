#include <nds.h>
#include <fat.h>
#include <iostream>
#include "sound.h"
#include <maxmod9.h>

using namespace std;
FILE* file = 0;
mm_stream mystream;
mm_word stream(mm_word length, mm_addr dest, mm_stream_formats format);

void playSong() {
	mm_ds_system sys;
	sys.mod_count 			= 0;
	sys.samp_count			= 0;
	sys.mem_bank			= 0;
	sys.fifo_channel		= FIFO_MAXMOD;
	mmInit( &sys );

	file = fopen("/ddr/song.wav", "rb");

    mystream.sampling_rate = 8000;
    mystream.buffer_length = 800;
    mystream.callback = stream;
    mystream.format = MM_STREAM_16BIT_STEREO;
    mystream.timer = MM_TIMER2;
    mystream.manual = 1;
    
    mmStreamOpen( &mystream );
}

mm_word stream(mm_word length, mm_addr dest, mm_stream_formats format) {
	if (file) {
		int res = fread(dest, 4, length, file);
		if (res) {
			length = res;
		} else {
			mmStreamClose();
			fclose(file);
			length = 0;
		}
	}
	return length;
}

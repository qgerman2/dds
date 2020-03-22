#include <nds.h>
#include <fat.h>
#include <iostream>
#include "sound.h"
#include <maxmod9.h>

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

using namespace std;
FILE* file = 0;

OggVorbis_File vf;
mm_stream mystream;

mm_word stream(mm_word length, mm_addr dest, mm_stream_formats format);

void s_play() {
	mm_ds_system sys;
	sys.mod_count 			= 0;
	sys.samp_count			= 0;
	sys.mem_bank			= 0;
	sys.fifo_channel		= FIFO_MAXMOD;
	mmInit( &sys );

	FILE * myOgg = fopen("/ddr/song.ogg", "rb");
	if(ov_open(myOgg, &vf, NULL, 0) < 0) {
		fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
	}
	else {
		cout << "\n";
		{
	    char **ptr=ov_comment(&vf,-1)->user_comments;
	    vorbis_info *vi=ov_info(&vf,-1);
	    while(*ptr){
	      fprintf(stderr,"%s\n",*ptr);
	      ++ptr;
	    }
	    fprintf(stderr,"\nBitstream is %d channel, %ldHz\n",vi->channels,vi->rate);
	    fprintf(stderr,"\nDecoded length: %ld samples\n",
	            (long)ov_pcm_total(&vf,-1));
	    fprintf(stderr,"Encoded by: %s\n\n",ov_comment(&vf,-1)->vendor);
	  }
	}
	//file = fopen("/ddr/song.wav", "rb");

    mystream.sampling_rate = 44100;
    mystream.buffer_length = 3200;
    mystream.callback = stream;
    mystream.format = MM_STREAM_16BIT_STEREO;
    mystream.timer = MM_TIMER2;
    mystream.manual = 1;
    
    mmStreamOpen( &mystream );
}

int eof = 0;
int current_section;
mm_word stream(mm_word length, mm_addr dest, mm_stream_formats format) {
	char* output = (char*)dest;
	if (!eof) {
		int res = ov_read(&vf, output, length * 4, &current_section);
		if (res) {
			length = res / 4;
		} else {
			mmStreamClose();
			length = 0;
			eof = 1;
		}
	}
	return length;
	//return length;
	/*if (file) {
		int res = fread(buffer, 4, length, file);
		if (res) {
			length = res;
			for (uint i = 0; i < length * 4; i++) {
				output[i] = buffer[i];
			}
		} else {
			mmStreamClose();
			fclose(file);
			length = 0;
		}
	}
	return length;*/
}

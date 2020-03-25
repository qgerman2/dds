#include <nds.h>
#include <fat.h>
#include <iostream>
#include "sound.h"
#include <maxmod9.h>

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>
#include <mad.h>
#include <climits>

using namespace std;
FILE* file = 0;

OggVorbis_File vf;
mm_stream mystream;
struct mad_stream stream;
struct mad_frame frame;
struct mad_synth synth;
FILE* myMp3;

mm_word streammp3(mm_word length, mm_addr dest, mm_stream_formats format);
mm_word streamogg(mm_word length, mm_addr dest, mm_stream_formats format);

static signed short MadFixedToSshort(mad_fixed_t Fixed)
{
	/* A fixed point number is formed of the following bit pattern:
	 *
	 * SWWWFFFFFFFFFFFFFFFFFFFFFFFFFFFF
	 * MSB                          LSB
	 * S ==> Sign (0 is positive, 1 is negative)
	 * W ==> Whole part bits
	 * F ==> Fractional part bits
	 *
	 * This pattern contains MAD_F_FRACBITS fractional bits, one
	 * should alway use this macro when working on the bits of a fixed
	 * point number. It is not guaranteed to be constant over the
	 * different platforms supported by libmad.
	 *
	 * The signed short value is formed, after clipping, by the least
	 * significant whole part bit, followed by the 15 most significant
	 * fractional part bits. Warning: this is a quick and dirty way to
	 * compute the 16-bit number, madplay includes much better
	 * algorithms.
	 */

	/* Clipping */
	if(Fixed>=MAD_F_ONE)
		return(SHRT_MAX);
	if(Fixed<=-MAD_F_ONE)
		return(-SHRT_MAX);

	/* Conversion. */
	Fixed=Fixed>>(MAD_F_FRACBITS-15);
	return((signed short)Fixed);
}

void s_play() {
	mm_ds_system sys;
	sys.mod_count 			= 0;
	sys.samp_count			= 0;
	sys.mem_bank			= 0;
	sys.fifo_channel		= FIFO_MAXMOD;
	mmInit( &sys );

	mad_stream_init(&stream);
	mad_frame_init(&frame);
	mad_synth_init(&synth);

	loadmp3();
	//file = fopen("/ddr/song.wav", "rb");

    mystream.sampling_rate = 44100;
    mystream.buffer_length = 3200;
    mystream.callback = streammp3;
    mystream.format = MM_STREAM_16BIT_STEREO;
    mystream.timer = MM_TIMER2;
    mystream.manual = 1;
    
 	mmStreamOpen( &mystream );
}

void loadmp3() {
	myMp3 = fopen("/ddr/song.mp3", "rb");
	fillmp3();
}

/*void fillmp3() {
	int res;
	while (1) {
		if (mad_frame_decode(&frame, &stream)) {
			cout << "\n" << mad_stream_errorstr(&stream);
			if (stream.error == MAD_ERROR_BUFLEN) {
				cout << "\nrecargando buffer";
				int remlen = stream.bufend - stream.next_frame;
				//cout << "\nremlen: " << remlen;
				//cout << "\nat next_frame 0 " << *(u16*)stream.next_frame;
				//cout << "\nat readbuffer 0 before " << *(u16*)readbuffer;
				memmove(readbuffer, stream.next_frame, remlen);
				//cout << "\nat readbuffer 0 " << *(u16*)readbuffer;
				res = fread(readbuffer + remlen, 1, 2000 - remlen, myMp3);
				//cout << "\n" << remlen + res;
				mad_stream_buffer(&stream, readbuffer, res + remlen);
			} else if (stream.error == MAD_ERROR_BUFPTR) {
				cout << "\nptr" << (stream.next_frame == NULL);
				res = fread(readbuffer, 1, 2000, myMp3);
				mad_stream_buffer(&stream, readbuffer, 2000);
			} else if (stream.error != MAD_ERROR_LOSTSYNC) {
				cout << "\nerror";
				break;
			}
		} else {
			
		}
	}
}*/
u_char* guard;
const int bufsize = 1500;
u_char readbuffer[bufsize];
void fillmp3() {
	int res;
	int rem = 0;
	if (stream.next_frame != NULL) {
		rem = stream.bufend - stream.next_frame;
		memmove(readbuffer, stream.next_frame, rem);
		res = fread(readbuffer + rem, 1, bufsize - rem - MAD_BUFFER_GUARD + 1, myMp3);
	} else {
		res = fread(readbuffer, 1, bufsize - MAD_BUFFER_GUARD + 1, myMp3);
	}
	if (res + rem <= bufsize - MAD_BUFFER_GUARD) {
		cout << "\n" << (res + rem);
		guard = readbuffer + res + rem;
		memset(guard, 0, MAD_BUFFER_GUARD);
		mad_stream_buffer(&stream, readbuffer, res + rem + MAD_BUFFER_GUARD);
	} else {
		mad_stream_buffer(&stream, readbuffer, bufsize - MAD_BUFFER_GUARD + 1);
	}
}
int f = 0;
mm_word streammp3(mm_word length, mm_addr dest, mm_stream_formats format) {
	s16* output = (s16*)dest;
	uint samples = 0;
	while (samples < length) {
		for(int i = f; i < synth.pcm.length; i++){
			s16 sample;
			sample = MadFixedToSshort(synth.pcm.samples[0][i]);
			*(output++) = sample;
			if(MAD_NCHANNELS(&frame.header)==2){
				sample=MadFixedToSshort(synth.pcm.samples[1][i]);
			}
			*(output++) = sample;
			samples++;
			f++;
			if (samples == length) {
				return samples;
			}
		}
		if (mad_frame_decode(&frame, &stream)) {
			//cout << "\n" << mad_stream_errorstr(&stream);
			if (stream.error == MAD_ERROR_BUFPTR || stream.error == MAD_ERROR_BUFLEN) {
				if (stream.this_frame != guard) {
					fillmp3();
				} else {
					mmStreamClose();
					break;
				}
			} else if (stream.error != MAD_ERROR_LOSTSYNC) {
				mmStreamClose();
				break;
			}
		} else {
			mad_synth_frame(&synth, &frame);
			f = 0;
		}
	}
	return samples;
}

void loadogg() {
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
}

int eof = 0;
int current_section;
mm_word streamogg(mm_word length, mm_addr dest, mm_stream_formats format) {
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

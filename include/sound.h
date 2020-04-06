#ifndef SOUND
#define SOUND
#define MAXMODBUFFER 1600
#define MP3BUFFER 4800
#include <string>
#include <stdio.h>
#include <maxmod9.h>
#include <mad.h>
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>
struct mp3 {
	struct mad_stream stream;
	struct mad_frame frame;
	struct mad_synth synth;
	u_char buffer[MP3BUFFER];
	int f = 0;
	u_char *guard = NULL;
};
struct ogg {
	OggVorbis_File vf;
};
struct audio {
	FILE* inbuf = NULL;
	mm_ds_system sys;
	mm_stream stream;
	struct mp3* mp3 = NULL;
	struct ogg* ogg = NULL;
	audio();
	void end();
};
bool loadAudio(std::string path);
bool loadMp3();
void fillMp3();
mm_word mm_mp3_callback(mm_word length, mm_addr dest, mm_stream_formats format);
bool loadOgg();
mm_word mm_ogg_callback(mm_word length, mm_addr dest, mm_stream_formats format);
bool playAudio();
bool idleAudio();
#endif
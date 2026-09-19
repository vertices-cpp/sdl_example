#include "Sound_Example.h"
#include<string>
#include<cmath>

//#include <algorithm>
using namespace std;
  

#define PI2 6.28318530718f

static float cycle = 0;
static int audioVolume = 32000;
static int samplesFreq = 44100;
static int curFreq = 440;

static int bytesPerSample;
static int gBufferByteMaxPosition;
static int gBufferBytePosition;

short* musicData = NULL;

 Sound_Example::Sound_Example()
 {
	 SDL_AudioSpec spec, aspec;

	 SDL_zero(spec);
	 spec.freq = samplesFreq;			// 每秒（单个）样本数（Hz） 

 //	spec.format = AUDIO_S16LSB;	// AUDIO_S16LSB; // 位深度，16
	 spec.format = AUDIO_S16SYS;
	 // 位振幅值
	 spec.channels = 1;			// 立体声，左声道和右声道
	 spec.samples = 4096;		// 音频缓冲区的大小（以样本帧为单位）（每帧两个样本，左声道和右声道）。

	 spec.callback = forwardCallback;	// 当声音设备需要数据时调用
	 aspec.userdata = this;

	 SDL_Log("%s", SDL_GetError());

	 if ((id = SDL_OpenAudioDevice(nullptr, 0, &spec, &aspec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE)) <= 0)
	 {
		 fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		 exit(-1);
	 }
	 //	aspec.size = 0x2000;

	 bytesPerSample = aspec.channels * sizeof(aspec.format);
 
	 InitSinWave(1280);
 }


 void  Sound_Example::InitSinWave(int max_wave_length) {
		#include "sin_wave.inc"	 
	 add(max_wave_length);
	 outText;
 }
 void  Sound_Example::InitSquareWave(int max_wave_length) {
	#include "square_wave.inc"	 
 }
 void Sound_Example::callback(Uint8 * stream, int len)
 {

	 short *snd = (short*)stream;
	 int typeLength = sizeof(*snd);
	 for (int i = 0; i < len / typeLength; ++i)
	 {
		 if (gBufferBytePosition < gBufferByteMaxPosition)
		 {
			 snd[i] = musicData[gBufferBytePosition];
			 gBufferBytePosition++;
		 }
		 else
			 snd[i] = 0;

	 }
 }
void Sound_Example::free() {
	if (musicData != NULL)
	{
		delete[]musicData;
		musicData = NULL;
	}
}
void Sound_Example::test()
{
	if (gBufferBytePosition >= gBufferByteMaxPosition && musicData != NULL)
	{
		SDL_PauseAudioDevice(id, SDL_TRUE);
		free();
	}

}
  
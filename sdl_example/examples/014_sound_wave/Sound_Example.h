#ifndef _SDL_SOUND_EXAMPLE_H_
#define _SDL_SOUND_EXAMPLE_H_

#pragma 


#if defined(_WIN32)||defined(_WIN64)
#include <SDL.h> 
//#include <SDL_mixer.h>
#include <SDL_audio.h>
#else
#include <SDL2/SDL.h> 
//#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_audio.h>
#endif
#include<string>
using namespace std;

#include "SDLView.h"



struct  Sound_Example
{
	

	int id;
 
	//static int cnt;
 
 
  public:
	  static Sound_Example *getInstance( )
	  {
		  static Sound_Example _instance;
		  return &_instance;
	  }
	Sound_Example();
	~Sound_Example()
	{
		free();
		SDL_CloseAudioDevice(id);
	}

	void InitSinWave(int max_wave_length);
	void InitSquareWave(int max_wave_length);
 

	static void forwardCallback(void *userdata, Uint8 *stream, int len)
	{
		static_cast<Sound_Example*>(userdata)->callback(stream, len);
	}


	void callback(Uint8 * stream, int len);

	void free();

	void test();
};

 

#endif
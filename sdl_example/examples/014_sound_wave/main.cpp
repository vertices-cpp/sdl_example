
#if defined(_WIN32)||defined(_WIN64)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_audio.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_audio.h>
#endif
#include <cmath>
#include "path_head.h"

#include "SDLView.h"
#include "Sound_Example.h"


/* 
   #include <chrono> #include <thread> #include<functional> #include<sstream> */

using namespace std;

int main(int, char **)
{
	SDLView *e = SDLView::getInstance();
	e->openFont(og::checkPath("014_sound_wave/msyh.ttf").c_str(), 30);
	e->Message("6", 0, 20);

	auto  s = Sound_Example::getInstance();
//	s->InitSinWave(128);
//	s->setObj(e);

	while (!e->event())
	{
		s->test( );
	}
	return 0;
}

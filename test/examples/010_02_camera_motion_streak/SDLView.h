#ifndef _SDL_VIEW_H_
#define _SDL_VIEW_H_
#pragma 

#include<iostream>
#include<string>
#include "OGPlatformMacros.h"
#include "OGMathGeometry.h"

#if defined(_WIN32)||defined(_WIN64)
#include <SDL.h> 
#else
#include <SDL2/SDL.h> 
#endif
#define uint32 unsigned int

OG_BEGIN

struct   SDLView
{
public:
	SDL_Window *win;
	SDL_Renderer *ren; 
	 Vec2 _mousePos;
	 Size _winSize;

	static SDLView *getInstance()
	{
		static SDLView _instance;
		return &_instance;
	}
	SDLView()  :win(NULL), ren(NULL) 
	{
	}
	void init(int x = SDL_WINDOWPOS_UNDEFINED,int y = SDL_WINDOWPOS_UNDEFINED ,int w = 640,int h=480) {
#if defined(_WIN32)||defined(_WIN64)
		//SDL_setenv("SDL_AUDIODRIVER", "directsound", 1);
#endif
		 SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		win = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h,
				SDL_WINDOW_SHOWN);
		if (win == NULL)
			exit(1);
		ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (ren == NULL)
			exit(1);

		_winSize.width = w, _winSize.height = h;
		SDL_Log("%s", SDL_GetError());
	}
	 
	~SDLView()
	{ 
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		 
		SDL_Quit();
	}
	 
	Vec2 GetMousePos() { return _mousePos; }
	Size GetWinSize() { return _winSize; }
	SDL_Renderer* getRender() { return ren; }
	 
	SDLView* cls() {
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		return this;
	}
	SDLView* refresh() {
		SDL_RenderPresent(ren);
		return this;
	} 
	bool event()
	{
		bool quit = false;
		SDL_Event e;
		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				quit = true;
			else if (e.type == SDL_MOUSEMOTION)
			{
				_mousePos.x = e.motion.x;
				_mousePos.y = e.motion.y;
			}
		} 
		return quit;
	} 
private:
	SDLView(const SDLView&);
};
 
OG_END

#endif

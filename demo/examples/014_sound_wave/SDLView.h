#ifndef _SDL_VIEW_H_
#define _SDL_VIEW_H_
#pragma 

#include "Sound_Example.h"
#include<iostream>
#include<string>
using namespace std;

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
#define uint32 unsigned int

struct   SDLView
{
public:
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Texture *mTexture;
	TTF_Font *font;

	static SDLView *getInstance()
	{
		static SDLView _instance;
		return &_instance;
	}
	SDLView() ;
	
	void freeTexture()
	{
		if (mTexture)
			SDL_DestroyTexture(mTexture);
	}
	~SDLView()
	{
		freeTexture();
		if (font)
		{
			TTF_CloseFont(font);
			font = NULL;
		}

		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	}
	void openFont(const char *ttf, int fontSize)
	{
		font = TTF_OpenFont(ttf, fontSize);
		if (font == NULL)
			exit(1);
	}

	void *mPixels;

	int mPitch;
	int x, y, mw, mh;
	bool Message(const string &text, int cx = 0, int cy = 0, SDL_Color c = { 255,255,255,255 })
	{
		Message(text.c_str(), cx, cy, c);
		return true;
	}
	bool Message(const char *msg, int cx, int cy, SDL_Color c = { 255,255,255,255 });
	SDLView* cls() {
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		return this;
	}
	SDLView* refresh() {
		SDL_RenderPresent(ren);
		return this;
	}

	SDLView* render() {
		SDL_Rect clip = { 0,0,mw,mh };
		SDL_Rect renderQuad = { x,y,mw * 2,mh };
		SDL_RenderCopy(ren, mTexture, &clip, &renderQuad);
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
		}
		callBack();
		return quit;
	}
	void callBack() {
		this->cls()->render()->refresh();
	}
private:
	SDLView(const SDLView&);
};

//extern SDLView *global_view;
static std::string text;
#define outText  SDLView::getInstance()->Message(text)
#define add(x) text = to_string(x);
#define adds(x) text.assign(x);
#endif

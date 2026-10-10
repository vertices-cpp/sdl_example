#ifndef _SDL_MANAGER_H_
#define _SDL_MANAGER_H_
#pragma 
 
#include<iostream>
#include<string> 

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

class AudioEngine;

struct   SDLmanager
{
public:
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Texture *mTexture;
	TTF_Font *font;

	static SDLmanager *getInstance()
	{
		static SDLmanager _instance;
		return &_instance;
	}
	SDLmanager() ;
	
	void freeTexture()
	{
		if (mTexture)
			SDL_DestroyTexture(mTexture);
	}
	~SDLmanager()
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
	bool Message(const std::string &text, int cx = 0, int cy = 0, SDL_Color c = { 255,255,255,255 })
	{
		Message(text.c_str(), cx, cy, c);
		return true;
	}
	bool Message(const char *msg, int cx, int cy, SDL_Color c = { 255,255,255,255 });
	SDLmanager* cls() {
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		return this;
	}
	SDLmanager* refresh() {
		SDL_RenderPresent(ren);
		return this;
	}

	SDLmanager* render() {
		SDL_Rect clip = { 0,0,mw,mh };
		SDL_Rect renderQuad = { x,y,mw * 2,mh };
		SDL_RenderCopy(ren, mTexture, &clip, &renderQuad);
		return this;
	}
	bool event();
	void callBack() {
		this->cls()->render()->refresh();
	}
private:
	SDLmanager(const SDLmanager&);
};
 

#endif
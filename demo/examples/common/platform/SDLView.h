#ifndef _SDL_VIEW_H_
#define _SDL_VIEW_H_
 
#include "math/Vec2.h"
#include "math/OGGeometry.h"
#include "base/OGRef.h"
#include <SDL.h>

OG_BEGIN
 

class   SDLView:public Ref
{
public:
	SDL_Window *_mainWindow;
	SDL_Renderer *_renderer;
	 Vec2 _mousePos;
	 Size _winSize;
	 Rect _viewPortRect = Rect::ZERO;
	 const Uint8 *mKeyState;


	 Instance(SDLView);

	SDLView()  :_mainWindow(NULL), _renderer(NULL)
	{
	}
	bool init(int x = SDL_WINDOWPOS_UNDEFINED,int y = SDL_WINDOWPOS_UNDEFINED ,int w = 1000,int h=1000) {
#if defined(_WIN32)||defined(_WIN64)
		//SDL_setenv("SDL_AUDIODRIVER", "directsound", 1);
#endif
		 SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
		SDL_SetHint(/*SDL_HINT_RENDER_SCALE_QUALITY*/SDL_HINT_MOUSE_TOUCH_EVENTS, "1");
		_mainWindow = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h,
				SDL_WINDOW_SHOWN);
		if (_mainWindow == NULL)
			exit(1);
		_renderer = SDL_CreateRenderer(_mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (_renderer == NULL)
			exit(1);

		_winSize.width = (float)w, _winSize.height = (float)h;
		SDL_Log("%s", SDL_GetError());

		setRenderBlended();

		return true;
	}
	SDL_Window* getWindow() { return _mainWindow; }
	int getMaxTextureSize() {
		SDL_RendererInfo info;
		SDL_GetRendererInfo(_renderer, &info);
		int maxSize = info.max_texture_width;
		return maxSize;
	}
	 
	~SDLView()
	{ 
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_mainWindow);
		 
		SDL_Quit();
	}
	void setRenderBlended() {
		SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	}

	void setIMEKeyboardState(bool open) {
		if (open)
		{
			SDL_StartTextInput();
		}
		else
			SDL_StopTextInput();
	}
 

	void setScissorInPoints(float x, float y, float w, float h);

	void resetScissor();

	bool isScissorEnabled();

	Rect getScissorRect() const;

	Vec2 getMousePos();
	   Size getWindowSize();
	   Size getWindowScale() const;
	SDL_Renderer* getRender() { return _renderer; }
	 
	SDLView* cls() {
		SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
		SDL_RenderClear(_renderer);
		return this;
	}
	SDLView* refresh() {
		SDL_RenderPresent(_renderer);
		return this;
	} 
	void KeyDown() { mKeyState = SDL_GetKeyboardState(nullptr); }
	bool event();
private:
	SDLView(const SDLView&);
};


 
OG_END

#endif

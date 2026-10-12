#ifndef _SDL_VIEW_H_
#define _SDL_VIEW_H_

#include "platform/OGHeader.h"
 
#include "math/Vec2.h"
#include "math/OGGeometry.h"
 

OG_BEGIN
 

class   SDLView
{
public:
	SDL_Window *win;
	SDL_Renderer *render; 
	 Vec2 _mousePos;
	 Size _winSize;
	 const Uint8 *mKeyState;
	 Instance(SDLView);

	SDLView()  :win(NULL), render(NULL)
	{
	}
	bool init(int x = SDL_WINDOWPOS_UNDEFINED,int y = SDL_WINDOWPOS_UNDEFINED ,int w = 1000,int h=1000) {
#if defined(_WIN32)||defined(_WIN64)
		//SDL_setenv("SDL_AUDIODRIVER", "directsound", 1);
#endif
		 SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
		SDL_SetHint(/*SDL_HINT_RENDER_SCALE_QUALITY*/SDL_HINT_MOUSE_TOUCH_EVENTS, "1");
		win = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h,
				SDL_WINDOW_SHOWN);
		if (win == NULL)
			exit(1);
		render = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (render == NULL)
			exit(1);

		_winSize.width = (float)w, _winSize.height = (float)h;
		SDL_Log("%s", SDL_GetError());

		setRenderBlended();

		return true;
	}
	int getMaxTextureSize() {
		SDL_RendererInfo info;
		SDL_GetRendererInfo(render, &info);
		int maxSize = info.max_texture_width;
		return maxSize;
	}
	 
	~SDLView()
	{ 
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(win);
		 
		SDL_Quit();
	}
	void setRenderBlended() {
		SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
	}

	void setIMEKeyboardState(bool open) {
		if (open)
		{
			SDL_StartTextInput();
		}
		else
			SDL_StopTextInput();
	}
	 
	  Vec2 getMousePos();
	   Size getWindowSize();
	SDL_Renderer* getRender() { return render; }
	 
	SDLView* cls() {
		SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
		SDL_RenderClear(render);
		return this;
	}
	SDLView* refresh() {
		SDL_RenderPresent(render);
		return this;
	} 
	void KeyDown() { mKeyState = SDL_GetKeyboardState(nullptr); }
	bool event();
private:
	SDLView(const SDLView&);
};


 
OG_END

#endif

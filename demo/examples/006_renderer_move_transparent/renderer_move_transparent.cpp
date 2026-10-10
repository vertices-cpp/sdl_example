#if defined(_WIN32)||defined(_WIN64)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <SDL_image.h>
#include <SDL_syswm.h>
#include <windows.h>
#include <vector>
using std::vector;

const int Width = 640, Height = 480;

SDL_Window *gWin; SDL_Renderer* gRenderer;
int rectangle_size = 100, Total_Buttons = 2, my_sprite_position = 20;
vector<SDL_Point> mPisition{ {my_sprite_position,my_sprite_position},{rectangle_size- my_sprite_position,rectangle_size- my_sprite_position} };
vector<SDL_Color> setRectCheekColor{ {} };

bool SetWindowTransparent(COLORREF colorKey) {
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(gWin, &wmInfo);
	HWND hWnd = wmInfo.info.win.window;

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE )| WS_EX_LAYERED);
	return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}
void init() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	gWin = SDL_CreateWindow("测试", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, Width, Height, /*SDL_WINDOW_SHOWN*/SDL_WINDOW_BORDERLESS);
	gRenderer = SDL_CreateRenderer(gWin, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_MOD);
}
void Close() {
	SDL_DestroyWindow(gWin);
	SDL_DestroyRenderer(gRenderer);
	SDL_Quit();
}
int main(int argc,char *argv[])
{
	init();
	bool q = false;
	int mouse_key_press[2]{ 0 };
	SDL_Event e;
	SDL_Color preRectColor[] = { {255,255,255,0},{0,0,255,255 },{ 0,255,0,255},{255,0,0,255} };
	while (!q)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				q = true;
			else if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
				for (size_t i=0;i<mPisition.size();++i)
				{
					int x, y;
					SDL_GetMouseState(&x, &y);
					if (x < mPisition[i].x || x > mPisition[i].x + rectangle_size ||
						y < mPisition[i].y || y>mPisition[i].y + rectangle_size) 
						mouse_key_press[i] = 0;
					else {
						switch (e.type)
						{
						case SDL_MOUSEMOTION:
							mouse_key_press[i] = 1;
							break;
						case  SDL_MOUSEBUTTONDOWN:
							mouse_key_press[i] = 2;
							break;
						case  SDL_MOUSEBUTTONUP:
							mouse_key_press[i] = 3;
							break;
						}
					}
				}
			}
		}
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);
		for (size_t i = 0; i < Total_Buttons; ++i)
		{
			SDL_SetRenderDrawColor(gRenderer, preRectColor[mouse_key_press[i]].r,
				preRectColor[mouse_key_press[i]].g,
				preRectColor[mouse_key_press[i]].b,
				preRectColor[mouse_key_press[i]].a/3);

			for (int j = 1; j <= 4; ++j)
			{
				SDL_Rect renderQuad = { mPisition[i].x - j,mPisition[i].y - j,rectangle_size + j*2,rectangle_size + j * 2 };
				SDL_RenderDrawRect(gRenderer, &renderQuad);
			}
		}
		
		SDL_SetRenderDrawColor(gRenderer, 0, 255, 255, 255);
		SDL_Rect renderA[] = { {mPisition[0].x,mPisition[0].y,rectangle_size,rectangle_size},
		{mPisition[1].x,mPisition[1].y,rectangle_size,rectangle_size} };
		SDL_RenderFillRect(gRenderer, renderA);
		SDL_SetRenderDrawColor(gRenderer, 255, 0, 255, 255);
		SDL_RenderFillRect(gRenderer, renderA+1);

		SetWindowTransparent(RGB(255, 255, 255));

		SDL_RenderPresent(gRenderer);
	}
	Close();
	return 0;
}
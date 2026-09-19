#include <SDL.h>
#include <SDL_image.h>
#include <SDL_syswm.h>
#include "path_head.h"

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <iostream>
using std::cout; using std::endl;
#include <windows.h>




const int Width = 1080, Height = 480, TotalSprite = 3;
SDL_Window *gWin;
SDL_Surface *gSurface, *gSpriteClips[TotalSprite];


HWND hwnd;
SDL_SysWMinfo info;

/* 初始化 */
void load(const string&);
void Init() {
	SDL_Init(SDL_INIT_VIDEO);
	gWin = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(gWin, &info))
	{
		hwnd = info.info.win.window;
	}
	/*设置窗口colorkey*/
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, RGB(255, 0, 255), 0, LWA_COLORKEY);
	/*设置窗口为悬浮窗 */
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);


	gSurface = SDL_GetWindowSurface(gWin);
	load(og::checkPath("005_surface_move_transparent/mario1_move"));
}
void load(const string &path) {
	for (int i = 0; i < TotalSprite; ++i)
	{
		gSpriteClips[i] = SDL_LoadBMP((path + std::to_string(i)+".bmp").c_str());
		if (gSpriteClips[i] == NULL)
			cout << "载入错误: " << SDL_GetError() << endl;
		SDL_SetColorKey(gSpriteClips[i], SDL_TRUE, SDL_MapRGB(gSpriteClips[i]->format, 255, 0, 255));
	}
}
/* 释放它 */
void Close() {
	for (int i=0;i<TotalSprite;++i)
	{
		SDL_FreeSurface(gSpriteClips[i]);
	}
	SDL_DestroyWindow(gWin);
	SDL_Quit();
}
int main(int argc,char *argv[])
{
	Init();
	bool quit = false;
	SDL_Event e;
	int i = 0;
	SDL_Surface *gCurrentScr = gSpriteClips[i];
	SDL_Rect SpriteRect = { 0,0,32,64 };
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				quit = true;
		}
		SDL_FillRect(gSurface, NULL, SDL_MapRGB(gSurface->format, 255, 0, 255));
		
		gCurrentScr = gSpriteClips[i/3];
		SDL_BlitScaled(gCurrentScr, NULL, gSurface, &SpriteRect);
		i++;
		if (i / 3 >= 3)
			i = 0;
		SpriteRect.x+=2;
		SDL_UpdateWindowSurface(gWin);
		SDL_Delay(1000 / 60);
	}
	Close();
	return 0;
}
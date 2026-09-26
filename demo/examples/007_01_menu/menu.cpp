#include<vector>
#include<string>
#include "path_head.h"
using namespace std;
#if defined(_WIN32)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif
int WIDTH = 640, HEIGHT = 480;
SDL_Window *gWindow;
SDL_Renderer *gRenderer;
TTF_Font* gFont;
class LTexture
{
	SDL_Texture *mTexture;
	int mWidth, mHeight;
public:
	void loadtext(const char *text, SDL_Color a)
	{
		free();
		SDL_Surface *load = TTF_RenderUTF8_Blended(gFont, text, a);
		SDL_SetColorKey(load, SDL_TRUE, SDL_MapRGB(load->format, 0, 255, 255));
		mTexture = SDL_CreateTextureFromSurface(gRenderer, load);
		mWidth = load->w, mHeight = load->h;
		SDL_FreeSurface(load);
	}
	void loadFile(const char *path)
	{
		free();
		SDL_Surface *load = IMG_Load(path);
		SDL_SetColorKey(load, SDL_TRUE, SDL_MapRGB(load->format, 0, 255, 255));
		mTexture = SDL_CreateTextureFromSurface(gRenderer, load);
		mWidth = load->w, mHeight = load->h;
		SDL_FreeSurface(load);
	}
	void render(int x, int y, SDL_Rect * clip = NULL, double angle = .0, SDL_Point * center =
		NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };
		if (clip)
			renderQuad.w = clip->w, renderQuad.h = clip->h ;
		SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	}
	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }
	void setColor(int r, int g, int b)
	{
		SDL_SetTextureColorMod(mTexture, r, g, b);
	}
	void setAlpha(int a)
	{
		SDL_SetTextureAlphaMod(mTexture, a);
	}
	void SetTextureBlendMode(SDL_BlendMode blended) {
		SDL_SetTextureBlendMode(mTexture, blended);
	}
	void setDimension(int w, int h) {
		mWidth = w, mHeight = h;
	}
	void free()
	{
		if (mTexture)
			SDL_DestroyTexture(mTexture);
	}
};

LTexture gText;
LTexture gBg;
void init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
// 	SDL_DisplayMode dm;
// 	SDL_GetCurrentDisplayMode(0, &dm);
// 	WIDTH = dm.w, HEIGHT = dm.h;
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	gWindow =
		SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	gRenderer =
		SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	gFont = TTF_OpenFont(og::checkPath("007_01_menu/msyh.ttf").c_str(), 28);
	if (!gFont) {
		printf("TTF_OpenFont failed: %s\n", TTF_GetError());
	}
}

void close()
{
	gText.free();
	TTF_CloseFont(gFont);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void loadMedia()
{
// 	gBg.loadFile("window.png");
// 	gBg.setDimension(WIDTH, HEIGHT);
}

SDL_Color TextColor[2] = { {192,192,192,192},{0,255,0,255} };

struct Label
{
	SDL_Rect mPosition;
	int item_nums,tigger, select_item;
	string text, *items;

public:
	Label(const string &s, int tigger_,string *item_, int n=0)
		:text(s),tigger(tigger_), items(item_), item_nums(n)
	{
		 select_item = -1;
	}
	void show()
	{
	
	}
	int HandleEvent(SDL_Point &MousePosition) {
		
		for (int i=0;i<item_nums;++i)
		{
			SDL_Rect tmp = mPosition;
			tmp.y *= i;
			if (SDL_PointInRect(&MousePosition, &tmp))
				select_item = i;
		}

		return select_item;
	}
};
class MenuBar {
 	Label* lable;
	int lable_nums, select_lable;
	int mWidth, mHeight; 
 public:
	 MenuBar(Label *lable_,int num,int w,int h) :lable(lable_), lable_nums(num) ,mWidth(w),mHeight(h){
		 int currentPos = 0;
		 select_lable = -1;
		 for (int i=0;i< lable_nums;++i)
		 {
			 lable[i].mPosition = { currentPos,0,mWidth,mHeight };
			 currentPos += mWidth;
		 }
 	}

	 void handleTarger(SDL_Event &e) {
		 SDL_Point mousePos = { e.motion.x,e.motion.y };
		 if (e.type == SDL_MOUSEBUTTONDOWN) {
			
			 bool click = false;
			 for (int i=0;i< lable_nums;i++)
			 {
				 if (SDL_PointInRect(&mousePos, &lable[i].mPosition))
				 {
					 select_lable = i;
					 click = true;
				 }
			 }
			 if (!click)
			 {
				 select_lable = -1;
				 for (int i = 0; i < lable_nums; ++i)
				 {
					 lable[i].select_item = -1;
				 }
			 }
		 }
		 else if (e.type == SDL_MOUSEMOTION) {
			 if (select_lable!=-1)
			 {
				 for ( int i = 0; i <lable[select_lable].item_nums; i++)
				 {
					 SDL_Rect tmp = { lable[select_lable].mPosition.x ,
						 lable[select_lable].mPosition.y + (i+1)*mHeight ,
						  lable[select_lable].mPosition.w,
					  lable[select_lable].mPosition.h
					 };
					 if (SDL_PointInRect(&mousePos, &tmp))
						 lable[select_lable].select_item = i;
				 }
			 }
		 }
	 }

	 void draw()
	 {
		 for (int i = 0; i < lable_nums; ++i)
		 {
			 drawMenu(lable[i], select_lable == i);
		 }
		 //select_lable = -1;
	 }
	 void drawText(Label lable, bool is_selected,const string &text,int x,int y) {
		 
		 SDL_SetRenderDrawColor(gRenderer, TextColor[is_selected].r, TextColor[is_selected].g, TextColor[is_selected].b, TextColor[is_selected].a);
		 SDL_Rect tmp = lable.mPosition;
		 tmp.x = x, tmp.y = y;
		 SDL_RenderDrawRect(gRenderer, &tmp);
		 gText.loadtext(text.c_str(), { TextColor[is_selected].r,TextColor[is_selected].g,TextColor[is_selected].b });
		 gText.setDimension(mWidth, mHeight);
		 gText.render(x, y);
	 }
	 void drawMenu(Label lable, bool is_selected)
	 {
		 int start_x = lable.mPosition.x;
		 string text = lable.text;
		 drawText(lable, is_selected, text, lable.mPosition.x, lable.mPosition.y);
		 if (is_selected)
			 drawMenuItems(lable);
	 }

	 void drawMenuItems(Label lable)
	 {
		 for (int i = 0; i < lable.item_nums; ++i)
		 {
			 drawText(lable, i == lable.select_item, lable.items[i].c_str(), lable.mPosition.x ,mHeight * (i+1));
		 }
	 }
 };

int main(int, char **)
{
	init();
	loadMedia();
#if defined(_WIN32)
	string menu1[] = { u8"新建", u8"打开", u8"保存",u8"退出" },
		menu2[] = { u8"拷贝", u8"剪切",u8"粘贴" }, 
		menu3[] = { u8"设置", u8"帮助",u8"关于" };

	Label lable[]{
	   {u8"文件", 1, menu1, 4},
	   {u8"编辑", 2, menu2, 3},
	   {u8"选项", 3, menu3, 3},
	};

	/* 创建菜单 */
	MenuBar menubar = MenuBar(lable, 3,50,30);
#else
	string menu1[] = { "新建", "打开", "保存","退出" },
		menu2[] = { "拷贝", "剪切","粘贴" },
		menu3[] = { "设置", "帮助","关于" };

	Label lable[]{
	   {"文件", 1, menu1, 4},
	   {"编辑", 2, menu2, 3},
	   {"选项", 3, menu3, 3},
	};

	/* 创建菜单 */
	MenuBar menubar = MenuBar(lable, 3, 200, 200);
#endif
	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				quit = true;
			menubar.handleTarger(e);
		}
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);
// 		gBg.render(0, 0);
 		menubar.draw();
		
		SDL_RenderPresent(gRenderer);
	}

	close();

	return 0;
}

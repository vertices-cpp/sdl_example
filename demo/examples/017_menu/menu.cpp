#include<vector>
#include<string>

#define  PATH_RES

#include "path_head.h" 
#include "SDLView.h"
#include "OGTexture2D.h"
using namespace std;
#if defined(_WIN32)
#include <SDL.h>
 //#include <SDL_image.h>
// #include <SDL_ttf.h>

#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif


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

OG::Texture2D gtext;

void  render(int x, int y, OG::Texture2D *tex)
{
	auto size = tex->getContentSize();
	/* 设置渲染空间并渲染到屏幕 */
	SDL_Rect renderQuad = { x, y, size.width, size.height };

	/* 设置剪辑渲染尺寸 */
// 	if (clip != NULL)
// 	{
// 		renderQuad.w = clip->w;
// 		renderQuad.h = clip->h;
// 	}

	/* 渲染到屏幕 */
	SDL_RenderCopyEx(OG::SDLView::getInstance()->getRender(), tex->getTexture(), 
		nullptr, &renderQuad, 0.0f, nullptr, SDL_FLIP_NONE);
}



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
		 
		 orange::FontDefinition fontDef;
		 fontDef._fontName = og::checkPath("003_n_order_bezier/msyh.ttf");  // 字体文件路径
		 fontDef._fontSize = 28;
		 fontDef._fontFillColor = orange::Color3B(0, 0, 0);   // 白色
		 fontDef._fontAlpha = 255;
		 fontDef._alignment = orange::TextHAlignment::LEFT;
		 fontDef._vertAlignment = orange::TextVAlignment::TOP;
		 fontDef._enableWrap = true;
		 fontDef._dimensions = orange::Size(500, 0);                 // 400 宽换行
		 fontDef._stroke._strokeEnabled = false;

		 gtext.initWithString(text.c_str(), fontDef);
		  render(x, y,&gtext);
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
	OG::SDLView *e = OG::SDLView::getInstance();
	e->init();
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
	MenuBar menubar = MenuBar(lable, 3,60,30);
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
	SDL_Event u;
	while (!quit)
	{
		while (SDL_PollEvent(&u))
		{
			if (u.type == SDL_QUIT)
				quit = true;
			menubar.handleTarger(u);
		}
		SDL_SetRenderDrawColor(e->getRender(), 255, 255, 255, 255);
		SDL_RenderClear(e->getRender());
		// 		gBg.render(0, 0);
		menubar.draw();

		SDL_RenderPresent(e->getRender());
	}

	return 0;
}

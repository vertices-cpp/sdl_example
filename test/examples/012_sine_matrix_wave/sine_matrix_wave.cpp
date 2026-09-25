//顶点示例  


#if defined(_MSC_VER) 
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#endif

#include <string>
using std::string;
#include <sstream>
using std::stringstream;
#include <iostream>
using std::cout;
using std::endl;

#include "path_head.h"

const int SCREEN_WIDTH = 600, SCREEN_HEIGHT = 800;
SDL_Window* window;
SDL_Renderer *render;
SDL_Texture *texture;
TTF_Font *font;

#include <vector>
std::vector<int> wav_channels;
std::vector<Mix_Chunk *>wavs;


#include <SDL_syswm.h>
#include <windows.h>

void SetWindowTransparent(COLORREF colorKey) {
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hWnd = wmInfo.info.win.window;

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE)/* | WS_EX_LAYERED*/);
	 SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	
}
void init() {
	/* 初始化SDL */
	SDL_Init(SDL_INIT_VIDEO);
	/* 线性纹理 */
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	/* 产生窗口 */
	window = SDL_CreateWindow("SDL Tutorial", 200, 300,
		SCREEN_WIDTH, SCREEN_HEIGHT, /*SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS |*/ SDL_WINDOW_HIDDEN);
	
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_SHOWN);


	/* 即将初始化的渲染器驱动程序的索引，如果是-1表示flags标志的第一个可用驱动程序。 */
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
 	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
 	SDL_RenderClear(render);
	//SetWindowTransparent(RGB(255, 255, 255));
	SetWindowTransparent(RGB(0, 0, 0));
	//SetWindowTransparent(RGB(255, 255, 255));
	SDL_ShowWindow(window);

	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

	//auto t = IMG_Init(IMG_INIT_PNG);
	/* 初始化png加载 */
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	Mix_Init(MIX_INIT_MP3);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
}

void close()
{
	//释放图像
	SDL_DestroyTexture(texture);

	//释放窗口
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);

	//退出子系统
	IMG_Quit();
	SDL_Quit();
}


void free()
{
	//释放纹理（如果存在）
	if (texture != NULL)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
	}
}

#include <mutex>
#include <functional>
#include <thread>
std::mutex _threadMutex; 

void play2d_func( ) {
	_threadMutex.lock();
	Mix_Chunk* wav = Mix_LoadWAV(og::checkPath("012_sine_matrix_wave/01.wav").c_str());
	int  channel = Mix_PlayChannel(-1, wav, 0);
	wav_channels.push_back(channel);
	wavs.push_back(wav);
	_threadMutex.unlock();
 
}



int main(int argc, char *argv[]) {

	
	init();
	

	bool quit = false;

	//事件处理
	SDL_Event e;
	
//  	font = TTF_OpenFont("c:\\windows\\Fonts\\stxingka.ttf", 80);
//  	std::string textMessage = u8" 2d 波浪特效哈哈";
//  	SDL_Surface*surface = TTF_RenderUTF8_Blended(font, textMessage.c_str(), SDL_Color{ 0,255,0,255 });
	SDL_Surface*surface = IMG_Load(og::checkPath("012_sine_matrix_wave/HelloWorld.png").c_str());
	texture = SDL_CreateTextureFromSurface(render, surface);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	int fontw = surface->w;
	int fonth = surface->h;
	SDL_FreeSurface(surface);

	int fontw2 = fontw /2 ;
	int fonth2 = fonth /2;

	int num = fontw/2;
	int vc = fontw /(fontw / num)  ;
	int wsize = fontw / vc  ;
	int hsize = fonth / vc  ;
	SDL_Vertex *verts = new SDL_Vertex[vc * 4];
	/*for (int i = 0; i < vc; i++)
	{
	
		float m1 = i * (float)vsize;
		float m2 = i * (float)vsize + (float)vsize;
		float m3 = 0;
		float m4 = fonth;

		verts[i * 4 + 0] = { {m1,m3}, {255, 0, 0, 255}, { m1 / fontw, m3 / fonth} };
		verts[i * 4 + 1] = { {m2,m3}, {0, 255, 0, 255}, {m2 / fontw,m3 / fonth} };
		verts[i * 4 + 2] = { {m1,m4}, {255, 0, 0, 255}, {m1/ fontw,m4/fonth} };
		verts[i * 4 + 3] = { {m2,m4}, {0, 255, 255, 255}, {m2 / fontw,m4 / fonth} };
	}*/
	//映射到顶点的坐标，颜色，UV裁剪
	for (int i = 0; i < vc; i++)
	{

		float m1 = i * (float)wsize;
		float m2 = i * (float)wsize + (float)wsize;
		float m3 = 0;
		float m4 = (float)fonth;

// 		float m3 = i * (float)hsize;
// 		float m4 = i * (float)hsize + (float)hsize;

		verts[i * 4 + 0] = { {m1,m3}, {255, 255, 255, 255}, { m1 / fontw, m3 / fonth} };
		verts[i * 4 + 1] = { {m2,m3}, {255, 255, 255, 255}, {m2 / fontw,m3 / fonth} };
		verts[i * 4 + 2] = { {m1,m4}, {255, 255, 255, 255}, {m1 / fontw,m4 / fonth} };
		verts[i * 4 + 3] = { {m2,m4}, {255, 255, 255, 255}, {m2 / fontw,m4 / fonth} };
	}
	
	int *indices = new int[vc * 6];
	//顶点索引
	for (int i = 0; i < vc; i++)
	{
		indices[i * 6 + 0] = { i * 4  + 0 };
		indices[i * 6 + 1] = { i * 4  + 1 };
		indices[i * 6 + 2] = { i * 4 + 2 };
		indices[i * 6 + 3] = { i * 4  + 1 };
		indices[i * 6 + 4] = { i * 4 + 2 };
		indices[i * 6 + 5] = { i * 4 + 3 };
	}
 

	float sx = 1., sy = 1;
	float x = 100, y = 100;
	int cx = 400, cy = 200;
	float rx = 0, ry = 0;
	int offset = 0;

	// y = A * sin(ωx + φ) + B
	/*
	A代表振幅，值越大，曲线上下幅度就越大，ω代表频率，值越大，一个周期的宽度越小，
	曲线就越尖锐，φ代表初相位，即原点处曲线的位置，B本身不影响正弦曲线的形状，只会让曲线整体上下移动。
	
	*/
	//调整频率 ，
	int A = 20;
	int w = 3;
	//修改一下B参数，让正弦波整体往上移
	int b = 20;
	float PI = 3.1415926f;

	float t1 = 0, t2 = 0;

	SDL_Vertex *verts_tmp = new SDL_Vertex[vc * 4];

	while (!quit)
	{
		//处理队列上的事件
		while (SDL_PollEvent(&e) != 0)
		{
			//用户请求退出
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case  SDLK_1:   //1键播放 
				{
					//多线程播放
 					
 					std::thread(play2d_func).detach();
 					 

					//普通播放
				//	play2d_func();

				}
					break;
				case SDLK_2:
				{
					for (int i =0;i<wavs.size();++i)
					{
						Mix_HaltChannel(wav_channels[i]);
						Mix_FreeChunk(wavs[i]);
					}
					wavs.clear();
					wav_channels.clear();
				}
				break;
				default:

					break;
				}
			}

			//处理窗口事件
		//	handleEvent(e);
		}
		
		//仅在未最小化时绘制
		//if (!mMinimized)
		{
			//清屏
			SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
			SDL_RenderClear(render);

			//渲染文本纹理
			//SDL_Rect renderQuadA = { 400,400 ,mWidth, mHeight };



			memset(verts_tmp, 0, vc * 4 * sizeof(SDL_Vertex));
			memcpy(verts_tmp, verts, vc * 4 * sizeof(SDL_Vertex));

			float radiansX =- (rx* 0.01745329252f);
			float radiansY =-(ry* 0.01745329252f);
			float cosx = cosf(radiansX);
			float sinx = sinf(radiansX);
			float cosy = cosf(radiansY);
			float siny = sinf(radiansY);
			 
		//	旋转部分
//  			for (int i = 0; i < vc; ++i)
//  			{
//  				int offset = i * 4;
//  				verts_tmp[offset + 0].position = {
//  					 fontw2 + (verts_tmp[offset + 0].position.x - fontw2)* cosy - (verts_tmp[offset + 0].position.y - fontw2)*sinx,
//  					 fonth2 + (verts_tmp[offset + 0].position.x - fonth2)*siny + (verts_tmp[offset + 0].position.y - fonth2)*cosx,
//  
//  				};
//  				verts_tmp[offset + 1].position = {
//  					 fontw2 + (verts_tmp[offset + 1].position.x - fontw2)* cosy - (verts_tmp[offset + 1].position.y - fontw2)*sinx,
//  					 fonth2 + (verts_tmp[offset + 1].position.x - fonth2)*siny + (verts_tmp[offset + 1].position.y - fonth2)*cosx,
//  				};
//  				verts_tmp[offset + 2].position = {
//  					 fontw2 + (verts_tmp[offset + 2].position.x - fontw2)* cosy - (verts_tmp[offset + 2].position.y - fontw2)*sinx,
//  					 fonth2 + (verts_tmp[offset + 2].position.x - fonth2)*siny + (verts_tmp[offset + 2].position.y - fonth2)*cosx,
//  				};
//  				verts_tmp[offset + 3].position = {
//  					 fontw2 + (verts_tmp[offset + 3].position.x - fontw2)* cosy - (verts_tmp[offset + 3].position.y - fontw2)*sinx,
//  					 fonth2 + (verts_tmp[offset + 3].position.x - fonth2)*siny + (verts_tmp[offset + 3].position.y - fonth2)*cosx,
//  				};
//  			}

// 			rx++;
// 			ry ++;
			int vc_cnt = 2 ;
			for (int i = 0; i < vc * 4; ++i)
			{
				 
					float tmp1 = A * (float)cos(w * (verts_tmp[i].position.y * (PI / 180)) + t1) + b;
					float tmp2 = A * (float)sin(w * (verts_tmp[i].position.x * (PI / 180)) + t2) + b;

					if (!(i&vc_cnt))
						tmp2 = -tmp2;

					verts_tmp[i].position = {
							verts_tmp[i].position.x /*+ tmp1*/,
							verts_tmp[i].position.y + tmp2
					};
	  

			}
			 
		//	t1 -= 2 * (PI / 180);
			t2 += 5 * (PI / 180);

 //坐标移动cx,cy.
//sx,sy为缩放
// 				for (int i = 0; i < vc; ++i)
// 				{
// 					int offset = i * 4;
// 					verts_tmp[offset + 0].position = {
// 						verts_tmp[offset + 0].position.x*sx + cx * (sx) ,
// 						verts_tmp[offset + 0].position.y*sy + cy * (sy)
// 					};
// 					verts_tmp[offset + 1].position = {
// 						verts_tmp[offset + 1].position.x*sx + cx * (sx) ,
// 						verts_tmp[offset + 1].position.y*sy + cy * (sy)
// 					};
// 					verts_tmp[offset + 2].position = {
// 						verts_tmp[offset + 2].position.x*sx + cx * (sx) ,
// 						verts_tmp[offset + 2].position.y*sy + cy * (sy)
// 					};
// 					verts_tmp[offset + 3].position = {
// 						verts_tmp[offset + 3].position.x*sx + cx * (sx) ,
// 						verts_tmp[offset + 3].position.y*sy + cy * (sy)
// 					};
// 				}
			
			SDL_RenderGeometry(render, texture, verts_tmp, vc * 4, indices, vc * 6);



			SDL_RenderPresent(render);
		}
	}

	//释放资源并关闭 SDL
	close();

	return 0;
}

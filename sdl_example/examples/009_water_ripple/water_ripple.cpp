// /*
// *		js代码来源， https://asset.uusama.com/example/water_ripple.html
// *		更改为C++ SDL版本，BY vertices c_cpp123
// *       根目录放上test.jpg或test.png
// *
// *
// *
// *
// */
// 
// #if defined(_WIN32)||defined(_WIN64)
// #include <SDL.h>
// #include <SDL_image.h>
// #elif defined(__ANDROID__)
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
// #endif
// #include <iostream>
// #include <vector>
// #include <algorithm> 
// #include <string>
// #include <fstream>
// 
//  
// using namespace std;
// 
//  
// int SCREEN_WIDTH = 640	, SCREEN_HEIGHT = 480;
// int TOTAL_PIXEL = SCREEN_WIDTH * SCREEN_HEIGHT * 4;
// 
// SDL_Window* window;
//  
// SDL_Renderer* renderer;
// 
// 
// struct  My_RGBA {
// 	int r, g, b, a;
// 	bool operator == (const My_RGBA &rhs)const
// 	{
// 		return r == rhs.r &&
// 			g == rhs.g &&
// 			b == rhs.b &&
// 			a == rhs.a;
// 	}
// 	bool operator != (const My_RGBA &rhs)const 
// 	{
// 		return r != rhs.r ||
// 			g != rhs.g ||
// 			b != rhs.b ||
// 			a != rhs.a;
// 	}
// 	friend ostream &operator<<(ostream& os,const My_RGBA &rhs)
// 	{
// 		return os << "( " << rhs.r << " " << rhs.g << " " << rhs.b << " " << rhs.a << " )";
// 	}
// };
// int debug = 0, use_motion = 0;
//  vector<My_RGBA> debug_1;
//   vector<My_RGBA> debug_2;
// 
// 
//   
// 
// // 初始化 SDL
// bool init() {
// 	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
// 		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
// 		return false;
// 	}
// 	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
// #if defined(__ANDROID__)
// 	SDL_DisplayMode dm;
// 	SDL_GetCurrentDisplayMode(0,&dm);
// 
// 	SCREEN_WIDTH = dm.w, SCREEN_HEIGHT = dm.h;
// #endif
// 	window = SDL_CreateWindow("Water Ripple Effect by vertices", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
// 		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
// 	if (window == nullptr) {
// 		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
// 		return false;
// 	}
// 	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
// 
// 	if (renderer == nullptr) {
// 		std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
// 		return false;
// 	}
// 	auto flag = IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG);
// 	if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)  ==  (IMG_INIT_PNG | IMG_INIT_JPG))) {
// 		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
// 		return false;
// 	}
// 	return true;
// }
// 
// // 清理资源
// void close() {
// 	  
// 	SDL_DestroyRenderer(renderer);
// 	IMG_Quit();
// 	SDL_Quit();
// }
// 
// class Texture2d {
// 	SDL_Texture *mTexture;
// 	void *mPixels;
// 	int mPitch;
// 	int m_width, m_height;
// 	int mPixelCount;
// public:
// 	Texture2d() {
// 		mTexture = NULL;
// 		mPixels = NULL;
// 		mPitch = 0;
// 		mPixelCount = 0;
// 	}
// 	~Texture2d() {
// 		free();
// 	}
// 	void free() {
// 		if (mTexture)
// 		{
// 			SDL_DestroyTexture(mTexture);
// 		}
// 	}
// 	void LockTexture() {
// 		SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch);
// 	}
// 	void UnlockTexture() {
// 		SDL_UnlockTexture(mTexture);
// 	}
// 	void getImageData(Uint8* vector_pixels)
// 	{
// 		LockTexture(); 
// 		memcpy(vector_pixels, mPixels, mPixelCount);
// 		  
// 		UnlockTexture();
// 	}
// 	int getWidth()const { return m_width; }
// 	int getHeight()const { return m_height; }
// 	int getPixelCount()const {
// 		return mPixelCount;
// 	}
// 	void putImageData( Uint8* vector_pixels,int x,int y)
// 	{
// 		LockTexture(); 
// 		int offset_pos = y * mPitch + x;
// 		memcpy(mPixels, vector_pixels, mPixelCount);
// 
// 		UnlockTexture();
// 	}
// 
// 	// 加载图像
// 	bool loadFrom(const std::string image) {
// 		free();
// 		SDL_Surface* loadedSurface = IMG_Load(image.c_str());
// 		if (loadedSurface == nullptr) {
// 			std::cerr << "Unable to load image! SDL_image Error: " << IMG_GetError() << std::endl;
// 			return false;
// 		}
// 		SDL_Surface *formatSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
// 
// 		mTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
// 			formatSurface->w,
// 			formatSurface->h);
// 
// 		if (mTexture == nullptr) {
// 			std::cerr << "Unable to create texture from surface! SDL Error: " << SDL_GetError() << std::endl;
// 			return false;
// 		}
// 		m_width = formatSurface->w;
// 		m_height = formatSurface->h;
// 		
// 		
// 
// 		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
// 
// 
// 		SDL_LockTexture(mTexture, &formatSurface->clip_rect, &mPixels, &mPitch);
// 
// 		memcpy(mPixels, formatSurface->pixels, formatSurface->h * formatSurface->pitch);
// 
// 	//	Uint32 * pixels = (Uint32 *)mPixels;
// 	//	int pixelCount = formatSurface->pitch / 4 * formatSurface->h;
// 		SDL_SetWindowSize(window,m_width, m_height);
// 		SDL_SetWindowPosition(window,SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED); 
// 		mPixelCount = m_height * mPitch;
// 		// 	for (int i =0;i < pixelCount;++i)
// 		// 	{
// 		// 		ripple[i] = pixels[i];
// 		// 	}
// 	//	TOTAL_PIXEL = mPixelCount;
// 		SDL_UnlockTexture(mTexture);
// 
// 		SDL_FreeSurface(formatSurface);
// 
// 		SDL_FreeSurface(loadedSurface);
// 		return true;
// 	}
// 	void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *pointer = NULL,
// 		SDL_RendererFlip flip = SDL_FLIP_NONE) {
// 		SDL_Rect renderQuad = { x,y,m_width,m_height };
// 		if (clip != NULL)
// 		{
// 			renderQuad.w = clip->w;
// 			renderQuad.h = clip->h;
// 		}
// 		SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, pointer, flip);
// 
// 		SDL_RenderPresent(renderer);
// 
// 	}
// };
// 
// struct  WaterSetting {
// 	std::string image;  // 背景图像的路径
// 	float	dropRadius;  // 波源半径大小
// 	int	width,  // 画布宽度
// 		height;  // 画布高度
// 	float	delay,  // 自动产生波源的延迟时间（秒）
// 		attenuation,  // 波的衰减级别
// 		maxAmplitude,  // 波的最大振幅
// 		sourceAmplitude ; // 震源振幅
// 	bool auto_value; // 是否自动产生波源
// };
// 
// struct  WaterRipple {
// 
// 	Texture2d texture2d;
// 
// 	WaterSetting &settings;
// 	WaterSetting defaults;
// 
// 
// 	int width,
// 		height;
// 
// 	float	dropRadius,
// 		delay,  // 将延迟时间转换为毫秒
// 		attenuation,  // 衰减级别
// 		maxAmplitude,  // 最大振幅
// 		sourceAmplitude;
// 
// 	int	half_width,  // 宽度的一半
// 		half_height,  // 高度的一半
// 		amplitude_size,  // 振幅数组的大小
// 		old_index,  // 旧振幅数组的索引偏移量
// 		new_index,  // 新振幅数组的索引偏移量
// 		map_index;  // 振幅数组索引
// 
// 	Uint8 *ripple,
// 	*texture;
// 	std::vector<int> ripple_map;  // 振幅数组
// 	std::vector<int>	last_map;
// 
// public:
// 	~WaterRipple() {
// 		if (ripple!=NULL)
// 		{
// 			delete[]ripple;
// 		}
// 		if (texture != NULL)
// 		{
// 			delete[]texture;
// 		}
// 	}
// 	WaterRipple(WaterSetting& settings_) :settings(settings_)
// 	{
// 		// 默认设置
// 		texture2d.loadFrom(settings_.image);
// 
// 		settings_.width = texture2d.getWidth();
// 		settings_.height = texture2d.getHeight();
// 		defaults.image = "",  // 背景图像的路径
// 			defaults.dropRadius = 3,  // 波源半径大小
// 			defaults.width = settings_.width,  // 画布宽度
// 			defaults.height = settings_.height,  // 画布高度
// 			defaults.delay = 1,  // 自动产生波源的延迟时间（秒）
// 			defaults.attenuation = 5,  // 波的衰减级别
// 			defaults.maxAmplitude = 1024  ,  // 波的最大振幅
// 			defaults.sourceAmplitude = 512  ,  // 震源振幅
// 			defaults.auto_value = !0;  // 是否自动产生波源
// 
// 	// 合并设置，将用户传入的设置覆盖默认设置
// 		settings.attenuation = defaults.attenuation;
// 		settings.maxAmplitude = defaults.maxAmplitude;
// 		settings.sourceAmplitude = defaults.sourceAmplitude;
// 
// 		// 检测背景图，如果没有提供背景图路径，则返回false
// 		if (!settings.image.length()) {
// 			return;
// 		}
// 
// 		// 提取设置中的参数
// 		width = settings.width,
// 			height = settings.height;
// 
// 		dropRadius = settings.dropRadius,
// 			delay = settings.delay * 1000,  // 将延迟时间转换为毫秒
// 			attenuation = settings.attenuation,  // 衰减级别
// 			maxAmplitude = settings.maxAmplitude,  // 最大振幅
// 			sourceAmplitude = settings.sourceAmplitude;
// 
// 		half_width = width >> 1,  // 宽度的一半
// 			half_height = height >> 1,  // 高度的一半
// 			amplitude_size = width * (height + 2) * 2,  // 振幅数组的大小
// 			old_index = width,  // 旧振幅数组的索引偏移量
// 			new_index = width * (height + 3),  // 新振幅数组的索引偏移量
// 			map_index;  // 振幅数组索引
// 		/*
// 			texture,   // 原始图像像素信息
// 			ripple,    // 参数波纹的图像像素信息
// 			image,  // Image对象
// 			autoRepeat, // 自动产生波源的重复事件
// 			ripple_map = [],  // 振幅数组
// 			last_map = [];  // 上一帧的振幅数组
// 			*/
// 
// 		ripple_map.resize(amplitude_size, 0);
// 		last_map.resize(amplitude_size, 0);
// 
// 			 
// 		init();
// 	}
// 
// 	// 加载图片
// 	 
// 	void clear(){
// 		
// 		SDL_SetRenderDrawColor(renderer,255,255,255,255);
// 		SDL_RenderClear(renderer);
// 		}
// 	// 保存图像的所有像素信息
// 	void saveImageData() {
// 		int pixelsCount = texture2d.getPixelCount();
// 		texture = new Uint8[pixelsCount];
// 		texture2d.getImageData(texture);
// 		ripple = new Uint8[pixelsCount];
// 		texture2d.getImageData(ripple);
//  
// 	}
// 
// 	// 初始化函数
// 	void init() {
// 		saveImageData();
// 		// 波幅数组初始化为0
// 		for (int i = 0; i < amplitude_size; i++) {
// 			ripple_map[i] = last_map[i] = 0;
// 		}
//  
//  
// 	}
// 
// 	 
// 
// 	// 在指定地点产生波源
// 	void  disturb(float circleX, float circleY) {
// 		// 将值向下取整
// 		circleX =(int)	circleX <<0;
// 		circleY = (int)circleY << 0;
// 		int maxDistanceX = circleX + dropRadius,
// 			maxDistanceY = circleY + dropRadius;
// 		// 在指定的圆形区域内增加波源振幅
// 		for (int y = circleY - dropRadius; y < maxDistanceY; y++) {
// 			for (int x = circleX - dropRadius; x < maxDistanceX; x++) {
// 				int resoult = old_index + y * width + x;
// 				if (!(resoult < 0 ||resoult >= amplitude_size))
// 				 
// 				ripple_map[resoult] += sourceAmplitude;
// 			}
// 		}
// 	}
// 
// 	// 渲染下一帧
// 	void renderRipple() {
// 
// 	//	texture2d.getImageData(texture);
// 		int i = old_index,
// 			deviation_x,  // x水平方向偏移
// 			deviation_y,  // y竖直方向偏移
// 			pixel_deviation, // 偏移后的ImageData对象像素索引
// 			pixel_source;  // 原始ImageData对象像素索引
// 
// 		// 交换新旧振幅数组的索引
// 		old_index = new_index;
// 		new_index = i;
// 
// 		// 设置像素索引和振幅索引
// 		i = 0;
// 		map_index = old_index;
// 
// 		// 使用局部变量优化全局作用域查询
// 		int _map_index = map_index,
// 			_width = width,
// 			_height = height,
// 			_half_width = half_width,
// 			_half_height = half_height;
// 
// 		auto &_ripple_map = ripple_map;
// 			auto 	_last_map = last_map;
// 		Uint8*		_ripple_data = ripple;  // 引用修改
// 		Uint8*	_texture_data = texture; // 引用修改
// 		int	_new_index = new_index,
// 			_attenuation = attenuation,
// 			_maxAmplitude = maxAmplitude;
// 
// 		// 渲染所有像素点
// 		for (int y = 0; y < height; y++) {
// 			for (int x = 0; x < _width; x++) {
// 				int x_boundary = 0, judge = _map_index % _width;
// 				if (judge <= 0) {
// 					x_boundary = 1; // 左边边界
// 				}
// 				else if (judge >= _width ) {
// 					x_boundary = 2; // 右边边界
// 				}
// 				int top = _ripple_map[_map_index - _width],// 上边的相邻点
// 					bottom = _ripple_map[_map_index + _width],// 下边的相邻点
// 					left = x_boundary != 1 ? _ripple_map[_map_index - 1] : 0,// 左边的相邻点
// 					right = x_boundary != 2 ? _ripple_map[_map_index + 1] : 0;// 右边的相邻点
// 				// 计算当前像素点下一时刻的振幅
// 				int amplitude = (top + bottom + left + right) >> 1;
// 				amplitude -= _ripple_map[_new_index + i];
// 				amplitude -= amplitude >> _attenuation;  // 计算衰减
// 
// 				// 更新振幅数组
// 				_ripple_map[_new_index + i] = amplitude;
// 
// 				amplitude = _maxAmplitude - amplitude;
// 				int old_amplitude = _last_map[i];
// // 				if (old_amplitude != amplitude)
// // 				{
// // 					int a = 9;
// // 					_last_map[i] = amplitude;
// // 				}
// 				_last_map[i] = amplitude;
// 				if (old_amplitude != amplitude) {
// 					deviation_x = (((x - _half_width) * amplitude / _maxAmplitude) << 0) + _half_width;
// 					deviation_y = (((y - _half_height) * amplitude / _maxAmplitude) << 0) + _half_height;
// 
// 					// 检查边界
// 					if (deviation_x >= _width) {
// 						deviation_x = _width - 1;
// 					}
// 					if (deviation_x < 0) {
// 						deviation_x = 0;
// 					}
// 					if (deviation_y >= _height) {
// 						deviation_y = _height - 1;
// 					}
// 					if (deviation_y < 0) {
// 						deviation_y = 0;
// 					}
// 
// 					pixel_source = i * 4;
// 					
// 				
// 					pixel_deviation = (deviation_x + (deviation_y * width)) * 4;
// 					
// 					// 移动像素的RGBA信息
// 					int r = _texture_data[pixel_deviation] & 0xff;
// 					int b = _texture_data[pixel_deviation + 1] & 0xff;
// 					int g = _texture_data[pixel_deviation + 2] & 0xff;
// 					int a = _texture_data[pixel_deviation + 3] & 0xff;
// 
// 						_ripple_data[pixel_source] = r; //R
// 						_ripple_data[pixel_source + 1] =b;//G
// 						_ripple_data[pixel_source + 2] =g; //B
// 						_ripple_data[pixel_source + 3] = a; //A
// 
// 					if (debug)
// 					{
// 						debug_1.push_back({ r,g,b,a });
// 					}
// 				}
// 				++i;
// 				++_map_index;
// 			}
// 		}
// 		if (debug)
// 		{
// 			if (debug_2.size() && debug_1.size())
// 			{
// 				for (int i =0;i < debug_2.size();++i)
// 				{
// 					if (debug_1[i]!=debug_2[i])
// 					{
// 						cout <<i<<" "<< debug_1[i] << debug_2[i] << endl;
// 					}
// 				}
// 			}
// 			system("cls");
// 			debug_2 = debug_1;
// 			debug_1.clear();
// 		}
// 		
// 
// 		map_index = _map_index;
// 		// 将修改后的ImageData对象绘制到canvas上
// 	    texture2d.putImageData(_ripple_data, 0, 0);
// 		render();
// 		
// 	}
// 
// 	void render() {
// 		clear();
// 		texture2d.render(0, 0);
// 	//	renderRipple();
// 	}
// 
// };
// 
// 
// 
// 
// // 主循环
// void mainLoop() {
// 	bool quit = false;
// 	SDL_Event e;
//  
// 	WaterSetting waterSetting;
// 	
// 	string fileName = "background.jpg";
// 	std::fstream is(fileName, ios::in | ios::binary);
// 	
// 	int flag = 0;
// 	if (!is)
// 	{
// 		is.close();
// 		
// 		fileName = "background.png";
// 		is.open(fileName, ios::in | ios::binary);
// 		if (!is)
// 		{
// 			cout << "不能打开:" << fileName << " and  background.jpg" << endl;
// 			cout << "回车继续" << endl;
// 			getchar();
// 			is.close();
// 			return ;
// 
// 		}
// 
// 	}
// 	is.close();
// 	//加载图片
// 	waterSetting.image = fileName;
// 	 
// 	
// //	waterSetting.image = "girl.png",//image path
// 		waterSetting.dropRadius = 3,//radius of the ripple
// 		 
// 		waterSetting.delay = 1,//if auto param === true. 1 === 1 second delay for animation
// 		waterSetting.auto_value = 1;//if auto param === true, animation starts on it′s own
// 
// 	WaterRipple *waterRipple = new WaterRipple(waterSetting);
// 
// 	while (!quit) {
// 		while (SDL_PollEvent(&e) != 0) {
// 			if (e.type == SDL_QUIT) {
// 				quit = true;
// 			}
// 			else if (e.type == SDL_MOUSEBUTTONDOWN) {
// 				if (e.button.button == SDL_BUTTON_LEFT)
// 				{
// 					int x, y;
// 					SDL_GetMouseState(&x, &y);
// 					waterRipple->disturb(x, y);
// 					use_motion ^= 1;
// 				}
// // 				else if (e.button.button == SDL_BUTTON_RIGHT)
// // 				{
// // 					debug ^= 1;
// // 					if (debug)
// // 						SDL_SetWindowTitle(window, "debug");
// // 					else
// // 						SDL_SetWindowTitle(window, "release");
// // 				}
// 			}
//   				else if (e.type == SDL_MOUSEMOTION) {
//   				if (use_motion)
//   				{
//   					int x, y;
//   					SDL_GetMouseState(&x, &y);
//   					waterRipple->disturb(x, y);
//   				}
//   				}
// // 				else if (e.type == SDL_KEYDOWN) {
// // 					if (e.key.keysym.sym == SDLK_RETURN) {
// // 						waterRipple->disturb(200, 200);
// // 					}
// // 				}
// 			
// 			
// 		 
// 		}
// 		
// // 		std::thread t(&WaterRipple::renderRipple, waterRipple)/*.detach()*/;
// // 		t.join();
// //		waterRipple->render();
// 		waterRipple->renderRipple();
// 	}
// }
// 
// 
// 
// int main(int argc, char* args[]) {
// 	if (!init()) {
// 		return 1;
// 	}
//  
// 	mainLoop();
// 	close();
// 	return 0;
// }


/*
*		js代码来源， https://asset.uusama.com/example/water_ripple.html
*		更改为C++ SDL版本，BY vertices c_cpp123
*       根目录放上test.jpg或test.png
*
*
*
*
*/

#if defined(_WIN32)||defined(_WIN64)
#include <SDL.h>
#include <SDL_image.h>
#elif defined(__ANDROID__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <memory> // 用于智能指针

#include "path_head.h"

using namespace std;

/* 边界限制函数
 * @param value 需要限制的值
 * @param min 最小值
 * @param max 最大值
 * @return 限制在[min,max]范围内的值
 */
inline int my_clamp(int value, int min, int max) {
	return (value < min) ? min : (value > max) ? max : value;
}

// 全局变量
int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;
SDL_Window* window;     // SDL窗口指针
SDL_Renderer* renderer; // SDL渲染器指针

/* RGBA颜色结构体
 * 包含红(r)、绿(g)、蓝(b)、透明度(a)四个分量
 * 重载了==和<<运算符便于比较和输出
 */
struct My_RGBA {
	int r, g, b, a;
	bool operator==(const My_RGBA &rhs) const {
		return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
	}
	friend ostream &operator<<(ostream& os, const My_RGBA &rhs) {
		return os << "( " << rhs.r << " " << rhs.g << " " << rhs.b << " " << rhs.a << " )";
	}
};

/* 纹理类
 * 封装SDL_Texture的相关操作
 */
class Texture2d {
	SDL_Texture* mTexture; // SDL纹理对象
	void* mPixels;         // 纹理像素数据指针
	int mPitch;            // 纹理每行字节数
	int m_width, m_height; // 纹理宽高

public:
	Texture2d() : mTexture(nullptr), mPixels(nullptr), mPitch(0), m_width(0), m_height(0) {}

	~Texture2d() { free(); }

	// 释放纹理资源
	void free() {
		if (mTexture) {
			SDL_DestroyTexture(mTexture);
			mTexture = nullptr;
		}
	}

	/* 从文件加载纹理
	 * @param image 图片文件路径
	 * @return 是否加载成功
	 */
	bool loadFrom(const std::string& image) {
		free(); // 先释放现有纹理

		// 加载图片表面
		SDL_Surface* loadedSurface = IMG_Load(image.c_str());
		if (!loadedSurface) {
			cerr << "无法加载图片: " << IMG_GetError() << endl;
			return false;
		}

		// 转换为RGBA8888格式
		SDL_Surface* formatSurface = SDL_ConvertSurfaceFormat(
			loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
		if (!formatSurface) {
			cerr << "表面格式转换失败: " << SDL_GetError() << endl;
			SDL_FreeSurface(loadedSurface);
			return false;
		}

		// 创建纹理
		mTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STREAMING,
			formatSurface->w, formatSurface->h);
		if (!mTexture) {
			cerr << "创建纹理失败: " << SDL_GetError() << endl;
			SDL_FreeSurface(formatSurface);
			SDL_FreeSurface(loadedSurface);
			return false;
		}

		// 保存纹理属性
		m_width = formatSurface->w;
		m_height = formatSurface->h;
		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

		// 锁定纹理并复制像素数据
		if (SDL_LockTexture(mTexture, nullptr, &mPixels, &mPitch) != 0) {
			cerr << "锁定纹理失败: " << SDL_GetError() << endl;
			SDL_FreeSurface(formatSurface);
			SDL_FreeSurface(loadedSurface);
			return false;
		}

		memcpy(mPixels, formatSurface->pixels, formatSurface->h * formatSurface->pitch);
		SDL_UnlockTexture(mTexture);

		// 释放临时表面
		SDL_FreeSurface(formatSurface);
		SDL_FreeSurface(loadedSurface);
		return true;
	}

	/* 获取纹理像素数据
	 * @param buffer 存储像素数据的缓冲区
	 */
	void getImageData(Uint8* buffer) {
		if (SDL_LockTexture(mTexture, nullptr, &mPixels, &mPitch) == 0) {
			memcpy(buffer, mPixels, m_height * mPitch);
			SDL_UnlockTexture(mTexture);
		}
	}

	/* 更新纹理像素数据
	 * @param buffer 包含新像素数据的缓冲区
	 */
	void putImageData(Uint8* buffer) {
		if (SDL_LockTexture(mTexture, nullptr, &mPixels, &mPitch) == 0) {
			memcpy(mPixels, buffer, m_height * mPitch);
			SDL_UnlockTexture(mTexture);
		}
	}

	// 渲染纹理到屏幕
	void render(int x = 0, int y = 0) {
		SDL_Rect dest = { x, y, m_width, m_height };
		SDL_RenderCopy(renderer, mTexture, nullptr, &dest);
	}

	// 获取纹理宽度
	int getWidth() const { return m_width; }
	// 获取纹理高度
	int getHeight() const { return m_height; }
	// 获取纹理每行字节数
	int getPitch() const { return mPitch; }
};

/* 水波效果设置结构体
 * 包含水波效果的各种参数
 */
struct WaterSetting {
	string image;              // 背景图片路径
	float dropRadius = 3.0f;   // 水滴半径
	int width = 0;             // 画布宽度(自动设置)
	int height = 0;            // 画布高度(自动设置)
	float delay = 1.0f;        // 自动产生水滴的延迟(秒)
	float attenuation = 5.0f;  // 波衰减系数
	float maxAmplitude = 1024.0f; // 最大振幅
	float sourceAmplitude = 512.0f; // 震源振幅
	bool auto_value = true;    // 是否自动产生水滴
};

/* 水波效果模拟类
 * 实现水波效果的物理模拟和渲染
 */
class WaterRipple {
	Texture2d texture;      // 背景纹理
	WaterSetting settings;  // 效果设置

	int width, height;      // 画布尺寸
	float dropRadius;       // 水滴半径
	int half_width, half_height; // 画布半宽高
	int amplitude_size;     // 振幅数组大小
	int old_index, new_index; // 振幅数组索引

	unique_ptr<Uint8[]> ripple_data;  // 水波效果像素数据
	unique_ptr<Uint8[]> texture_data; // 原始纹理像素数据
	vector<int> ripple_map;  // 当前振幅数组
	vector<int> last_map;    // 上一帧振幅数组

public:
	/* 构造函数
	 * @param settings 水波效果设置
	 */
	WaterRipple(WaterSetting& settings) : settings(settings) {
		// 加载背景纹理
		if (!texture.loadFrom(settings.image)) {
			return;
		}

		// 初始化参数
		width = texture.getWidth();
		height = texture.getHeight();
		dropRadius = settings.dropRadius;
		half_width = width >> 1;
		half_height = height >> 1;
		amplitude_size = width * (height + 2) * 2;
		old_index = width;
		new_index = width * (height + 3);

		// 分配像素数据缓冲区
		ripple_data = make_unique<Uint8[]>(width * height * 4);
		texture_data = make_unique<Uint8[]>(width * height * 4);

		// 初始化振幅数组
		ripple_map.resize(amplitude_size, 0);
		last_map.resize(amplitude_size, 0);

		// 获取原始纹理数据
		texture.getImageData(texture_data.get());
		memcpy(ripple_data.get(), texture_data.get(), width * height * 4);
	}

	/* 在指定位置产生水滴扰动
	 * @param x x坐标
	 * @param y y坐标
	 */
	void disturb(float x, float y) {
		// 确保位置在有效范围内
		x = my_clamp(x, dropRadius, width - dropRadius - 1);
		y = my_clamp(y, dropRadius, height - dropRadius - 1);

		int ix = static_cast<int>(x);
		int iy = static_cast<int>(y);
		int radius = static_cast<int>(dropRadius);

		// 在圆形区域内增加振幅
		for (int dy = -radius; dy <= radius; dy++) {
			for (int dx = -radius; dx <= radius; dx++) {
				// 检查是否在圆形区域内
				if (dx*dx + dy * dy <= radius * radius) {
					int px = ix + dx;
					int py = iy + dy;
					// 检查边界
					if (px >= 0 && px < width && py >= 0 && py < height) {
						int index = old_index + py * width + px;
						ripple_map[index] += settings.sourceAmplitude;
					}
				}
			}
		}
	}

	/* 渲染水波效果 */
	void renderRipple() {
		// 交换新旧振幅数组索引
		swap(old_index, new_index);

		const int _width = width;
		const int _height = height;
		const int _half_width = half_width;
		const int _half_height = half_height;
		const int _new_index = new_index;
		const int _attenuation = static_cast<int>(settings.attenuation);
		const int _maxAmplitude = static_cast<int>(settings.maxAmplitude);

		bool updated = false;  // 标记是否有像素更新
		int map_pos = old_index;
		Uint8* dst_pixel = ripple_data.get();

		// 遍历所有像素计算水波效果
		for (int y = 0; y < _height; y++) {
			for (int x = 0; x < _width; x++) {
				// 获取相邻点的振幅(带边界检查)
				int top = (y > 0) ? ripple_map[map_pos - _width] : 0;
				int bottom = (y < _height - 1) ? ripple_map[map_pos + _width] : 0;
				int left = (x > 0) ? ripple_map[map_pos - 1] : 0;
				int right = (x < _width - 1) ? ripple_map[map_pos + 1] : 0;

				// 计算新振幅
				int amplitude = (top + bottom + left + right) >> 1;
				amplitude -= ripple_map[_new_index + y * _width + x];
				amplitude -= amplitude >> _attenuation; // 应用衰减

				// 边界吸收效果
				if (x == 0 || x == _width - 1 || y == 0 || y == _height - 1) {
					amplitude = amplitude * 3 / 4; // 边界处额外衰减25%
				}

				// 更新振幅数组
				ripple_map[_new_index + y * _width + x] = amplitude;
				amplitude = _maxAmplitude - amplitude; // 转换为位移量

				// 如果振幅有变化，则更新像素
				if (last_map[y*_width + x] != amplitude) {
					last_map[y*_width + x] = amplitude;
					updated = true;

					// 计算纹理坐标偏移
					int dx = my_clamp(
						((x - _half_width) * amplitude / _maxAmplitude) + _half_width,
						0, _width - 1);
					int dy = my_clamp(
						((y - _half_height) * amplitude / _maxAmplitude) + _half_height,
						0, _height - 1);

					// 从原始纹理获取偏移后的像素
					Uint8* src = texture_data.get() + (dy * _width + dx) * 4;
					memcpy(dst_pixel, src, 4); // 复制RGBA四个分量
				}

				dst_pixel += 4; // 移动到下一个像素
				map_pos++;      // 更新振幅数组索引
			}
		}

		// 如果有像素更新，则更新纹理
		if (updated) {
			texture.putImageData(ripple_data.get());
		}

		// 渲染到屏幕
		render();
	}

	// 简单渲染纹理
	void render() {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		texture.render();
		SDL_RenderPresent(renderer);
	}
};

/* 主循环 */
void mainLoop() {
	bool quit = false;
	SDL_Event e;

	// 尝试加载背景图片
	string fileName = og::checkPath("009_water_ripple/background.png");
	ifstream is(fileName, ios::binary);
	if (!is) {
		fileName = og::checkPath("009_water_ripple/background.png");
		is.open(fileName, ios::binary);
		if (!is) {
			cout << "无法打开背景图片(background.jpg/png)" << endl;
			cout << "按回车键退出..." << endl;
			cin.get();
			return;
		}
	}
	is.close();

	// 配置水波效果
	WaterSetting waterSetting;
	waterSetting.image = fileName;
	waterSetting.dropRadius = 3.0f;
	waterSetting.delay = 1.0f;
	waterSetting.auto_value = true;

	// 创建水波效果实例
	WaterRipple waterRipple(waterSetting);

	// 主循环
	while (!quit) {
		// 处理事件
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					// 鼠标左键点击产生水波
					int x, y;
					SDL_GetMouseState(&x, &y);
					waterRipple.disturb(x, y);
				}
			}
			else if (e.type == SDL_MOUSEMOTION) {
				// 鼠标移动时持续产生水波
				int x, y;
				SDL_GetMouseState(&x, &y);
				waterRipple.disturb(x, y);
			}
		}

		// 渲染水波效果
		waterRipple.renderRipple();
	}
}

/* 程序入口 */
int main(int argc, char* args[]) {
	// 初始化SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cerr << "SDL初始化失败: " << SDL_GetError() << endl;
		return 1;
	}

	// 创建窗口和渲染器
	window = SDL_CreateWindow(u8"水波效果模拟",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	if (!window) {
		cerr << "窗口创建失败: " << SDL_GetError() << endl;
		SDL_Quit();
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED |
		SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		cerr << "渲染器创建失败: " << SDL_GetError() << endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// 初始化SDL_image
	if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
		cerr << "SDL_image初始化失败: " << IMG_GetError() << endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// 运行主循环
	mainLoop();

	// 清理资源
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}
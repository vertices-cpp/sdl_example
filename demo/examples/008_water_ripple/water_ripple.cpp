// /*
// *		js代码来源， https://asset.uusama.com/example/water_ripple.html
// *		更改为C++ SDL版本，BY vertices c_cpp123
// *       根目录放上test.jpg或test.png
// *
// *
// *
// *
// */

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
	string fileName = og::checkPath("background.png");
	cout << fileName << endl;
	ifstream is(fileName, ios::binary);
	if (!is) {
		fileName = og::checkPath("background.png");
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
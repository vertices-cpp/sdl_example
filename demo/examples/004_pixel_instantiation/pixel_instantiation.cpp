/****************/
/****表面or纹理***/
/****像素实例化***/
/**by悠然小赐*****/
/***代码有点乱*****/
/****************/
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <cmath>
#include <string>
#include <algorithm>
#include "path_head.h"
using namespace std;

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <SDL_image.h>
#include <SDL_ttf.h>

const int totalframe = 4;
//可以增加每个斜切的重复次数
const int Animationframe = 3;
//选择注释下面这句，调用不同像素编辑。
//#define SURFACE

#ifdef SURFACE
//表面像素处理
#define SURFACE_OR_TEXTURE1
#else
//纹理像素处理
#define SURFACE_OR_TEXTURE2
#endif

const int WIDTH = 350, HEIGHT = 350;
const int SCREEN_PER_TICK_FRAMES = 1000 / 60;

SDL_Window *win;
SDL_Renderer *ren;
TTF_Font *gFont;
SDL_Texture *uTexture;
SDL_Rect uWin;

const int PERSON_WIDTH = 16, PERSON_HEIGHT = 16, PERSON_VEL = 5;
const int mWidth = 40, mHeight = 40;


const char *PERSON = "004_pixel_instantiation/1.png";
class LTexture
{
	SDL_Texture *mTexture;
	int mWidth, mHeight;
	bool Dimentsion;
	void *mPixel;
	int mPitch;
public:
	LTexture()
	{
		Dimentsion = false, mWidth = mHeight = 0, mTexture = NULL;
		mPixel = NULL;
	}
	~LTexture()
	{
		free();
	}
	void lockTexture()
	{
		if (mPixel == NULL)
		{
			SDL_LockTexture(mTexture, NULL, &mPixel, &mPitch);
		}
	}
	void UnlockTexture()
	{
		if (mPixel != NULL)
		{
			mPixel = NULL, mPitch = 0;
			SDL_UnlockTexture(mTexture);
		}
	}
	SDL_Texture *GetTexture()
	{
		return mTexture;
	}
	void free()
	{
		if (mTexture)
			SDL_DestroyTexture(mTexture);
	}
	int GetWidth()
	{
		return mWidth;
	}
	int GetHeight()
	{
		return mHeight;
	}
	void setBlended(SDL_BlendMode blend)
	{
		SDL_SetTextureBlendMode(mTexture, blend);
	}
	void setTextureColor(int r, int g, int b) {
		SDL_SetTextureColorMod(mTexture, r, g, b);
	}
	void setTextureAlpha(int a) {
		SDL_SetTextureAlphaMod(mTexture, a);
	}
	void setDimentsion(int w, int h)
	{
		Dimentsion = true;
		mWidth = w, mHeight = h;
	}

	void loadFile(const char *path)
	{
		free();
		SDL_Surface *load = IMG_Load(path);
		// SDL_SetColorKey(load, SDL_TRUE, SDL_MapRGB(load->format, 0, 0, 0));

		SDL_Surface *formatedSurface = SDL_ConvertSurfaceFormat(load, SDL_PIXELFORMAT_RGBA8888, 0);

		mWidth = formatedSurface->w, mHeight = formatedSurface->h;
		mTexture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888 /* SDL_GetWindowPixelFormat(win) */, SDL_TEXTUREACCESS_STREAMING, formatedSurface->w, formatedSurface->h);	// SDL_SetTextureBlendMode(mTexture,SDL_BLENDMODE_BLEND);
		SDL_LockTexture(mTexture, NULL, &mPixel, &mPitch);

		memcpy(mPixel, formatedSurface->pixels, formatedSurface->pitch * formatedSurface->h);
		int format = SDL_PIXELFORMAT_RGBA8888;

		SDL_PixelFormat *mappingFormat = SDL_AllocFormat(format);
		int *pixeldata = (int *)mPixel;
		int color = SDL_MapRGBA(mappingFormat, 0, 0, 0, 255), Transparent =
			SDL_MapRGBA(mappingFormat, 0, 0, 0, 0);
		int pixelCount = mPitch * mHeight / 4;
		for (int i = 0; i < pixelCount; ++i)
			if (pixeldata[i] == color)
			{
				// exit ( 1);
				pixeldata[i] = Transparent;
			}

		SDL_UnlockTexture(mTexture);
		SDL_FreeSurface(formatedSurface);
		SDL_FreeSurface(load);
		mPixel = NULL, mPitch = 0;

	}
	void loadText(const char *text, SDL_Color a)
	{
		free();
		SDL_Surface *load = TTF_RenderUTF8_Blended(gFont, text, a);
		SDL_SetColorKey(load, SDL_TRUE, SDL_MapRGB(load->format, 0, 0, 0));
		mWidth = load->w, mHeight = load->h;
		mTexture = SDL_CreateTextureFromSurface(ren, load);
		SDL_FreeSurface(load);
	}
	void render(int x, int y, SDL_Rect * clip = NULL,
		double angle = 0.0, SDL_Point * center = NULL, SDL_RendererFlip flip =
		SDL_FLIP_NONE)
	{
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };
		if (clip != NULL && !Dimentsion)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}
		SDL_RenderCopyEx(ren, mTexture, clip, &renderQuad, angle, center, flip);
	}
};

class Particle
{
	float x, y;
	float vx, vy;
	Uint32 color;
	int frame = 235;
public:
	Particle(float, float, Uint32, float, float);
	float GetX()
	{
		return x;
	}
	float GetY()
	{
		return y;
	}
	void SetX(float px)
	{
		x = px;
	}
	void SetY(float py)
	{
		y = py;
	}
	int isdead() { return frame < 0; }
	void move();
	void render();
};



class Person
{
private:
	int direction, PersonState, frames, mVelX, mVelY;
	int mPosX, mPosY;
public:
	Person();
	int GetX()
	{
		return mPosX;
	}
	int GetY()
	{
		return mPosY;
	}

	int GetWidth()
	{
		return mWidth;
	}
	int GetHeight()
	{
		return mHeight;
	}
	int isState()
	{
		return PersonState;
	}
	int GetDirection()
	{
		return direction;
	}
	int GetFrames()
	{
		return frames;
	}
	void SetState(int i)
	{
		PersonState = i;
	}
	void move();
	void handleEvent(SDL_Event &);
	void render();
};

LTexture gBackground, gPreson, gTexture, gFire;

void init()
{
	SDL_Init(SDL_INIT_VIDEO);



	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	win = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

#if !defined(_WIN32)
	SDL_RenderSetLogicalSize(ren, WIDTH, HEIGHT + 300);

#endif

	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	gFont = TTF_OpenFont(og::checkPath("004_pixel_instantiation/font.ttf").c_str(), 28);

	uWin = { 30, 30, WIDTH - 60, HEIGHT - 60 };
	uTexture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, uWin.w, uWin.h);
	SDL_SetTextureBlendMode(uTexture, SDL_BLENDMODE_BLEND);
}

void close()
{
	gPreson.free();
	gTexture.free();
	TTF_CloseFont(gFont);

	SDL_DestroyTexture(uTexture);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void loadMedia()
{
	gBackground.loadFile(og::checkPath("004_pixel_instantiation/background.png").c_str());
	gPreson.loadFile(og::checkPath(string(PERSON)).c_str());
	gPreson.setBlended(SDL_BLENDMODE_BLEND);
	gPreson.setDimentsion(WIDTH, HEIGHT);
	/*	gFire.loadFile("fire.png");
		gFire.setDimentsion(1, 1);
		gFire.setTextureAlpha(255);*/
}

Particle::Particle(float px, float py, Uint32 c, float pvx, float pvy) :x(px), y(py), color(c), vx(pvx), vy(pvy)
{
}
float timeStamp = 0;
float secondsPassed = 0;
float oldTimeStamp = 0;
const int gravity = 9.83;
const int lift_force = 33.28;
const int wind_power = 20.5;
void Particle::move()
{
	vx += (-wind_power * secondsPassed);
	vy += (gravity*secondsPassed);

	vy += (-lift_force * secondsPassed);
	x += vx * secondsPassed + (rand() % 10 - 20)*secondsPassed, y += vy * secondsPassed + (rand() % 5 - 6)*secondsPassed;
	frame -= 1;
}

void Particle::render()
{
	//	move();
#ifdef SURFACE_OR_TEXTURE1
//Surface版本
	SDL_SetRenderDrawColor(ren, color >> 16, color >> 8, color, 255);
#endif
#ifdef SURFACE_OR_TEXTURE2

	SDL_SetRenderDrawColor(ren, color>>24 , color  >> 16, color>>8 , color);

#endif //SURFACE_OR_TEXTURE2
	//	SDL_SetRenderDrawColor(ren, color>>24,(color &0xff0000)>> 16, (color & 0xff0000) >> 8,0xff);
	SDL_RenderDrawPoint(ren, x, y);
	/*	gFire.setTextureColor(color >> 16, color >> 8, color);
		gFire.render(x-gFire.GetWidth()/2, y-gFire.GetHeight()/2);*/
}


Person::Person()
{
	mPosX = uWin.w - mWidth, mPosY = uWin.h - mHeight, direction = mVelX = mVelY = frames = 0;
	PersonState = 1;
	gPreson.setDimentsion(mWidth, mHeight);
}

void Person::move()
{
	mPosX += mVelX;
	if (mPosX < 0 || mPosX + mWidth > uWin.w)
		mPosX -= mVelX;
	mPosY += mVelY;
	if (mPosY < 0 || mPosY + mHeight > uWin.h)
		mPosY -= mVelY;

	frames++;
	if (frames / 10 >= 2)
		frames = 0;
}

void Person::handleEvent(SDL_Event & e)
{
	if (e.key.repeat == 0)		// &&e.type == SDL_KEYDOWN) 
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP:
			direction = 2;
			mVelY -= PERSON_VEL;
			break;
		case SDLK_DOWN:
			direction = 0;
			mVelY += PERSON_VEL;
			break;
		case SDLK_LEFT:
			direction = 4;
			mVelX -= PERSON_VEL;
			break;
		case SDLK_RIGHT:
			direction = 6;
			mVelX += PERSON_VEL;
			break;
		case SDLK_KP_ENTER:
		case SDLK_END:
		case SDLK_RETURN:
			//回车测试
			//	if (PersonState)
			PersonState = 2;
			break;
		default:

			mVelY = mVelX = 0;
			break;
		}
	}
	if (e.type == SDL_KEYUP)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP:
			mVelY += PERSON_VEL;
			break;
		case SDLK_DOWN:
			mVelY -= PERSON_VEL;
			break;
		case SDLK_LEFT:
			mVelX += PERSON_VEL;
			break;
		case SDLK_RIGHT:
			mVelX -= PERSON_VEL;
			break;
		default:
			break;
		}
	}
}

void Person::render()
{
	SDL_Rect r =
	{ direction * PERSON_WIDTH + frames / 10 * PERSON_WIDTH, 0, PERSON_WIDTH, PERSON_HEIGHT };
	gPreson.render(mPosX, mPosY, &r);
}


struct Point
{
	int x, y;
};




void GetBezier(vector < Point > &v, int x1, int y1, int x2, int y2)
{
	v.clear();
	for (float t = 0; t < 1; t += 0.1f)
	{
		float temp = 1 - t;
		v.push_back(
			{
			int(temp * x1 + t * x2), int(temp * y1 + t * y2) });
	}
}
//获取像素所在坐标与RGB
vector<Particle> ret(Uint32 *piexl, vector < Point > &c, int w)
{
	vector < Particle > r;
	for (auto i : c)
		for (auto y = w - 1; y >= i.y; y--)
		{
			for (int x = 0; x <= i.x; ++x)
			{
				//cout << y << ":" << x << ":" << (int *)&piexl[x + y * w] << endl;
				if (piexl[(x + y * w)])
					//if (piexl[(x + y * w)]&& (unsigned)(piexl[(x + y * w)])>255)
				{
					//	cout << piexl[(x + y * w)] << endl;
					Uint32 C = piexl[(x + y * w)];
					r.push_back({ (float)x, (float)y,piexl[(x + y * w)],(float)(rand() % 8 - 9),(float)(rand() % 6 - 7 )});
					//r.push_back({x, y, SDL_MapRGB(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888),C>>16,C>>8,C) });
					piexl[(x + y * w)] = 0;
				}
			}
		}
	return r;
}

vector < vector < Particle >> particle;

void shiftColliders(int x, int y)
{

	for (auto & i : particle)
	{
		for (auto & j : i)
		{
			// 			j.SetX(j.GetX() + x+uWin.x);
			// 			j.SetY(j.GetY() + y+uWin.y);
			j.SetX(j.GetX() + x);
			j.SetY(j.GetY() + y);
		}
	}
}
#ifdef SURFACE_OR_TEXTURE1
/* 创建动画帧 */

void CeateAnimation(Person & person)
{
	particle.clear();
	// 渲染目标空 
//	SDL_SetRenderTarget(ren, NULL);
	// 创建要计算切面的表面 
	SDL_Surface *load = SDL_ConvertSurface(IMG_Load(PERSON), SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 0);
	//裁剪位置
	SDL_Rect clip = { person.GetFrames() / 10 * PERSON_WIDTH + person.GetDirection() *PERSON_WIDTH,0,
		PERSON_WIDTH,
		PERSON_HEIGHT };
	// 创建粘贴剪切的目标 
	SDL_Surface * clipSprite = SDL_CreateRGBSurface(SDL_PIXELFORMAT_RGBA8888,
		person.GetWidth(), person.GetHeight(), 32, 0, 0, 0, 0);
	//复制过去
	SDL_BlitScaled(load, &clip, clipSprite, NULL);

	//	SDL_Texture *newTexture = SDL_CreateTextureFromSurface(ren, clipSprite);
	//测试屏幕渲染用的
	// 	SDL_SetRenderDrawColor(ren, 128, 128, 128, 200);
	// 		SDL_RenderClear(ren);
	// 		SDL_Rect r = { 0,0,clipSprite->w,clipSprite->h };
	// 		cout << clipSprite->w << ":" << clipSprite->h << endl;
	// 		SDL_RenderCopy(ren, newTexture, NULL, &r);
	// 		SDL_RenderPresent(ren);
	// 		SDL_Delay(20000);
	//编辑像素，斜切按4分一切矩形
	Uint32 *dstPixel = (Uint32*)clipSprite->pixels;
	vector < Point > v;
	// 4帧
	GetBezier(v, 0, person.GetHeight() / 2, person.GetWidth() / 2, person.GetHeight());
	particle.push_back(ret(dstPixel, v, person.GetWidth()));

	GetBezier(v, 0, 0, person.GetWidth(), person.GetHeight());
	particle.push_back(ret(dstPixel, v, person.GetWidth()));

	GetBezier(v, person.GetWidth() / 2, 0, person.GetWidth() + 2, person.GetHeight() / 2);
	particle.push_back(ret(dstPixel, v, person.GetWidth()));

	GetBezier(v, person.GetWidth(), 0, person.GetWidth(), 0);
	particle.push_back(ret(dstPixel, v, person.GetWidth()));
	//设置偏移到屏幕的位置
	shiftColliders(person.GetX(), person.GetY());
}
#endif
#ifdef SURFACE_OR_TEXTURE2
void CeateAnimation(Person & person)
{
	particle.clear();
	SDL_Texture *tex = SDL_GetRenderTarget(ren);

	SDL_SetRenderTarget(ren, NULL);
	SDL_Texture *newTexture =
		SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			person.GetWidth(), person.GetHeight());


	SDL_SetRenderTarget(ren, newTexture);
	SDL_Rect r =
	{ person.GetDirection()* PERSON_WIDTH + person.GetFrames() / 10 * PERSON_WIDTH, 0, PERSON_WIDTH, PERSON_HEIGHT };
	gPreson.render(0, 0, &r);// , 0, NULL, SDL_FLIP_VERTICAL);
	//细节，别拷贝到目标屏幕，否则需要刷新一遍，不然会闪屏。
	SDL_Rect x = { 0,0,100,100 };
	void *pixel;//= new char[400* person.GetWidth()]{ 0 };
	int pitch;
	

	SDL_Texture *dst =
		SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
			person.GetWidth(), person.GetHeight());

	SDL_LockTexture(dst, NULL, &pixel,
		&pitch);

	SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_RGBA8888, pixel, pitch);
	//	char *beg=(char*)p,*end=(char*)(p+pitch*person.GetHeight());
	//	string s(beg,end);
	//	s=string(s.rbegin(),s.rend());
	//	memcpy(pixel,s.c_str(),pitch*person.GetHeight());

	SDL_SetRenderTarget(ren, NULL);

	// 先获取贝塞尔斜切 
	vector < Point > v;


	Uint32 *dstPixel = (Uint32*)pixel;

	// 4帧
	GetBezier(v, 0, person.GetHeight() / 2, person.GetWidth() / 2, person.GetHeight());
	particle.push_back(ret(dstPixel, v, person.GetWidth()));

	GetBezier(v, 0, 0, person.GetWidth(), person.GetHeight());
	particle.push_back(ret(dstPixel, v, person.GetWidth()));

	GetBezier(v, person.GetWidth() / 2, 0, person.GetWidth() + 2, person.GetHeight() / 2);
	particle.push_back(ret(dstPixel, v, person.GetWidth()));

	GetBezier(v, person.GetWidth(), 0, person.GetWidth(), 0);
	particle.push_back(ret(dstPixel, v, person.GetWidth()));

	SDL_SetRenderTarget(ren, tex);
	SDL_UnlockTexture(dst);
	SDL_DestroyTexture(dst);
	SDL_DestroyTexture(newTexture);
	shiftColliders(person.GetX(), person.GetY());
}
#endif
int main(int argc, char *argv[])
{
	srand(time(0));
	init();

	loadMedia();

	Person person;
	int capTimer = 0, fpsTimer = SDL_GetTicks(), counted = 0;
	stringstream timeText;
	int cnt = 0, frame = 0;

	bool quit = false;
	SDL_Event e;
	int mouse = 0;
	while (!quit)
	{
		capTimer = SDL_GetTicks();

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = true;
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (mouse)
				{
					mouse = 0;
					SDL_StopTextInput();
				}
				else
				{
					mouse = 1;
					SDL_StartTextInput();
				}
			}

			person.handleEvent(e);
		}

		timeText.str("");
		float avgFps = counted / ((SDL_GetTicks() - fpsTimer) / 1000.f);
		timeText /* <<hex<<SDL_GetWindowPixelFormat(win) */ << " " << ":" <<
			setprecision(1) << fixed << avgFps;
		int sum = 0;
		if (particle.size()) {
			for (auto &i : particle)
				sum += i.size();
			timeText << " :" << sum;
		}
		//	 SDL_SetRenderTarget(ren, NULL); 




		SDL_SetRenderTarget(ren, uTexture);
		SDL_SetRenderDrawColor(ren, 192, 192, 192, 240);
		SDL_RenderClear(ren);

		gTexture.loadText(timeText.str().c_str(),
			{
			0, 0, 255 });
		gTexture.render((uWin.w - gTexture.GetWidth()) / 2, (uWin.h - gTexture.GetHeight()) / 2);

		if (person.isState() == 1)
		{
			person.move();
			person.render();
		}
		else if (person.isState() == 2)
		{
			CeateAnimation(person);
			decltype(particle) t;
			for (int i = 0; i < totalframe; ++i)
				for (int j = 0; j < Animationframe; ++j)
					t.push_back(particle[i]);
			particle.clear();
			particle = t;

			person.SetState(0);
			frame = 1;
		}

		if (!person.isState())
		{

			for (auto &i : particle)
			{
				for (auto it = i.begin(); it != i.end();)
				{
					if (!it->isdead())
					{
						it->render();
						++it;
					}
					else
						it = i.erase(it);
				}
			}
			for (int i = 0; i < frame; ++i) {
				for (auto &j : particle[i])
					j.move();
			}
			timeStamp = (float)SDL_GetTicks();
			secondsPassed = (timeStamp - oldTimeStamp) / 1000;

			secondsPassed = (float)min((double)secondsPassed, 0.1);

			oldTimeStamp = timeStamp;

		}

		SDL_SetRenderTarget(ren, NULL);
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);
		gBackground.render(0, 0);
		SDL_RenderCopy(ren, uTexture, NULL, &uWin);

		SDL_RenderPresent(ren);
		++counted;
		cnt++;
		if (cnt >= 8)
		{
			cnt = 0;
			frame++;
		}
		//如果上面注释t取消。这里两要改20
		//依次调用重复的，1>5,6>10,11>15,16>20
		if (frame >= Animationframe * totalframe)
			frame = Animationframe * totalframe;
		float fpsTimer = (float)SDL_GetTicks() - (float)capTimer;
		if (fpsTimer < SCREEN_PER_TICK_FRAMES - fpsTimer)
			SDL_Delay(SCREEN_PER_TICK_FRAMES - fpsTimer);
	}
	return 0;
}
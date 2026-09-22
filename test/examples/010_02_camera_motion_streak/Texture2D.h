
#ifndef _TEXTURE_2D_H_
#define _TEXTURE_2D_H_
 
#include "SDLView.h"

#define STB_IMAGE_STATIC            // ← 新增，让 STBIDEF 变成 static
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "OGMathGeometry.h"

#include <algorithm>

#define    OG_ENABLE_PREMULTIPLIED_ALPHA  0
 
OG_BEGIN

class Texture2D
{
	SDL_Texture *mTexture = nullptr;
	Size _imageSize;
	int mPitch;
	void* mPixels;
	bool _hasPremultipliedAlpha;
public: 
	Texture2D() {
 
	}
	~Texture2D()
	{
		free();
	}

	void free()
	{
		if (mTexture != nullptr)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = nullptr;
		}
	}
	void setRGBA(Color4B color)
	{
		//调制纹理rgb
		SDL_SetTextureColorMod(mTexture, color.r, color.g, color.b);
		SDL_SetTextureAlphaMod(mTexture, color.a);
	}

	void setColor(Uint8 red, Uint8 green, Uint8 blue)
	{
		//调制纹理rgb
		SDL_SetTextureColorMod(mTexture, red, green, blue);
	}
	void SetTextureAlphaMod(Uint8 a) {
		SDL_SetTextureAlphaMod(mTexture, a);
	}

	void setBlendMode(SDL_BlendMode blending)
	{
		SDL_SetTextureBlendMode(mTexture, blending);
	}


	void loadFrom(const char* fileName)
	{
		free();

		int w = 0, h = 0, comp = 0;
		unsigned char* pixels = stbi_load(fileName, &w, &h, &comp, 4);
		if (!pixels) {
			printf("stbi_load failed: %s (%s)\n", fileName, stbi_failure_reason());
			return;
		}

#if OG_ENABLE_PREMULTIPLIED_ALPHA != 0
		const int total = w * h;
		for (int i = 0; i < total; ++i) {
			unsigned char* p = pixels + i * 4;
			unsigned char a = p[3];
			p[0] = (unsigned char)((p[0] * a) / 255);
			p[1] = (unsigned char)((p[1] * a) / 255);
			p[2] = (unsigned char)((p[2] * a) / 255);
		}
#endif

		auto renderer = SDLView::getInstance()->getRender();
		mTexture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_STATIC,
			w, h);
		if (!mTexture) {
			stbi_image_free(pixels);
			return;
		}

		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		SDL_UpdateTexture(mTexture, nullptr, pixels, w * 4);

		_imageSize = Size((float)w, (float)h);

		stbi_image_free(pixels);
	}

	bool loadMemData(unsigned char* data, int len)
	{
		free();

		int w = 0, h = 0, comp = 0;
		// 强制 4 通道 = RGBA
		unsigned char* pixels = stbi_load_from_memory(data, len, &w, &h, &comp, 4);
		if (!pixels) {
			printf("stbi_load_from_memory failed: %s\n", stbi_failure_reason());
			return false;
		}

		// 预乘 alpha（直接操作 RGBA 字节）
#if OG_ENABLE_PREMULTIPLIED_ALPHA != 0
		const int total = w * h;
		for (int i = 0; i < total; ++i) {
			unsigned char* p = pixels + i * 4;   // R,G,B,A
			unsigned char a = p[3];
			p[0] = (unsigned char)((p[0] * a) / 255);
			p[1] = (unsigned char)((p[1] * a) / 255);
			p[2] = (unsigned char)((p[2] * a) / 255);
		}
#endif

		auto renderer = SDLView::getInstance()->getRender();

		// stb 给的内存顺序是 R,G,B,A —— 对应 SDL 的 ABGR8888
		mTexture = SDL_CreateTexture(renderer,
			SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_STATIC,
			w, h);
		if (!mTexture) {
			stbi_image_free(pixels);
			return false;
		}

		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		SDL_UpdateTexture(mTexture, nullptr, pixels, w * 4);

		_imageSize = Size((float)w, (float)h);

		stbi_image_free(pixels);   // 必须用 stbi_image_free，不是 free
		return true;
	}
	// Texture2D.cpp
	bool createTarget(float w, float h)
	{
		free();   // 释放旧的

		mTexture = SDL_CreateTexture(
			SDLView::getInstance()->getRender(),
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET,   // 关键
			(int)w, (int)h);

		if (!mTexture) return false;

		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		_imageSize.width = (float)w;
		_imageSize.height = (float)h;
		return true;
	}
// 	void createTexture(Texture2D* texture, const Rect & rect)
// 	{
// 		auto renderer = SDLView::getInstance()->getRender();
// 		auto getTexture = texture;
// 
// 		float w = rect.size.width, h = rect.size.height;
// 
// 		SDL_Texture *tex = SDL_GetRenderTarget(renderer);
// 
// 		mTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)w, (int)h);
// 		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
// 
// 		// 1. 保存原渲染目标
// 		SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
// 
// 		SDL_SetRenderTarget(renderer, mTexture);
// 		//创建目标纹理
// 		Rect dest = { 0,0,w,h };
// 		getTexture->render(rect, dest, Vec2::ZERO,
// 			rect.size / 2, Color4B(255, 255, 255, 255), false, false);
// 		// 	SDL_Rect rrr = { 0,0,w,h };
// 		// 	SDL_Rect ddd = { 0,0,w,h };
// 		//	SDL_RenderCopyEx(SDLView::getInstance()->getRender(), getTexture.get()->getTexture(),  &rrr, &ddd, 0, 0,SDL_FLIP_NONE);
// 		_imageSize.width = w;
// 		_imageSize.height = h;
// 
// 		// 4. 切到新目标，把源纹理渲染上来
// 		SDL_SetRenderTarget(renderer, oldTarget);
// 	}

	SDL_Texture *getTexture() { return mTexture; }
private:
	Texture2D(const Texture2D &) = delete;
	Texture2D &operator =(const Texture2D &) = delete;
};


OG_END

#endif
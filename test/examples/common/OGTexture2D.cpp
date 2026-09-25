// 只在一个 cpp 里定义 STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "OGTexture2D.h"
#include "SDLView.h"

#ifndef OG_ENABLE_PREMULTIPLIED_ALPHA
# define OG_ENABLE_PREMULTIPLIED_ALPHA 1
#endif


//#include "Texture2DManage.h"

//#define TRANSPRENT_BLACK

OG_BEGIN

Texture2D::Texture2D() :_texture(nullptr), _contentSize(Size::ZERO), mPitch(0), mPixels(nullptr),   _antialiasEnabled(true)
{
}


Texture2D::~Texture2D()
{
	free();
}

void Texture2D::free()
{
	if (_texture != nullptr)
	{
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
	}
}
void Texture2D::setRGBA(Color4B color)
{
	//调制纹理rgb
	SDL_SetTextureColorMod(_texture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(_texture, color.a);
}

void Texture2D::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//调制纹理rgb
	SDL_SetTextureColorMod(_texture, red, green, blue);
}
void Texture2D::SetTextureAlphaMod(Uint8 a) {
	SDL_SetTextureAlphaMod(_texture, a);
}

void Texture2D::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(_texture,blending);
}
 
 
/*
void Texture2D::loadFont(TTF_Font* ttf,const char *text,Color4B color)
{
	free();

	SDL_Color r = { color.r,color.g,color.b,color.a };

	SDL_Surface *load = TTF_RenderUTF8_Blended(ttf,text,r);

	assert(load != NULL);

	SDL_Surface *surfaceFormat = SDL_ConvertSurfaceFormat(load, SDL_PIXELFORMAT_RGBA8888, 0);

	mTexture = SDL_CreateTexture(SDLView::getInstance()->getRender(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		surfaceFormat->w, surfaceFormat->h);

	SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

	SDL_LockTexture(mTexture, &surfaceFormat->clip_rect, &mPixels, &mPitch);

	memcpy(mPixels, surfaceFormat->pixels, mPitch * surfaceFormat->h);



	_imageSize = Size((float)surfaceFormat->w, (float)surfaceFormat->h);

#ifdef TRANSPRENT_BLACK
	Uint32 *pixels = (Uint32*)mPixels;
	Uint32 colorKey = SDL_MapRGB(surfaceFormat->format, 0, 0, 0);
	Uint32 transParent = SDL_MapRGBA(surfaceFormat->format, 0, 0, 0, 0);

	Uint32 pixelCount = mPitch / 4 * surfaceFormat->h;

	for (Uint32 i = 0; i < pixelCount; ++i)
	{
		if (pixels[i] == colorKey)
			pixels[i] = transParent;
	}

#endif

	SDL_UnlockTexture(mTexture);

	SDL_FreeSurface(surfaceFormat);
	SDL_FreeSurface(load);
}
*/

/*void Texture2D::loadFrom(const char * fileName)
{
	free();

	SDL_Surface *load = IMG_Load(fileName);

	assert(load != NULL);

	SDL_Surface *surfaceFormat = SDL_ConvertSurfaceFormat(load, SDL_PIXELFORMAT_RGBA8888, 0);

	mTexture = SDL_CreateTexture(SDLView::getInstance()->getRender(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		surfaceFormat->w, surfaceFormat->h);

 

	SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
 
	SDL_LockTexture(mTexture, &surfaceFormat->clip_rect, &mPixels, &mPitch);

	memcpy(mPixels, surfaceFormat->pixels, mPitch * surfaceFormat->h);

	_imageSize = Size((float)surfaceFormat->w, (float)surfaceFormat->h);


#if 	OG_ENABLE_PREMULTIPLIED_ALPHA !=0

	Uint32 *pixels = (Uint32*)mPixels;
	Uint32 colorKey = SDL_MapRGB(surfaceFormat->format, 0, 0, 0);
	Uint32 transParent = SDL_MapRGBA(surfaceFormat->format, 0, 0, 0, 0);
	Uint32 pixelCount = mPitch / 4 * surfaceFormat->h;
	for (Uint32 i = 0; i < pixelCount; i++)
	{
		Uint8 r, g, b, a;
		SDL_GetRGBA(pixels[i], surfaceFormat->format, &r, &g, &b, &a);
		//cout << "r:" << r << "g:" << g << "b" << b << "(";
		r = (r * a) / 255;
		g = (g * a) / 255;
		b = (b * a) / 255;
		//cout << "r:" << r << "g:" << g << "b" << b << ")";
		pixels[i] = SDL_MapRGBA(surfaceFormat->format, r, g, b, a);
	}

#endif

#ifdef TRANSPRENT_BLACK
	Uint32 *pixels = (Uint32*)mPixels;
	Uint32 colorKey = SDL_MapRGB(surfaceFormat->format, 0, 0, 0);
	Uint32 transParent = SDL_MapRGBA(surfaceFormat->format, 0, 0, 0, 0);

	Uint32 pixelCount = mPitch / 4 * surfaceFormat->h;

	for (Uint32 i = 0; i < pixelCount; ++i)
	{
		if (pixels[i] == colorKey)
			pixels[i] = transParent;
	}

#endif

	SDL_UnlockTexture(mTexture);

	SDL_FreeSurface(surfaceFormat);
	SDL_FreeSurface(load);
	
}


bool Texture2D::loadMemData(unsigned char * data, int len)
{
	free();

	SDL_RWops *memp = SDL_RWFromMem((void*)data, len);
	SDL_Surface *load = IMG_Load_RW(memp,0);

	assert(load != NULL);

	SDL_Surface *surfaceFormat = SDL_ConvertSurfaceFormat(load, SDL_PIXELFORMAT_RGBA8888, 0);

	mTexture = SDL_CreateTexture(SDLView::getInstance()->getRender(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		surfaceFormat->w, surfaceFormat->h);

	SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);

	SDL_LockTexture(mTexture, &surfaceFormat->clip_rect, &mPixels, &mPitch);

	memcpy(mPixels, surfaceFormat->pixels, mPitch * surfaceFormat->h);

	_imageSize = Size((float)surfaceFormat->w, (float)surfaceFormat->h );

#if 	OG_ENABLE_PREMULTIPLIED_ALPHA !=0

// 	Uint32 *pixels = (Uint32*)mPixels;
// 	Uint32 colorKey = SDL_MapRGB(surfaceFormat->format, 0, 0, 0);
// 	Uint32 transParent = SDL_MapRGBA(surfaceFormat->format, 0, 0, 0, 0);
// 	Uint32 pixelCount = mPitch / 4 * surfaceFormat->h;
// 	for (Uint32 i = 0; i < pixelCount; i++)
// 	{
// 		Uint8 r, g, b, a;
// 		SDL_GetRGBA(pixels[i], surfaceFormat->format, &r, &g, &b, &a);
// 	 
	//r = (r * a) / 255;
	//g = (g * a) / 255;
	//b = (b * a) / 255;
//  
// 		pixels[i] = SDL_MapRGBA(surfaceFormat->format, r, g, b, a);
//}

#endif

#ifdef TRANSPRENT_BLACK
	Uint32 *pixels = (Uint32*)mPixels;
	Uint32 colorKey = SDL_MapRGB(surfaceFormat->format, 0, 0, 0);
	Uint32 transParent = SDL_MapRGBA(surfaceFormat->format, 0, 0, 0, 0);

	Uint32 pixelCount = mPitch / 4 * surfaceFormat->h;

	for (Uint32 i = 0; i < pixelCount; ++i)
	{
		if (pixels[i] == colorKey)
			pixels[i] = transParent;
	}

#endif

	SDL_UnlockTexture(mTexture);

	SDL_FreeSurface(surfaceFormat);
	SDL_FreeSurface(load);
	SDL_FreeRW(memp);
	return true;
}

*/




// SDL 里一个纹理就是 SDL_Texture*，不需要 backend 抽象
bool Texture2D::initWithMipmaps(MipmapInfo* mipmaps, int mipmapsNum,
	backend::PixelFormat pixelFormat,
	int pixelsWide, int pixelsHigh)
{
	if (!mipmaps || mipmapsNum <= 0) return false;
	if (pixelsWide <= 0 || pixelsHigh <= 0) return false;

	// 只取 level 0
	unsigned char* data = mipmaps[0].address;
	int            len = mipmaps[0].len;

	// 1. 像素格式映射到 SDL
	Uint32 sdlFmt = toSDLPixelFormat(pixelFormat);
	if (sdlFmt == SDL_PIXELFORMAT_UNKNOWN) {
		SDL_Log("不支持的 PixelFormat");
		return false;
	}

	// 2. 建 surface（不拷贝 data）
	//    注意：pitch 是每行字节数，不是每像素字节数
	int bpp = SDL_BITSPERPIXEL(sdlFmt);   // 8/16/24/32
	int pitch = pixelsWide * (bpp / 8);

	SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormatFrom(
		data, pixelsWide, pixelsHigh,
		bpp, pitch, sdlFmt);
	if (!surf) {
		SDL_Log("创建 surface 失败: %s", SDL_GetError());
		return false;
	}

	// 3. 上传到 GPU
	_texture = SDL_CreateTextureFromSurface(SDLView::getInstance()->getRender(), surf);
	SDL_FreeSurface(surf);   // data 不受影响，因为用的是 From 版本
	if (!_texture) {
		SDL_Log("创建纹理失败: %s", SDL_GetError());
		return false;
	}

	// 4. 抗锯齿开关（对应 2DX 的 _antialiasEnabled）
	SDL_SetTextureScaleMode(_texture,
		_antialiasEnabled ? SDL_ScaleModeLinear : SDL_ScaleModeNearest);

	// 5. 记录属性
	_pixelsWide = pixelsWide;
	_pixelsHigh = pixelsHigh;
	_pixelFormat = pixelFormat;
	_contentSize = Size((float)pixelsWide, (float)pixelsHigh);

	return true;
}
void Texture2D::loadFrom(const char* fileName)
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
	_texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STATIC,
        w, h);
    if (!_texture) {
        stbi_image_free(pixels);
        return;
    }

    SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(_texture, nullptr, pixels, w * 4);

	_contentSize = Size((float)w, (float)h);

    stbi_image_free(pixels);
}

bool Texture2D::loadMemData(unsigned char* data, int len)
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
	_texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STATIC,
        w, h);
    if (!_texture) {
        stbi_image_free(pixels);
        return false;
    }

    SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(_texture, nullptr, pixels, w * 4);

	_contentSize = Size((float)w, (float)h);

    stbi_image_free(pixels);   // 必须用 stbi_image_free，不是 free
    return true;
}
// Texture2D.cpp
bool Texture2D::createTarget( float w, float h)
{
	free();   // 释放旧的

	_texture = SDL_CreateTexture(
		SDLView::getInstance()->getRender(),
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,   // 关键
		w, h);

	if (!_texture) return false;

	SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
	_contentSize.width = (float)w;
	_contentSize.height = (float)h;
	return true;
}
void Texture2D::createTexture(Texture2D* texture, const Rect & rect)
{
	auto renderer = SDLView::getInstance()->getRender();
	auto getTexture = texture;

	float w = rect.size.width, h = rect.size.height;

	SDL_Texture *tex = SDL_GetRenderTarget(renderer);

	_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)w, (int)h);
	SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);

	// 1. 保存原渲染目标
	SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);

	SDL_SetRenderTarget(renderer, _texture);
	//创建目标纹理
	Rect dest = { 0,0,w,h };
 	getTexture->render(rect, dest, Vec2::ZERO,
 		rect.size/2, Color4B( 255,255,255,255 ), false, false);
// 	SDL_Rect rrr = { 0,0,w,h };
// 	SDL_Rect ddd = { 0,0,w,h };
//	SDL_RenderCopyEx(SDLView::getInstance()->getRender(), getTexture.get()->getTexture(),  &rrr, &ddd, 0, 0,SDL_FLIP_NONE);
	_contentSize.width = w;
	_contentSize.height = h;
	
	// 4. 切到新目标，把源纹理渲染上来
	SDL_SetRenderTarget(renderer, oldTarget);
}

// void Texture2D::render(int x, int y,SDL_Rect *clip,double degress ,SDL_Point *pointer,SDL_RendererFlip flip)
// {
// 	SDL_Rect renderQuad = { x,y,(int)_contentSize.width,(int)_contentSize.height };
// 	if (clip != nullptr)
// 	{
// 		renderQuad.w = clip->w;
// 		renderQuad.h = clip->h;
// 	}
// 
// 	SDL_RenderCopyEx(SDLView::getInstance()->getRender(), mTexture, clip, &renderQuad, degress, pointer, flip);
// }
// 
// void Texture2D::render(SDL_Rect *dest, SDL_Rect *clip, double degress,SDL_Point *pointer,SDL_RendererFlip flip)
// {
// 
// 	SDL_RenderCopyEx(SDLView::getInstance()->getRender(), mTexture, clip, dest, degress, pointer, flip);
// }
// 
// 
// void Texture2D::render(const Rect& clip, const Rect &dest, double degress, const Vec2 &pointer, bool flipX, bool flipY)
// {
// 	
// 		SDL_RendererFlip  flip = (SDL_RendererFlip)((flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE) | (flipY ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE));
// 
// 
// 		SDL_Rect src = { (int)clip.origin.x,
// 			(int)clip.origin.y,
// 			(int)clip.size.width,
// 			(int)clip.size.height };
// 
// 		SDL_Rect dst = { (int)dest.origin.x,
// 			(int)dest.origin.y,
// 			(int)dest.size.width,
// 			(int)dest.size.height };
// 
// 		//it->second->setBlendMode(SDL_BLENDMODE_INVALID);
// 		SDL_Point point = { (int)pointer.x,(int)pointer.y };
// 		SDL_RenderCopyEx(SDLView::getInstance()->getRender(), mTexture, nullptr, &dst, degress, &point, flip);
// 	
// }


void Texture2D::render(const Rect& clip, const Rect &dest,Vec2 rotate, const Vec2 &pointer, const Color4B &color, bool flipX, bool flipY)
{
	tmpQuad.TextureCvRenderer(_texture, _contentSize, clip, dest, rotate, pointer, color,flipX, flipY);
}

OG_END

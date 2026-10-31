#include "renderer/OGTexture2D.h"
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


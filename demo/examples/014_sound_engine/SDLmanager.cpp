#include "SDLmanager.h"
#include "AudioEngine.h"

SDLmanager::SDLmanager() :win(NULL), ren(NULL), mTexture(NULL)
{
#if defined(_WIN32)||defined(_WIN64)
	//SDL_setenv("SDL_AUDIODRIVER", "directsound", 1);
#endif
	SDL_Init(SDL_INIT_AUDIO);

	int w = 640, h = 480;	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	win =
		SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h,
			SDL_WINDOW_SHOWN);
	if (win == NULL)
		exit(1);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL)
		exit(1);
	 
	IMG_Init(IMG_INIT_PNG);
	//Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	TTF_Init();
	SDL_Log("%s", SDL_GetError());
	//Mix_Init(MIX_INIT_MP3);
		//Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,4096);
	//Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,MIX_DEFAULT_CHANNELS,4096);

}

bool SDLmanager::Message(const char *msg, int cx, int cy, SDL_Color c  )
{
	freeTexture();
	x = cx, y = cy;
	SDL_Surface *surface = TTF_RenderUTF8_Blended(font, msg, c);
	if (surface == NULL)
		exit(1);
	SDL_Surface *Sformat = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
	mw = surface->w, mh = surface->h;
	mTexture =
		SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, mw, mh);
	if (mTexture == NULL)
		exit(1);
	SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
	SDL_LockTexture(mTexture, &Sformat->clip_rect, &mPixels, &mPitch);
	memcpy(mPixels, Sformat->pixels, Sformat->h*Sformat->pitch);
	uint32 *pixels = (uint32 *)mPixels;
	uint32 colorKey = SDL_MapRGB(Sformat->format, 0, 0, 0);
	uint32 transparent = SDL_MapRGBA(Sformat->format, 0, 0, 0, 0);
	uint32 pixelCount = Sformat->h * Sformat->pitch / 4;
	for (size_t i = 0; i < pixelCount; ++i)
		if (pixels[i] == colorKey)
			pixels[i] = transparent;
	SDL_UnlockTexture(mTexture);
	SDL_FreeSurface(Sformat);
	SDL_FreeSurface(surface);
	return true;
}


bool SDLmanager::event()
{
	bool quit = false;
	SDL_Event e;
	if (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
			quit = true;
		else if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_1:
				OG::AudioEngine::getInstace()->speedInc();
				break;
			case SDLK_2:
				OG::AudioEngine::getInstace()->speedDec();
				break;
			case SDLK_3:
 
				// Í£Ö¹ËùÓÐ chunk
				OG::AudioEngine::getInstace()->pauseAll();
		 
				break;

			case SDLK_4:
				// »Ö¸´Ä³¸ö channel
				OG::AudioEngine::getInstace()->resumeAll();
 
			case SDLK_5:
				// »Ö¸´Ä³¸ö channel
				OG::AudioEngine::getInstace()->free();
				break;
			default:
				break;
			}
		}
	}
	callBack();
	return quit;
}
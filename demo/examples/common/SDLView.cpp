#include "SDLView.h"
#include "OGRenderer.h"
#include "OGKeyCode.h"
#include "OGDirector.h"


OG_BEGIN

  Vec2 SDLView::GetMousePos()
{
  return _mousePos;  
 
}

  Size SDLView::GetWinSize()
{
	return _winSize;
}

bool SDLView::event()
{
	bool quit = false;
	SDL_Event e;
	if (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
			quit = true;
		else if (e.type == SDL_MOUSEMOTION)
		{
			_mousePos.x = e.motion.x;
			_mousePos.y = e.motion.y;
		}
		if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
		{
			if (e.type == SDL_KEYDOWN && AllKeyMap[e.key.keysym.scancode] != true)
			{
				KeyDown();
				EventKeyboard event(g_keyCodeMap[e.key.keysym.scancode], 1);
				Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

				AllKeyMap[e.key.keysym.scancode] = true;
			}
			else if (e.type == SDL_KEYUP)
			{
				KeyDown();
				EventKeyboard event(g_keyCodeMap[e.key.keysym.scancode], 0);
				Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

				AllKeyMap[e.key.keysym.scancode] = false;
			}
		}
	}
	return quit;
}


 
OG_END
 

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
  static std::map<intptr_t, int> g_touchIdReorderMap;
  Touch*  g_touches[15]  ;
  unsigned int g_indexBitsUsed = 0; 

bool SDLView::event()
{
	bool quit = false;
	SDL_Event e;
	while (SDL_PollEvent(&e))
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
		else if (e.type == SDL_FINGERDOWN || e.type == SDL_FINGERUP || e.type == SDL_FINGERMOTION)
		{
			int64_t id = e.tfinger.fingerId;

			int w, h;
			//SDL_RenderGetLogicalSize(ren, &w, &h);

			Size logicSize = GetWinSize();

			float x = e.tfinger.x *logicSize.width;
			float y = e.tfinger.y *logicSize.height;
			printf("%d,%d", x, y);

			auto  f = [&]() {
				int i;
				int temp = g_indexBitsUsed;

				for (i = 0; i < 15; i++) {
					if (!(temp & 0x00000001)) {
						g_indexBitsUsed |= (1 << i);
						return i;
					}

					temp >>= 1;
				}

				// all bits are used
				return -1;
			};

			switch (e.type)
			{
			case SDL_FINGERDOWN:
			{
				int unusedIndex = 0;
				EventTouch touchEvent;


				auto iter = g_touchIdReorderMap.find(id);

				// it is a new touch
				if (iter == g_touchIdReorderMap.end())
				{
					unusedIndex =  f();

					// The touches is more than MAX_TOUCHES ?
					if (unusedIndex == -1) {

						continue;
					}

					Touch* touch =  g_touches[unusedIndex] = new (std::nothrow) Touch();
					touch->setTouchInfo(unusedIndex, x, y);

					g_touchIdReorderMap.emplace(id, unusedIndex);
					touchEvent._touches.push_back(touch);
				}


				if (touchEvent._touches.size() == 0)
				{
					break;
				}

 
				auto dispatcher = Director::getInstance()->getEventDispatcher();
				dispatcher->dispatchEvent(&touchEvent);
			}
			break;
			case SDL_FINGERUP:
			{

				EventTouch touchEvent;


				auto iter = g_touchIdReorderMap.find(id);
				if (iter == g_touchIdReorderMap.end())
				{

					continue;
				}

				/* Add to the set to send to the director */
				Touch* touch =  g_touches[iter->second];
				if (touch)
				{

					touch->setTouchInfo(iter->second, x, y);

					touchEvent._touches.push_back(touch);

					 g_touches[iter->second] = nullptr;
 

					g_touchIdReorderMap.erase(id);
				}
				else
				{

					break;
				}



				if (touchEvent._touches.size() == 0)
				{

					break;
				}

 
				auto dispatcher = Director::getInstance()->getEventDispatcher();
				dispatcher->dispatchEvent(&touchEvent);

				for (auto& touch : touchEvent._touches)
				{
					// release the touch object.
					touch->release();
				}
			}
			break;
			case SDL_FINGERMOTION:
			{
				float force = 0.0f;
				float maxForce = 0.0f;
				EventTouch touchEvent;



				auto iter = g_touchIdReorderMap.find(id);
				if (iter == g_touchIdReorderMap.end())
				{

					continue;
				}


				Touch* touch =  g_touches[iter->second];
				if (touch)
				{
					touch->setTouchInfo(iter->second, x, y);

					touchEvent._touches.push_back(touch);
				}
				else
				{
					// It is error, should return.

					break;
				}


				if (touchEvent._touches.size() == 0)
				{

					break;
				}

 
				auto dispatcher = Director::getInstance()->getEventDispatcher();
				dispatcher->dispatchEvent(&touchEvent);

			}
			break;
			default:
				break; 
			}
		}
	}
	return quit;
}


 
OG_END
 

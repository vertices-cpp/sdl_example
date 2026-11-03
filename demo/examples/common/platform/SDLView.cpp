#include "platform/SDLView.h"
#include "renderer/OGRenderer.h"
#include "platform/OGKeyCode.h"
#include "base/OGDirector.h"

#include "base/OGEventTouch.h"
 


OG_BEGIN


void SDLView::pushScissor(float x, float y, float w, float h)
{
	if (!_renderer) return;

	// ① points → pixels
	int wLog = 0, hLog = 0;
	SDL_GetWindowSize(_mainWindow, &wLog, &hLog);
	int wPix = 0, hPix = 0;
	SDL_GetRendererOutputSize(_renderer, &wPix, &hPix);
	float scaleX = (wLog > 0) ? (float)wPix / wLog : 1.0f;
	float scaleY = (hLog > 0) ? (float)hPix / hLog : 1.0f;

	SDL_Rect mine;
	mine.x = (int)std::floor(x * scaleX);
	mine.y = (int)std::floor(y * scaleY);
	mine.w = (int)std::ceil(w * scaleX);
	mine.h = (int)std::ceil(h * scaleY);

	// ② 保存进入前的旧状态
	ScissorEntry old;
	old.enabled = (SDL_RenderIsClipEnabled(_renderer) == SDL_TRUE);
	if (old.enabled) SDL_RenderGetClipRect(_renderer, &old.rect);
	_scissorStack.push_back(old);

	// ③ 与旧值求交
	SDL_Rect finalRect = mine;
	if (old.enabled) {
		SDL_Rect isect;
		if (SDL_IntersectRect(&old.rect, &mine, &isect))
			finalRect = isect;
		else
			finalRect = old.rect;       // ★ 求交失败：保持之前的裁剪
	}

	// ④ 应用
	SDL_RenderSetClipRect(_renderer, &finalRect);

	// ⑤ 记录当前生效值（给 getScissorRect 用，或者直接查 SDL 也行）
	_viewPortRect = { (float)finalRect.x, (float)finalRect.y,
					  (float)finalRect.w, (float)finalRect.h };
}

void SDLView::popScissor()
{
	if (!_renderer) return;

	if (_scissorStack.empty()) {
		SDL_RenderSetClipRect(_renderer, nullptr);
		_viewPortRect = { 0, 0, 0, 0 };
		return;
	}

	ScissorEntry prev = _scissorStack.back();
	_scissorStack.pop_back();

	if (prev.enabled) {
		SDL_RenderSetClipRect(_renderer, &prev.rect);
		_viewPortRect = { (float)prev.rect.x, (float)prev.rect.y,
						  (float)prev.rect.w, (float)prev.rect.h };
	}
	else {
		SDL_RenderSetClipRect(_renderer, nullptr);
		_viewPortRect = { 0, 0, 0, 0 };
	}
}

// 旧接口转调
void SDLView::setScissorInPoints(float x, float y, float w, float h)
{
	pushScissor(x, y, w, h);
}

void SDLView::resetScissor()
{
	popScissor();
}

bool SDLView::isScissorEnabled()
{
	if (!_renderer) return false;
	return SDL_RenderIsClipEnabled(_renderer) == SDL_TRUE;
}

Rect SDLView::getScissorRect() const
{
	if (!_renderer) return Rect::ZERO;
	if (SDL_RenderIsClipEnabled(_renderer) != SDL_TRUE) return Rect::ZERO;

	SDL_Rect r;
	SDL_RenderGetClipRect(_renderer, &r);

	// pixels → points
	int wLog = 0, hLog = 0;
	SDL_GetWindowSize(_mainWindow, &wLog, &hLog);
	int wPix = 0, hPix = 0;
	SDL_GetRendererOutputSize(_renderer, &wPix, &hPix);
	float invX = (wPix > 0) ? (float)wLog / wPix : 1.0f;
	float invY = (hPix > 0) ? (float)hLog / hPix : 1.0f;

	return Rect(r.x * invX, r.y * invY, r.w * invX, r.h * invY);
}

Vec2 SDLView::getMousePos()
{
  return _mousePos;  
 
}

Size SDLView::getWindowSize()
{
	return _winSize;
}
Size SDLView::getWindowScale() const 
{
	if (!_mainWindow || !_renderer) return Size(1.0f, 1.0f);

	int wLog = 0, hLog = 0;
	SDL_GetWindowSize(_mainWindow, &wLog, &hLog);

	int wPix = 0, hPix = 0;
	if (SDL_GetRendererOutputSize(_renderer, &wPix, &hPix) != 0)
		return Size(1.0f, 1.0f);

	float scaleX = (wLog > 0) ? (float)wPix / wLog : 1.0f;
	float scaleY = (hLog > 0) ? (float)hPix / hLog : 1.0f;

	return Size(scaleX, scaleY);
}


float SDLView::getDPI() const
{
	float ddpi = 96.0f, hdpi = 96.0f, vdpi = 96.0f;
	// 获取主显示器的 DPI
	if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi) != 0) {
		// 如果获取失败（有些老旧系统不支持），返回桌面端默认的 96
		return 96.0f;
	}
	// 取水平方向的 DPI 即可
	return hdpi > 0.0f ? hdpi : 96.0f;
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

			Size logicSize = getWindowSize();

			float x = e.tfinger.x *logicSize.width;
			float y = e.tfinger.y *logicSize.height;
			//printf("%d,%d", x, y);

			auto  getUnUsedIndex = [&]() {
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
			auto  removeUsedIndexBit = [&](int index) {
				if (index < 0 || index >=15)
				{
					return;
				}

				unsigned int temp = 1 << index;
				temp = ~temp;
				g_indexBitsUsed &= temp;
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
					unusedIndex = getUnUsedIndex();

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

				touchEvent._eventCode = EventTouch::EventCode::BEGAN;
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
					 removeUsedIndexBit(iter->second);

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

				touchEvent._eventCode = EventTouch::EventCode::ENDED;
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

				touchEvent._eventCode = EventTouch::EventCode::MOVED;
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
 

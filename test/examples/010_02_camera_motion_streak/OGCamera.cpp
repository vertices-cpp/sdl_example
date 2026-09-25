#include "OGCamera.h"




OG_BEGIN

Camera* Camera::_visitingCamera = nullptr;

Camera::Camera()
	:_zoom(1.0f)
	, _viewport(Rect::ZERO)
	, _marginX(64.0f)     // 默认外扩 64 像素
	, _marginY(64.0f)
	, _showDebug(false)
{ 
}

Camera::~Camera() {}


Camera* Camera::create()
{
	Camera* ret = new (std::nothrow) Camera();
	if (ret && ret->init()) {  return ret; }
	delete (ret); (ret) = nullptr;
	return nullptr;
}

Camera* Camera::create(const Size& viewportSize)
{
	Camera* ret = new (std::nothrow) Camera();
	if (ret && ret->initWithViewportSize(viewportSize)) {
		  return ret;
	}
	delete (ret); (ret) = nullptr;  
	return nullptr;
}

bool Camera::init()
{
	auto size = SDLView::getInstance()->GetWinSize();
	return initWithViewportSize(size);
}

bool Camera::initWithViewportSize(const Size& size)
{
 
	_viewport.origin = Vec2::ZERO;
	_viewport.size = size;
	_zoom = 1.0f;
	_center = size / 2;
	return true;
}
  


Mat3 Camera::getViewMatrix() const
{
	// sx = (wx - cx) * zoom + vw/2
	// sy = (wy - cy) * zoom + vh/2
	float vcw  = _viewport.size.width * 0.5f;
	float vch = _viewport.size.height * 0.5f;

	Mat3 m;
	m.a = _zoom;
	m.b = 0.0f;
	m.c = 0.0f;
	m.d = _zoom;
	m.tx = -_center.x * _zoom + _viewport.origin.x + vcw;// +vcw * (1 - _zoom);
	m.ty = -_center.y * _zoom + _viewport.origin.y + vch;// +vch * (1 - _zoom);
	return m;
}


Mat3 Camera::getScreenToWorldMatrix() const
{
	return getViewMatrix().getInversed();
}

Rect Camera::getScreenRect() const
{
	return  _viewport;
}

Rect Camera::getWorldRect() const
{
	float vw = _viewport.size.width / _zoom;
	float vh = _viewport.size.height / _zoom;
	return Rect(_center.x - vw * 0.5f, _center.y - vh * 0.5f, vw, vh);
}
Rect Camera::getScreenRectWithMargin() const
{
	return Rect(
		_viewport.origin.x - _marginX,
		_viewport.origin.y - _marginY,
		_viewport.size.width + _marginX * 2,
		_viewport.size.height + _marginY * 2
	);
}

Rect Camera::getWorldVisibleRect() const
{
	// 外扩是屏幕像素，除以 zoom 转成世界单位
	float vw = _viewport.size.width / _zoom;
	float vh = _viewport.size.height / _zoom;
	float mx = _marginX / _zoom;
	float my = _marginY / _zoom;
	return Rect(_center.x - vw * 0.5f - mx,
		_center.y - vh * 0.5f - my,
		vw + mx * 2,
		vh + my * 2);
}

Vec2 Camera::screenToWorld(const Vec2& p) const
{
	Mat3 m = getScreenToWorldMatrix();
	float x = p.x, y = p.y;
	m.out(x, y);
	return Vec2(x, y);
}

Vec2 Camera::worldToScreen(const Vec2& p) const
{
	Mat3 m = getViewMatrix();
	float x = p.x, y = p.y;
	m.out(x, y);
	return Vec2(x, y);
}

void Camera::clampToRect(const Rect& worldRect)
{
	float vw = _viewport.size.width / _zoom;
	float vh = _viewport.size.height / _zoom;
	float halfW = vw * 0.5f;
	float halfH = vh * 0.5f;

	if (worldRect.size.width <= vw) {
		_center.x = worldRect.origin.x + worldRect.size.width * 0.5f;
	}
	else {
		if (_center.x - halfW < worldRect.origin.x)
			_center.x = worldRect.origin.x + halfW;
		if (_center.x + halfW > worldRect.origin.x + worldRect.size.width)
			_center.x = worldRect.origin.x + worldRect.size.width - halfW;
	}

	if (worldRect.size.height <= vh) {
		_center.y = worldRect.origin.y + worldRect.size.height * 0.5f;
	}
	else {
		if (_center.y - halfH < worldRect.origin.y)
			_center.y = worldRect.origin.y + halfH;
		if (_center.y + halfH > worldRect.origin.y + worldRect.size.height)
			_center.y = worldRect.origin.y + worldRect.size.height - halfH;
	}
	 
}
void Camera::Draw(const Rect & r)
{
	float x1 = r.origin.x;
	float y1 = r.origin.y;
	float x2 = r.origin.x + r.size.width;
	float y2 = r.origin.y + r.size.height;
	SDL_SetRenderDrawColor(SDLView::getInstance()->getRender(), 0, 0, 255, 255);
	SDL_RenderDrawLine(SDLView::getInstance()->getRender(), x1, y1, x2, y1);
	SDL_RenderDrawLine(SDLView::getInstance()->getRender(), x2, y1, x2, y2);
	SDL_RenderDrawLine(SDLView::getInstance()->getRender(), x2, y2, x1, y2);
	SDL_RenderDrawLine(SDLView::getInstance()->getRender(), x1, y2, x1, y1);
}
// 
//  
// void Camera::setDepth(int8_t depth)
// {
// 	if (_depth != depth)
// 	{
// 		_depth = depth;
// 		if (_scene)
// 		{
// 			//notify scene that the camera order is dirty
// 			_scene->setCameraOrderDirty();
// 		}
// 	}
// }

OG_END
 
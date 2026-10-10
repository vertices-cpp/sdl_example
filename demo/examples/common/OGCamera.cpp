#include "OGCamera.h"
//#include "OGScene.h"
#include "OGDirector.h"

OG_BEGIN

Camera* Camera::_visitingCamera = nullptr;
 



Camera::Camera()
	: _center(Vec2::ZERO)
	, _zoom(1.0f)
	, _viewport(Rect::ZERO)
	, _marginX(64.0f)     // 默认外扩 64 像素
	, _marginY(64.0f)
	, _showDebug(false)
// 	, _viewportFrame(nullptr)
// 	, _marginFrame(nullptr)
{
	_name = "Camera";
// 	_viewportFrame = DrawNode::create();
// 	_marginFrame = DrawNode::create();
// 
// 	addChild(_viewportFrame);
// 	_viewportFrame->setGlobalZOrder(99999);
// 	addChild(_marginFrame);
// 	_marginFrame->setGlobalZOrder(99999);
}

Camera::~Camera() {}


Camera* Camera::create()
{
	Camera* ret = new (std::nothrow) Camera();
	if (ret && ret->init()) { ret->autorelease(); return ret; }
	OG_SAFE_DELETE(ret);
	return nullptr;
}

Camera* Camera::create(const Size& viewportSize)
{
	Camera* ret = new (std::nothrow) Camera();
	if (ret && ret->initWithViewportSize(viewportSize)) {
		ret->autorelease(); return ret;
	}
	OG_SAFE_DELETE(ret);
	return nullptr;
}

bool Camera::init()
{
	auto size = SDLView::getInstance()->getWindowSize();
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

// 
// void Camera::onEnter()
// {
// 	if (_scene == nullptr)
// 	{
// 		auto scene = getScene();
// 		if (scene)
// 		{
// 			setScene(scene);
// 		}
// 	}
// 	Node::onEnter();
// }
// 
// void Camera::onExit()
// {
// 	// remove this camera from scene
// 	setScene(nullptr);
// 	Node::onExit();
// }
// 
// void Camera::setScene(Scene* scene)
// {
// 	if (_scene != scene)
// 	{
// 		//remove old scene
// 		if (_scene)
// 		{
// 			auto& cameras = _scene->_cameras;
// 			auto it = std::find(cameras.begin(), cameras.end(), this);
// 			if (it != cameras.end())
// 				cameras.erase(it);
// 			_scene = nullptr;
// 		}
// 		//set new scene
// 		if (scene)
// 		{
// 			_scene = scene;
// 			auto& cameras = _scene->_cameras;
// 			auto it = std::find(cameras.begin(), cameras.end(), this);
// 			if (it == cameras.end())
// 			{
// 				_scene->_cameras.push_back(this);
// 				//notify scene that the camera order is dirty
// 				_scene->setCameraOrderDirty();
// 			}
// 		}
// 	}
// }
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
int Camera::getRenderOrder() const
{
	int result(0);
	result = 127 << 8;
	result += _depth;
	return result;
}
void Camera::DrawScreen()
{
	Rect screenRect =  getScreenRect();

	SDL_Renderer* sdlRen = SDLView::getInstance()->getRender();
	SDL_Rect r1 = {
		(int)screenRect.origin.x,
		(int)screenRect.origin.y,
		(int)screenRect.size.width,
		(int)screenRect.size.height
	};
	SDL_Rect r2 = {
		(int)screenRect.origin.x -  getMarginX(),
		(int)screenRect.origin.y -  getMarginY(),
		(int)screenRect.size.width +  getMarginX() * 2,
		(int)screenRect.size.height +  getMarginY() * 2
	};
	SDL_SetRenderDrawColor(sdlRen, 255, 0, 0, 255);
	SDL_RenderDrawRect(sdlRen, &r1);
	SDL_RenderDrawRect(sdlRen, &r2);
}

OG_END
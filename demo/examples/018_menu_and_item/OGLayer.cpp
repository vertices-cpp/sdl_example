
#include <stdarg.h>
#include "OGLayer.h"

#include "OGRenderer.h"
#include "OGDirector.h"
#include "OGEventDispatcher.h"
#include "OGEventListenerTouch.h"
#include "OGEventTouch.h"
#include "OGEventKeyboard.h"
#include "OGEventListenerKeyboard.h"

#include "OGCamera.h"

//#include "OGUTF8.h" 

#if (OG_TARGET_PLATFORM == OG_PLATFORM_MAC)
#include "desktop/OGGLViewImpl-desktop.h"
#endif

OG_BEGIN

// Layer
Layer::Layer()
	: _touchEnabled(false)
	, _accelerometerEnabled(false)
	, _keyboardEnabled(false)
	, _touchListener(nullptr)
	, _keyboardListener(nullptr)
	, _accelerationListener(nullptr)
	, _touchMode(Touch::DispatchMode::ALL_AT_ONCE)
	, _swallowsTouches(true)
{
	_name = "Layer";
	//_ignoreAnchorPointForPosition = true;
	//setAnchorPoint(Vec2(0.5f, 0.5f));
}

Layer::~Layer()
{

}

bool Layer::init()
{
	Director * director = Director::getInstance();
	setContentSize(director->getWinSize());
	return true;
}

Layer *Layer::create()
{
	Layer *ret = new (std::nothrow) Layer();
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		OG_SAFE_DELETE(ret);
		return nullptr;
	}
} 

void Layer::onKeyPressed(EventKeyboard::KeyCode /*keyCode*/, Event* /*unused_event*/)
{
}

void Layer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* /*unused_event*/)
{
	 
}

/// Callbacks

bool Layer::onTouchBegan(Touch *touch, Event *event)
{ 
	OGASSERT(false, "Layer#ccTouchBegan override me");
	return true;
}

void Layer::onTouchMoved(Touch *touch, Event *event)
{ 
}

void Layer::onTouchEnded(Touch *touch, Event *event)
{ 
}

void Layer::onTouchCancelled(Touch *touch, Event *event)
{
	 
}

void Layer::onTouchesBegan(const std::vector<Touch*>& touches, Event *event)
{ 
}

void Layer::onTouchesMoved(const std::vector<Touch*>& touches, Event *event)
{ 
}

void Layer::onTouchesEnded(const std::vector<Touch*>& touches, Event *event)
{ 
}

void Layer::onTouchesCancelled(const std::vector<Touch*>& touches, Event *event)
{ 
}

/// LayerColor

LayerColor::LayerColor()
{
	// default blend function
	_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
	// ===== 索引：tl, tr, bl, br 两个三角形 =====
	_indices[0] = 0;   // tl
	_indices[1] = 1;   // tr
	_indices[2] = 2;   // bl
	_indices[3] = 1;   // tr
	_indices[4] = 3;   // br
	_indices[5] = 2;   // bl
	 
}

LayerColor::~LayerColor()
{
}

/// blendFunc getter
const BlendFunc &LayerColor::getBlendFunc() const
{
	return _blendFunc;
}
/// blendFunc setter
void LayerColor::setBlendFunc(const BlendFunc &var)
{
	_blendFunc = var;
}

LayerColor* LayerColor::create()
{
	LayerColor* ret = new (std::nothrow) LayerColor();
	if (ret && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		OG_SAFE_DELETE(ret);
	}
	return ret;
}

LayerColor * LayerColor::create(const Color4B& color, float width, float height)
{
	LayerColor * layer = new (std::nothrow) LayerColor();
	if (layer && layer->initWithColor(color, width, height))
	{
		layer->autorelease();
		return layer;
	}
	OG_SAFE_DELETE(layer);
	return nullptr;
}

LayerColor * LayerColor::create(const Color4B& color)
{
	LayerColor * layer = new (std::nothrow) LayerColor();
	if (layer && layer->initWithColor(color))
	{
		layer->autorelease();
		return layer;
	}
	OG_SAFE_DELETE(layer);
	return nullptr;
}

bool LayerColor::init()
{
	Size s = Director::getInstance()->getWinSize();
	return initWithColor(Color4B(0, 0, 0, 0), s.width, s.height);
}

bool LayerColor::initWithColor(const Color4B& color, float w, float h)
{
	if (Layer::init())
	{

		// default blend function
//		_blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;

		_displayedColor.r = _realColor.r = color.r;
		_displayedColor.g = _realColor.g = color.g;
		_displayedColor.b = _realColor.b = color.b;
		_displayedOpacity = _realOpacity = color.a;

// 		for (size_t i = 0; i < sizeof(_squareVertices) / sizeof(_squareVertices[0]); i++)
// 		{
// 			_squareVertices[i].x = 0.0f;
// 			_squareVertices[i].y = 0.0f;
// 		}

		updateColor();
		setContentSize(Size(w, h));

		return true;
	}
	return false;
}

bool LayerColor::initWithColor(const Color4B& color)
{
	Size s = Director::getInstance()->getWinSize();
	return initWithColor(color, s.width, s.height);
}

/// override contentSize
void LayerColor::setContentSize(const Size & size)
{
// 	_squareVertices[1].x = size.width;
// 	_squareVertices[2].y = size.height;
// 	_squareVertices[3].x = size.width;
// 	_squareVertices[3].y = size.height;

	Layer::setContentSize(size);
}

void LayerColor::changeWidthAndHeight(float w, float h)
{
	this->setContentSize(Size(w, h));
}

void LayerColor::changeWidth(float w)
{
	this->setContentSize(Size(w, _contentSize.height));
}

void LayerColor::changeHeight(float h)
{
	this->setContentSize(Size(_contentSize.width, h));
}

void LayerColor::updateColor()
{
	//c4f转c4b直接给值就好了/
	for (int i = 0; i < 4; i++)
	{
		_vertexData[i].colors.r = _displayedColor.r;
		_vertexData[i].colors.g = _displayedColor.g;
		_vertexData[i].colors.b = _displayedColor.b;
		_vertexData[i].colors.a = _displayedOpacity;
		_vertexData[i].texCoords = { 0, 0 };   // 无纹理，随便填
	}

}
void LayerColor::draw(Renderer* renderer, const Mat3& transform, uint32_t flags)
{
	if (!_visible) return;

	float w = _contentSize.width;
	float h = _contentSize.height;

	// ===== 0. 算局部可见范围 =====
	float localMinX = -FLT_MAX, localMinY = -FLT_MAX;
	float localMaxX = FLT_MAX, localMaxY = FLT_MAX;
	bool hasCull = false;

	auto camera = Camera::getVisitingCamera();
	if (camera)
	{
		Rect screenRect = camera->getScreenRectWithMargin();

		// 屏幕 -> 局部
		Mat3 screenToLocal = getModelViewTransformInverse();   // 直接用，零计算 transform.getInversed();

		Vec2 corners[4] = {
			{screenRect.origin.x, screenRect.origin.y},
			{screenRect.origin.x + screenRect.size.width, screenRect.origin.y},
			{screenRect.origin.x, screenRect.origin.y + screenRect.size.height},
			{screenRect.origin.x + screenRect.size.width, screenRect.origin.y + screenRect.size.height}
		};

		localMinX = FLT_MAX; localMinY = FLT_MAX;
		localMaxX = -FLT_MAX; localMaxY = -FLT_MAX;
		for (auto& c : corners)
		{
			float x = c.x, y = c.y;
			screenToLocal.out(x, y);
			localMinX = std::min(localMinX, x);
			localMinY = std::min(localMinY, y);
			localMaxX = std::max(localMaxX, x);
			localMaxY = std::max(localMaxY, y);
		}
		hasCull = true;
	}

	// ===== 1. 剔除：算这个 quad 的局部 AABB =====
	// LayerColor 的 quad 就是 (0,0)~(w,h)
	if (hasCull)
	{
		if (w < localMinX || 0 > localMaxX ||
			h < localMinY || 0 > localMaxY)
		{
			return;   // 整个层在屏幕外，不提交
		}
	}

	// ===== 2. 顶点位置：局部 -> 屏幕 =====
	// 按 QuadCommand 索引顺序：tl, tr, bl, br
	Vec2 localPoints[4] = {
		{0, h},   // 0: tl
		{w, h},   // 1: tr
		{0, 0},   // 2: bl
		{w, 0}    // 3: br
	};

	for (int i = 0; i < 4; ++i)
	{
		float vx = localPoints[i].x;
		float vy = localPoints[i].y;
		transform.out(vx, vy);
		_vertexData[i].vertices.x = vx;
		_vertexData[i].vertices.y = vy;
	}

#ifdef ORANGE_DEBUG
	_customCommand.setOwner(this);
#endif 
	// ===== 3. 提交 =====
	_customCommand.init(_globalZOrder, nullptr, _vertexData, _indices,4,6);
	renderer->addCommand(&_customCommand);
}

// void LayerColor::updateVertexBuffer()
// {
// 	//_customCommand.updateVertexBuffer(_vertexData, sizeof(_vertexData));
// }

//
// LayerGradient
//
LayerGradient::LayerGradient()
{
}

LayerGradient::~LayerGradient()
{
}

LayerGradient* LayerGradient::create(const Color4B& start, const Color4B& end)
{
	LayerGradient * layer = new (std::nothrow) LayerGradient();
	if (layer && layer->initWithColor(start, end))
	{
		layer->autorelease();
		return layer;
	}
	OG_SAFE_DELETE(layer);
	return nullptr;
}

LayerGradient* LayerGradient::create(const Color4B& start, const Color4B& end, const Vec2& v)
{
	LayerGradient * layer = new (std::nothrow) LayerGradient();
	if (layer && layer->initWithColor(start, end, v))
	{
		layer->autorelease();
		return layer;
	}
	OG_SAFE_DELETE(layer);
	return nullptr;
}

LayerGradient* LayerGradient::create()
{
	LayerGradient* ret = new (std::nothrow) LayerGradient();
	if (ret && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		OG_SAFE_DELETE(ret);
	}
	return ret;
}

bool LayerGradient::init()
{
	return initWithColor(Color4B(0, 0, 0, 255), Color4B(0, 0, 0, 255));
}

bool LayerGradient::initWithColor(const Color4B& start, const Color4B& end)
{
	return initWithColor(start, end, Vec2(0, -1));
}

bool LayerGradient::initWithColor(const Color4B& start, const Color4B& end, const Vec2& v)
{
	_endColor.r = end.r;
	_endColor.g = end.g;
	_endColor.b = end.b;

	_endOpacity = end.a;
	_startOpacity = start.a;
	_alongVector = v;

	_compressedInterpolation = true;

	return LayerColor::initWithColor(Color4B(start.r, start.g, start.b, 255));
}

void LayerGradient::updateColor()
{
	LayerColor::updateColor(); 

	float h = _alongVector.getLength();
	if (h == 0)
		return;

	float c = sqrtf(2.0f);
	Vec2 u(_alongVector.x / h, _alongVector.y / h);

	// Compressed Interpolation mode
	if (_compressedInterpolation)
	{
		float h2 = 1 / (fabsf(u.x) + fabsf(u.y));
		u = u * (h2 * (float)c);
	}

	float opacityf = (float)_displayedOpacity / 255.0f;

	Color4F S(
		_displayedColor.r / 255.0f,
		_displayedColor.g / 255.0f,
		_displayedColor.b / 255.0f,
		_startOpacity * opacityf / 255.0f
	);

	Color4F E(
		_endColor.r / 255.0f,
		_endColor.g / 255.0f,
		_endColor.b / 255.0f,
		_endOpacity * opacityf / 255.0f
	);

	// (-1, -1)
	float r0 = E.r + (S.r - E.r) * ((c + u.x + u.y) / (2.0f * c));
	float g0 = E.g + (S.g - E.g) * ((c + u.x + u.y) / (2.0f * c));
	float b0 = E.b + (S.b - E.b) * ((c + u.x + u.y) / (2.0f * c));
	float a0 = E.a + (S.a - E.a) * ((c + u.x + u.y) / (2.0f * c));
	// (1, -1)
	float r1 = E.r + (S.r - E.r) * ((c - u.x + u.y) / (2.0f * c));
	float g1 = E.g + (S.g - E.g) * ((c - u.x + u.y) / (2.0f * c));
	float b1 = E.b + (S.b - E.b) * ((c - u.x + u.y) / (2.0f * c));
	float a1 = E.a + (S.a - E.a) * ((c - u.x + u.y) / (2.0f * c));
	// (-1, 1)
	float r2 = E.r + (S.r - E.r) * ((c + u.x - u.y) / (2.0f * c));
	float g2 = E.g + (S.g - E.g) * ((c + u.x - u.y) / (2.0f * c));
	float b2 = E.b + (S.b - E.b) * ((c + u.x - u.y) / (2.0f * c));
	float a2 = E.a + (S.a - E.a) * ((c + u.x - u.y) / (2.0f * c));
	// (1, 1)
	float r3 = E.r + (S.r - E.r) * ((c - u.x - u.y) / (2.0f * c));
	float g3 = E.g + (S.g - E.g) * ((c - u.x - u.y) / (2.0f * c));
	float b3 = E.b + (S.b - E.b) * ((c - u.x - u.y) / (2.0f * c));
	float a3 = E.a + (S.a - E.a) * ((c - u.x - u.y) / (2.0f * c));
	// (-1, -1)
	_vertexData[0].colors.r = (uint8_t)(r0 * 255.0f);
	_vertexData[0].colors.g = (uint8_t)(g0 * 255.0f);
	_vertexData[0].colors.b = (uint8_t)(b0 * 255.0f);
	_vertexData[0].colors.a = (uint8_t)(a0 * 255.0f);
	
	// (1, -1)    
	_vertexData[1].colors.r = (uint8_t)(r1 * 255.0f);
	_vertexData[1].colors.g = (uint8_t)(g1 * 255.0f);
	_vertexData[1].colors.b = (uint8_t)(b1 * 255.0f);
	_vertexData[1].colors.a = (uint8_t)(a1 * 255.0f);   
	// (-1, 1)    
	_vertexData[2].colors.r = (uint8_t)(r2 * 255.0f);
	_vertexData[2].colors.g = (uint8_t)(g2 * 255.0f);
	_vertexData[2].colors.b = (uint8_t)(b2 * 255.0f);
	_vertexData[2].colors.a = (uint8_t)(a2 * 255.0f);   
	// (1, 1)     
	_vertexData[3].colors.r = (uint8_t)(r3 * 255.0f);
	_vertexData[3].colors.g = (uint8_t)(g3 * 255.0f);
	_vertexData[3].colors.b = (uint8_t)(b3 * 255.0f);
	_vertexData[3].colors.a = (uint8_t)(a3 * 255.0f);   
}

const Color3B& LayerGradient::getStartColor() const
{
	return _realColor;
}

void LayerGradient::setStartColor(const Color3B& color)
{
	setColor(color);
}

void LayerGradient::setEndColor(const Color3B& color)
{
	_endColor = color;
	updateColor();
}

const Color3B& LayerGradient::getEndColor() const
{
	return _endColor;
}

void LayerGradient::setStartOpacity(uint8_t o)
{
	_startOpacity = o;
	updateColor();
}

uint8_t LayerGradient::getStartOpacity() const
{
	return _startOpacity;
}

void LayerGradient::setEndOpacity(uint8_t o)
{
	_endOpacity = o;
	updateColor();
}

uint8_t LayerGradient::getEndOpacity() const
{
	return _endOpacity;
}

void LayerGradient::setVector(const Vec2& var)
{
	_alongVector = var;
	updateColor();
}

const Vec2& LayerGradient::getVector() const
{
	return _alongVector;
}

bool LayerGradient::isCompressedInterpolation() const
{
	return _compressedInterpolation;
}

void LayerGradient::setCompressedInterpolation(bool compress)
{
	_compressedInterpolation = compress;
	updateColor();
}
 
/**
 * LayerRadialGradient
 */
LayerRadialGradient* LayerRadialGradient::create(const Color4B& startColor, const Color4B& endColor, float radius, const Vec2& center, float expand)
{
	auto layerGradient = new LayerRadialGradient();
	if (layerGradient && layerGradient->initWithColor(startColor, endColor, radius, center, expand))
	{
		layerGradient->autorelease();
		return layerGradient;
	}

	delete layerGradient;
	return nullptr;
}

LayerRadialGradient* LayerRadialGradient::create()
{
	auto layerGradient = new LayerRadialGradient();
	if (layerGradient && layerGradient->initWithColor(Color4B::BLACK, Color4B::BLACK, 0, Vec2(0, 0), 0))
	{
		layerGradient->autorelease();
		return layerGradient;
	}

	delete layerGradient;
	return nullptr;
}

LayerRadialGradient::LayerRadialGradient() {}

LayerRadialGradient::~LayerRadialGradient()
{
}

bool LayerRadialGradient::initWithColor(const orange::Color4B &startColor, const orange::Color4B &endColor, float radius, const Vec2& center, float expand)
{
	// should do it before Layer::init()
// 	for (int i = 0; i < 4; ++i)
// 		_vertices[i] = { 0.0f, 0.0f };

	if (Layer::init())
	{
		convertColor4B24F(_startColorRend, startColor);
		_startColor = startColor;

		convertColor4B24F(_endColorRend, endColor);
		_endColor = endColor;

		_expand = expand;

		setRadius(radius);
		setCenter(center);

		return true;
	}

	return false;
}

// 判断点是否在矩形内
static bool pointInRect(const Vec2& p, float minX, float minY, float maxX, float maxY)
{
	return p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY;
}

// 用一条直线（ax + by + c = 0）裁剪多边形
// 保留 ax + by + c >= 0 的部分
static void clipPolygonByLine(std::vector<Vec2>& poly,
	float a, float b, float c)
{
	if (poly.empty()) return;

	std::vector<Vec2> output;
	output.reserve(poly.size() + 4);

	size_t n = poly.size();
	for (size_t i = 0; i < n; ++i)
	{
		const Vec2& cur = poly[i];
		const Vec2& next = poly[(i + 1) % n];

		float dCur = a * cur.x + b * cur.y + c;
		float dNext = a * next.x + b * next.y + c;

		bool curIn = (dCur >= 0);
		bool nextIn = (dNext >= 0);

		if (curIn)
		{
			output.push_back(cur);
		}

		// 跨越边界，求交点
		if (curIn != nextIn)
		{
			float t = dCur / (dCur - dNext);
			Vec2 inter = cur + (next - cur) * t;
			output.push_back(inter);
		}
	}

	poly = std::move(output);
}

void LayerRadialGradient::draw(Renderer* renderer, const Mat3& transform, uint32_t flags)
{
	if (!_visible) return;
	if (_radius <= 0) return;

	const int SEGMENTS = 64;
	float w = _contentSize.width;
	float h = _contentSize.height;

	static std::vector<V2F_C4B_T2F> verts;
	static std::vector<int> indices;
	verts.clear();
	indices.clear();

	// ===== 1. 剔除 =====
	float localMinX = -FLT_MAX, localMinY = -FLT_MAX;
	float localMaxX = FLT_MAX, localMaxY = FLT_MAX;
	bool hasCull = false;

	auto camera = Camera::getVisitingCamera();
	if (camera)
	{
		Rect screenRect = camera->getScreenRectWithMargin();
		Mat3 screenToLocal = getModelViewTransformInverse();   // 直接用，零计算 transform.getInversed();

		Vec2 corners[4] = {
			{screenRect.origin.x, screenRect.origin.y},
			{screenRect.origin.x + screenRect.size.width, screenRect.origin.y},
			{screenRect.origin.x, screenRect.origin.y + screenRect.size.height},
			{screenRect.origin.x + screenRect.size.width, screenRect.origin.y + screenRect.size.height}
		};

		localMinX = FLT_MAX; localMinY = FLT_MAX;
		localMaxX = -FLT_MAX; localMaxY = -FLT_MAX;
		for (auto& c : corners)
		{
			float x = c.x, y = c.y;
			screenToLocal.out(x, y);
			//y = -y;
			localMinX = std::min(localMinX, x);
			localMinY = std::min(localMinY, y);
			localMaxX = std::max(localMaxX, x);
			localMaxY = std::max(localMaxY, y);
		}
		hasCull = true;
	}

	// 圆的 AABB vs 局部可见 AABB
	if (hasCull)
	{
		float circleMinX = _center.x - _radius;
		float circleMaxX = _center.x + _radius;
		float circleMinY = _center.y - _radius;
		float circleMaxY = _center.y + _radius;

		// 圆 AABB 和层的交集为空，且和局部可见 AABB 也没交集
		float isectMinX = std::max(circleMinX, 0.0f);
		float isectMinY = std::max(circleMinY, 0.0f);
		float isectMaxX = std::min(circleMaxX, w);
		float isectMaxY = std::min(circleMaxY, h);

		if (isectMinX > isectMaxX || isectMinY > isectMaxY) return;   // 圆和层不相交

		if (isectMaxX < localMinX || isectMinX > localMaxX ||
			isectMaxY < localMinY || isectMinY > localMaxY)
			return;   // 相交区域在屏幕外
	}

	// ===== 2. 逐扇段裁剪 =====
	std::vector<Vec2> poly;   // 每个扇段的三角形

	for (int i = 0; i < SEGMENTS; ++i)
	{
		float a1 = (float)i / SEGMENTS * 6.2831853f;
		float a2 = (float)(i + 1) / SEGMENTS * 6.2831853f;

		Vec2 p1(_center.x + cosf(a1) * _radius, _center.y + sinf(a1) * _radius);
		Vec2 p2(_center.x + cosf(a2) * _radius, _center.y + sinf(a2) * _radius);
		Vec2 pc = _center;

		// 三角形：center, p1, p2
		poly.clear();
		poly.push_back(pc);
		poly.push_back(p1);
		poly.push_back(p2);

		// 用层的 4 条边裁剪
		clipPolygonByLine(poly, 1.0f, 0.0f, 0.0f);
		clipPolygonByLine(poly, -1.0f, 0.0f, w);
		clipPolygonByLine(poly, 0.0f, 1.0f, 0.0f);
		clipPolygonByLine(poly, 0.0f, -1.0f, h);
		 

		if (poly.size() < 3) continue;   // 裁剪后不到 3 个顶点，跳过

		// ===== 3. 按扇形三角化，出顶点 =====
		int base = (int)verts.size();

		for (size_t k = 0; k < poly.size(); ++k)
		{
			//OGLOG("poly[%d]=(%f,%f)", (int)k, poly[k].x, poly[k].y);
			Vec2 p = poly[k];
		//	p.y = -p.y;
			transform.out(p.x, p.y);
		//	OGLOG("poly[%d] screen=(%f,%f)", (int)k, p.x, p.y);
			// 颜色：按“到 center 的距离”算
			// 但裁剪后的顶点不在圆周上，得根据“原点到 center 的距离”算
			// 简化：都用 endColor，或者按原点到 center 的距离插值
			// 这里简化：裁剪后的顶点用 endColor，原始 center 用 startColor
			// 更精确：按距离插值

			// 判断是“原始 center 顶点”还是“裁剪后的边界顶点”
			// 这里简化：第 0 个顶点是 center，其他用 endColor
			Color4B color = (k == 0) ? _startColor : _endColor;

			verts.push_back({ {p.x, p.y}, color, {0, 0} });
		}

		// 扇形三角化：0,1,2 / 0,2,3 / ...
		for (size_t k = 1; k + 1 < poly.size(); ++k)
		{
			indices.push_back(base + 0);
			indices.push_back(base + (int)k);
			indices.push_back(base + (int)k + 1);
		}
	}

	if (verts.empty()) return;
#ifdef ORANGE_DEBUG
	_customCommand.setOwner(this);
#endif 
	// ===== 4. 提交 =====
	_customCommand.init(_globalZOrder, nullptr,
		verts.data(), indices.data(),
		(int)verts.size(), (int)indices.size());
	renderer->addCommand(&_customCommand);
}

void LayerRadialGradient::setContentSize(const Size& size)
{
// 	_vertices[1].x = size.width;
// 	_vertices[2].y = size.height;
// 	_vertices[3].x = size.width;
// 	_vertices[3].y = size.height;
	Layer::setContentSize(size);

	//_customCommand.updateVertexBuffer(_vertices, sizeof(_vertices));
}

void LayerRadialGradient::setStartOpacity(uint8_t opacity)
{
	_startColorRend.a = opacity / 255.0f;
	_startColor.a = opacity;
}

uint8_t LayerRadialGradient::getStartOpacity() const
{
	return _startColor.a;
}

void LayerRadialGradient::setEndOpacity(uint8_t opacity)
{
	_endColorRend.a = opacity / 255.0f;
	_endColor.a = opacity;
}

uint8_t LayerRadialGradient::getEndOpacity() const
{
	return _endColor.a;
}

void LayerRadialGradient::setRadius(float radius)
{
	_radius = radius;
}

float LayerRadialGradient::getRadius() const
{
	return _radius;
}

void LayerRadialGradient::setCenter(const Vec2& center)
{
	_center = center;
}

Vec2 LayerRadialGradient::getCenter() const
{
	return _center;
}

void LayerRadialGradient::setExpand(float expand)
{
	_expand = expand;
}

float LayerRadialGradient::getExpand() const
{
	return _expand;
}

void LayerRadialGradient::setStartColor(const Color3B& color)
{
	setStartColor(Color4B(color));
}

void LayerRadialGradient::setStartColor(const orange::Color4B &color)
{
	_startColor = color;
	convertColor4B24F(_startColorRend, _startColor);
}

Color4B LayerRadialGradient::getStartColor() const
{
	return _startColor;
}

Color3B LayerRadialGradient::getStartColor3B() const
{
	return Color3B(_startColor);
}

void LayerRadialGradient::setEndColor(const Color3B& color)
{
	setEndColor(Color4B(color));
}

void LayerRadialGradient::setEndColor(const orange::Color4B &color)
{
	_endColor = color;
	convertColor4B24F(_endColorRend, _endColor);
}

Color4B LayerRadialGradient::getEndColor() const
{
	return _endColor;
}

Color3B LayerRadialGradient::getEndColor3B() const
{
	return Color3B(_endColor);
}

void LayerRadialGradient::setBlendFunc(const BlendFunc& blendFunc)
{
	_blendFunc = blendFunc;
}

BlendFunc LayerRadialGradient::getBlendFunc() const
{
	return _blendFunc;
}

void LayerRadialGradient::convertColor4B24F(Color4F& outColor, const Color4B& inColor)
{
	outColor.r = inColor.r / 255.0f;
	outColor.g = inColor.g / 255.0f;
	outColor.b = inColor.b / 255.0f;
	outColor.a = inColor.a / 255.0f;
}


/// MultiplexLayer

LayerMultiplex::LayerMultiplex()
	: _enabledLayer(0)
{
}

LayerMultiplex::~LayerMultiplex()
{
	for (const auto &layer : _layers) {
		layer->cleanup();
	}
}

LayerMultiplex * LayerMultiplex::create(Layer * layer, ...)
{
	va_list args;
	va_start(args, layer);

	LayerMultiplex * multiplexLayer = new (std::nothrow) LayerMultiplex();
	if (multiplexLayer && multiplexLayer->initWithLayers(layer, args))
	{
		multiplexLayer->autorelease();
		va_end(args);
		return multiplexLayer;
	}
	va_end(args);
	OG_SAFE_DELETE(multiplexLayer);
	return nullptr;
}

LayerMultiplex * LayerMultiplex::createWithLayer(Layer* layer)
{
	return LayerMultiplex::create(layer, nullptr);
}

LayerMultiplex* LayerMultiplex::create()
{
	LayerMultiplex* ret = new (std::nothrow) LayerMultiplex();
	if (ret && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		OG_SAFE_DELETE(ret);
	}
	return ret;
}

LayerMultiplex* LayerMultiplex::createWithArray(const Vector<Layer*>& arrayOfLayers)
{
	LayerMultiplex* ret = new (std::nothrow) LayerMultiplex();
	if (ret && ret->initWithArray(arrayOfLayers))
	{
		ret->autorelease();
	}
	else
	{
		OG_SAFE_DELETE(ret);
	}
	return ret;
}

void LayerMultiplex::addLayer(Layer* layer)
{

	_layers.pushBack(layer);
}

bool LayerMultiplex::init()
{
	if (Layer::init())
	{
		_enabledLayer = 0;
		return true;
	}
	return false;
}

bool LayerMultiplex::initWithLayers(Layer *layer, va_list params)
{
	if (Layer::init())
	{
		_layers.reserve(5);

		_layers.pushBack(layer);

		Layer *l = va_arg(params, Layer*);
		while (l) {

			_layers.pushBack(l);
			l = va_arg(params, Layer*);
		}

		_enabledLayer = 0;
		this->addChild(_layers.at(_enabledLayer));
		return true;
	}

	return false;
}

bool LayerMultiplex::initWithArray(const Vector<Layer*>& arrayOfLayers)
{
	if (Layer::init())
	{

		_layers.reserve(arrayOfLayers.size());
		_layers.pushBack(arrayOfLayers);

		_enabledLayer = 0;
		this->addChild(_layers.at(_enabledLayer));
		return true;
	}
	return false;
}

void LayerMultiplex::switchTo(int n)
{

	switchTo(n, true);
}

void LayerMultiplex::switchTo(int n, bool cleanup)
{
	OGASSERT(n < _layers.size(), "Invalid index in MultiplexLayer switchTo message");

	this->removeChild(_layers.at(_enabledLayer), cleanup);

	_enabledLayer = n;

	this->addChild(_layers.at(n));
}

void LayerMultiplex::switchToAndReleaseMe(int n)
{
	OGASSERT(n < _layers.size(), "Invalid index in MultiplexLayer switchTo message");

	this->removeChild(_layers.at(_enabledLayer), true);


	_layers.replace(_enabledLayer, nullptr);

	_enabledLayer = n;

	this->addChild(_layers.at(n));
}

// std::string LayerMultiplex::getDescription() const
// {
// 	return StringUtils::format("<LayerMultiplex | Tag = %d, Layers = %d", _tag, static_cast<int>(_children.size()));
// }

OG_END

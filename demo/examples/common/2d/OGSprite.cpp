#include "2d/OGSprite.h"
//#include "2d/Texture2D.h"

#include "base/ogTypes.h"

#include "2d/OGSpriteFrameCache.h"
#include "2d/OGSpriteFrame.h"
#include "renderer/OGTextureCache.h"
#include "renderer/OGTexture2D.h"
#include "renderer/OGRenderer.h"

#include "base/OGDirector.h"
#include "2d/OGCamera.h"
#include "platform/OGFileUtils.h"



// #include "renderer/OGTextureAtlas.h"
// #include "2d/OGSpriteBatchNode.h"

OG_BEGIN

Sprite* Sprite::create()
{
	Sprite *sprite = new (std::nothrow) Sprite();
	if (sprite && sprite->init())
	{
		sprite->autorelease();
		return sprite;
	}
	OG_SAFE_DELETE(sprite);
	return nullptr;
}
// MARK: create, init, dealloc
Sprite* Sprite::createWithTexture(Texture2D *texture)
{
	Sprite *sprite = new (std::nothrow) Sprite();
	if (sprite && sprite->initWithTexture(texture))
	{
		sprite->autorelease();
		return sprite;
	}
	OG_SAFE_DELETE(sprite);
	return nullptr;
}

Sprite* Sprite::createWithTexture(Texture2D *texture, const Rect& rect, bool rotated)
{
	Sprite *sprite = new (std::nothrow) Sprite();
	if (sprite && sprite->initWithTexture(texture, rect, rotated))
	{
		sprite->autorelease();
		return sprite;
	}
	OG_SAFE_DELETE(sprite);
	return nullptr;
}

Sprite* Sprite::create(const std::string& filename)
{
	Sprite *sprite = new (std::nothrow) Sprite();
	if (sprite && sprite->initWithFile(filename))
	{
		sprite->autorelease();
		return sprite;
	}
	OG_SAFE_DELETE(sprite);
	return nullptr;
}



Sprite* Sprite::create(const std::string& filename, const Rect& rect)
{
	Sprite *sprite = new (std::nothrow) Sprite();
	if (sprite && sprite->initWithFile(filename, rect))
	{
		sprite->autorelease();
		return sprite;
	}
	OG_SAFE_DELETE(sprite);
	return nullptr;
}

Sprite* Sprite::createWithSpriteFrame(SpriteFrame *spriteFrame)
{
	Sprite *sprite = new (std::nothrow) Sprite();
	if (sprite && spriteFrame && sprite->initWithSpriteFrame(spriteFrame))
	{
		sprite->autorelease();
		return sprite;
	}
	OG_SAFE_DELETE(sprite);
	return nullptr;
}

Sprite* Sprite::createWithSpriteFrameName(const std::string& spriteFrameName)
{
	SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);

#if COCOS2D_DEBUG > 0
	char msg[256] = { 0 };
	sprintf(msg, "Invalid spriteFrameName: %s", spriteFrameName.c_str());
	OGASSERT(frame != nullptr, msg);
#endif

	return createWithSpriteFrame(frame);
}



bool Sprite::init()
{
	initWithTexture(nullptr, Rect::ZERO);
	return true;
}

bool Sprite::initWithTexture(Texture2D *texture)
{
	OGASSERT(texture != nullptr, "Invalid texture for sprite");

	Rect rect = Rect::ZERO;
	if (texture)
		rect.size = texture->getContentSize();

	return initWithTexture(texture, rect, false);
}

bool Sprite::initWithTexture(Texture2D *texture, const Rect& rect)
{
	return initWithTexture(texture, rect, false);
}

bool Sprite::initWithFile(const std::string& filename)
{
	if (filename.empty())
	{
		OGLOG("Call Sprite::initWithFile with blank resource filename.");
		return false;
	}

	_fileName = filename;

	Texture2D *texture = _director->getTextureCache()->addImage(filename);
	if (texture)
	{
		Rect rect = Rect::ZERO;
		rect.size = texture->getContentSize();
		return initWithTexture(texture, rect);
	}

	// don't release here.
	// when load texture failed, it's better to get a "transparent" sprite then a crashed program
	// this->release();
	return false;
}

bool Sprite::initWithFile(const std::string &filename, const Rect& rect)
{
	OGASSERT(!filename.empty(), "Invalid filename");
	if (filename.empty())
		return false;

	_fileName = filename;

	Texture2D *texture = _director->getTextureCache()->addImage(filename);
	if (texture)
		return initWithTexture(texture, rect);

	// don't release here.
	// when load texture failed, it's better to get a "transparent" sprite then a crashed program
	// this->release();
	return false;
}

bool Sprite::initWithSpriteFrameName(const std::string& spriteFrameName)
{
	OGASSERT(!spriteFrameName.empty(), "Invalid spriteFrameName");
	if (spriteFrameName.empty())
		return false;

	_fileName = spriteFrameName;
	_fileType = 1;

	SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
	return initWithSpriteFrame(frame);
}

bool Sprite::initWithSpriteFrame(SpriteFrame *spriteFrame)
{
	OGASSERT(spriteFrame != nullptr, "spriteFrame can't be nullptr!");
	if (spriteFrame == nullptr)
		return false;

	bool ret = initWithTexture(spriteFrame->getTexture(), spriteFrame->getRect(), spriteFrame->isRotated());
	setSpriteFrame(spriteFrame);

	return ret;
}

void Sprite::setSpriteFrame(const std::string &spriteFrameName)
{
	OGASSERT(!spriteFrameName.empty(), "spriteFrameName must not be empty");
	if (spriteFrameName.empty())
		return;

	SpriteFrameCache *cache = SpriteFrameCache::getInstance();
	SpriteFrame *spriteFrame = cache->getSpriteFrameByName(spriteFrameName);

	OGASSERT(spriteFrame, std::string("Invalid spriteFrameName :").append(spriteFrameName).c_str());

	setSpriteFrame(spriteFrame);
}

void Sprite::setSpriteFrame(SpriteFrame *spriteFrame)
{
	// retain the sprite frame
	// do not removed by SpriteFrameCache::removeUnusedSpriteFrames
	if (_spriteFrame != spriteFrame)
	{
		OG_SAFE_RELEASE(_spriteFrame);
		_spriteFrame = spriteFrame;
		spriteFrame->retain();
	}
	//	_unflippedOffsetPositionFromCenter = spriteFrame->getOffset();

	Texture2D *texture = spriteFrame->getTexture();
	// update texture before updating texture rect
	if (texture != _texture)
		setTexture(texture);

	// update rect
	_rectRotated = spriteFrame->isRotated();
	setTextureRect(spriteFrame->getRect(), _rectRotated, spriteFrame->getOriginalSize());

	//if (spriteFrame->hasPolygonInfo())
	{
		if (_flippedX) flipX();
		if (_flippedY) flipY();
		updateColor();
	}
	//	if (spriteFrame->hasAnchorPoint())
	setAnchorPoint(spriteFrame->getAnchorPoint());
	// 	if (spriteFrame->hasCenterRect())
	// 		setCenterRect(spriteFrame->getCenterRect());
}

// designated initializer
bool Sprite::initWithTexture(Texture2D *texture, const Rect& rect, bool rotated)
{
	bool result = false;
	if (Node::init())
	{
		// 		_batchNode = nullptr;
		// 
		// 		_recursiveDirty = false;
		//		setDirty(false);

		_flippedX = _flippedY = false;

		// default transform anchor: center
		setAnchorPoint(Vec2::ANCHOR_MIDDLE);

		// zwoptex default values
		_offsetPosition.setZero();

		// clean the Quad
		memset(&_quad, 0, sizeof(_quad));

		// Atlas: Color
		_quad.bl.colors = Color4B::WHITE;
		_quad.br.colors = Color4B::WHITE;
		_quad.tl.colors = Color4B::WHITE;
		_quad.tr.colors = Color4B::WHITE;

		// update texture (calls updateBlendFunc)
		setTexture(texture);
		setTextureRect(rect, rotated, rect.size);

		// by default use "Self Render".
		// if the sprite is added to a batchnode, then it will automatically switch to "batchnode Render"
	//	setBatchNode(nullptr);
		result = true;
	}

	// 	_recursiveDirty = true;
	// 	setDirty(true);

	return result;
}

Sprite::Sprite()
{
#if OG_SPRITE_DEBUG_DRAW
	_debugDrawNode = DrawNode::create();
	addChild(_debugDrawNode);
#endif //OG_SPRITE_DEBUG_DRAW
}

Sprite::~Sprite()
{
	// 	OG_SAFE_FREE(_trianglesVertex);
	// 	OG_SAFE_FREE(_trianglesIndex);
	OG_SAFE_RELEASE(_spriteFrame);
	OG_SAFE_RELEASE(_texture);
}

/*
 * Texture methods
 */

 /*
  * This array is the data of a white image with 2 by 2 dimension.
  * It's used for creating a default texture when sprite's texture is set to nullptr.
  * Supposing codes as follows:
  *
  *   auto sp = new (std::nothrow) Sprite();
  *   sp->init();  // Texture was set to nullptr, in order to make opacity and color to work correctly, we need to create a 2x2 white texture.
  *
  * The test is in "TestCpp/SpriteTest/Sprite without texture".
  */
static unsigned char og_2x2_white_image[] = {
	// RGBA8888
	0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF
};

#define OG_2x2_WHITE_IMAGE_KEY  "/og_2x2_white_image"

// MARK: texture
void Sprite::setTexture(const std::string &filename)
{
	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(filename);
	setTexture(texture);
	//	_unflippedOffsetPositionFromCenter = Vec2::ZERO;
	Rect rect = Rect::ZERO;
	if (texture)
		rect.size = texture->getContentSize();

	setTextureRect(rect);
}


void Sprite::setTexture(Texture2D *texture)
{
	// 	auto isETC1 = texture && texture->getAlphaTextureName();
	// 	setProgramState((isETC1) ? backend::ProgramType::ETC1 : backend::ProgramType::POSITION_TEXTURE_COLOR);
	// 	OGASSERT(!_batchNode || (texture &&  texture == _batchNode->getTexture()), "OGSprite: Batched sprites should use the same texture as the batchnode");
		// accept texture==nil as argument
	OGASSERT(!texture || dynamic_cast<Texture2D*>(texture), "setTexture expects a Texture2D. Invalid argument");

	if (texture == nullptr)
	{
		// Gets the texture by key firstly.
		texture = _director->getTextureCache()->getTextureForKey(OG_2x2_WHITE_IMAGE_KEY);

		// If texture wasn't in cache, create it from RAW data.
		if (texture == nullptr)
		{
			texture = new Texture2D;
			if (!texture->loadMemData(const_cast<unsigned char*>(og_2x2_white_image), sizeof(og_2x2_white_image), true, 2, 2))
			{
				OG_SAFE_DELETE(texture);
				return;
			}


			texture = _director->getTextureCache()->addImage(og_2x2_white_image, sizeof(og_2x2_white_image), OG_2x2_WHITE_IMAGE_KEY);
			OG_SAFE_RELEASE(_texture);
		}
		updateBlendFunc();
	}
	if (_texture != texture || _texture != nullptr)
	{
		OG_SAFE_RETAIN(texture);
		OG_SAFE_RELEASE(_texture);
		_texture = texture;
	}

}
void Sprite::updateBlendFunc()
{
	if (!_texture || !_texture->hasPremultipliedAlpha())
	{
		_blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
		setOpacityModifyRGB(false);
	}
	else
	{
		_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
		setOpacityModifyRGB(true);
	}
}
Texture2D* Sprite::getTexture() const
{
	return _texture;
}


void Sprite::setTextureRect(const Rect& rect)
{
	setTextureRect(rect, false, rect.size);
}

void Sprite::setTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize)
{
	_rectRotated = rotated;

	Node::setContentSize(untrimmedSize);
	_originalContentSize = untrimmedSize;
	setVertexRect(rect);
	updateVertices();
}
void Sprite::setVertexRect(const Rect& rect)
{
	_rect = rect;
}

void Sprite::setContentSize(const Size& size)
{

	Node::setContentSize(size);
	updateVertices();
}

void Sprite::setFlippedX(bool flippedX)
{
	if (_flippedX != flippedX)
	{
		_flippedX = flippedX;
		flipX();
	}
}

bool Sprite::isFlippedX() const
{
	return _flippedX;
}

void Sprite::setFlippedY(bool flippedY)
{
#ifdef OG_USE_METAL
	if (_texture->isRenderTarget())
		flippedY = !flippedY;
#endif
	if (_flippedY != flippedY)
	{
		_flippedY = flippedY;
		flipY();
	}
}

bool Sprite::isFlippedY() const
{
	return _flippedY;
}

void Sprite::flipX()
{
	_quad.tl.vertices.x = _contentSize.width - _quad.tl.vertices.x;
	_quad.tr.vertices.x = _contentSize.width - _quad.tr.vertices.x;
	_quad.bl.vertices.x = _contentSize.width - _quad.bl.vertices.x;
	_quad.br.vertices.x = _contentSize.width - _quad.br.vertices.x;
	updateVertices();
}

void Sprite::flipY()
{
	_quad.tl.vertices.y = _contentSize.width - _quad.tl.vertices.y;
	_quad.tr.vertices.y = _contentSize.width - _quad.tr.vertices.y;
	_quad.bl.vertices.y = _contentSize.width - _quad.bl.vertices.y;
	_quad.br.vertices.y = _contentSize.width - _quad.br.vertices.y;
	updateVertices();
}

//
// MARK: RGBA protocol
//

void Sprite::updateColor()
{
	Color4B color4(_displayedColor.r, _displayedColor.g,
		_displayedColor.b, _displayedOpacity);
	//是否预乘
	if (_opacityModifyRGB)
	{
		color4.r *= _displayedOpacity / 255.0f;
		color4.g *= _displayedOpacity / 255.0f;
		color4.b *= _displayedOpacity / 255.0f;
	}

	_quad.bl.colors = _quad.tl.colors = _quad.br.colors = _quad.tr.colors = color4;

	// self render
	// do nothing
}

void Sprite::setOpacityModifyRGB(bool modify)
{
	if (_opacityModifyRGB != modify)
	{
		_opacityModifyRGB = modify;
		updateColor();
	}
}

bool Sprite::isOpacityModifyRGB() const
{
	return _opacityModifyRGB;
}

// MARK: Frames






SpriteFrame* Sprite::getSpriteFrame() const
{
	if (nullptr != this->_spriteFrame)
		return this->_spriteFrame;

	return SpriteFrame::createWithTexture(_texture, _rect);
}

void Sprite::setBlendFunc(const BlendFunc & blendFunc)
{
	_blendFunc = blendFunc;
}
const BlendFunc& Sprite::getBlendFunc()const
{
	return _blendFunc;
}


void Sprite::updateVertices() {
	// ===== 3. UV/颜色：只在 _dirty 时更新 =====
	if (_texture)
	{
		setOpacityModifyRGB(true);

		_texture->setBlendMode(_blendFunc.getBlendMode());

		Size imageSize = _originalContentSize;

		float uvX = _rect.origin.x / imageSize.width;
		float uvY = _rect.origin.y / imageSize.height;
		float uvW = (_rect.origin.x + _rect.size.width) / imageSize.width;
		float uvH = (_rect.origin.y + _rect.size.height) / imageSize.height;

		if (!_rectRotated)
		{
			// 普通情况：UV 直接对应四角
			_quad.tl.texCoords = { uvX, uvY };
			_quad.tr.texCoords = { uvW, uvY };
			_quad.bl.texCoords = { uvX, uvH };
			_quad.br.texCoords = { uvW, uvH };
		}
		// 		 else
		// 		 {
		// 
		// 		//	 a = x,y   b = w,y...c=x,h...d = w,h   abcd->dabc
		// 			 // 这里采用：tr 取 (uvX, uvH)，br 取 (uvX, uvY)，tl 取 (uvW, uvH)，bl 取 (uvW, uvY)
		// 			 // 具体方向取决于打包器是顺时针还是逆时针转，见下面说明
		// 			 _quad.tl.texCoords = { uvW, uvY };  // 屏幕左上 ← 图集右上
		// 			 _quad.tr.texCoords = { uvW, uvH };  // 屏幕右上 ← 图集右下
		// 			 _quad.bl.texCoords = { uvX, uvY };  // 屏幕左下 ← 图集左上
		// 			 _quad.br.texCoords = { uvX, uvH };  // 屏幕右下 ← 图集左下
		// 		 }
		else
		{
			// cocos 原版：逆时针 90° 映射
			_quad.tl.texCoords = { uvX, uvH };
			_quad.tr.texCoords = { uvX, uvY };
			_quad.bl.texCoords = { uvW, uvH };
			_quad.br.texCoords = { uvW, uvY };
		}
		// 	 _quad.tl.colors = { _realColor.r, _realColor.g, _realColor.b, _realColor.a };
		// 	 _quad.tr.colors = { _realColor.r, _realColor.g, _realColor.b, _realColor.a };
		// 	 _quad.bl.colors = { _realColor.r, _realColor.g, _realColor.b, _realColor.a };
		// 	 _quad.br.colors = { _realColor.r, _realColor.g, _realColor.b, _realColor.a };

		_quadCount = 1;

		// 		 _indices[0] = 0;
		// 		 _indices[1] = 1;
		// 		 _indices[2] = 2;
		// 		 _indices[3] = 1;
		// 		 _indices[4] = 3;
		// 		 _indices[5] = 2;
		// 		 _indexCount = 6;

		if (_flippedX) {
			std::swap(_quad.tl.texCoords.u, _quad.tr.texCoords.u);
			std::swap(_quad.bl.texCoords.u, _quad.br.texCoords.u);
		}
		if (_flippedY) {
			std::swap(_quad.tl.texCoords.v, _quad.bl.texCoords.v);
			std::swap(_quad.tr.texCoords.v, _quad.br.texCoords.v);
		}

	}
}
void Sprite::draw(Renderer *renderer, const Mat3 &transform, uint32_t parentFlags)
{
	if (_texture == nullptr) return;

	_texture->setBlendMode(_blendFunc.getBlendMode());
	// ===== 1. 合并偏移 =====
	Mat3 finalTransform = transform;
	finalTransform.tx += _offsetPosition.x;
	finalTransform.ty += _offsetPosition.y;

	// ===== 2. 剔除（用屏幕矩形 + margin）=====
	auto camera = Camera::getVisitingCamera();
	if (camera)
	{
		Rect vpRect = camera->getViewport();
		Size vpSize = vpRect.size;


		float mx = camera->getMarginX();
		float my = camera->getMarginY();

		float w = _contentSize.width;
		float h = _contentSize.height;
		Vec2 corners[4] = { {0,0},{w,0},{0,h},{w,h} };

		float minX = FLT_MAX, minY = FLT_MAX;
		float maxX = -FLT_MAX, maxY = -FLT_MAX;
		for (auto& c : corners) {
			float vx = c.x, vy = c.y;
			finalTransform.out(vx, vy);
			minX = std::min(minX, vx);
			minY = std::min(minY, vy);
			maxX = std::max(maxX, vx);
			maxY = std::max(maxY, vy);
		}
		float screenLeft = vpRect.origin.x - mx;
		float screenRight = vpRect.origin.x + vpRect.size.width + mx;
		float screenTop = vpRect.origin.y - my;
		float screenBottom = vpRect.origin.y + vpRect.size.height + my;

		if (maxX < screenLeft || minX > screenRight ||
			maxY < screenTop || minY > screenBottom)//camera view
			return;

	}



	if (parentFlags)
	{
		// ===== 4. 顶点位置：每帧重算（因为相机可能移动）=====
		float w = _contentSize.width;
		float h = _contentSize.height;
		Vec2 localPoints[4] = {
			{0, 0}, {w, 0}, {0, h}, {w, h}
		};
		V2F_C4B_T2F* verts[4] = { &_quad.tl, &_quad.tr, &_quad.bl, &_quad.br };
		for (int i = 0; i < 4; ++i) {
			float vx = localPoints[i].x;
			float vy = localPoints[i].y;
			finalTransform.out(vx, vy);      // 含偏移，含相机
			verts[i]->vertices.x = vx;
			verts[i]->vertices.y = vy;
		}

	}
	// ===== 5. 提交 =====

#ifdef ORANGE_DEBUG
	_quadCommand.setOwner(this);
#endif 

	_quadCommand.init(_globalZOrder, _texture->getTexture(),
		(V2F_C4B_T2F*)&_quad,
		/*_indices, */_quadCount/*, _indexCount*/);
	renderer->addCommand(&_quadCommand);
}

OG_END

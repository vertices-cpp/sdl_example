#include "OGSprite.h"
//#include "Texture2D.h"

#include "ogTypes.h"

#include "OGSpriteFrame.h"
// #include "OGTextureCache.h"
#include "OGTexture2D.h"
#include "OGRenderer.h"

#include "OGDirector.h"
#include "OGCamera.h"
#include "OGFileUtils.h"

OG_BEGIN

Sprite::Sprite() : _clip(Rect::ZERO), _offsetPosition(Vec2::ZERO)
, _opacityModifyRGB(false)

, _flippedX(false)
, _flippedY(false)
{
	_name = "Sprite";
	_bfunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
	//_bfunc.setBlendMode(SDL_BLENDMODE_NONE);
	_texture = nullptr;

	_quadCount = 1;
	//_indexCount = 6;
	 
	//_indices.resize(_indexCount);
	
}


Sprite::~Sprite()
{
	free();
	//_filename.clear();
	
}

void Sprite::free()
{
	if (_texture != nullptr)
	{
		//_director->getTextureCache()->removeTexture(_texture);
		OG_SAFE_RELEASE(_texture);
		//OG_SAFE_RELEASE(_spriteFrame);
	}

}
Sprite* Sprite::create()
{
	  
	Sprite *sprite = new (std::nothrow) Sprite();
	if (sprite )
	{
		sprite->autorelease();
		return sprite;
	}
	OG_SAFE_DELETE(sprite);
	return nullptr;

}
Sprite* Sprite::create(const std::string &filename, const Rect & clip)
{ 
	Sprite *sprite = new (std::nothrow) Sprite();
	if (sprite && sprite->initWithFile(filename, clip))
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
bool Sprite::initWithSpriteFrame(SpriteFrame *spriteFrame)
{
	OGASSERT(spriteFrame != nullptr, "spriteFrame can't be nullptr!");
	if (spriteFrame == nullptr)
		return false;

	bool ret = initWithTexture(spriteFrame->getTexture(), spriteFrame->getRect(), spriteFrame->isRotated());
	setSpriteFrame(spriteFrame);

	return ret;
}
void  Sprite::setFlippedX(bool flippedX) {
	_flippedX = flippedX;
	updateVertices();
}


void  Sprite::setFlippedY(bool flippedY) {
	_flippedY = flippedY;
	updateVertices();
}
bool  Sprite::isFlippedX() {
	return _flippedX;
}
bool  Sprite::isFlippedY() {
	return _flippedY;
}


void Sprite::setContentSize(const Size & size)
{
	Node::setContentSize(size);
}
void Sprite::setTextureClipRect(const Rect& rect)
{
	//	_rectRotated = rotated;
	_clip = rect;
	//Node::setCenter(_clip.size / 2);
	
	updateVertices();
}

bool Sprite::initWithFile(const std::string &filename, const Rect & clip)
{ 
	OGASSERT(!filename.empty(), "Invalid filename");

	Texture2D* texture = _director->getTextureCache()->addImage(filename);
	 
	if (texture != nullptr )
	{
		_filename = filename;

		setTexture(texture, clip);
		 
		return true;
	}
	
	return false;
}

void Sprite::setTexture(const std::string &filename, const Rect & clip)
{
	this->initWithFile(filename, clip);
}
void Sprite::setTexture(Texture2D* texture, const Rect & clip)
{
	assert(texture != nullptr);

	Size texSize = texture->getContentSize();

	if (clip.equals(Rect::ZERO)) //如果为0则设置为图片大小
	{
		_clip.origin = Vec2::ZERO;
		_clip.size = texSize;
		 
	}
	else
	{
		_clip = clip;
	 
	}
	if (_rectRotated)
		setContentSize(Size(_clip.size.height, _clip.size.width));  // 对调
	else
		setContentSize(_clip.size);

	OG_SAFE_RETAIN(texture);
	OG_SAFE_RELEASE(_texture);
	_texture = texture;


	setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
	updateVertices();

}


// 
void Sprite::setSpriteFrame(SpriteFrame *spriteFrame)
{
	if (spriteFrame==nullptr)
	{
		return;
	}
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
		setTexture(texture, spriteFrame->getRect());
	if (_filename != spriteFrame->getFileName())
	{
		_filename = spriteFrame->getFileName();
	}
 
	
	updateVertices();

}
// SpriteFrame* Sprite::getSpriteFrame() const
// {
// 	if (nullptr != this->_spriteFrame)
// 		return this->_spriteFrame;
// 
// 	return SpriteFrame::createWithTexture(_texture,
// 		 _clip );
// }



Sprite* Sprite::createWithTexture(Texture2D* texture, const Rect& rect, bool rotated)
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
bool Sprite::initWithTexture(Texture2D* texture, const Rect& rect, bool  rotated)
{
	OGASSERT(texture != nullptr, "Invalid texture for sprite");
	bool result = false;
	if (Node::init())
	{

		//	setDirty(false);
		
		 

		_flippedX = _flippedY = false;
		 
		_offsetPosition.setZero();

		// clean the Quad
		memset(&_quad, 0, sizeof(_quad));

		// Atlas: Color
		_quad.bl.colors = Color4B::WHITE;
		_quad.br.colors = Color4B::WHITE;
		_quad.tl.colors = Color4B::WHITE;
		_quad.tr.colors = Color4B::WHITE;

// 		_indices[0] = 0;
// 		_indices[1] = 1;
// 		_indices[2] = 2;
// 		_indices[3] = 1;
// 		_indices[4] = 3;
// 		_indices[5] = 2;

		// update texture (calls updateBlendFunc)
		_clip = { 0,0,rect.size.width,rect.size.height };
		_rectRotated = rotated;
		_contentSize = rect.size;
		setTexture(texture,rect);
		
		
		//添加渲染模式
	 //	_texture->setBlendMode(_bfunc.getBlendMode());
	//	setTextureRect(rect, rotated, rect.size);

		// by default use "Self Render".
		// if the sprite is added to a batchnode, then it will automatically switch to "batchnode Render"
	//	setBatchNode(nullptr);
		result = true;
	}

// 	_recursiveDirty = true;
// 	setDirty(true);

	return result;
}



void Sprite::setBlendFunc(const BlendFunc &blendFunc)
{
	_bfunc = blendFunc;
}

 void Sprite::setOpacityModifyRGB(bool modify)
 {
	 if (_opacityModifyRGB != modify)
	 {
		 _opacityModifyRGB = modify;
		 updateColor();
	 }
 }




 void Sprite::updateVertices() {
	 // ===== 3. UV/颜色：只在 _dirty 时更新 =====
	 if (_texture)
	 {
		 setOpacityModifyRGB(true);

		 _texture->setBlendMode(_bfunc.getBlendMode());

		 Size imageSize = _texture->getContentSize();

		 float uvX = _clip.origin.x / imageSize.width;
		 float uvY = _clip.origin.y / imageSize.height;
		 float uvW = (_clip.origin.x + _clip.size.width) / imageSize.width;
		 float uvH = (_clip.origin.y + _clip.size.height) / imageSize.height;

		 if (!_rectRotated)
		 {
			 // 普通情况：UV 直接对应四角
			 _quad.tl.texCoords = { uvX, uvY };
			 _quad.tr.texCoords = { uvW, uvY };
			 _quad.bl.texCoords = { uvX, uvH };
			 _quad.br.texCoords = { uvW, uvH };
		 }
		 else
		 {

		//	 a = x,y   b = w,y...c=x,h...d = w,h   abcd->dabc
			 // 这里采用：tr 取 (uvX, uvH)，br 取 (uvX, uvY)，tl 取 (uvW, uvH)，bl 取 (uvW, uvY)
			 // 具体方向取决于打包器是顺时针还是逆时针转，见下面说明
			 _quad.tl.texCoords = { uvW, uvY };  // 屏幕左上 ← 图集右上
			 _quad.tr.texCoords = { uvW, uvH };  // 屏幕右上 ← 图集右下
			 _quad.bl.texCoords = { uvX, uvY };  // 屏幕左下 ← 图集左上
			 _quad.br.texCoords = { uvX, uvH };  // 屏幕右下 ← 图集左下
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

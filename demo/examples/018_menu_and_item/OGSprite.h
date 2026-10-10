#ifndef _SPRITE_H_
#define _SPRITE_H_


#include "OGNode.h"
#include "OGTexture2D.h" 
#include "OGQuadCommand.h"

OG_BEGIN

//class Texture2D;
class SpriteFrame;

class Sprite :public Node
{
	BlendFunc _bfunc;
	std::string _filename;
	Texture2D *_texture = nullptr;
	SpriteFrame* _spriteFrame = nullptr;

	Vec2 _offsetPosition;
	Rect _clip;
	bool _rectRotated = false;
	bool _flippedX, _flippedY;

	bool _opacityModifyRGB;

	//SpriteFrame *_spriteFrame = nullptr;

	V2F_C4B_T2F_Quad _quad;
	
	int _quadCount;

// 	int _indices[6];
// 	int _indexCount;

	QuadCommand _quadCommand;
public:
	
	Sprite();
	~Sprite();

	void free();

	static Sprite * create();

	static Sprite* create(const std::string &filename, const Rect & clip = Rect::ZERO);

	static Sprite * createWithSpriteFrame(SpriteFrame * spriteFrame);

	bool initWithSpriteFrame(SpriteFrame * spriteFrame);

	void setFlippedX(bool flippedX);
	void setFlippedY(bool flippedY);
	bool isFlippedX();
	bool isFlippedY();

	void setContentSize(const Size & size);
	void setTextureClipRect(const Rect & rect);
	 
	bool initWithFile(const std::string & filename, const Rect & rect = Rect::ZERO);

	void setTexture(const std::string & filename, const Rect & rect = Rect::ZERO);

	void setTexture(Texture2D* texture, const Rect & rect = Rect::ZERO);
	void  setOffset(const Vec2& pos)
	{
		_offsetPosition = pos;
 
	}
	Vec2  getOffset()const
	{
		return _offsetPosition;
	}
	 
	void setSpriteFrame(SpriteFrame * spriteFrame);

	SpriteFrame * getSpriteFrame() const;
	
	void  setPosition(const Vec2& pos)
	{
		
		Node::setPosition(pos);
		 _transformDirty = true;
	}

	void  setPosition(float x, float y)
	{ 
		Node::setPosition(x, y); 
	}

	void updateColor() {
		Color4B color4(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);
		//是否预乘
		if (_opacityModifyRGB)
		{
			color4.r *= _displayedOpacity / 255.0f;
			color4.g *= _displayedOpacity / 255.0f;
			color4.b *= _displayedOpacity / 255.0f;
		}

		  _quad.bl.colors = _quad.tl.colors = _quad.br.colors = _quad.tr.colors = color4;
 
		 
	}
	void setOpacityModifyRGB(bool modify);
	

	static Sprite * createWithTexture(Texture2D* texture, const Rect & rect, bool  rotated=false);
	virtual bool initWithTexture(Texture2D* texture, const Rect & rect, bool  rotated = false);
 

	void setBlendFunc(const BlendFunc &blendFunc);
	BlendFunc getBlendFunc() { return _bfunc; }
	 
	void updateVertices();
	void draw(Renderer *renderer, const Mat3 &transform, uint32_t parentFlags);
	
private:
	friend class JsonLayer;
	

 
	Sprite(const Sprite &) = delete; 
	Sprite &operator =(const Sprite &) = delete;
};

OG_END

#endif


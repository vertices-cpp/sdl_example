#ifndef _SPRITE_H_
#define _SPRITE_H_


#include "2d/OGNode.h"
#include "renderer/OGTexture2D.h" 
#include "renderer/OGQuadCommand.h"

OG_BEGIN

//class Texture2D;
class SpriteFrame;
//class SpriteBatchNode;

class Sprite :public Node//, public TextureProtocol
{
public:
	// 	enum class RenderMode {
	// 		QUAD,
	// 		POLYGON,
	// 		SLICE9,
	// 		QUAD_BATCHNODE
	// 	};

protected:
	BlendFunc _blendFunc;
	std::string _fileName;
	int _fileType = 0;

	Texture2D *_texture = nullptr;
	SpriteFrame* _spriteFrame = nullptr;


	Vec2 _offsetPosition;
	Rect _rect;
	Size _originalContentSize;

	bool _rectRotated = false;
	bool _flippedX, _flippedY;

	bool _opacityModifyRGB;
	V2F_C4B_T2F_Quad _quad;

	int _quadCount;
	QuadCommand _quadCommand;

public:

	bool initWithTexture(Texture2D * texture, const Rect & rect, bool rotated);

	Sprite();
	~Sprite();

	static Sprite * create();
	static Sprite * create(const std::string & filename);
	static Sprite * createWithTexture(Texture2D * texture);
	static Sprite * createWithTexture(Texture2D * texture, const Rect & rect, bool rotated = false);


	static Sprite * create(const std::string & filename, const Rect & rect);
	static Sprite * createWithSpriteFrame(SpriteFrame * spriteFrame);
	static Sprite * createWithSpriteFrameName(const std::string & spriteFrameName);

	virtual bool init();
	virtual bool initWithTexture(Texture2D * texture);
	virtual bool initWithTexture(Texture2D * texture, const Rect & rect);
	virtual bool initWithFile(const std::string & filename);
	virtual bool initWithFile(const std::string & filename, const Rect & rect);
	virtual bool initWithSpriteFrameName(const std::string & spriteFrameName);
	virtual bool initWithSpriteFrame(SpriteFrame * spriteFrame);

	virtual void setSpriteFrame(const std::string & spriteFrameName);
	virtual void setSpriteFrame(SpriteFrame * spriteFrame);

	virtual void setTexture(const std::string & filename);

	virtual void setTexture(Texture2D * texture);
	virtual void updateBlendFunc();
	virtual Texture2D * getTexture() const;
	virtual void setTextureRect(const Rect & rect);
	virtual void setTextureRect(const Rect & rect, bool rotated, const Size & untrimmedSize);
	virtual void setVertexRect(const Rect & rect);
	virtual void setContentSize(const Size & size);
	void setFlippedX(bool flippedX);
	bool isFlippedX() const;
	void setFlippedY(bool flippedY);
	bool isFlippedY() const;
	void flipX();
	void flipY();
	virtual void updateColor();

	virtual void setOpacityModifyRGB(bool modify);
	virtual bool isOpacityModifyRGB() const;

	virtual SpriteFrame * getSpriteFrame() const;

	void setBlendFunc(const BlendFunc &blendFunc);
	const BlendFunc& getBlendFunc()const;
	int getResourceType() const { return _fileType; }
	bool isTextureRectRotated() const { return _rectRotated; }
	const std::string& getResourceName() const { return _fileName; }

	V2F_C4B_T2F_Quad getQuad() const { return _quad; }
	const Rect& getTextureRect() const { return _rect; }

	void updateVertices();
	void draw(Renderer *renderer, const Mat3 &transform, uint32_t parentFlags);

private:
	friend class JsonLayer;



	Sprite(const Sprite &) = delete;
	Sprite &operator =(const Sprite &) = delete;

};

OG_END

#endif


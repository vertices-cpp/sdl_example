#ifndef _SPRITE_H_
#define _SPRITE_H_


#include "2d/OGNode.h"
#include "renderer/OGTexture2D.h" 
#include "renderer/OGQuadCommand.h"

OG_BEGIN

//class Texture2D;
class SpriteFrame;
// class SpriteBatchNode;
// class TextureAtlas;

class Sprite :public Node//, public TextureProtocol
{

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
	enum class RenderMode {
		QUAD,
		POLYGON,
		SLICE9,
		QUAD_BATCHNODE
	};

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
	
	
	virtual void setVertexRect(const Rect & rect);
	virtual void setTextureRect(const Rect & rect);
	virtual void setTextureRect(const Rect & rect, bool rotated, const Size & untrimmedSize);
	virtual void setContentSize(const Size & size);
	void flipX();
	void flipY();
	void setFlippedX(bool flippedX);
	bool isFlippedX() const;
	void setFlippedY(bool flippedY);
	bool isFlippedY() const;
	
	
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

	//批部分
// 	virtual void updateTransform(); 
// 	virtual SpriteBatchNode* getBatchNode() const;
	//virtual void setBatchNode(SpriteBatchNode *spriteBatchNode);

	//unsigned int getAtlasIndex() const { return _atlasIndex; }
	//void setAtlasIndex(unsigned int atlasIndex) { _atlasIndex = atlasIndex; }
	//virtual bool isDirty() const { return _dirty; }
	//virtual void setDirty(bool dirty) { _dirty = dirty; }

	//virtual void sortAllChildren() override;
	//virtual void setReorderChildDirtyRecursively();
	//void updatePoly();
	//TextureAtlas* getTextureAtlas() const { return _textureAtlas; }
	//void setTextureAtlas(TextureAtlas *textureAtlas) { _textureAtlas = textureAtlas; }
	//void setTextureCoords(const Rect & rectInPoints);
	//void setTextureCoords(const Rect & rectInPoints, V2F_C4B_T2F_Quad * outQuad);

	//void setVertexCoords(const Rect & rect, V2F_C4B_T2F_Quad * outQuad);
protected:
		////批渲染
		//TextureAtlas* _textureAtlas = nullptr;          /// SpriteBatchNode texture atlas (weak reference)
		//unsigned int _atlasIndex = 0;
		//RenderMode _renderMode = Sprite::RenderMode::QUAD;
		//SpriteBatchNode* _batchNode = nullptr;
		//bool _dirty = false;             /// Whether the sprite needs to be updated
		//bool _recursiveDirty = false;    /// Whether all of the sprite's children needs to be updated
		//bool _shouldBeHidden = false;    /// should not be drawn because one of the ancestors is not visible
		//Mat3 _transformToBatch;
public:
//渲染相关
	void updateVertices();
	void draw(Renderer *renderer, const Mat3 &transform, uint32_t parentFlags);

private:
	friend class JsonLayer;



	Sprite(const Sprite &) = delete;
	Sprite &operator =(const Sprite &) = delete;
	
};

OG_END

#endif


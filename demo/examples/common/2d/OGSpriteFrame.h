
#ifndef _OG_SPRITE_FRAME_H_
#define _OG_SPRITE_FRAME_H_

#include "2d/OGNode.h"
//#include "2d/OGAutoPolygon.h"
#include "base/OGRef.h"
#include "math/OGGeometry.h"

OG_BEGIN

class Texture2D;
 
class SpriteFrame : public Ref, public Clonable
{
public: 
	static SpriteFrame* create(const std::string& filename, const Rect& rect); 
	static SpriteFrame* create(const std::string& filename, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize);
 
	static SpriteFrame* createWithTexture(Texture2D* pobTexture, const Rect& rect); 
	static SpriteFrame* createWithTexture(Texture2D* pobTexture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize);
 
	const Rect& getRectInPixels() const { return _rectInPixels; } 
	void setRectInPixels(const Rect& rectInPixels); 
	bool isRotated() const { return _rotated; } 
	void setRotated(bool rotated) { _rotated = rotated; }

	/** Get rect of the frame.
	 *
	 * @return The rect of the sprite frame.
	 */
	const Rect& getRect() const { return _rect; }
	/** Set rect of the frame.
	 *
	 * @param rect The rect of the sprite.
	 */
	void setRect(const Rect& rect);

	/** Get center rect of the frame.
	 *
	 * Useful to create 9-slice sprites
	 *
	 * @return The center rect of the sprite frame in points
	 */
	const Rect& getCenterRect() const { return _centerRect; } 
	void setCenterRectInPixels(const Rect& centerRect); 
	bool hasCenterRect() const; 
	const Vec2& getOffsetInPixels() const; 
	void setOffsetInPixels(const Vec2& offsetInPixels); 
	const Size& getOriginalSizeInPixels() const { return _originalSizeInPixels; } 
	void setOriginalSizeInPixels(const Size& sizeInPixels) { _originalSizeInPixels = sizeInPixels; }

	/** Get original size of the trimmed image.
	 *
	 * @return The original size of the trimmed image.
	 */
	const Size& getOriginalSize() const { return _originalSize; }
	/** Set original size of the trimmed image.
	 *
	 * @param sizeInPixels The original size of the trimmed image.
	 */
	void setOriginalSize(const Size& sizeInPixels) { _originalSize = sizeInPixels; }
	 
	Texture2D* getTexture(); 
	void setTexture(Texture2D* pobTexture);

	/** Get offset of the frame.
	 *
	 * @return The offset of the sprite frame.
	 */
	const Vec2& getOffset() const;
	/** Set offset of the frame.
	 *
	 * @param offsets The offset of the sprite frame.
	 */
	void setOffset(const Vec2& offsets);

	/** Get anchor point of the frame.
	 *
	 * @return The anchor point of the sprite frame.
	 */
	const Vec2& getAnchorPoint() const;
	/** Set anchor point of the frame.
	 *
	 * @param anchorPoint The anchor point of the sprite frame.
	 */
	void setAnchorPoint(const Vec2& anchorPoint);
	/** Check if anchor point is defined for the frame.
	 *
	 * @return true if anchor point is available.
	 */
	bool hasAnchorPoint() const;

	// Overrides
	virtual SpriteFrame *clone() const override; 
// 	void setPolygonInfo(const PolygonInfo &polygonInfo); 
// 	const PolygonInfo& getPolygonInfo() const; 
// 	bool hasPolygonInfo() const;

public:
	/**
	 * @lua NA
	 */
	SpriteFrame(); 
	virtual ~SpriteFrame(); 
	bool initWithTexture(Texture2D* pobTexture, const Rect& rect); 
	bool initWithTextureFilename(const std::string& filename, const Rect& rect);
	bool initWithTexture(Texture2D* pobTexture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize);
	bool initWithTextureFilename(const std::string& filename, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize);

protected:
	Vec2 _offset;
	Vec2 _anchorPoint;
	Size _originalSize;
	Rect _rectInPixels;
	bool   _rotated;
	Rect _rect;
	Rect _centerRect;
	Vec2 _offsetInPixels;
	Size _originalSizeInPixels;
	Texture2D *_texture;
	std::string  _textureFilename; 
};

// end of _2d group
/// @}

OG_END

#endif //_OG_SPRITE_FRAME_H_

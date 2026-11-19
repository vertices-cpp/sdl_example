#ifndef _OG_DRAW_NODE_H_
#define _OG_DRAW_NODE_H_

#include "2d/OGNode.h"
#include "base/ogTypes.h" 
#include "math/Vec2.h"
#include "renderer/OGCustomCommand.h"

#include "renderer/OGTexture2D.h"

OG_BEGIN

class   DrawNode : public Node
{
	BlendFunc _bfunc;
	bool _opacityModifyRGB;
	bool _isolated = false;

	std::vector<V2F_C4B_T2F> _verts;
	std::vector<int> _indices;

	std::vector<int> _primVertexCounts;

	std::vector<V2F_C4B_T2F> _outVerts;
	std::vector<int>_outIndices;

	int _vertCount;
	int _indexCount;
	CustomCommand _customCommand;

	void drawLineBase(const Vec2& origin, const Vec2& dest,
		float lineWidth, const Color4F& color);
public:

	static DrawNode* create();
	virtual bool init() override;


	void drawPoint(const Vec2& point, const float pointSize, const Color4F &color);
	void drawDot(const Vec2 & pos, float radius, const Color4F & color);

	void drawLine(const Vec2 & origin, const Vec2 & dest, const Color4F & color);

	void drawSegment(const Vec2 & a, const Vec2 & b, float width, const Color4F & color);

	void drawPolygon(const Vec2 * verts, int count, const Color4F & fillColor, float borderWidth, const Color4F & borderColor);

	void drawTriangle(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Color4F &color);
	void drawFillTriangle(const Vec2 & p1, const Vec2 & p2, const Vec2 & p3, const Color4F & color);
	void drawRect(const Vec2 &origin, const Vec2 &destination, const Color4F &color);
	void drawFillRect(const Vec2 & pos, const Size &size, const Color4F & color);
	void drawCircle(const Vec2& center, float radius, float lineWidth, const Color4F& color);
	void drawFillCircle(const Vec2& center, float radius, int segments, const Color4F& color);

	void drawCubicBezier(const Vec2& p0, const Vec2& p1,
		const Vec2& p2, const Vec2& p3,
		unsigned int segments, float lineWidth,
		const Color4F& color);

	void clear() {
		_verts.clear();
		_indices.clear();
		_primVertexCounts.clear();
		_vertCount = _indexCount = 0;
	}
	const BlendFunc& getBlendFunc() const;
	void setBlendFunc(const BlendFunc &blendFunc);
	//virtual void visit(Renderer* renderer, const Mat3 &parentTransform, uint32_t parentFlags) override;
	virtual void draw(Renderer *renderer, const Mat3 &transform, uint32_t parentFlags) override;
	DrawNode();
	virtual ~DrawNode() { clear(); }

	void setIsolated(bool isolated) { _isolated = isolated; }
	bool isIsolated() const { return _isolated; }
private:

	OG_DISALLOW_COPY_AND_ASSIGN(DrawNode);
};

OG_END
#endif
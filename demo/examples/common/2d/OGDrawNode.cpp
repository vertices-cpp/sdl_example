#include "2d/OGDrawNode.h"
#include "2d/OGCamera.h"
#include "renderer/OGRenderer.h"

OG_BEGIN

inline uint8_t toByte(float f) {
	if (f <= 0.0f) return 0;
	if (f >= 1.0f) return 255;
	return static_cast<uint8_t>(f * 255.0f + 0.5f);
}
inline Color4B  Color4FToColor4B(const Color4F &f) {
	Color4B ret;
	ret.r = toByte(f.r);
	ret.g = toByte(f.g);
	ret.b = toByte(f.b);
	ret.a = toByte(f.a);
	return ret;
}

DrawNode::DrawNode()
{
	_name = "DrawNode";
	_bfunc = BlendFunc::ALPHA_NON_PREMULTIPLIED; 
	_vertCount = 0;
}
DrawNode* DrawNode::create()
{
	DrawNode* ret = new (std::nothrow) DrawNode();
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

bool DrawNode::init()
{
	clear();

	return true;
}

void DrawNode::drawPoint(const Vec2 & pos, const float pointSize, const Color4F & color)
{
	drawFillRect(pos, Size(pointSize, pointSize), color);
}

// OGDrawNode.cpp
void DrawNode::drawDot(const Vec2& pos, float radius, const Color4F& color)
{
	const int segments = 16;   // 调试圆点，16 段够
	drawFillCircle(pos, radius, segments, color);
}

void DrawNode::drawLineBase(const Vec2& origin, const Vec2& dest,
	float lineWidth, const Color4F& color)//私有
{

	Color4B final_color = Color4FToColor4B(color);

	/* 带有长度和方向的箭头 */
	Vec2 d = dest - origin;
	/* 两点之间的距离 */
	float len = sqrtf(d.x * d.x + d.y * d.y);
	if (len < 0.0001f) return;

	// 垂直单位向量
	Vec2 n(-d.y / len, d.x / len);
	float hw = lineWidth * 0.5f;

	Vec2 p1 = origin + n * hw;
	Vec2 p2 = origin - n * hw;
	Vec2 p3 = dest + n * hw;
	Vec2 p4 = dest - n * hw;



	V2F_C4B_T2F tl = { {p1.x, p1.y}, final_color, {0, 0} };
	V2F_C4B_T2F tr = { {p3.x, p3.y}, final_color, {1, 0} };
	V2F_C4B_T2F bl = { {p2.x, p2.y}, final_color, {0, 1} };
	V2F_C4B_T2F br = { {p4.x, p4.y}, final_color, {1, 1} };
	_verts.push_back(tl);
	_verts.push_back(tr);
	_verts.push_back(bl);
	_verts.push_back(br);

	int idx[6] = { _vertCount + 0, _vertCount + 1, _vertCount + 2, _vertCount + 1,_vertCount + 3, _vertCount + 2 };
	_indices.insert(_indices.end(), std::begin(idx), std::end(idx));
	_vertCount += 4;
	_indexCount += 6;
	//添加索引
	_primVertexCounts.push_back(4);
}

void DrawNode::drawLine(const Vec2& origin, const Vec2& dest, const Color4F& color)
{
	//init();
	drawLineBase(origin, dest, 1.0f, color);
}
void DrawNode::drawSegment(const Vec2& a, const Vec2& b, float width, const Color4F& color)
{
	drawLineBase(a, b, width, color);
}

void DrawNode::drawPolygon(const Vec2* verts, int count,
	const Color4F& fillColor,
	float borderWidth,
	const Color4F& borderColor)
{
	if (count < 3) return;

	//每个函数里都有相应索引
	// 1. 填充：以 verts[0] 为中心，扇形三角剖分
	for (int i = 1; i < count - 1; ++i)
	{
		drawFillTriangle(verts[0], verts[i], verts[i + 1], fillColor);
	}

	// 2. 描边：画每条边
	for (int i = 0; i < count; ++i)
	{
		int j = (i + 1) % count;
		drawLineBase(verts[i], verts[j], borderWidth, borderColor);
	}
}

void DrawNode::drawTriangle(const Vec2 & p1, const Vec2 & p2, const Vec2 & p3, const Color4F & color)
{
	//init();
	drawLineBase(p1, p2, 1.0f, color);
	drawLineBase(p2, p3, 1.0f, color);
	drawLineBase(p1, p3, 1.0f, color);
}
void DrawNode::drawFillTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4F& color)
{
	Color4B final_color = Color4FToColor4B(color);


	V2F_C4B_T2F  tl = { {p1.x, p1.y}, final_color, {0,0} };
	V2F_C4B_T2F  tr = { {p2.x, p2.y}, final_color, {1,0} };
	V2F_C4B_T2F  bl = { {p3.x, p3.y}, final_color, {0,1} };


	_verts.push_back(tl);
	_verts.push_back(tr);
	_verts.push_back(bl);

	_indices.push_back(_vertCount + 0);
	_indices.push_back(_vertCount + 1);
	_indices.push_back(_vertCount + 2);
	// 	_indices.push_back(_vertCount + 1);
	// 	_indices.push_back(_vertCount + 3);
	// 	_indices.push_back(_vertCount + 2);

	_vertCount += 3;

	_primVertexCounts.push_back(3);
}

void DrawNode::drawRect(const Vec2 & origin, const Vec2 & destination, const Color4F & color)
{
	//init();
	Size size = (Size)(destination - origin);

	Vec2 topLeft = origin;
	Vec2 topRight = Vec2(origin.x + size.width, origin.y);
	Vec2 bottomLeft = Vec2(origin.x, origin.y + size.height);
	Vec2 bottomRight = Vec2(origin.x + size.width, origin.y + size.height);

	drawLineBase(origin, topRight, 1, color);
	drawLineBase(topRight, bottomRight, 1, color);
	drawLineBase(bottomRight, bottomLeft, 1, color);
	drawLineBase(bottomLeft, topLeft, 1, color);
}

void DrawNode::drawFillRect(const Vec2 & pos, const Size &size, const Color4F & color)
{
	//init();

	Color4B final_color = Color4FToColor4B(color);


	float x = pos.x;
	float y = pos.y;


	V2F_C4B_T2F tl = { {x , y},final_color,{0,0} };
	V2F_C4B_T2F tr = { {x + size.width , y},final_color,{1,0} };
	V2F_C4B_T2F bl = { {x ,y + size.height},final_color,{0,1} };
	V2F_C4B_T2F br = { {x + size.width , y + size.height} , final_color,{1,1} };

	_verts.push_back(tl);
	_verts.push_back(tr);
	_verts.push_back(bl);
	_verts.push_back(br);

	_indices.push_back(_vertCount + 0);
	_indices.push_back(_vertCount + 1);
	_indices.push_back(_vertCount + 2);
	_indices.push_back(_vertCount + 1);
	_indices.push_back(_vertCount + 3);
	_indices.push_back(_vertCount + 2);

	_vertCount += 4;

	_primVertexCounts.push_back(4);
}

void DrawNode::drawCircle(const Vec2& center, float radius, float lineWidth, const Color4F& color)
{
	Color4B final_color = Color4FToColor4B(color);

	const int segments = 32;
	//init();

	float r1 = radius - lineWidth * 0.5f;
	float r2 = radius + lineWidth * 0.5f;

	for (int i = 0; i < segments; ++i) {
		float a1 = (float)i / segments * 6.2831853f;
		float a2 = (float)(i + 1) / segments * 6.2831853f;

		Vec2 p1(center.x + cosf(a1) * r1, center.y + sinf(a1) * r1);
		Vec2 p2(center.x + cosf(a1) * r2, center.y + sinf(a1) * r2);
		Vec2 p3(center.x + cosf(a2) * r1, center.y + sinf(a2) * r1);
		Vec2 p4(center.x + cosf(a2) * r2, center.y + sinf(a2) * r2);

		V2F_C4B_T2F tl = { {p1.x, p1.y}, final_color, {0,0} };
		V2F_C4B_T2F tr = { {p2.x, p2.y}, final_color, {1,0} };
		V2F_C4B_T2F bl = { {p3.x, p3.y}, final_color, {0,1} };
		V2F_C4B_T2F br = { {p4.x, p4.y}, final_color, {1,1} };

		_verts.push_back(tl);
		_verts.push_back(tr);
		_verts.push_back(bl);
		_verts.push_back(br);

		_indices.push_back(_vertCount + 0);
		_indices.push_back(_vertCount + 1);
		_indices.push_back(_vertCount + 2);
		_indices.push_back(_vertCount + 1);
		_indices.push_back(_vertCount + 3);
		_indices.push_back(_vertCount + 2);

		_vertCount += 4;
		_indexCount += 6;
		_primVertexCounts.push_back(4);
	}
}


void DrawNode::drawFillCircle(const Vec2& center, float radius,
	int segments, const Color4F& color)
{
	//init();
	Color4B final_color = Color4FToColor4B(color);

	Vec2 prev(center.x + radius, center.y);   // 起始点（角度 0）
	for (int i = 1; i <= segments; ++i) {
		float a = (float)i / segments * 6.2831853f;
		Vec2 cur(center.x + cosf(a) * radius,
			center.y + sinf(a) * radius);

		// 三角形：center, prev, cur

		V2F_C4B_T2F tl = { {center.x, center.y}, final_color, {0,0} };
		V2F_C4B_T2F tr = { {prev.x, prev.y},     final_color, {1,0} };
		V2F_C4B_T2F bl = { {cur.x, cur.y},       final_color, {0,1} };
		V2F_C4B_T2F br = { {cur.x, cur.y},       final_color, {1,1} };   // 补

		_verts.push_back(tl);
		_verts.push_back(tr);
		_verts.push_back(bl);
		_verts.push_back(br);

		_indices.push_back(_vertCount + 0);
		_indices.push_back(_vertCount + 1);
		_indices.push_back(_vertCount + 2);
		_indices.push_back(_vertCount + 1);
		_indices.push_back(_vertCount + 3);
		_indices.push_back(_vertCount + 2);

		_vertCount += 4;
		_indexCount += 6;

		_primVertexCounts.push_back(4);

		prev = cur;
	}

}
//三次贝塞尔：起点、控制点1、控制点2、终点、段数、线宽、颜色
// origin
// control1
// control2
// destination
// segments，越大越平滑
// lineWidth
void DrawNode::drawCubicBezier(const Vec2& p0, const Vec2& p1,
	const Vec2& p2, const Vec2& p3,
	unsigned int segments, float lineWidth,
	const Color4F& color)
{

	if (segments < 1) segments = 1;

	Color4B final_color = Color4FToColor4B(color);

	float hw = lineWidth * 0.5f;
	float step = 1.0f / segments;

	Vec2 prev(p0.x, p0.y);

	for (unsigned int i = 1; i <= segments; ++i) {
		float t = i * step;
		float mt = 1.0f - t;
		float mt2 = mt * mt, mt3 = mt2 * mt;
		float t2 = t * t, t3 = t2 * t;

		Vec2 cur(
			mt3*p0.x + 3 * mt2*t*p1.x + 3 * mt*t2*p2.x + t3 * p3.x,
			mt3*p0.y + 3 * mt2*t*p1.y + 3 * mt*t2*p2.y + t3 * p3.y
		);

		// ---- 内联 drawLine ----
		Vec2 d = cur - prev;
		float len = sqrtf(d.x*d.x + d.y*d.y);
		if (len > 0.0001f) {
			Vec2 n(-d.y / len, d.x / len);
			Vec2 q1 = prev + n * hw;
			Vec2 q2 = prev - n * hw;
			Vec2 q3 = cur + n * hw;
			Vec2 q4 = cur - n * hw;

			int base = _vertCount;

			V2F_C4B_T2F tl = { {q1.x, q1.y}, final_color, {0,0} };
			V2F_C4B_T2F tr = { {q3.x, q3.y}, final_color, {0,0} };
			V2F_C4B_T2F bl = { {q2.x, q2.y}, final_color, {0,0} };
			V2F_C4B_T2F br = { {q4.x, q4.y}, final_color, {0,0} };

			_verts.push_back(tl);
			_verts.push_back(tr);
			_verts.push_back(bl);
			_verts.push_back(br);

			_indices.push_back(base + 0);
			_indices.push_back(base + 1);
			_indices.push_back(base + 2);
			_indices.push_back(base + 1);
			_indices.push_back(base + 3);
			_indices.push_back(base + 2);

			_vertCount += 4;

			_indexCount += 6;

			_primVertexCounts.push_back(4);
		}
		prev = cur;
	}
}

const BlendFunc & DrawNode::getBlendFunc() const
{
	return _bfunc;
}

void DrawNode::setBlendFunc(const BlendFunc & blendFunc)
{
	_bfunc = blendFunc;
}

// void DrawNode::draw(Renderer* renderer, const Mat3& transform, uint32_t flags)
// {
// 	if (_quads.empty() || _indices.empty() || !_visible) return;
// 
// 	// ===== 剔除（可选）=====
// 	auto camera = Camera::getVisitingCamera();
// 	if (camera) {
// 		Rect vpRect = camera->getViewport();
// 		float mx = camera->getMarginX();
// 		float my = camera->getMarginY();
// 
// 		float minX = FLT_MAX, minY = FLT_MAX;
// 		float maxX = -FLT_MAX, maxY = -FLT_MAX;
// 		for (const auto& quad : _quads) {
// 			const V2F_C4B_T2F* verts[4] = { &quad.tl, &quad.tr, &quad.bl, &quad.br };
// 			for (int j = 0; j < 4; ++j) {
// 				float vx = verts[j]->vertices.x;
// 				float vy = verts[j]->vertices.y;
// 				transform.out(vx, vy);
// 				minX = std::min(minX, vx);
// 				minY = std::min(minY, vy);
// 				maxX = std::max(maxX, vx);
// 				maxY = std::max(maxY, vy);
// 			}
// 		}
// 
// 		float left = vpRect.origin.x - mx;
// 		float right = vpRect.origin.x + vpRect.size.width + mx;
// 		float top = vpRect.origin.y - my;
// 		float bottom = vpRect.origin.y + vpRect.size.height + my;
// 
// 		if (maxX < left || minX > right ||
// 			maxY < top || minY > bottom)
// 			return;
// 	}
// 
// 	// ===== 局部坐标 -> 屏幕坐标 =====
// 	_outVerts.clear();
// 	_outVerts.reserve(_quads.size() * 4);
// 
// 	for (const auto& quad : _quads) {
// 		const V2F_C4B_T2F* verts[4] = { &quad.tl, &quad.tr, &quad.bl, &quad.br };
// 		for (int j = 0; j < 4; ++j) {
// 			float vx = verts[j]->vertices.x;
// 			float vy = verts[j]->vertices.y;
// 			transform.out(vx, vy);   // ★ 局部 -> 屏幕（含父节点 + 相机）
// 
// 			V2F_C4B_T2F sv;
// 			sv.vertices.x = vx;
// 			sv.vertices.y = vy;
// 			sv.colors = { verts[j]->colors.r, verts[j]->colors.g,
// 						 verts[j]->colors.b, verts[j]->colors.a };
// 			sv.texCoords = { verts[j]->texCoords.u, verts[j]->texCoords.v };
// 			_outVerts.push_back(sv);
// 		}
// 	}
// 
// 	// ===== 提交 =====
// 	if (_outVerts.empty()) return;
// 
// 
// #ifdef ORANGE_DEBUG
// 	_customCommand.setOwner(this);
// #endif 
// 
// 	_customCommand.init(_globalZOrder, nullptr, _outVerts.data(), _indices.data(),
// 		(int)_outVerts.size(), (int)_indices.size());
// 	renderer->addCommand(&_customCommand);
// 
// }


// void DrawNode::visit(Renderer* renderer, const Mat3 &parentTransform, uint32_t parentFlags)
// {
// 	if (_isolated)
// 	{
// 		//ignore `parentTransform` from parent
// 		Node::visit(renderer, Mat3::IDENTITY, parentFlags);
// 	}
// 	else
// 	{
// 		Node::visit(renderer, parentTransform, parentFlags);
// 	}
// }
//在多边形或圆剔除时是按三角形出视口,所以不一定是按圆面剔除

void DrawNode::draw(Renderer* renderer, const Mat3& transform, uint32_t parentFlags)
{
	if (_verts.empty() || _indices.empty() || !_visible) return;

	auto camera = Camera::getVisitingCamera();
	if (!camera) return;

	Rect vpRect = camera->getViewport();
	float mx = camera->getMarginX();
	float my = camera->getMarginY();
	float left = vpRect.origin.x - mx;
	float right = vpRect.origin.x + vpRect.size.width + mx;
	float top = vpRect.origin.y - my;
	float bottom = vpRect.origin.y + vpRect.size.height + my;

	_outVerts.clear();
	_outIndices.clear();
	_outVerts.reserve(_verts.size());
	_outIndices.reserve(_indices.size());

	int base = 0;   // 当前图元在 _verts 里的起点
	for (size_t i = 0; i < _primVertexCounts.size(); ++i)
	{
		int vcount = _primVertexCounts[i];

		// 1. 算这个图元的 AABB（顶点变换到屏幕）
		float minX = FLT_MAX, minY = FLT_MAX;
		float maxX = -FLT_MAX, maxY = -FLT_MAX;
		for (int j = 0; j < vcount; ++j)
		{
			float vx = _verts[base + j].vertices.x;
			float vy = _verts[base + j].vertices.y;
			transform.out(vx, vy);
			minX = std::min(minX, vx);
			minY = std::min(minY, vy);
			maxX = std::max(maxX, vx);
			maxY = std::max(maxY, vy);
		}

		// 2. 不可见 → 跳过
		if (maxX < left || minX > right || maxY < top || minY > bottom)
		{
			base += vcount;
			continue;
		}

		// 3. 可见 → 变换顶点 push 到 _outVerts
		int outBase = (int)_outVerts.size();
		for (int j = 0; j < vcount; ++j)
		{
			const V2F_C4B_T2F& src = _verts[base + j];

			float vx = src.vertices.x;
			float vy = src.vertices.y;
			transform.out(vx, vy);

			V2F_C4B_T2F sv;
			sv.vertices.x = vx;
			sv.vertices.y = vy;
			sv.colors = src.colors;
			sv.texCoords = src.texCoords;
			_outVerts.push_back(sv);
		}

		// 4. 重建索引（3 顶点 → 3 索引；4 顶点 → 6 索引）
		if (vcount == 3)
		{
			_outIndices.push_back(outBase + 0);
			_outIndices.push_back(outBase + 1);
			_outIndices.push_back(outBase + 2);
		}
		else if (vcount == 4)
		{
			_outIndices.push_back(outBase + 0);
			_outIndices.push_back(outBase + 1);
			_outIndices.push_back(outBase + 2);
			_outIndices.push_back(outBase + 1);
			_outIndices.push_back(outBase + 3);
			_outIndices.push_back(outBase + 2);
		}

		base += vcount;
	}

	if (_outVerts.empty()) return;

#ifdef ORANGE_DEBUG
	_customCommand.setOwner(this);
#endif

	_customCommand.init(_globalZOrder, nullptr,
		_outVerts.data(), _outIndices.data(),
		(int)_outVerts.size(), (int)_outIndices.size());
	renderer->addCommand(&_customCommand);
}

OG_END
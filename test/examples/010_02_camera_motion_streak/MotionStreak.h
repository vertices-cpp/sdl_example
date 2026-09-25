#ifndef _MOTION_STREAK_H_
#define _MOTION_STREAK_H_

#include "OGPlatformMacros.h"
#include "OGMathGeometry.h"
#include "Texture2D.h" 
#include "OGCamera.h"

#include <vector> 

OG_BEGIN

class Texture2D;

// ---------------------------------------------------------------------------
// Catmull-Rom 样条插值
// ---------------------------------------------------------------------------
static Vec2 catmullRom(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;
	return (p1 * 2.0f +
		(p2 - p0) * t +
		(p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3) * t2 +
		(p1 * 3.0f - p0 - p2 * 3.0f + p3) * t3) * 0.5f;
}

// ---------------------------------------------------------------------------
// 拖尾采样点
// ---------------------------------------------------------------------------
struct StreakPoint {
	Vec2  pos;
	float life = 1.0f;   // 1.0 → 0.0
};

// ---------------------------------------------------------------------------
// SDL2 MotionStreak
//
// 相比旧版的改动：
//   1. 每帧不再 new 3 个 vector，改用成员缓冲，只 clear 不重分配
//   2. IMG_Load 有 null 检查（见 loadFromFile）
//   3. UV 支持按累计弧长平铺（setTextureTileLength），默认仍是整条拉伸
//   4. 采样后剔除过近点，防止 miter 法线除零 / 塌陷
//   5. 采样点数量有上限（setMaxPoints），防止内存无限涨
//   6. 时间驱动补点：鼠标不动也按固定频率推点，让拖尾"流动"
//   7. 三角形用索引绘制，省一半顶点拷贝
// ---------------------------------------------------------------------------
class MotionStreak {
	MotionStreak(const MotionStreak &) = delete;
	MotionStreak &operator =(const MotionStreak &) = delete;


	Color4B _displayedColor;
public:
	// 主循环用：鼠标"当前"是否在视口外（跨帧保留）
	inline static bool s_lastOutside = false;

	// draw 用：只在"刚离开"的那一帧为 true
	inline static bool s_verbose = false;

	// 主循环调这个：传入鼠标位置和视口，内部做变化检测并决定 s_verbose
	static void tickMouseState(const Vec2& screenPos, const Rect& viewport)
	{
		bool outside =
			screenPos.x < viewport.origin.x ||
			screenPos.x > viewport.origin.x + viewport.size.width ||
			screenPos.y < viewport.origin.y ||
			screenPos.y > viewport.origin.y + viewport.size.height;

		if (outside != s_lastOutside) {
			s_lastOutside = outside;

			if (outside) {
				printf(">>> 鼠标离开视口 screen=(%.1f,%.1f) viewport=(%.0f,%.0f)-(%.0f,%.0f)\n",
					screenPos.x, screenPos.y,
					viewport.origin.x, viewport.origin.y,
					viewport.origin.x + viewport.size.width,
					viewport.origin.y + viewport.size.height);
				s_verbose = true;    // 这一帧开详细日志
			}
			else {
				printf("<<< 鼠标回到视口 screen=(%.1f,%.1f)\n",
					screenPos.x, screenPos.y);
				s_verbose = false;
			}
		}
		else {
			s_verbose = false;       // 状态没变，关掉
		}
	}
	MotionStreak() {}
	~MotionStreak()
	{
		release(_texture);
	}
	void release(Texture2D *tex) {
		if (tex) delete tex;
	}
	static MotionStreak* create(float fadeTime, float minSegDistance, float strokeWidth, const Color4B& color, const std::string& path)
	{
		// 创建并初始化拖尾对象
		MotionStreak *ret = new (std::nothrow) MotionStreak();
		if (ret && ret->initWithFade(fadeTime, minSegDistance, strokeWidth, color, path))
		{
			return ret;
		}

		delete  ret;
		ret = nullptr;
		return nullptr;
	}
	bool initWithFade(float fadeTime, float minSegDistance, float strokeWidth, const Color4B& color, const std::string& path)
	{
		//CCASSERT(!path.empty(), "Invalid filename");

		Texture2D *texture = new Texture2D;
		texture->loadFrom(path.c_str());
		return initWithFade(fadeTime, minSegDistance, strokeWidth, color, texture);
	}
	bool initWithFade(float fadeTime, float minSegDistance, float strokeWidth, const Color4B& color, Texture2D *texture)
	{

		_fadeTime = (fadeTime > 0.001f ? fadeTime : 0.001f),
			_minSegSq = (minSegDistance * minSegDistance);
		_stroke = strokeWidth;

		setTexture(texture);
		_displayedColor = (color);

		return true;
	}
	void setTexture(Texture2D *texture)
	{
		// 安全设置纹理
		if (_texture != texture)
		{
			release(_texture);
			_texture = texture;
		}
	}

	// 纹理沿路径平铺的像素长度；<=0 表示整条拉伸（默认）
	void setTextureTileLength(float px) { _tileLen = px; }

	// 采样点数量上限，超出裁剪最旧的
	void setMaxPoints(size_t n) { _maxPoints = (n < 2) ? 2 : n; }

	// 每秒钟最多推入多少个采样点（时间驱动补点用）
	void setSampleRate(float timesPerSec) {
		if (timesPerSec > 0.01f) _sampleInterval = 1.0f / timesPerSec;
	}

	void reset() {
		_points.clear();
		_hasCursor = false;
		_cursorTimer = 0.f;
	}

	// 鼠标/光标位置
	void addPoint(const Vec2 &v)
	{
		addPoint(v.x, v.y);
	}
	void addPoint(float x, float y) {
		_cursor.x = x;
		_cursor.y = y;
		_hasCursor = true;

		if (_points.empty()) {
			_points.push_back({ _cursor, 1.0f });
			return;
		}
		if ((_cursor - _points.back().pos).getLengthSq() >= _minSegSq) {
			_points.push_back({ _cursor, 1.0f });
		}
	}

// 	void  update(float dt)
// 	{
// 		if (dt <= 0.f) return;
// 
// 		// ---- 时间驱动补点 ----
// 		if (_hasCursor) {
// 			_cursorTimer += dt;
// 			if (_cursorTimer >= _sampleInterval) {
// 				_cursorTimer = 0.f;
// 				if (_points.empty()) {
// 					_points.push_back({ _cursor, 1.0f });
// 				}
// 				else if ((_cursor - _points.back().pos).getLengthSq() > 1e-4f) {
// 					_points.push_back({ _cursor, 1.0f });
// 				}
// 				else {
// 					// 鼠标原地不动：刷新尾点 life，让拖尾尖端始终最亮
// 					_points.back().life = 1.0f;
// 				}
// 			}
// 		}
// 
// 		// ---- 生命衰减 ----
// 		const float fadeDelta = dt / _fadeTime;
// 		for (auto& p : _points) p.life -= fadeDelta;
// 
// 		// ---- 移除死亡点 ----
// 		_points.erase(
// 			std::remove_if(_points.begin(), _points.end(),
// 				[](const StreakPoint& p) { return p.life <= 0.0f; }),
// 			_points.end());
// 
// 		// ---- 上限裁剪 ----
// 		if (_points.size() > _maxPoints) {
// 			_points.erase(_points.begin(),
// 				_points.begin() + (_points.size() - _maxPoints));
// 		}
// 	}
// 	void  draw(SDL_Renderer* renderer, const Mat3& transform, uint32_t parentFlags)
// 	{
// 		if (_points.size() < 2) return;
// 
// 		Rect r1, r2;
// 
// 		// ========== 剔除放这里 ==========
// 		auto camera = Camera::getVisitingCamera();
// 		if (camera)
// 		{
// 			// _points 是局部坐标，先求 AABB（加 stroke 半宽）
// 			float minX = FLT_MAX, minY = FLT_MAX;
// 			float maxX = -FLT_MAX, maxY = -FLT_MAX;
// 			for (auto& p : _points) {
// 				minX = std::min(minX, p.pos.x);
// 				minY = std::min(minY, p.pos.y);
// 				maxX = std::max(maxX, p.pos.x);
// 				maxY = std::max(maxY, p.pos.y);
// 			}
// 			float half = _stroke * 0.5f;
// 			minX -= half; minY -= half;
// 			maxX += half; maxY += half;
// 
// 			// 屏幕可见矩形（含 margin），反变换到局部空间
// 			Rect screenRect = camera->getScreenRectWithMargin();
// 			Mat3 screenToLocal;
// 			auto m = transform;
// 			if (fabsf(m.a * m.d - m.b * m.c) > 1e-4f)
// 				screenToLocal = transform.getInversed();
// 			else
// 				printf("11\n");
// 
// 			Vec2 corners[4] = {
// 				{ screenRect.origin.x,                            screenRect.origin.y },
// 				{ screenRect.origin.x + screenRect.size.width,    screenRect.origin.y },
// 				{ screenRect.origin.x,                            screenRect.origin.y + screenRect.size.height },
// 				{ screenRect.origin.x + screenRect.size.width,    screenRect.origin.y + screenRect.size.height }
// 			};
// 			float lminX = FLT_MAX, lminY = FLT_MAX;
// 			float lmaxX = -FLT_MAX, lmaxY = -FLT_MAX;
// 			for (auto& c : corners) {
// 				float x = c.x, y = c.y;
// 				screenToLocal.out(x, y);
// 				lminX = std::min(lminX, x); lminY = std::min(lminY, y);
// 				lmaxX = std::max(lmaxX, x); lmaxY = std::max(lmaxY, y);
// 			}
// 			r1 = { minX, minY, maxX, maxY };
// 			r2 = { lminX, lminY, lmaxX, lmaxY };
// 			// 不相交 → 整条不画
// 			if (maxX < lminX || minX > lmaxX || maxY < lminY || minY > lmaxY)
// 				return;
// 		}
// 		// ========== 剔除结束 ==========
// 
// 		printf("points AABB: [%f,%f]-[%f,%f]\n", r1.origin.x, r1.origin.y, r1.size.width, r1.size.height);
// 		printf("local visible: [%f,%f]-[%f,%f]\n", r2.origin.x, r2.origin.y, r2.size.width, r2.size.height);
// 
// 		// =====================================================================
// 		// 1. Catmull-Rom 加密，同时剔除过近点
// 		// =====================================================================
// 		_dense.clear();
// 		const int SUBDIV = 6;   // 每段采样数，越大越平滑也越慢
// 
// 		for (size_t i = 0; i + 1 < _points.size(); ++i) {
// 			const Vec2& p0 = (i == 0) ? _points[i].pos : _points[i - 1].pos;
// 			const Vec2& p1 = _points[i].pos;
// 			const Vec2& p2 = _points[i + 1].pos;
// 			const Vec2& p3 = (i + 2 < _points.size()) ? _points[i + 2].pos : p2;
// 
// 			for (int s = 0; s < SUBDIV; ++s) {
// 				float t = (float)s / (float)SUBDIV;
// 				Vec2  pos = catmullRom(p0, p1, p2, p3, t);
// 				float life = _points[i].life +
// 					(_points[i + 1].life - _points[i].life) * t;
// 
// 				// 剔除与上一点距离平方 < 1e-4 (即距离 < 0.01px) 的点
// 				if (_dense.empty() ||
// 					(_dense.back().pos - pos).getLengthSq() > 1e-4f) {
// 					_dense.push_back({ pos, life });
// 				}
// 			}
// 		}
// 		// 补上尾点
// 		{
// 			const StreakPoint& tail = _points.back();
// 			if (_dense.empty() ||
// 				(_dense.back().pos - tail.pos).getLengthSq() > 1e-4f) {
// 				_dense.push_back(tail);
// 			}
// 			else {
// 				_dense.back() = tail;
// 			}
// 		}
// 
// 		const size_t count = _dense.size();
// 		if (count < 2) return;
// 
// 		// =====================================================================
// 		// 2. 生成顶点与索引
// 		// =====================================================================
// 		_verts.clear();
// 		_verts.reserve(count * 2);
// 		_indices.clear();
// 		_indices.reserve((count - 1) * 6);
// 
// 		const float halfStroke = _stroke * 0.5f;
// 		const bool  hasTransform =
// 			transform.a != 1.f || transform.b != 0.f ||
// 			transform.c != 0.f || transform.d != 1.f ||
// 			transform.tx != 0.f || transform.ty != 0.f;
// 
// 		float accLen = 0.f;   // 累计弧长，用于 UV
// 
// 		for (size_t i = 0; i < count; ++i) {
// 			const Vec2& p = _dense[i].pos;
// 
// 			if (i > 0) accLen += (p - _dense[i - 1].pos).length();
// 
// 			// ---- 法线 ----
// 			Vec2 perp;
// 			if (i == 0) {
// 				Vec2 d = _dense[1].pos - p;
// 				perp = (d.getLengthSq() < 1e-6f)
// 					? Vec2(0.f, halfStroke)
// 					: d.getNormalized().getPerp() * halfStroke;
// 			}
// 			else if (i == count - 1) {
// 				Vec2 d = p - _dense[i - 1].pos;
// 				perp = (d.getLengthSq() < 1e-6f)
// 					? Vec2(0.f, halfStroke)
// 					: d.getNormalized().getPerp() * halfStroke;
// 			}
// 			else {
// 				Vec2 v1 = p - _dense[i - 1].pos;
// 				Vec2 v2 = _dense[i + 1].pos - p;
// 				if (v1.getLengthSq() < 1e-6f || v2.getLengthSq() < 1e-6f) {
// 					perp = Vec2(0.f, halfStroke);
// 				}
// 				else {
// 					Vec2 n1 = v1.getNormalized().getPerp();
// 					Vec2 n2 = v2.getNormalized().getPerp();
// 					Vec2 m = n1 + n2;
// 					if (m.getLengthSq() < 1e-6f) {
// 						// 180° 折返，退化为单边法线
// 						perp = n1 * halfStroke;
// 					}
// 					else {
// 						m.normalize();
// 						float d = m.dot(n1);
// 						if (d < 0.25f) d = 0.25f;   // 防尖角过长
// 						perp = m * (halfStroke / d);
// 					}
// 				}
// 			}
// 
// 			Vec2 leftPos = { p.x + perp.x, p.y + perp.y };
// 			Vec2 rightPos = { p.x - perp.x, p.y - perp.y };
// 
// 			if (hasTransform) {
// 				transform.out(&leftPos);
// 				transform.out(&rightPos);
// 			}
// 
// 			// ---- 颜色 ----
// 			float life = clampf(_dense[i].life, 0.f, 1.f);
// 			Uint8 alpha = (Uint8)(life * _displayedColor.a);
// 			Color4B vc = { _displayedColor.r, _displayedColor.g, _displayedColor.b, alpha };
// 
// 			// ---- UV ----
// 			float u;
// 			if (_tileLen > 0.f) {
// 				u = accLen / _tileLen;
// 				u -= std::floor(u);          // wrap 到 [0, 1)
// 			}
// 			else {
// 				u = (float)i / (float)(count - 1);   // 整条拉伸
// 			}
// 
// 			SDL_Vertex vL, vR;
// 			vL.position = { leftPos.x,  leftPos.y };
// 			vL.color = *(SDL_Color*)&vc;
// 			vL.tex_coord = { u, 0.f };
// 
// 			vR.position = { rightPos.x, rightPos.y };
// 			vR.color = *(SDL_Color*)&vc;
// 			vR.tex_coord = { u, 1.f };
// 
// 
// 			_verts.push_back(vL);
// 			_verts.push_back(vR);
// 
// 			// ---- 索引（每相邻两点构成一个四边形 = 2 三角形）----
// 			if (i > 0) {
// 				int a = (int)(2 * (i - 1));   // 上一点左
// 				int b = a + 1;                // 上一点右
// 				int c = (int)(2 * i);         // 当前点左
// 				int d = c + 1;                // 当前点右
// 				_indices.push_back(a); _indices.push_back(b); _indices.push_back(c);
// 				_indices.push_back(b); _indices.push_back(d); _indices.push_back(c);
// 			}
// 		}
// 
// 		// =====================================================================
// 		// 3. 渲染
// 		// =====================================================================
// 
// 		SDL_RenderGeometry(renderer, _texture->getTexture(),
// 			_verts.data(), _verts.size(), _indices.data(), _indices.size());
// 	}

	void update(float dt);

	void  draw(SDL_Renderer* renderer, const Mat3& transform, uint32_t parentFlags);
private:
	// 点数据
	std::vector<StreakPoint> _points;

	// 复用缓冲：每帧只 clear，不重新分配
	std::vector<StreakPoint> _dense;
	std::vector<SDL_Vertex>  _verts;
	std::vector<int>         _indices;

	// 参数
	float  _fadeTime = 0.5f;
	float  _minSegSq = 100.f;   // 注意是平方
	float  _stroke = 30.f;
	float  _tileLen = 0.f;     // <=0 表示整条拉伸
	size_t _maxPoints = 512;

	//Color4B   _color{ 255, 255, 255, 255 }; 
	Texture2D* _texture = nullptr;

	// 光标与时间采样
	Vec2  _cursor;
	bool  _hasCursor = false;
	float _cursorTimer = 0.f;
	float _sampleInterval = 1.f / 60.f;
};





OG_END


#endif
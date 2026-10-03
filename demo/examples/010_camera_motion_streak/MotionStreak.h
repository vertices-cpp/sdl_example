#ifndef _MOTION_STREAK_H_
#define _MOTION_STREAK_H_
 
#include "Vec2.h"
#include "Mat3.h"

#include "ogTypes.h" 
#include "OGTexture2D.h"
#include "OGCamera.h"
#include <vector> 

OG_BEGIN

class Mat3;
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
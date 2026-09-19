#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

#include "path_head.h"

#include "Vec2.h"
#include "Mat3.h"

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
class SDLMotionStreak {
public:
	SDLMotionStreak(float fadeTime, float minSegDistance, float strokeWidth,
		SDL_Color color, SDL_Texture* texture = nullptr)
		: _fadeTime(fadeTime > 0.001f ? fadeTime : 0.001f),
		_minSegSq(minSegDistance * minSegDistance),
		_stroke(strokeWidth),
		_color(color),
		_texture(texture)
	{}

	void setTransform(const Mat3& m) { _transform = m; }
	const Mat3& getTransform() const { return _transform; }

	void setTexture(SDL_Texture* tex) { _texture = tex; }

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

	void update(float dt) {
		if (dt <= 0.f) return;

		// ---- 时间驱动补点 ----
		if (_hasCursor) {
			_cursorTimer += dt;
			if (_cursorTimer >= _sampleInterval) {
				_cursorTimer = 0.f;
				if (_points.empty()) {
					_points.push_back({ _cursor, 1.0f });
				}
				else if ((_cursor - _points.back().pos).getLengthSq() > 1e-4f) {
					_points.push_back({ _cursor, 1.0f });
				}
				else {
					// 鼠标原地不动：刷新尾点 life，让拖尾尖端始终最亮
					_points.back().life = 1.0f;
				}
			}
		}

		// ---- 生命衰减 ----
		const float fadeDelta = dt / _fadeTime;
		for (auto& p : _points) p.life -= fadeDelta;

		// ---- 移除死亡点 ----
		_points.erase(
			std::remove_if(_points.begin(), _points.end(),
				[](const StreakPoint& p) { return p.life <= 0.0f; }),
			_points.end());

		// ---- 上限裁剪 ----
		if (_points.size() > _maxPoints) {
			_points.erase(_points.begin(),
				_points.begin() + (_points.size() - _maxPoints));
		}
	}

	void draw(SDL_Renderer* renderer) {
		if (_points.size() < 2) return;

		// =====================================================================
		// 1. Catmull-Rom 加密，同时剔除过近点
		// =====================================================================
		_dense.clear();
		const int SUBDIV = 6;   // 每段采样数，越大越平滑也越慢

		for (size_t i = 0; i + 1 < _points.size(); ++i) {
			const Vec2& p0 = (i == 0) ? _points[i].pos : _points[i - 1].pos;
			const Vec2& p1 = _points[i].pos;
			const Vec2& p2 = _points[i + 1].pos;
			const Vec2& p3 = (i + 2 < _points.size()) ? _points[i + 2].pos : p2;

			for (int s = 0; s < SUBDIV; ++s) {
				float t = (float)s / (float)SUBDIV;
				Vec2  pos = catmullRom(p0, p1, p2, p3, t);
				float life = _points[i].life +
					(_points[i + 1].life - _points[i].life) * t;

				// 剔除与上一点距离平方 < 1e-4 (即距离 < 0.01px) 的点
				if (_dense.empty() ||
					(_dense.back().pos - pos).getLengthSq() > 1e-4f) {
					_dense.push_back({ pos, life });
				}
			}
		}
		// 补上尾点
		{
			const StreakPoint& tail = _points.back();
			if (_dense.empty() ||
				(_dense.back().pos - tail.pos).getLengthSq() > 1e-4f) {
				_dense.push_back(tail);
			}
			else {
				_dense.back() = tail;
			}
		}

		const size_t count = _dense.size();
		if (count < 2) return;

		// =====================================================================
		// 2. 生成顶点与索引
		// =====================================================================
		_verts.clear();
		_verts.reserve(count * 2);
		_indices.clear();
		_indices.reserve((count - 1) * 6);

		const float halfStroke = _stroke * 0.5f;
		const bool  hasTransform =
			_transform.a != 1.f || _transform.b != 0.f ||
			_transform.c != 0.f || _transform.d != 1.f ||
			_transform.tx != 0.f || _transform.ty != 0.f;

		float accLen = 0.f;   // 累计弧长，用于 UV

		for (size_t i = 0; i < count; ++i) {
			const Vec2& p = _dense[i].pos;

			if (i > 0) accLen += (p - _dense[i - 1].pos).length();

			// ---- 法线 ----
			Vec2 perp;
			if (i == 0) {
				Vec2 d = _dense[1].pos - p;
				perp = (d.getLengthSq() < 1e-6f)
					? Vec2(0.f, halfStroke)
					: d.getNormalized().getPerp() * halfStroke;
			}
			else if (i == count - 1) {
				Vec2 d = p - _dense[i - 1].pos;
				perp = (d.getLengthSq() < 1e-6f)
					? Vec2(0.f, halfStroke)
					: d.getNormalized().getPerp() * halfStroke;
			}
			else {
				Vec2 v1 = p - _dense[i - 1].pos;
				Vec2 v2 = _dense[i + 1].pos - p;
				if (v1.getLengthSq() < 1e-6f || v2.getLengthSq() < 1e-6f) {
					perp = Vec2(0.f, halfStroke);
				}
				else {
					Vec2 n1 = v1.getNormalized().getPerp();
					Vec2 n2 = v2.getNormalized().getPerp();
					Vec2 m = n1 + n2;
					if (m.getLengthSq() < 1e-6f) {
						// 180° 折返，退化为单边法线
						perp = n1 * halfStroke;
					}
					else {
						m.normalize();
						float d = m.dot(n1);
						if (d < 0.25f) d = 0.25f;   // 防尖角过长
						perp = m * (halfStroke / d);
					}
				}
			}

			Vec2 leftPos = { p.x + perp.x, p.y + perp.y };
			Vec2 rightPos = { p.x - perp.x, p.y - perp.y };

			if (hasTransform) {
				_transform.out(&leftPos);
				_transform.out(&rightPos);
			}

			// ---- 颜色 ----
			float life = clampf(_dense[i].life, 0.f, 1.f);
			Uint8 alpha = (Uint8)(life * _color.a);
			SDL_Color vc = { _color.r, _color.g, _color.b, alpha };

			// ---- UV ----
			float u;
			if (_tileLen > 0.f) {
				u = accLen / _tileLen;
				u -= std::floor(u);          // wrap 到 [0, 1)
			}
			else {
				u = (float)i / (float)(count - 1);   // 整条拉伸
			}

			SDL_Vertex vL, vR;
			vL.position = { leftPos.x,  leftPos.y };
			vL.color = vc;
			vL.tex_coord = { u, 0.f };

			vR.position = { rightPos.x, rightPos.y };
			vR.color = vc;
			vR.tex_coord = { u, 1.f };

			_verts.push_back(vL);
			_verts.push_back(vR);

			// ---- 索引（每相邻两点构成一个四边形 = 2 三角形）----
			if (i > 0) {
				int a = (int)(2 * (i - 1));   // 上一点左
				int b = a + 1;                // 上一点右
				int c = (int)(2 * i);         // 当前点左
				int d = c + 1;                // 当前点右
				_indices.push_back(a); _indices.push_back(b); _indices.push_back(c);
				_indices.push_back(b); _indices.push_back(d); _indices.push_back(c);
			}
		}

		// =====================================================================
		// 3. 渲染
		// =====================================================================
		SDL_RenderGeometry(renderer, _texture,
			_verts.data(), (int)_verts.size(),
			_indices.data(), (int)_indices.size());
	}

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

	SDL_Color    _color{ 255, 255, 255, 255 };
	Mat3         _transform = Mat3::IDENTITY;
	SDL_Texture* _texture = nullptr;

	// 光标与时间采样
	Vec2  _cursor;
	bool  _hasCursor = false;
	float _cursorTimer = 0.f;
	float _sampleInterval = 1.f / 60.f;
};

// ---------------------------------------------------------------------------
// 纹理加载
// ---------------------------------------------------------------------------
static void loadFromFile(SDL_Renderer* renderer, SDL_Texture*& tex,
	const std::string& path)
{
	if (tex) {
		SDL_DestroyTexture(tex);
		tex = nullptr;
	}

	SDL_Surface* surf = IMG_Load(path.c_str());
	if (!surf) {
		SDL_Log("IMG_Load 失败: %s (%s)", path.c_str(), IMG_GetError());
		return;
	}

	// 注意：如果 PNG 本身带 alpha，千万别在这里 SetColorKey，
	// 否则会把图像里同色的像素误抠成透明。
	// 只有老的 24-bit 位图（无 alpha 通道）才需要 colorkey。
	tex = SDL_CreateTextureFromSurface(renderer, surf);
	if (!tex) {
		SDL_Log("SDL_CreateTextureFromSurface 失败: %s", SDL_GetError());
	}
	SDL_FreeSurface(surf);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("SDL_Init 失败: %s", SDL_GetError());
		return 1;
	}
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		SDL_Log("IMG_Init(PNG) 失败: %s", IMG_GetError());
		// 不一定要退出——如果没有 png 就只画纯色
	}

	SDL_Window* window = SDL_CreateWindow("SDL2 MotionStreak with Mat3",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1024, 768, SDL_WINDOW_SHOWN);
	if (!window) {
		SDL_Log("CreateWindow 失败: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		SDL_Log("CreateRenderer 失败: %s", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// 纹理（失败也只是不显示纹理，不影响运行）
	SDL_Texture* streakTexture = nullptr;
	loadFromFile(renderer, streakTexture, og::checkPath( "010_motion_streak/circle_texture.png"));

	// 构造：fadeTime, minSegDistance, strokeWidth, color, texture
	SDLMotionStreak streak(1.4f, 5.0f, 32.0f,
		{ 0, 128, 255, 255 }, streakTexture);

	// 可选调节：
	// streak.setMaxPoints(256);
	// streak.setSampleRate(60.f);
	// streak.setTextureTileLength(64.f);   // 打开 UV 平铺

	bool  quit = false;
	Uint32 lastTime = SDL_GetTicks();
	float angle = 0.f;
	Mat3  transform = Mat3::IDENTITY;

	while (!quit) {
		Uint32 now = SDL_GetTicks();
		float  dt = (now - lastTime) / 1000.0f;
		lastTime = now;

		// 防止长时间无输入导致 dt 巨大（例如窗口拖动后）
		if (dt > 0.1f) dt = 0.1f;

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_MOUSEMOTION) {
				streak.addPoint((float)e.motion.x, (float)e.motion.y);
			}
		}

		streak.update(dt);

		// 可选：动态变换演示（默认关闭）
		// angle += 30.f * dt;
		// transform = Mat3::createRotation({512.f, 384.f}, angle);
		streak.setTransform(transform);

		SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
		SDL_RenderClear(renderer);

		streak.draw(renderer);

		SDL_RenderPresent(renderer);
	}

	if (streakTexture) SDL_DestroyTexture(streakTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	if (IMG_Init(0)) IMG_Quit();   // 无害调用
	IMG_Quit();
	SDL_Quit();
	return 0;
}
#include "MotionStreak.h"
#include "Mat3.h"

OG_BEGIN

void MotionStreak::update(float dt)
{
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
void MotionStreak::draw(SDL_Renderer* renderer, const Mat3& transform, uint32_t parentFlags)
{
	if (_points.size() < 2) return;

	// =====================================================================
	// 1. Catmull-Rom 加密（全部点都算，不做剔除）
	// =====================================================================
	_dense.clear();
	const int SUBDIV = 6;

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

			if (_dense.empty() ||
				(_dense.back().pos - pos).getLengthSq() > 1e-4f) {
				_dense.push_back({ pos, life });
			}
		}
	}
	// 补尾点
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
	// 2. 生成顶点（只对 view 内的点出顶点；法线仍用原始邻居）
	// =====================================================================
	_verts.clear();
	_verts.reserve(count * 2);
	_indices.clear();
	_indices.reserve((count - 1) * 6);

	std::vector<int> vidx(count, -1);   // _dense 下标 → _verts 起始下标，-1 表示未出顶点

	const float halfStroke = _stroke * 0.5f;
	float accLen = 0.f;

	for (size_t i = 0; i < count; ++i)
	{
		const Vec2& p = _dense[i].pos;

		// 累计弧长（无论是否可见都累积，保证 UV 与路径一致）
		if (i > 0) accLen += (p - _dense[i - 1].pos).length();

		// ---- 法线（用原始邻居，和是否可见无关）----
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
					perp = n1 * halfStroke;       // 180° 折返
				}
				else {
					m.normalize();
					float d = m.dot(n1);
					if (d < 0.25f) d = 0.25f;
					perp = m * (halfStroke / d);
				}
			}
		}
		// =====================================================================
	// 3. 求局部空间的可见矩形（只影响"是否出顶点"，不影响计算）
	// =====================================================================
		float lminX = -FLT_MAX, lminY = -FLT_MAX;
		float lmaxX = FLT_MAX, lmaxY = FLT_MAX;
		bool  hasClip = false;

		auto camera = Camera::getVisitingCamera();
		if (camera)
		{
			Rect screenRect = camera->getScreenRectWithMargin();

			// 屏幕矩形 4 角反变换到局部空间
			Mat3 screenToLocal;
			float det = transform.a * transform.d - transform.b * transform.c;
			if (fabsf(det) > 1e-6f)
				screenToLocal = transform.getInversed();
			else
				screenToLocal = Mat3::IDENTITY;   // 退化时不剔除

			Vec2 corners[4] = {
				{ screenRect.origin.x,                         screenRect.origin.y },
				{ screenRect.origin.x + screenRect.size.width, screenRect.origin.y },
				{ screenRect.origin.x,                         screenRect.origin.y + screenRect.size.height },
				{ screenRect.origin.x + screenRect.size.width, screenRect.origin.y + screenRect.size.height }
			};
			lminX = lminY = FLT_MAX;
			lmaxX = lmaxY = -FLT_MAX;
			for (auto& c : corners) {
				float x = c.x, y = c.y;
				screenToLocal.out(x, y);
				lminX = std::min(lminX, x); lminY = std::min(lminY, y);
				lmaxX = std::max(lmaxX, x); lmaxY = std::max(lmaxY, y);
			}
			if (p.x < lminX || p.x > lmaxX ||
				p.y < lminY || p.y > lmaxY)
			{
				continue;   // 出界：不出顶点，但计算已全部完成
			}
		} 

		// ---- 顶点位置 ----
		Vec2 leftPos = { p.x + perp.x, p.y + perp.y };
		Vec2 rightPos = { p.x - perp.x, p.y - perp.y };
		transform.out(&leftPos);
		transform.out(&rightPos);

		// ---- 颜色 ----
		float life = clampf(_dense[i].life, 0.f, 1.f);
		Uint8 alpha = (Uint8)(life * _displayedColor.a);
		Color4B vc = { _displayedColor.r, _displayedColor.g,
						_displayedColor.b, alpha };

		// ---- UV ----
		float u;
		if (_tileLen > 0.f) {
			u = accLen / _tileLen;
			u -= std::floor(u);
		}
		else {
			u = (float)i / (float)(count - 1);
		}

		SDL_Vertex vL, vR;
		vL.position = { leftPos.x,  leftPos.y };
		vL.color = *(SDL_Color*)&vc;
		vL.tex_coord = { u, 0.f };

		vR.position = { rightPos.x, rightPos.y };
		vR.color = *(SDL_Color*)&vc;
		vR.tex_coord = { u, 1.f };

		vidx[i] = (int)_verts.size();
		_verts.push_back(vL);
		_verts.push_back(vR);
	}

	// =====================================================================
	// 4. 索引（两个相邻点都出过顶点才连接）
	// =====================================================================
	for (size_t i = 0; i + 1 < count; ++i)
	{
		if (vidx[i] < 0 || vidx[i + 1] < 0) continue;

		int a = vidx[i];
		int b = a + 1;
		int c = vidx[i + 1];
		int d = c + 1;

		_indices.push_back(a); _indices.push_back(b); _indices.push_back(c);
		_indices.push_back(b); _indices.push_back(d); _indices.push_back(c);
	}

	if (_indices.empty()) return;

	// =====================================================================
	// 5. 提交
	// =====================================================================
	SDL_RenderGeometry(renderer, _texture->getTexture(),
		_verts.data(), (int)_verts.size(),
		_indices.data(), (int)_indices.size());
}


OG_END
#ifndef _OG_CAMERA_H_
#define _OG_CAMERA_H_
 

#include "OGPlatformMacros.h"
#include "OGGeometry.h"

OG_BEGIN
 

enum class CameraFlag
{
	DEFAULT = 1,
	USER1 = 1 << 1,
	USER2 = 1 << 2,
	USER3 = 1 << 3,
	USER4 = 1 << 4,
	USER5 = 1 << 5,
	USER6 = 1 << 6,
	USER7 = 1 << 7,
	USER8 = 1 << 8,
};

class Mat3;


class Camera  
{
protected:
	bool _enableClip = false;   // 默认不裁剪
	Vec2  _center;          // 相机中心（世界坐标）
	float _zoom;            // 缩放（乘法）
	Rect  _viewport;    // 视口大小（逻辑分辨率，一般等于屏幕）
	float _marginX;         // 屏幕外扩（像素）
	float _marginY;
	int8_t  _depth = -1;
	uint32_t _viewUpdated = false;
	CameraFlag _cameraFlag = CameraFlag::DEFAULT;

	bool _showDebug;
// 	DrawNode * _viewportFrame;
// 	DrawNode *_marginFrame;

	Rect r1, r2;
public:

	static Camera* _visitingCamera;
//	Scene* _scene = nullptr;

	static Camera* getVisitingCamera() { return _visitingCamera; }
	void setEnableClip(bool v) { _enableClip = v; }
	bool isEnableClip() const { return _enableClip; }
	uint32_t isViewProjectionUpdated(){
		uint32_t updated = _viewUpdated;
		_viewUpdated = 0;
		return updated;
	}
	Camera();
	virtual ~Camera();

	static Camera* create();
	static Camera* create(const Size& viewportSize);
	bool init();
	bool initWithViewportSize(const Size& size);


	// --- 视口 ---
	void  setViewport(const Rect& view) { _viewport = view; }
	
	void  setViewportPos(const Vec2& origin) {
		_viewport.origin = origin; 
 
	}
	
	void  setViewportSize(const Size& newSize) {
		_viewport.size = newSize;  
		 
	}

	const Rect& getViewport() const { return _viewport; }
	const Vec2& getViewportPos() const { return _viewport.origin; }
	const Size& getViewportSize() const {
		return _viewport.size;
	}

	// --- 缩放（乘法） ---
	void  setZoom(float zoom) {
		_zoom = (zoom > 0.0001f) ? zoom : 0.0001f; 
	}
	float getZoom() const { return _zoom; }
	void  zoomBy(float factor) {
		_zoom *= factor;
		if (_zoom <= 0.0001f) _zoom = 0.0001f;
	}

	// --- 外扩（屏幕像素，tile 裁剪用） ---
	void  setMargin(float m) { _marginX = _marginY = m; }
	void  setMargin(float mx, float my) { _marginX = mx; _marginY = my; }
	float getMarginX() const { return _marginX; }
	float getMarginY() const { return _marginY; }

	// --- 相机位置（世界坐标） ---
	void  setCenter(float x, float y) {
		_center.set(x, y); 

	}
	void  setCenter(const Vec2& p) { setCenter(p.x, p.y); }
 
	Vec2 getCenter() { return _center; }
	const Vec2& getCenter() const { return _center; }
	void  move(float dx, float dy) { _center.x += dx; _center.y += dy; }

	CameraFlag getCameraFlag() const { return _cameraFlag; }
	void setCameraFlag(CameraFlag flag) { _cameraFlag = flag; }

	// --- 矩阵 ---
	Mat3 getViewMatrix() const;            // 世界 -> 屏幕

	Mat3 getScreenToWorldMatrix() const;   // 屏幕 -> 世界

	// --- 矩形 ---
	Rect getScreenRect() const;        // 屏幕矩形 [0,0,vw,vh]
	Rect getWorldRect() const;         // 世界可见矩形（不含外扩）
	Rect getScreenRectWithMargin() const;
	Rect getWorldVisibleRect() const;  // 世界可见矩形（含外扩，tile 裁剪用）

	// --- 坐标转换 ---
	Vec2 screenToWorld(const Vec2& p) const;
	Vec2 worldToScreen(const Vec2& p) const;

	// --- 边界限制（可选） ---
	void clampToRect(const Rect& worldRect);

	public:
	 void  setShowDebug(bool v) {
			_showDebug = v;
// 			if (_viewportFrame) _viewportFrame->setVisible(v);
// 			if (_marginFrame) _marginFrame->setVisible(v);
			if (v) updateDebugDraw();
		}
	 void Draw(const Rect &r);
	void updateDebugDraw()
	{
		if (!_showDebug) return;
	//	if (_viewportFrame) 
		{
	//		_viewportFrame->clear();
			r1  = getViewport();
// 			_viewportFrame->drawRect(
// 				Vec2(vp.origin.x  , vp.origin.y),
// 				Vec2(vp.origin.x + vp.size.width, vp.origin.y + vp.size.height),
// 				Color4B::WHITE);
			Draw(r1);
		}
	//	if (_marginFrame) 
		{
		//	_marginFrame->clear();
			r2 = getScreenRectWithMargin();
// 			_marginFrame->drawRect(
// 				Vec2(vpm.origin.x, vpm.origin.y),
// 				Vec2(vpm.origin.x + vpm.size.width, vpm.origin.y + vpm.size.height),
// 				Color4B::RED);
			Draw(r2);
		}
	}

};

 
OG_END

#endif
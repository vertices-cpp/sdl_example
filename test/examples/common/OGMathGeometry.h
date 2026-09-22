#ifndef _MATH_GEOMETRY_H_
#define _MATH_GEOMETRY_H_

#include "OGPlatformMacros.h"
#include <algorithm>
#include <functional>
#include <iostream>

OG_BEGIN

extern   float clampf(float value, float min_inclusive, float max_inclusive);
 

/**
 * Defines a 2-element floating point vector.
 */
class   Vec2
{
public: 
	float x = 0.f; 
	float y = 0.f; 
	Vec2(); 
	Vec2(float xx, float yy); 
	Vec2(const float* array); 
	Vec2(const Vec2& p1, const Vec2& p2); 
	 bool isZero() const; 
	 bool isOne() const;
	 
	static float angle(const Vec2& v1, const Vec2& v2); 
	 void add(const Vec2& v); 
	static void add(const Vec2& v1, const Vec2& v2, Vec2* dst); 
	void clamp(const Vec2& min, const Vec2& max); 
	static void clamp(const Vec2& v, const Vec2& min, const Vec2& max, Vec2* dst); 
	float distance(const Vec2& v) const; 
	 float distanceSquared(const Vec2& v) const; 
	 float dot(const Vec2& v) const; 
	static float dot(const Vec2& v1, const Vec2& v2); 
	float length() const; 
	 float lengthSquared() const; 
	 void negate(); 
	void normalize(); 
	Vec2 getNormalized() const; 
	 void scale(float scalar); 
	 void scale(const Vec2& scale); 
	void rotate(const Vec2& point, float angle); 
	 void set(float xx, float yy); 
	void set(const float* array); 
	 void set(const Vec2& v); 
	 void set(const Vec2& p1, const Vec2& p2); 
	 void setZero(); 
	 void subtract(const Vec2& v); 
	static void subtract(const Vec2& v1, const Vec2& v2, Vec2* dst); 
	 void smooth(const Vec2& target, float elapsedTime, float responseTime); 
	 Vec2 operator+(const Vec2& v) const; 
	 Vec2& operator+=(const Vec2& v); 
	 Vec2 operator-(const Vec2& v) const; 
	 Vec2& operator-=(const Vec2& v); 
	 Vec2 operator-() const; 
	 Vec2 operator*(float s) const; 
	 Vec2& operator*=(float s); 
	 Vec2 operator/(float s) const; 
	 bool operator<(const Vec2& v) const; 
	 bool operator>(const Vec2& v) const; 
	 bool operator==(const Vec2& v) const; 
	 bool operator!=(const Vec2& v) const; 
public: 
	 void setPoint(float xx, float yy);
	/**
	 * @js NA
	 */
	bool equals(const Vec2& target) const; 
	bool fuzzyEquals(const Vec2& target, float variance) const; 
	 float getLength() const {
		return sqrtf(x*x + y * y);
	} 
	 float getLengthSq() const {
		return dot(*this); //x*x + y*y;
	} 
	 float getDistanceSq(const Vec2& other) const {
		return (*this - other).getLengthSq();
	} 
	 float getDistance(const Vec2& other) const {
		return (*this - other).getLength();
	} 
	 float getAngle() const {
		return atan2f(y, x);
	}  
	float getAngle(const Vec2& other) const; 
	 float cross(const Vec2& other) const {
		return x * other.y - y * other.x;
	} 
	 Vec2 getPerp() const {
		return Vec2(-y, x);
	} 
	 Vec2 getMidpoint(const Vec2& other) const
	{
		return Vec2((x + other.x) / 2.0f, (y + other.y) / 2.0f);
	}
	 
	 Vec2 getClampPoint(const Vec2& min_inclusive, const Vec2& max_inclusive) const
	{
		return Vec2(clampf(x, min_inclusive.x, max_inclusive.x), clampf(y, min_inclusive.y, max_inclusive.y));
	}
	 
	 Vec2 compOp(std::function<float(float)> function) const
	{
		return Vec2(function(x), function(y));
	} 
	 Vec2 getRPerp() const {
		return Vec2(y, -x);
	} 
	 Vec2 project(const Vec2& other) const {
		return other * (dot(other) / other.dot(other));
	} 
	 Vec2 rotate(const Vec2& other) const {
		return Vec2(x*other.x - y * other.y, x*other.y + y * other.x);
	} 
	 Vec2 unrotate(const Vec2& other) const {
		return Vec2(x*other.x + y * other.y, y*other.x - x * other.y);
	} 
	 Vec2 lerp(const Vec2& other, float alpha) const {
		return *this * (1.f - alpha) + other * alpha;
	} 
	Vec2 rotateByAngle(const Vec2& pivot, float angle) const; 
	static  Vec2 forAngle(const float a)
	{
		return Vec2(cosf(a), sinf(a));
	} 
	static bool isLineIntersect(const Vec2& A, const Vec2& B,
		const Vec2& C, const Vec2& D,
		float *S = nullptr, float *T = nullptr); 
	static bool isLineOverlap(const Vec2& A, const Vec2& B,
		const Vec2& C, const Vec2& D); 
	static bool isLineParallel(const Vec2& A, const Vec2& B,
		const Vec2& C, const Vec2& D); 
	static bool isSegmentOverlap(const Vec2& A, const Vec2& B,
		const Vec2& C, const Vec2& D,
		Vec2* S = nullptr, Vec2* E = nullptr); 
	static bool isSegmentIntersect(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D); 
	static Vec2 getIntersectPoint(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D);

	/** equals to Vec2(0,0) */
	static const Vec2 ZERO;
	/** equals to Vec2(1,1) */
	static const Vec2 ONE;
	/** equals to Vec2(1,0) */
	static const Vec2 UNIT_X;
	/** equals to Vec2(0,1) */
	static const Vec2 UNIT_Y;
	/** equals to Vec2(0.5, 0.5) */
	static const Vec2 ANCHOR_MIDDLE;
	/** equals to Vec2(0, 0) */
	static const Vec2 ANCHOR_BOTTOM_LEFT;
	/** equals to Vec2(0, 1) */
	static const Vec2 ANCHOR_TOP_LEFT;
	/** equals to Vec2(1, 0) */
	static const Vec2 ANCHOR_BOTTOM_RIGHT;
	/** equals to Vec2(1, 1) */
	static const Vec2 ANCHOR_TOP_RIGHT;
	/** equals to Vec2(1, 0.5) */
	static const Vec2 ANCHOR_MIDDLE_RIGHT;
	/** equals to Vec2(0, 0.5) */
	static const Vec2 ANCHOR_MIDDLE_LEFT;
	/** equals to Vec2(0.5, 1) */
	static const Vec2 ANCHOR_MIDDLE_TOP;
	/** equals to Vec2(0.5, 0) */
	static const Vec2 ANCHOR_MIDDLE_BOTTOM;
};
 
 Vec2 operator*(float x, const Vec2& v);

typedef Vec2 Point;


class   Size
{
public:
	/**Width of the Size.*/
	float width;
	/**Height of the Size.*/
	float height;
public: 
	operator Vec2() const
	{
		return Vec2(width, height);
	}

public: 
	Size();
	Size(float width, float height);
	Size(const Size& other);
	explicit Size(const Vec2& point); 
	Size& operator= (const Size& other); 
	Size& operator= (const Vec2& point); 
	Size operator+(const Size& right) const; 
	Size operator-(const Size& right) const; 
	Size operator*(float a) const; 
	Size operator/(float a) const; 
	void setSize(float width, float height); 
	bool equals(const Size& target) const;
	/**Size(0,0).*/
	static const Size ZERO;
};

/**Rectangle area.*/
class   Rect
{
public:
	/**Low left point of rect.*/
	Vec2 origin;
	/**Width and height of the rect.*/
	Size  size;

public: 
	Rect(); 
	Rect(float x, float y, float width, float height); 
	Rect(const Vec2& pos, const Size& dimension); 
	Rect(const Rect& other); 
	Rect& operator= (const Rect& other); 
	void setRect(float x, float y, float width, float height); 
	float getMinX() const; /// return the leftmost x-value of current rect
	 
	float getMidX() const; /// return the midpoint x-value of current rect
	float getMaxX() const; /// return the rightmost x-value of current rect
	float getMinY() const; /// return the bottommost y-value of current rect
	float getMidY() const; /// return the midpoint y-value of current rect
	float getMaxY() const; /// return the topmost y-value of current rect 
	bool equals(const Rect& rect) const; 
	bool containsPoint(const Vec2& point) const; 
	bool intersectsRect(const Rect& rect) const; 
	bool intersectsCircle(const Vec2& center, float radius) const; 
	Rect unionWithRect(const Rect & rect) const; 
	void merge(const Rect& rect); 
	static const Rect ZERO;
};

struct Color4B;
struct Color4F;

/**
 * RGB color composed of bytes 3 bytes.
 * @since v3.0
 */
struct   Color3B
{
	Color3B();
	Color3B(uint8_t _r, uint8_t _g, uint8_t _b);
	explicit Color3B(const Color4B& color);
	explicit Color3B(const Color4F& color);

	bool operator==(const Color3B& right) const;
	bool operator==(const Color4B& right) const;
	bool operator==(const Color4F& right) const;
	bool operator!=(const Color3B& right) const;
	bool operator!=(const Color4B& right) const;
	bool operator!=(const Color4F& right) const;

	bool equals(const Color3B& other) const
	{
		return (*this == other);
	}

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

	static const Color3B WHITE;
	static const Color3B YELLOW;
	static const Color3B BLUE;
	static const Color3B GREEN;
	static const Color3B RED;
	static const Color3B MAGENTA;
	static const Color3B BLACK;
	static const Color3B ORANGE;
	static const Color3B GRAY;
};

/**
 * RGBA color composed of 4 bytes.
 * @since v3.0
 */
struct   Color4B
{
	Color4B();
	Color4B(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a);
	explicit Color4B(const Color3B& color, uint8_t _a = 255);
	explicit Color4B(const Color4F& color);

	 void set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	bool operator==(const Color4B& right) const;
	bool operator==(const Color3B& right) const;
	bool operator==(const Color4F& right) const;
	bool operator!=(const Color4B& right) const;
	bool operator!=(const Color3B& right) const;
	bool operator!=(const Color4F& right) const;

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;

	static const Color4B WHITE;
	static const Color4B YELLOW;
	static const Color4B BLUE;
	static const Color4B GREEN;
	static const Color4B RED;
	static const Color4B MAGENTA;
	static const Color4B BLACK;
	static const Color4B ORANGE;
	static const Color4B GRAY;
};


/**
 * RGBA color composed of 4 floats.
 * @since v3.0
 */
struct   Color4F
{
	Color4F();
	Color4F(float _r, float _g, float _b, float _a);
	explicit Color4F(const Color3B& color, float _a = 1.0f);
	explicit Color4F(const Color4B& color);

	bool operator==(const Color4F& right) const;
	bool operator==(const Color3B& right) const;
	bool operator==(const Color4B& right) const;
	bool operator!=(const Color4F& right) const;
	bool operator!=(const Color3B& right) const;
	bool operator!=(const Color4B& right) const;

	bool equals(const Color4F &other) const
	{
		return (*this == other);
	}

	float r = 0.f;
	float g = 0.f;
	float b = 0.f;
	float a = 0.f;

	static const Color4F WHITE;
	static const Color4F YELLOW;
	static const Color4F BLUE;
	static const Color4F GREEN;
	static const Color4F RED;
	static const Color4F MAGENTA;
	static const Color4F BLACK;
	static const Color4F ORANGE;
	static const Color4F GRAY;
};

Color4F& operator+=(Color4F& lhs, const Color4F& rhs);
Color4F operator+(Color4F lhs, const Color4F& rhs);

Color4F& operator-=(Color4F& lhs, const Color4F& rhs);
Color4F operator-(Color4F lhs, const Color4F& rhs);

Color4F& operator*=(Color4F& lhs, const Color4F& rhs);
Color4F operator*(Color4F lhs, const Color4F& rhs);
Color4F& operator*=(Color4F& lhs, float rhs);
Color4F operator*(Color4F lhs, float rhs);

Color4F& operator/=(Color4F& lhs, const Color4F& rhs);
Color4F operator/(Color4F lhs, const Color4F& rhs);
Color4F& operator/=(Color4F& lhs, float rhs);
Color4F operator/(Color4F lhs, float rhs);


struct Mat3
{
	static const Mat3 IDENTITY;
	float a, b,					// 0 1 2
		c, d,					// 3 4 5
		tx, ty;					// 6 7 8
	Mat3() :a(1.0f), b(0.0f), c(0.0f), d(1.0f), tx(0.0f), ty(0.0f) {}
	Mat3(float a, float b, float c, float d, float tx, float ty)
		: a(a), b(b), c(c), d(d), tx(tx), ty(ty) {}

	Mat3(const Mat3& r) :a(r.a), b(r.b), c(r.c), d(r.d), tx(r.tx), ty(r.ty) {}

	void reset() {
		*this = Mat3::IDENTITY;
	}

	Mat3 operator*(const Mat3 & r)const;
	static Mat3 createScale(float sx, float sy) {
		Mat3 S;
		S.a = sx; S.b = 0.0f;
		S.c = 0.0f; S.d = sy;
		S.tx = S.ty = 0.0f;
		return S;
	}
	static Mat3 createTranslation(const  Vec2 &translate)
	{
		return createTranslation(translate.x, translate.y);
	}
	static Mat3 createTranslation(float x, float y)
	{
		Mat3 R;
		R.tx = x, R.ty = y;
		return R;
	}
	// SDL2 (Y 轴向下) 的标准旋转矩阵构造
	static  Mat3 createRotationSDL(Vec2 center, Vec2 rotate);
	static Mat3 createRotation(Vec2 center, float angle);


	 void  transformPoint(const Vec2& point, Vec2* dst) const
	{
		Mat3::transformVec3((float*)this, point.x, point.y, (float*)dst);
	}
	static void  transformVec3(const float* m, float x, float y, float* dst)
	{
		dst[0] = x * m[0] + y * m[2] + m[4];
		dst[1] = x * m[1] + y * m[3] + m[5];
	}

	// 	static Mat3 createSkew(float skewX, float skewY) // 单位：度
	// 	{
	// 		float radX = skewX * 0.01745329252f;
	// 		float radY = skewY * 0.01745329252f;
	// 
	// 		Mat3 mat;
	// 		// 1 和 4 位置保持 1，表示不缩放
	// 		mat.a = 1.0f;
	// 		mat.d = 1.0f;
	// 		// 2 位置和 3 位置填入正切值（注意符号，通常 SDL 的 Y 轴向下，但倾斜公式不变）
	// 		mat.b = tanf(radY); // SkewY（影响 X 方向的切变）
	// 		mat.c = tanf(radX); // SkewX（影响 Y 方向的切变）
	// 		return mat;
	// 	}
	static Mat3 createSkew(float skewX, float skewY) // 单位：度 
	{
		float radX = skewX * 0.01745329252f;
		float radY = skewY * 0.01745329252f;

		Mat3 mat;
		mat.a = 1.0f;
		mat.d = 1.0f;

		// skewX 影响 X 轴向 Y 轴的倾斜程度 (Y 分量加上 X * tan(radX))
		// skewY 影响 Y 轴向 X 轴的倾斜程度 (X 分量加上 Y * tan(radY))
		mat.c = tanf(radY); // SkewY 
		mat.b = tanf(radX); // SkewX 
		return mat;
	}

	bool inverse();
	Mat3 getInversed() const;
	void out(Vec2* ret)const
	{
		out(ret->x, ret->y);
	}
	void out(float &x, float &y)const
	{
		float oldX = x, oldY = y;
		x = a * oldX + c * oldY + tx;	// 必须乘 oldY
		y = b * oldX + d * oldY + ty;	// 必须乘 oldX
	}

	friend std::ostream & operator<<(std::ostream & os, const Mat3 & r)
	{
		os << "a:" << r.a << " b:" << r.b << " c:" << r.c << " d:" << r.d << " tx=" << r.tx << " ty=" << r.
			ty << " \n";
		return os;
	}

};


OG_END


#endif
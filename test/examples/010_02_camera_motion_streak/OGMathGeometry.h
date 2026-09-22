#ifndef _MATH_GEOMETRY_H_
#define _MATH_GEOMETRY_H_

#include "OGPlatformMacros.h"
#include <algorithm>
#include <functional>
#include <cassert>
#include <cmath>


#include <iostream>

OG_BEGIN

extern   float clampf(float value, float min_inclusive, float max_inclusive);
 

/**
 * Defines a 2-element floating point vector.
 */
class Vec2
{
public:
	float x = 0.f;
	float y = 0.f;

	Vec2();
	Vec2(float xx, float yy);
	Vec2(const Vec2& p1, const Vec2& p2);

	Vec2  operator+(const Vec2& v) const;
	Vec2  operator-(const Vec2& v) const;
	Vec2  operator*(float s)       const;
	Vec2  operator/(float s)       const;
	Vec2  operator-()              const;

	Vec2& operator+=(const Vec2& v);
	Vec2& operator-=(const Vec2& v);
	Vec2& operator*=(float s);
	Vec2& operator/=(float s);

	bool operator==(const Vec2& v) const;
	bool operator!=(const Vec2& v) const;

	float dot(const Vec2& v)   const;
	float cross(const Vec2& v) const;
	float getLengthSq() const;
	float getLength()   const;
	float length()      const;

	void  normalize();
	Vec2  getNormalized() const;
	Vec2  getPerp()       const;

	void  set(float xx, float yy);
	bool  equals(const Vec2& target) const;

	static const Vec2 ZERO;
};

Vec2 operator*(float s, const Vec2& v);
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
	explicit Size(const Vec2& point) : width(point.x), height(point.y)
	{
	}
	void  setSize(float w, float h)
	{
		this->width = w;
		this->height = h;
	}	
	Size& operator= (const Size& other) {
		setSize(other.width, other.height);
		return *this;
	}
	Size& operator= (const Vec2& point) {
		setSize(point.x, point.y);
		return *this;
	}
	Size operator+(const Size& right) const { return Size(this->width + right.width, this->height + right.height); }
	Size operator-(const Size& right) const { return Size(this->width - right.width, this->height - right.height); }
	Size operator*(float a) const { return Size(this->width * a, this->height * a); }
	Size operator/(float a) const { 
		assert(a != 0);
		return Size(this->width / a, this->height / a);
	}
	bool equals(const Size& target) const {
		return (std::abs(this->width - target.width) < FLT_EPSILON)
			&& (std::abs(this->height - target.height) < FLT_EPSILON);
	}
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
	void setRect(float x, float y, float width, float height) {
		origin.x = x;
		origin.y = y;

		size.width = width;
		size.height = height;
	}
	float getMinX() const
	{
		return origin.x;
	}
	 
	float getMidX() const{ return origin.x + size.width / 2.0f; }
	float getMaxX() const { return origin.x + size.width; }
	float getMinY() const { return origin.y; }
	float getMidY() const{ return origin.y + size.height / 2.0f; }
	float getMaxY() const{ return origin.y + size.height; }
	bool equals(const Rect& rect) const; 

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

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
};
/**
 * RGBA color composed of 4 bytes.
 * @since v3.0
 */
struct   Color4B
{
	Color4B();
	Color4B(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a);

	explicit Color4B(const Color4F& color);

	 void set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
 
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;
 
};


/**
 * RGBA color composed of 4 floats.
 * @since v3.0
 */
struct   Color4F
{
	Color4F();
	Color4F(float _r, float _g, float _b, float _a);

	explicit Color4F(const Color4B& color);

	float r = 0.f;
	float g = 0.f;
	float b = 0.f;
	float a = 0.f;
 
};

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
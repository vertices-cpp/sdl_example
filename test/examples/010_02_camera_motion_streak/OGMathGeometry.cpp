#include "OGMathGeometry.h"


OG_BEGIN

  float clampf(float value, float min_inclusive, float max_inclusive)
{
	if (min_inclusive > max_inclusive) {
		std::swap(min_inclusive, max_inclusive);
	}
	return value < min_inclusive ? min_inclusive : value < max_inclusive ? value : max_inclusive;
}
// ---------------- 构造 ----------------
Vec2::Vec2()
	: x(0.0f), y(0.0f)
{
}

Vec2::Vec2(float xx, float yy)
	: x(xx), y(yy)
{
}

Vec2::Vec2(const Vec2& p1, const Vec2& p2)
	: x(p2.x - p1.x), y(p2.y - p1.y)
{
}

// ---------------- 基础运算：直接写表达式 ----------------
Vec2 Vec2::operator+(const Vec2& v) const
{
	return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator-(const Vec2& v) const
{
	return Vec2(x - v.x, y - v.y);
}

Vec2 Vec2::operator*(float s) const
{
	return Vec2(x * s, y * s);
}

Vec2 Vec2::operator/(float s) const
{
	return Vec2(x / s, y / s);
}

Vec2 Vec2::operator-() const
{
	return Vec2(-x, -y);
}

Vec2& Vec2::operator+=(const Vec2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vec2& Vec2::operator-=(const Vec2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vec2& Vec2::operator*=(float s)
{
	x *= s;
	y *= s;
	return *this;
}

Vec2& Vec2::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}

bool Vec2::operator==(const Vec2& v) const
{
	return x == v.x && y == v.y;
}

bool Vec2::operator!=(const Vec2& v) const
{
	return x != v.x || y != v.y;
}

// ---------------- 点积、叉积 ----------------
float Vec2::dot(const Vec2& v) const
{
	return x * v.x + y * v.y;
}

float Vec2::cross(const Vec2& v) const
{
	return x * v.y - y * v.x;
}

// ---------------- 长度 ----------------
float Vec2::getLengthSq() const
{
	return x * x + y * y;
}

float Vec2::getLength() const
{
	return sqrtf(x * x + y * y);
}

float Vec2::length() const
{
	return sqrtf(x * x + y * y);
}

// ---------------- 归一化 ----------------
void Vec2::normalize()
{
	float n = x * x + y * y;
	if (n == 1.0f)
		return;

	n = sqrtf(n);
	if (n < 2e-37f)
		return;

	n = 1.0f / n;
	x *= n;
	y *= n;
}

Vec2 Vec2::getNormalized() const
{
	Vec2 v(*this);
	v.normalize();
	return v;
}

Vec2 Vec2::getPerp() const
{
	return Vec2(-y, x);
}

// ---------------- set / equals ----------------
void Vec2::set(float xx, float yy)
{
	x = xx;
	y = yy;
}

bool Vec2::equals(const Vec2& target) const
{
	return (std::abs(x - target.x) < FLT_EPSILON)
		&& (std::abs(y - target.y) < FLT_EPSILON);
}

// ---------------- 静态常量 / 左乘 ----------------


Vec2 operator*(float s, const Vec2& v)
{
	return Vec2(v.x * s, v.y * s);
}

const Vec2 Vec2::ZERO(0.0f, 0.0f); 

 
// implementation of Size

Size::Size() : width(0), height(0)
{
}

Size::Size(float w, float h) : width(w), height(h)
{
}

Size::Size(const Size& other) : width(other.width), height(other.height)
{
}
 

const Size Size::ZERO = Size(0, 0);

// implementation of Rect

Rect::Rect()
{
	setRect(0.0f, 0.0f, 0.0f, 0.0f);
}

Rect::Rect(float x, float y, float width, float height)
{
	setRect(x, y, width, height);
}
Rect::Rect(const Vec2& pos, const Size& dimension)
{
	setRect(pos.x, pos.y, dimension.width, dimension.height);
}

Rect::Rect(const Rect& other)
{
	setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
}

Rect& Rect::operator= (const Rect& other)
{
	setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
	return *this;
}
 

bool Rect::equals(const Rect& rect) const
{
	return (origin.equals(rect.origin) &&
		size.equals(rect.size));
}
 
const Rect Rect::ZERO = Rect(0, 0, 0, 0);


/**
 * Color3B
 */

Color3B::Color3B()
{}

Color3B::Color3B(uint8_t _r, uint8_t _g, uint8_t _b)
	: r(_r)
	, g(_g)
	, b(_b)
{}

Color3B::Color3B(const Color4B& color)
	: r(color.r)
	, g(color.g)
	, b(color.b)
{}

Color3B::Color3B(const Color4F& color)
	: r(color.r * 255.0f)
	, g(color.g * 255.0f)
	, b(color.b * 255.0f)
{}


/**
 * Color4B
 */

Color4B::Color4B()
{}

Color4B::Color4B(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
	: r(_r)
	, g(_g)
	, b(_b)
	, a(_a)
{}


Color4B::Color4B(const Color4F& color)
	: r(color.r * 255)
	, g(color.g * 255)
	, b(color.b * 255)
	, a(color.a * 255)
{}


/**
 * Color4F
 */

Color4F::Color4F()
{}

Color4F::Color4F(float _r, float _g, float _b, float _a)
	: r(_r)
	, g(_g)
	, b(_b)
	, a(_a)
{}
Color4F::Color4F(const Color4B& color)
	: r(color.r / 255.0f)
	, g(color.g / 255.0f)
	, b(color.b / 255.0f)
	, a(color.a / 255.0f)
{}

/**
 * Color constants
 */


//Mat3 Mat3::operator*(const Mat3 & r)const
//{
//	Mat3 R;
//	R.a = a * r.a + b * r.c;
//	R.b = a * r.b + b * r.d;
//	R.c = c * r.a + d * r.c;
//	R.d = c * r.b + d * r.d;
//	R.tx = a * r.tx + b * r.ty + tx;
//	R.ty = c * r.tx + d * r.ty + ty; 
//
//	return R;
//}

Mat3  Mat3::operator*(const Mat3 & r)const
{
	Mat3 R;
	R.a = a * r.a + c * r.b;
	R.b = b * r.a + d * r.b;
	R.c = a * r.c + c * r.d;
	R.d = b * r.c + d * r.d;
	R.tx = a * r.tx + c * r.ty + tx;
	//cout << "开始\nx=" << a << " * " << r.tx << "+" << c << " * " << r.ty << " +" << tx << endl;
	R.ty = b * r.tx + d * r.ty + ty;
	// 	cout << "y=" << b << " * " << r.tx << "+" << d << " * " << r.ty << " +" << ty << endl;
	// 	cout << "(" << *this << " )(" << r << ")" << endl;
	return R;
}


Mat3 Mat3::createRotationSDL(Vec2 center, Vec2 rotate)
{
	float radX = -(rotate.x * 0.01745329252f);
	float radY = -(rotate.y * 0.01745329252f);

	Mat3 R;

	R.a = cosf(radY); R.c = -sinf(radY);
	R.b = sinf(radX); R.d = cosf(radX);


	Mat3 T_center = createTranslation(center.x, center.y);
	Mat3 T_neg = createTranslation(-center.x, -center.y);

	//	return   R  ;
	return T_center * R * T_neg;
}


Mat3 Mat3::createRotation(Vec2 center, float angle) {
	float rad = angle * 0.01745329252f;
	float c = cosf(rad), s = sinf(rad);
	Mat3 R;
	R.a = c; R.c = -s;
	R.b = s; R.d = c;
	R.tx = R.ty = 0.0f;


	Mat3  T_top = createTranslation(center.x, center.y);
	Mat3 T_neg = createTranslation(-center.x, -center.y);

	return T_top * R * T_neg;

}

// bool Mat3::inverse()
// {
// 	float det = a * d - b * c;
// 	if (std::fabs(det) < 1e-6f)	// 阈值可根据需求调整
// 		return false;
// 
// 	float invDet = 1.0f / det;
// 	float new_a = d * invDet;
// 	float new_b = -b * invDet;
// 	float new_c = -c * invDet;
// 	float new_d = a * invDet;
// 	float new_tx = (b * ty - d * tx) * invDet;
// 	float new_ty = (c * tx - a * ty) * invDet;
// 
// 	a = new_a;
// 	b = new_b;
// 	c = new_c;
// 	d = new_d;
// 	tx = new_tx;
// 	ty = new_ty;
// 	return true;
// }


bool Mat3::inverse()
{
	float det = a * d - b * c;
	if (std::fabs(det) < 1e-12f /* 1e-6f*/)
		return false;

	float invDet = 1.0f / det;
	float new_a = d * invDet;
	float new_b = -b * invDet;
	float new_c = -c * invDet;
	float new_d = a * invDet;

	// 列向量右乘体系下的正确平移逆变换[cite: 4]
	float new_tx = (c * ty - d * tx) * invDet;
	float new_ty = (b * tx - a * ty) * invDet;

	a = new_a;
	b = new_b;
	c = new_c;
	d = new_d;
	tx = new_tx;
	ty = new_ty;
	return true;
}


Mat3 Mat3::getInversed() const
{
	Mat3 mat(*this);
	mat.inverse();
	return mat;
}

const Mat3  Mat3::IDENTITY = Mat3(
	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f);


OG_END
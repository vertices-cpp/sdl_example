#include "OGMathGeometry.h"


OG_BEGIN

  float clampf(float value, float min_inclusive, float max_inclusive)
{
	if (min_inclusive > max_inclusive) {
		std::swap(min_inclusive, max_inclusive);
	}
	return value < min_inclusive ? min_inclusive : value < max_inclusive ? value : max_inclusive;
}

 Vec2::Vec2()
	: x(0.0f), y(0.0f)
{
}

 Vec2::Vec2(float xx, float yy)
	: x(xx), y(yy)
{
}

 Vec2::Vec2(const float* array)
{
	set(array);
}

 Vec2::Vec2(const Vec2& p1, const Vec2& p2)
{
	set(p1, p2);
}

 bool Vec2::isZero() const
{
	return x == 0.0f && y == 0.0f;
}

bool Vec2::isOne() const
{
	return x == 1.0f && y == 1.0f;
}

 void Vec2::add(const Vec2& v)
{
	x += v.x;
	y += v.y;
}

 float Vec2::distanceSquared(const Vec2& v) const
{
	float dx = v.x - x;
	float dy = v.y - y;
	return (dx * dx + dy * dy);
}

 float Vec2::dot(const Vec2& v) const
{
	return (x * v.x + y * v.y);
}

 float Vec2::lengthSquared() const
{
	return (x * x + y * y);
}

 void Vec2::negate()
{
	x = -x;
	y = -y;
}

 void Vec2::scale(float scalar)
{
	x *= scalar;
	y *= scalar;
}

 void Vec2::scale(const Vec2& scale)
{
	x *= scale.x;
	y *= scale.y;
}

 void Vec2::set(float xx, float yy)
{
	this->x = xx;
	this->y = yy;
}

 void Vec2::set(const Vec2& v)
{
	this->x = v.x;
	this->y = v.y;
}

 void Vec2::set(const Vec2& p1, const Vec2& p2)
{
	x = p2.x - p1.x;
	y = p2.y - p1.y;
}

void Vec2::setZero()
{
	x = y = 0.0f;
}

 void Vec2::subtract(const Vec2& v)
{
	x -= v.x;
	y -= v.y;
}

 void Vec2::smooth(const Vec2& target, float elapsedTime, float responseTime)
{
	if (elapsedTime > 0)
	{
		*this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
	}
}

 Vec2 Vec2::operator+(const Vec2& v) const
{
	Vec2 result(*this);
	result.add(v);
	return result;
}

 Vec2& Vec2::operator+=(const Vec2& v)
{
	add(v);
	return *this;
}

 Vec2 Vec2::operator-(const Vec2& v) const
{
	Vec2 result(*this);
	result.subtract(v);
	return result;
}

 Vec2& Vec2::operator-=(const Vec2& v)
{
	subtract(v);
	return *this;
}

 Vec2 Vec2::operator-() const
{
	Vec2 result(*this);
	result.negate();
	return result;
}

 Vec2 Vec2::operator*(float s) const
{
	Vec2 result(*this);
	result.scale(s);
	return result;
}

 Vec2& Vec2::operator*=(float s)
{
	scale(s);
	return *this;
}

 Vec2 Vec2::operator/(const float s) const
{
	return Vec2(this->x / s, this->y / s);
}

 bool Vec2::operator<(const Vec2& v) const
{
	if (x == v.x)
	{
		return y < v.y;
	}
	return x < v.x;
}

 bool Vec2::operator>(const Vec2& v) const
{
	if (x == v.x)
	{
		return y > v.y;
	}
	return x > v.x;
}

 bool Vec2::operator==(const Vec2& v) const
{
	return x == v.x && y == v.y;
}

 bool Vec2::operator!=(const Vec2& v) const
{
	return x != v.x || y != v.y;
}

 Vec2 operator*(float x, const Vec2& v)
{
	Vec2 result(v);
	result.scale(x);
	return result;
}

void Vec2::setPoint(float xx, float yy)
{
	this->x = xx;
	this->y = yy;
}


bool isOneDimensionSegmentOverlap(float A, float B, float C, float D, float *S, float * E)
{
	float ABmin = std::min(A, B);
	float ABmax = std::max(A, B);
	float CDmin = std::min(C, D);
	float CDmax = std::max(C, D);

	if (ABmax < CDmin || CDmax < ABmin)
	{
		// ABmin->ABmax->CDmin->CDmax or CDmin->CDmax->ABmin->ABmax
		return false;
	}
	else
	{
		if (ABmin >= CDmin && ABmin <= CDmax)
		{
			// CDmin->ABmin->CDmax->ABmax or CDmin->ABmin->ABmax->CDmax
			if (S != nullptr) *S = ABmin;
			if (E != nullptr) *E = CDmax < ABmax ? CDmax : ABmax;
		}
		else if (ABmax >= CDmin && ABmax <= CDmax)
		{
			// ABmin->CDmin->ABmax->CDmax
			if (S != nullptr) *S = CDmin;
			if (E != nullptr) *E = ABmax;
		}
		else
		{
			// ABmin->CDmin->CDmax->ABmax
			if (S != nullptr) *S = CDmin;
			if (E != nullptr) *E = CDmax;
		}
		return true;
	}
}

// cross product of 2 vector. A->B X C->D
float crossProduct2Vector(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
{
	return (D.y - C.y) * (B.x - A.x) - (D.x - C.x) * (B.y - A.y);
}

float Vec2::angle(const Vec2& v1, const Vec2& v2)
{
	float dz = v1.x * v2.y - v1.y * v2.x;
	return atan2f(fabsf(dz) + (1.0e-37f), dot(v1, v2));
}

void Vec2::add(const Vec2& v1, const Vec2& v2, Vec2* dst)
{
//	GP_ASSERT(dst);

	dst->x = v1.x + v2.x;
	dst->y = v1.y + v2.y;
}

void Vec2::clamp(const Vec2& min, const Vec2& max)
{
	//GP_ASSERT(!(min.x > max.x || min.y > max.y));

	// Clamp the x value.
	if (x < min.x)
		x = min.x;
	if (x > max.x)
		x = max.x;

	// Clamp the y value.
	if (y < min.y)
		y = min.y;
	if (y > max.y)
		y = max.y;
}

void Vec2::clamp(const Vec2& v, const Vec2& min, const Vec2& max, Vec2* dst)
{
// 	GP_ASSERT(dst);
// 	GP_ASSERT(!(min.x > max.x || min.y > max.y));

	// Clamp the x value.
	dst->x = v.x;
	if (dst->x < min.x)
		dst->x = min.x;
	if (dst->x > max.x)
		dst->x = max.x;

	// Clamp the y value.
	dst->y = v.y;
	if (dst->y < min.y)
		dst->y = min.y;
	if (dst->y > max.y)
		dst->y = max.y;
}

float Vec2::distance(const Vec2& v) const
{
	float dx = v.x - x;
	float dy = v.y - y;

	return std::sqrt(dx * dx + dy * dy);
}

float Vec2::dot(const Vec2& v1, const Vec2& v2)
{
	return (v1.x * v2.x + v1.y * v2.y);
}

float Vec2::length() const
{
	return std::sqrt(x * x + y * y);
}

void Vec2::normalize()
{
	float n = x * x + y * y;
	// Already normalized.
	if (n == 1.0f)
		return;

	n = std::sqrt(n);
	// Too close to zero.
	if (n < (2e-37f))
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

void Vec2::rotate(const Vec2& point, float angle)
{
	float sinAngle = std::sin(angle);
	float cosAngle = std::cos(angle);

	if (point.isZero())
	{
		float tempX = x * cosAngle - y * sinAngle;
		y = y * cosAngle + x * sinAngle;
		x = tempX;
	}
	else
	{
		float tempX = x - point.x;
		float tempY = y - point.y;

		x = tempX * cosAngle - tempY * sinAngle + point.x;
		y = tempY * cosAngle + tempX * sinAngle + point.y;
	}
}

void Vec2::set(const float* array)
{
	//GP_ASSERT(array);

	x = array[0];
	y = array[1];
}

void Vec2::subtract(const Vec2& v1, const Vec2& v2, Vec2* dst)
{
	//GP_ASSERT(dst);

	dst->x = v1.x - v2.x;
	dst->y = v1.y - v2.y;
}

bool Vec2::equals(const Vec2& target) const
{
	return (std::abs(this->x - target.x) < FLT_EPSILON)
		&& (std::abs(this->y - target.y) < FLT_EPSILON);
}

bool Vec2::fuzzyEquals(const Vec2& b, float var) const
{
	if (x - var <= b.x && b.x <= x + var)
		if (y - var <= b.y && b.y <= y + var)
			return true;
	return false;
}

float Vec2::getAngle(const Vec2& other) const
{
	Vec2 a2 = getNormalized();
	Vec2 b2 = other.getNormalized();
	float angle = atan2f(a2.cross(b2), a2.dot(b2));
	if (std::abs(angle) < FLT_EPSILON) return 0.f;
	return angle;
}

Vec2 Vec2::rotateByAngle(const Vec2& pivot, float angle) const
{
	return pivot + (*this - pivot).rotate(Vec2::forAngle(angle));
}

bool Vec2::isLineIntersect(const Vec2& A, const Vec2& B,
	const Vec2& C, const Vec2& D,
	float *S, float *T)
{
	// FAIL: Line undefined
	if ((A.x == B.x && A.y == B.y) || (C.x == D.x && C.y == D.y))
	{
		return false;
	}

	const float denom = crossProduct2Vector(A, B, C, D);

	if (denom == 0)
	{
		// Lines parallel or overlap
		return false;
	}

	if (S != nullptr) *S = crossProduct2Vector(C, D, C, A) / denom;
	if (T != nullptr) *T = crossProduct2Vector(A, B, C, A) / denom;

	return true;
}

bool Vec2::isLineParallel(const Vec2& A, const Vec2& B,
	const Vec2& C, const Vec2& D)
{
	// FAIL: Line undefined
	if ((A.x == B.x && A.y == B.y) || (C.x == D.x && C.y == D.y))
	{
		return false;
	}

	if (crossProduct2Vector(A, B, C, D) == 0)
	{
		// line overlap
		if (crossProduct2Vector(C, D, C, A) == 0 || crossProduct2Vector(A, B, C, A) == 0)
		{
			return false;
		}

		return true;
	}

	return false;
}

bool Vec2::isLineOverlap(const Vec2& A, const Vec2& B,
	const Vec2& C, const Vec2& D)
{
	// FAIL: Line undefined
	if ((A.x == B.x && A.y == B.y) || (C.x == D.x && C.y == D.y))
	{
		return false;
	}

	if (crossProduct2Vector(A, B, C, D) == 0 &&
		(crossProduct2Vector(C, D, C, A) == 0 || crossProduct2Vector(A, B, C, A) == 0))
	{
		return true;
	}

	return false;
}

bool Vec2::isSegmentOverlap(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D, Vec2* S, Vec2* E)
{

	if (isLineOverlap(A, B, C, D))
	{
		return isOneDimensionSegmentOverlap(A.x, B.x, C.x, D.x, &S->x, &E->x) &&
			isOneDimensionSegmentOverlap(A.y, B.y, C.y, D.y, &S->y, &E->y);
	}

	return false;
}

bool Vec2::isSegmentIntersect(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
{
	float S, T;

	if (isLineIntersect(A, B, C, D, &S, &T) &&
		(S >= 0.0f && S <= 1.0f && T >= 0.0f && T <= 1.0f))
	{
		return true;
	}

	return false;
}

Vec2 Vec2::getIntersectPoint(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
{
	float S, T;

	if (isLineIntersect(A, B, C, D, &S, &T))
	{
		// Vec2 of intersection
		Vec2 P;
		P.x = A.x + S * (B.x - A.x);
		P.y = A.y + S * (B.y - A.y);
		return P;
	}

	return Vec2::ZERO;
}

const Vec2 Vec2::ZERO(0.0f, 0.0f);
const Vec2 Vec2::ONE(1.0f, 1.0f);
const Vec2 Vec2::UNIT_X(1.0f, 0.0f);
const Vec2 Vec2::UNIT_Y(0.0f, 1.0f);
const Vec2 Vec2::ANCHOR_MIDDLE(0.5f, 0.5f);
const Vec2 Vec2::ANCHOR_BOTTOM_LEFT(0.0f, 0.0f);
const Vec2 Vec2::ANCHOR_TOP_LEFT(0.0f, 1.0f);
const Vec2 Vec2::ANCHOR_BOTTOM_RIGHT(1.0f, 0.0f);
const Vec2 Vec2::ANCHOR_TOP_RIGHT(1.0f, 1.0f);
const Vec2 Vec2::ANCHOR_MIDDLE_RIGHT(1.0f, 0.5f);
const Vec2 Vec2::ANCHOR_MIDDLE_LEFT(0.0f, 0.5f);
const Vec2 Vec2::ANCHOR_MIDDLE_TOP(0.5f, 1.0f);
const Vec2 Vec2::ANCHOR_MIDDLE_BOTTOM(0.5f, 0.0f);

 
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

Size::Size(const Vec2& point) : width(point.x), height(point.y)
{
}

Size& Size::operator= (const Size& other)
{
	setSize(other.width, other.height);
	return *this;
}

Size& Size::operator= (const Vec2& point)
{
	setSize(point.x, point.y);
	return *this;
}

Size Size::operator+(const Size& right) const
{
	return Size(this->width + right.width, this->height + right.height);
}

Size Size::operator-(const Size& right) const
{
	return Size(this->width - right.width, this->height - right.height);
}

Size Size::operator*(float a) const
{
	return Size(this->width * a, this->height * a);
}

Size Size::operator/(float a) const
{
	//CCASSERT(a != 0, "CCSize division by 0.");
	return Size(this->width / a, this->height / a);
}

void Size::setSize(float w, float h)
{
	this->width = w;
	this->height = h;
}

bool Size::equals(const Size& target) const
{
	return (std::abs(this->width - target.width) < FLT_EPSILON)
		&& (std::abs(this->height - target.height) < FLT_EPSILON);
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

void Rect::setRect(float x, float y, float width, float height)
{
	// CGRect can support width<0 or height<0
	// CCASSERT(width >= 0.0f && height >= 0.0f, "width and height of Rect must not less than 0.");

	origin.x = x;
	origin.y = y;

	size.width = width;
	size.height = height;
}

bool Rect::equals(const Rect& rect) const
{
	return (origin.equals(rect.origin) &&
		size.equals(rect.size));
}

float Rect::getMaxX() const
{
	return origin.x + size.width;
}

float Rect::getMidX() const
{
	return origin.x + size.width / 2.0f;
}

float Rect::getMinX() const
{
	return origin.x;
}

float Rect::getMaxY() const
{
	return origin.y + size.height;
}

float Rect::getMidY() const
{
	return origin.y + size.height / 2.0f;
}

float Rect::getMinY() const
{
	return origin.y;
}

bool Rect::containsPoint(const Vec2& point) const
{
	bool bRet = false;

	if (point.x >= getMinX() && point.x <= getMaxX()
		&& point.y >= getMinY() && point.y <= getMaxY())
	{
		bRet = true;
	}

	return bRet;
}

bool Rect::intersectsRect(const Rect& rect) const
{
	return !(getMaxX() < rect.getMinX() ||
		rect.getMaxX() < getMinX() ||
		getMaxY() < rect.getMinY() ||
		rect.getMaxY() < getMinY());
}

bool Rect::intersectsCircle(const Vec2& center, float radius) const
{
	Vec2 rectangleCenter((origin.x + size.width / 2),
		(origin.y + size.height / 2));

	float w = size.width / 2;
	float h = size.height / 2;

	float dx = std::abs(center.x - rectangleCenter.x);
	float dy = std::abs(center.y - rectangleCenter.y);

	if (dx > (radius + w) || dy > (radius + h))
	{
		return false;
	}

	Vec2 circleDistance(std::abs(center.x - origin.x - w),
		std::abs(center.y - origin.y - h));

	if (circleDistance.x <= (w))
	{
		return true;
	}

	if (circleDistance.y <= (h))
	{
		return true;
	}

	float cornerDistanceSq = powf(circleDistance.x - w, 2) + powf(circleDistance.y - h, 2);

	return (cornerDistanceSq <= (powf(radius, 2)));
}

void Rect::merge(const Rect& rect)
{
	float minX = std::min(getMinX(), rect.getMinX());
	float minY = std::min(getMinY(), rect.getMinY());
	float maxX = std::max(getMaxX(), rect.getMaxX());
	float maxY = std::max(getMaxY(), rect.getMaxY());
	setRect(minX, minY, maxX - minX, maxY - minY);
}

Rect Rect::unionWithRect(const Rect & rect) const
{
	float thisLeftX = origin.x;
	float thisRightX = origin.x + size.width;
	float thisTopY = origin.y + size.height;
	float thisBottomY = origin.y;

	if (thisRightX < thisLeftX)
	{
		std::swap(thisRightX, thisLeftX);   // This rect has negative width
	}

	if (thisTopY < thisBottomY)
	{
		std::swap(thisTopY, thisBottomY);   // This rect has negative height
	}

	float otherLeftX = rect.origin.x;
	float otherRightX = rect.origin.x + rect.size.width;
	float otherTopY = rect.origin.y + rect.size.height;
	float otherBottomY = rect.origin.y;

	if (otherRightX < otherLeftX)
	{
		std::swap(otherRightX, otherLeftX);   // Other rect has negative width
	}

	if (otherTopY < otherBottomY)
	{
		std::swap(otherTopY, otherBottomY);   // Other rect has negative height
	}

	float combinedLeftX = std::min(thisLeftX, otherLeftX);
	float combinedRightX = std::max(thisRightX, otherRightX);
	float combinedTopY = std::max(thisTopY, otherTopY);
	float combinedBottomY = std::min(thisBottomY, otherBottomY);

	return Rect(combinedLeftX, combinedBottomY, combinedRightX - combinedLeftX, combinedTopY - combinedBottomY);
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

bool Color3B::operator==(const Color3B& right) const
{
	return (r == right.r && g == right.g && b == right.b);
}

bool Color3B::operator==(const Color4B& right) const
{
	return (r == right.r && g == right.g && b == right.b && 255 == right.a);
}

bool Color3B::operator==(const Color4F& right) const
{
	return (right.a == 1.0f && Color4F(*this) == right);
}

bool Color3B::operator!=(const Color3B& right) const
{
	return !(*this == right);
}

bool Color3B::operator!=(const Color4B& right) const
{
	return !(*this == right);
}

bool Color3B::operator!=(const Color4F& right) const
{
	return !(*this == right);
}

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

Color4B::Color4B(const Color3B& color, uint8_t _a)
	: r(color.r)
	, g(color.g)
	, b(color.b)
	, a(_a)
{}

Color4B::Color4B(const Color4F& color)
	: r(color.r * 255)
	, g(color.g * 255)
	, b(color.b * 255)
	, a(color.a * 255)
{}

bool Color4B::operator==(const Color4B& right) const
{
	return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color4B::operator==(const Color3B& right) const
{
	return (r == right.r && g == right.g && b == right.b && a == 255);
}

bool Color4B::operator==(const Color4F& right) const
{
	return (*this == Color4B(right));
}

bool Color4B::operator!=(const Color4B& right) const
{
	return !(*this == right);
}

bool Color4B::operator!=(const Color3B& right) const
{
	return !(*this == right);
}

bool Color4B::operator!=(const Color4F& right) const
{
	return !(*this == right);
}

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

Color4F::Color4F(const Color3B& color, float _a)
	: r(color.r / 255.0f)
	, g(color.g / 255.0f)
	, b(color.b / 255.0f)
	, a(_a)
{}

Color4F::Color4F(const Color4B& color)
	: r(color.r / 255.0f)
	, g(color.g / 255.0f)
	, b(color.b / 255.0f)
	, a(color.a / 255.0f)
{}

bool Color4F::operator==(const Color4F& right) const
{
	return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color4F::operator==(const Color3B& right) const
{
	return (a == 1.0f && Color3B(*this) == right);
}

bool Color4F::operator==(const Color4B& right) const
{
	return (*this == Color4F(right));
}

bool Color4F::operator!=(const Color4F& right) const
{
	return !(*this == right);
}

bool Color4F::operator!=(const Color3B& right) const
{
	return !(*this == right);
}

bool Color4F::operator!=(const Color4B& right) const
{
	return !(*this == right);
}

Color4F& operator+=(Color4F& lhs, const Color4F& rhs) {
	lhs.r += rhs.r;
	lhs.g += rhs.g;
	lhs.b += rhs.b;
	lhs.a += rhs.a;
	return lhs;
}
Color4F operator+(Color4F lhs, const Color4F& rhs) {
	return lhs += rhs;
}
Color4F& operator-=(Color4F& lhs, const Color4F& rhs) {
	lhs.r -= rhs.r;
	lhs.g -= rhs.g;
	lhs.b -= rhs.b;
	lhs.a -= rhs.a;
	return lhs;
}
Color4F operator-(Color4F lhs, const Color4F& rhs) {
	return lhs -= rhs;
}

Color4F& operator*=(Color4F& lhs, const Color4F& rhs) {
	lhs.r *= rhs.r;
	lhs.g *= rhs.g;
	lhs.b *= rhs.b;
	lhs.a *= rhs.a;
	return lhs;
}
Color4F& operator*=(Color4F& lhs, float rhs) {
	lhs.r *= rhs;
	lhs.g *= rhs;
	lhs.b *= rhs;
	lhs.a *= rhs;
	return lhs;
}
Color4F operator*(Color4F lhs, const Color4F& rhs) {
	return lhs *= rhs;
}

Color4F operator*(Color4F lhs, float rhs) {
	return lhs *= rhs;
}

Color4F& operator/=(Color4F& lhs, const Color4F& rhs) {
	lhs.r /= rhs.r;
	lhs.g /= rhs.g;
	lhs.b /= rhs.b;
	lhs.a /= rhs.a;
	return lhs;
}
Color4F& operator/=(Color4F& lhs, float rhs) {
	lhs.r /= rhs;
	lhs.g /= rhs;
	lhs.b /= rhs;
	lhs.a /= rhs;
	return lhs;
}
Color4F operator/(Color4F lhs, const Color4F& rhs) {
	return lhs /= rhs;
}

Color4F operator/(Color4F lhs, float rhs) {
	return lhs /= rhs;
}

/**
 * Color constants
 */

const Color3B Color3B::WHITE(255, 255, 255);
const Color3B Color3B::YELLOW(255, 255, 0);
const Color3B Color3B::GREEN(0, 255, 0);
const Color3B Color3B::BLUE(0, 0, 255);
const Color3B Color3B::RED(255, 0, 0);
const Color3B Color3B::MAGENTA(255, 0, 255);
const Color3B Color3B::BLACK(0, 0, 0);
const Color3B Color3B::ORANGE(255, 127, 0);
const Color3B Color3B::GRAY(166, 166, 166);

const Color4B Color4B::WHITE(255, 255, 255, 255);
const Color4B Color4B::YELLOW(255, 255, 0, 255);
const Color4B Color4B::GREEN(0, 255, 0, 255);
const Color4B Color4B::BLUE(0, 0, 255, 255);
const Color4B Color4B::RED(255, 0, 0, 255);
const Color4B Color4B::MAGENTA(255, 0, 255, 255);
const Color4B Color4B::BLACK(0, 0, 0, 255);
const Color4B Color4B::ORANGE(255, 127, 0, 255);
const Color4B Color4B::GRAY(166, 166, 166, 255);

const Color4F Color4F::WHITE(1, 1, 1, 1);
const Color4F Color4F::YELLOW(1, 1, 0, 1);
const Color4F Color4F::GREEN(0, 1, 0, 1);
const Color4F Color4F::BLUE(0, 0, 1, 1);
const Color4F Color4F::RED(1, 0, 0, 1);
const Color4F Color4F::MAGENTA(1, 0, 1, 1);
const Color4F Color4F::BLACK(0, 0, 0, 1);
const Color4F Color4F::ORANGE(1, 0.5f, 0, 1);
const Color4F Color4F::GRAY(0.65f, 0.65f, 0.65f, 1);



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
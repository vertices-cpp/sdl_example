#include "Mat3.h"
#include "OGGeometry.h"

OG_BEGIN


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
	R.b =  sinf(radX); R.d = cosf(radX);
	 

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
		if (std::fabs(det) < /*1e-12f*/  1e-6f)
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


Rect RectApplyMat3(const Rect& rect, const Mat3& m)
{
	// 纯平移+缩放：直接算，不用四个角
	if (m.b == 0.0f && m.c == 0.0f) {
		float x0 = m.a * rect.origin.x + m.tx;
		float y0 = m.d * rect.origin.y + m.ty;
		float w = m.a * rect.size.width;
		float h = m.d * rect.size.height;
		if (w < 0) { x0 += w; w = -w; }
		if (h < 0) { y0 += h; h = -h; }
		return Rect(x0, y0, w, h);   // Y 向下：y0 是上边
	}

	// 含旋转/斜切：四角变换取 AABB
	float left = rect.origin.x;
	float top = rect.origin.y;
	float right = left + rect.size.width;
	float bottom = top + rect.size.height;

	Vec2 p, tl, tr, bl, br;
	m.transformPoint(Vec2(left, top), &tl);
	m.transformPoint(Vec2(right, top), &tr);
	m.transformPoint(Vec2(left, bottom), &bl);
	m.transformPoint(Vec2(right, bottom), &br);

	float minX = std::min(std::min(tl.x, tr.x), std::min(bl.x, br.x));
	float maxX = std::max(std::max(tl.x, tr.x), std::max(bl.x, br.x));
	float minY = std::min(std::min(tl.y, tr.y), std::min(bl.y, br.y));
	float maxY = std::max(std::max(tl.y, tr.y), std::max(bl.y, br.y));

	return Rect(minX, minY, maxX - minX, maxY - minY);
}

OG_END
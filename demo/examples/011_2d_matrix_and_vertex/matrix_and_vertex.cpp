//顶点示例  


#if defined(_MSC_VER) 
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#endif

#include <string>
using std::string;
#include <sstream>
using std::stringstream;
#include <iostream>
using std::cout;
using std::endl;

#include "path_head.h"

const int SCREEN_WIDTH = 600, SCREEN_HEIGHT = 800;
SDL_Window* window;
SDL_Renderer *render;
SDL_Texture *texture;
TTF_Font *font;

#include <vector>



void init() {
	/* 初始化SDL */
	SDL_Init(SDL_INIT_VIDEO);
	/* 线性纹理 */
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	/* 产生窗口 */
	window = SDL_CreateWindow("pixel test", 200, 300,
		SCREEN_WIDTH, SCREEN_HEIGHT, /*SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS |*/ SDL_WINDOW_HIDDEN);

	//SDL_SetWindowFullscreen(window, SDL_WINDOW_SHOWN);


	/* 即将初始化的渲染器驱动程序的索引，如果是-1表示flags标志的第一个可用驱动程序。 */
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	SDL_RenderClear(render);
	//SetWindowTransparent(RGB(255, 255, 255));
//	SetWindowTransparent(RGB(0, 0, 0));
	//SetWindowTransparent(RGB(255, 255, 255));
	SDL_ShowWindow(window);

	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

	//auto t = IMG_Init(IMG_INIT_PNG);
	/* 初始化png加载 */
	IMG_Init(IMG_INIT_PNG);

}

void close()
{
	//释放图像
	SDL_DestroyTexture(texture);

	//释放窗口
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);

	//退出子系统
	IMG_Quit();
	SDL_Quit();
}


void free()
{
	//释放纹理（如果存在）
	if (texture != NULL)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
	}
}


#define MATH_DEG_TO_RAD(x)          ((x) * 0.0174532925f)
#define MATH_RAD_TO_DEG(x)          ((x)* 57.29577951f)

struct Vec2 { float x, y; };
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

	Mat3 operator*(const Mat3 & r)const {
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
	static Mat3 createScale(float sx, float sy) {
		Mat3 S;
		S.a = sx; S.b = 0.0f;
		S.c = 0.0f; S.d = sy;
		S.tx = S.ty = 0.0f;
		return S;
	}

	static Mat3 createTranslation(float x, float y)
	{
		Mat3 R;
		R.tx = x, R.ty = y;
		return R;
	}
	// SDL2 (Y 轴向下) 的标准旋转矩阵构造
	Mat3 createRotationSDL(Vec2 center, Vec2 rotate) {
		float radX = -(rotate.x * 0.01745329252f);
		float radY = -(rotate.y * 0.01745329252f);
	
		Mat3 R;

		R.a = cosf(radY); R.c = sinf(radY);
		R.b = -sinf(radX); R.d = cosf(radX);

		cout <<"角度:"<<rotate.x<<  " radX:" << radX
			<< " radY:" << radY << " R.a:" << R.a << " R.b:" << R.b << " R.c:" << R.c << " R.d:" << R.d << endl;
 

		Mat3 T_center = createTranslation(center.x, center.y);
		Mat3 T_neg = createTranslation(-center.x, -center.y);

		return /*T_top * */T_center * R * T_neg;
	}
// 	Mat3 createRotationSDL(Vec2 center, Vec2 rotate) {
// 		float radX = -(rotate.x * 0.01745329252f);
// 		float radY = -(rotate.y * 0.01745329252f);
// 
// 		float cosX = cosf(radX);
// 		float sinX = sinf(radX);
// 		float cosY = cosf(radY);
// 		float sinY = sinf(radY);
// 
// 		Mat3 M;
// 		// 旋转分量
// 		M.a = cosY;
// 		M.c = -sinX;  // 注意 negative sin
// 		M.b = sinY;
// 		M.d = cosX;
// 
// 		// 直接写入手算推导出的一模一样的 tx, ty
// 		M.tx = center.x - center.x * cosY + center.y * sinX;
// 		M.ty = center.y - center.x * sinY - center.y * cosX;
// 
// 		return M;
// 	}
	static Mat3 createRotation(Vec2 center, float angle) {
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

	bool inverse() {
		float det = a * d - b * c;
		if (std::fabs(det) < 1e-6f)
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

	void out(float &x, float &y)const
	{
		float oldX = x, oldY = y;
		x = a * oldX + c * oldY + tx;	// 必须乘 oldY
		y = b * oldX + d * oldY + ty;	// 必须乘 oldX
	}

	friend std::ostream & operator<<(std::ostream & os, const Mat3 & r)
	{
		os << r.a << " " << r.b << " " << r.c << " " << r.d << " x=" << r.tx << " y=" << r.
			ty << " \n";
		return os;
	}

};
static const Mat3  IDENTITY = Mat3(
	1.0f, 0.0f, 
	0.0f, 1.0f, 
	0.0f, 0.0f);



template<class _Ty = float>
void og_swap(_Ty& _Left, _Ty& _Right)
{
	_Ty _Tmp = _Left;
	_Left = _Right;
	_Right = _Tmp;
}
#include <array>
int main(int argc, char *argv[]) {


	std::vector<std::array<int, 6>> _indices;
	_indices.resize(2);
	int k = 0;
	//std::array<int, 6> a = { 1 };

	for (auto &i:_indices)
	{
		i[0] = k++;
		i[1] = k++;
		i[2] = k++;
		i[3] = k++;
		i[4] = k++;
		i[5] = k++;
	}
	int  * p = _indices.data()->data();

	cout << p[7] << endl;

	init();


	bool quit = false;

	//事件处理
	SDL_Event e;
	 
	SDL_Surface*surface = IMG_Load(og::checkPath("011_2d_matrix_and_vertex/girl.png").c_str());
	texture = SDL_CreateTextureFromSurface(render, surface);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	float fontw = (float)surface->w;
	float fonth = (float)surface->h;
	SDL_FreeSurface(surface);

	SDL_Vertex verts[4], verts_tmp[4];
	
 

		verts[0] = { {0,0}, {255, 255, 255, 255}, { 0, 0} };
		verts[1] = { {fontw,0}, {255, 255, 255, 255}, {1,0} };
		verts[2] = { {0,fonth}, {255, 255, 255, 255}, {0,1} };
		verts[3] = { {fontw,fonth}, {255, 255, 255, 255}, {1,1} };
 

	int *indices = new int[6];
	//顶点索引 
		indices[0] = {  0 };
		indices[1] = {  1 };
		indices[2] = {  2 };
		indices[3] = {  1 };
		indices[4] = {  2 };
		indices[5] = {  3 };
	 
		
		Vec2 scale = {1,1};

		Vec2 angle = { 0,0 };
		Vec2 rVal = { 0,0 };
		Vec2 center = { fontw/2 ,fonth/2 };


		Vec2 position = { 200,200 };
		Vec2 speed = { 10,10 };
		Vec2 Vel = { 0,0 };
		Mat3 transform(IDENTITY), rotateVal(IDENTITY);

	while (!quit)
	{
		//处理队列上的事件
		while (SDL_PollEvent(&e) != 0)
		{
			//用户请求退出
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN &&e.key.repeat == 0)
			{  
				if (e.key.keysym.sym == SDLK_UP)
				{
					Vel.y -= speed.y;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					Vel.y += speed.y;
				}
				else if (e.key.keysym.sym == SDLK_LEFT)
				{
					Vel.x -= speed.x;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					Vel.x += speed.x;
				}
				else if (e.key.keysym.sym == SDLK_1)
				{
					rVal.x += 10;
				}
				else if (e.key.keysym.sym == SDLK_2)
				{
					rVal.x -= 10;
				}
				else if (e.key.keysym.sym == SDLK_3)
				{
					rVal.y += 10;
				}
				else if (e.key.keysym.sym == SDLK_4)
				{
					rVal.y -= 10;
				}
			}
			else if (e.type == SDL_KEYUP  )
			{
				if (e.key.keysym.sym == SDLK_UP)
				{
					Vel.y += speed.y;
				}
				else if (e.key.keysym.sym == SDLK_DOWN)
				{
					Vel.y -= speed.y;
				}
				else if (e.key.keysym.sym == SDLK_LEFT)
				{
					Vel.x += speed.x;
				}
				else if (e.key.keysym.sym == SDLK_RIGHT)
				{
					Vel.x -= speed.x;
				}
				else if (e.key.keysym.sym == SDLK_1)
				{
					rVal.x -= 10;
				}
				else if (e.key.keysym.sym == SDLK_2)
				{
					rVal.x += 10;
				}
				else if (e.key.keysym.sym == SDLK_3)
				{
					rVal.y -= 10;
				}
				else if (e.key.keysym.sym == SDLK_4)
				{
					rVal.y += 10;
				}
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				if (e.wheel.y > 0)
				{
					scale.x += 0.1f;
					scale.y += 0.1f;
				}
				else if (e.wheel.y < 0)
				{
					scale.x -= 0.1f;
					scale.y -= 0.1f;
					if (scale.x < 0.5 ||scale.y< 0.5)
					{
						scale.x  = 0.5f;
						scale.y  = 0.5f;
					}
				}
				
			}

			//处理窗口事件
		//	handleEvent(e);
		}

		//仅在未最小化时绘制
		//if (!mMinimized)
		{
			//清屏
			SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
			SDL_RenderClear(render);
			 
			position.x += Vel.x;
			position.y += Vel.y;
			angle.x += rVal.x;
			angle.y += rVal.y;
 
   			transform = IDENTITY;
   			rotateVal =  IDENTITY ;
//#define  MOTH	

#ifndef MOTH		
   
   		 	transform = transform.createScale(scale.x, scale.y);
   
   			Mat3 nm = Mat3::createTranslation(position.x, position.y); 
    
   			 
   			rotateVal = rotateVal.createRotationSDL(center, angle);
   
   			transform = nm *  transform *rotateVal ;
    
   
   			memset(verts_tmp, 0,  4 * sizeof(SDL_Vertex));
   			memcpy(verts_tmp, verts, 4 * sizeof(SDL_Vertex));
   
   			for (int i = 0; i < 4; ++i)
   			{
   				transform.out(verts_tmp[i].position.x, verts_tmp[i].position.y);
   			} 
   	
   			SDL_RenderGeometry(render, texture, verts_tmp,  4, indices,  6);
#else
			 
			//----------------------
			//|			20           |
			//|20|------------- |20 |
			//|  |              |   |
			//|  |              |   |
			//|	   ---------------   |
			//|          20         |
			// ---------------------- 
			SDL_Color color = { 255,255,255,255 };
			verts[0] = {//左上
				{ 0,0 }
				,{color.r,color.g,color.b,color.a}
				,{0,0}
			};
			verts[1] = {//左下
					{  0, fontw}
					,{color.r,color.g,color.b,color.a}
					,{0,1}
			};
			verts[2] = {//右上
				{fontw ,0}
				,{color.r,color.g,color.b,color.a}
				,{1,0}
			};
			verts[3] = {//右下
				{fontw,fonth}
				,{color.r,color.g,color.b,color.a}
				,{1,1}
			};


			 


			//计算旋转
			// 0.01745329252f = 3.14/180
			float radiansX = -(angle.x * 0.01745329252f);
			float radiansY = -(angle.y * 0.01745329252f);
			float rotate_cosX = cosf(radiansX);
			float rotate_sinX = sinf(radiansX);
			float rotate_cosY = cosf(radiansY);
			float rotate_sinY = sinf(radiansY);

			//x = x * cos(θ) - y * sin(θ);
			// y = x * sin(θ) + y * cos(θ);
		 
			verts[0].position = {
						center.x + (verts[0].position.x - center.x) * rotate_cosY - (verts[0].position.y - center.y) * rotate_sinX,
						center.y + (verts[0].position.x - center.x) * rotate_sinY + (verts[0].position.y - center.y) * rotate_cosX
			};

			verts[1].position = {
						center.x + (verts[1].position.x - center.x) * rotate_cosY - (verts[1].position.y - center.y) * rotate_sinX,
						center.y + (verts[1].position.x - center.x) * rotate_sinY + (verts[1].position.y - center.y) * rotate_cosX
			};
			verts[2].position = {
						center.x + (verts[2].position.x - center.x) * rotate_cosY - (verts[2].position.y - center.y) * rotate_sinX,
						center.y + (verts[2].position.x - center.x) * rotate_sinY + (verts[2].position.y - center.y) * rotate_cosX
			};
			verts[3].position = {
						center.x + (verts[3].position.x - center.x) * rotate_cosY - (verts[3].position.y - center.y) * rotate_sinX,
						center.y + (verts[3].position.x - center.x) * rotate_sinY + (verts[3].position.y - center.y) * rotate_cosX
			};
			SDL_RenderGeometry(render, texture, verts, 4, indices, 6);
#endif
			SDL_RenderPresent(render);
		}
	}

	//释放资源并关闭 SDL
	close();

	return 0;
}

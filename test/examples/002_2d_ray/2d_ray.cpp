/**************2d光追(像素操作*********/
/********手机c4droid可直接加载运行**/
/********改写来源js,转译c++/SDL2*****/
/********    by 悠然小赐    *********/
 /********qq:108201645       ******/
 /**********************************/
 /*********************************/
#include<iostream>
#include<cmath>
#include<vector>
#include<random>
#include<ctime>
#include<climits>
using namespace std;
#if defined(__ANDROID__)
#include <SDL2/SDL.h>
#else defined(__GNUG__)||defined(__WIN32)
#include <SDL.h>
#endif

int WIDTH = 400, HEIGHT = 400;

SDL_Window* win;
SDL_Surface* scr;

void init()
{
	SDL_Init(SDL_INIT_VIDEO);
#if defined(__ANDROID__)
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	WIDTH = dm.w, HEIGHT = dm.h;
#endif

	win = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	scr= SDL_GetWindowSurface(win);
}
void close()
{
	SDL_FreeSurface(scr);
	SDL_Quit();
}

	/*布雷森漢姆直線演算法 */
	void draw_line(int x1, int y1, int x2, int y2,int a)
	{
		uint32_t* pixels = (uint32_t*)scr->pixels;
		int incx = 1, incy = 1;

		int dx = x2 - x1;
		int dy = y2 - y1;
		if (dx < 0)
			dx = -dx;
		if (dy < 0)
			dy = -dy;

		if (x2 < x1)
			incx = -1;
		if (y2 < y1)
			incy = -1;

		int x = x1;
		int y = y1;

		if (dx > dy)
		{
			// Slope less than 1
			if (!(x + y * WIDTH < 0 || x + y * WIDTH >= WIDTH * HEIGHT))
			pixels[x + y * WIDTH] = a;
			int e = 2 * dy - dx;
			for (int i = 0; i < dx; i++)
			{
				if (e >= 0)
				{
					y += incy;
					e += 2 * (dy - dx);
				}
				else
				{
					e += 2 * dy;
				}
				x += incx;
				if (!(x + y * WIDTH < 0 || x + y *WIDTH >= WIDTH * HEIGHT))
				pixels[x + y * WIDTH] = a;
			}

		}
		else
		{
			// Slope greater than 1
			if (!(x + y * WIDTH < 0 || x + y * WIDTH >= WIDTH * HEIGHT))
			pixels[x + y * WIDTH] = a;
			int e = 2 * dx - dy;
			for (int i = 0; i < dy; i++)
			{
				if (e >= 0)
				{
					x += incx;
					e += 2 * (dx - dy);;
				}
				else
					e += 2 * dx;
				y += incy;
				if (!(x + y * WIDTH < 0 || x + y * WIDTH >= WIDTH * HEIGHT))
				pixels[x + y * WIDTH] = a;
			}
		}
	}
		void drawCircle(int x,int y,int radius){
		uint32_t* pixels = (uint32_t*)scr->pixels;
	for (int w = 0; w < radius * 2; w++)
		{
			for (int h = 0; h < radius * 2; h++)
			{
				int dx = radius - w; // horizontal offset
				int dy = radius - h; // vertical offset
				if ((dx*dx + dy * dy) <= (radius * radius))
				{
					if (!((dx + (int)x) + (dy + (int)y)*WIDTH < 0 ||
						(dx + (int)x) + (dy + (int)y)*WIDTH >= WIDTH * HEIGHT))
						pixels[(dx + (int)x) + (dy + (int)y) * WIDTH] =0x0;
				}
			}
		}
}
struct Point
{
	double x, y;
};
struct Boundary
{
	Point a, b;
	Boundary(double x1, double y1, double x2, double y2)
	{
		a.x = x1, a.y = y1;
		b.x = x2, b.y = y2;
	}

	void show()
	{
		draw_line( a.x, a.y, b.x, b.y,0xff00ffff);
	}
};
struct Ray
{
	Point dir;
	Point &pos, pt;
	Ray(Point &p, double angle):pos(p)
	{
		//pos = p;
		dir = { sin(angle),cos(angle) };
	}

	void lookAt(double x, double y)
	{
		dir.x = x - pos.x;
		dir.y = y - pos.y;
		double dist = sqrt(dir.x * dir.x + dir.y * dir.y);

		//归一化向量
		//return this.dir.normalize();
		dir = { dir.x / dist,dir.y / dist };
	}

	void show()
	{
		// stroke(255);
	
		// push();
		// translate(this.pos.x, this.pos.y);
		//	SDL_Rect r = { pos.x,pos.y,dir.x * 20,dir.y * 20 };
	//	SDL_FillRect(scr,&r,SDL_MapRGB(scr->format,255, 255, 255));
	
		//  pop();
	}

	bool cast(Boundary &wall)
	{
		double x1 = wall.a.x;
		double y1 = wall.a.y;
		double x2 = wall.b.x;
		double y2 = wall.b.y;

		double x3 = pos.x;
		double y3 = pos.y;
		double x4 = pos.x + dir.x;
		double y4 = pos.y + dir.y;

		double den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		if (den == 0)
		{
			return false;
		}

		double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
		double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
		if (t > 0 && t < 1 && u > 0)
		{
			pt.x = x1 + t * (x2 - x1);
			pt.y = y1 + t * (y2 - y1);
			return true;
		}
		else
		{
			return  false;
		}
	}
};
struct Particle
{
	Point pos;
	vector<Ray*>rays;
	Particle()
	{
		pos = { (double)WIDTH / 2,  (double)HEIGHT / 2 };
		// vector<Ray*>rays ;
		for (double a = 0; a < 360; a += 0.2)
		{
			rays.push_back(new Ray(pos, a*3.1415926 / 180));
		}
	}

	void update(double x, double y)
	{
		pos.x = x;
		pos.y = y;
	}

	void  look(vector<Boundary*> &walls)
	{
		for (int i = 0; i < rays.size(); i++)
		{
			Ray ray = *rays[i];
			Point closest = { -1,-1 };
			int record = 100000;
			for (int j=0; j< walls.size();++j)
			{
				Point pt ;
				if (ray.cast(*walls[j]))
				{
					pt = ray.pt;
					//if (pt != NULL)
					{
						double x = (pt.x - pos.x)*(pt.x - pos.x);
						double y = (pt.y - pos.y)*(pt.y - pos.y);
						double d = sqrt(x + y);
						if (d < record)
						{
							record = d;
							closest = pt;
						}
					}
				}
			}
			if (closest.x!=-1&&closest.y!=-1)
			{
				//    stroke(255, 100);
			
				draw_line( pos.x, pos.y, closest.x, closest.y,0xffffffff);
			}
		}
	}

		
	void show()
	{
		//  fill(255);
	//	gCircle.setDimentsion(8, 8);
	drawCircle(pos.x , pos.y ,30);
		//ellipse(this.pos.x, this.pos.y, 4);
		for (auto ray : rays)
		{
			 ray->show();
		}
	}
};
vector<Boundary*> walls;
//Ray ray;
Particle *particle;
	SDL_Point mousePos,clickOffset;
	bool leftMouseButtonDown,selectRect;
void draw(SDL_Event &e)
{
	SDL_FillRect(scr,NULL,SDL_MapRGB(scr->format, 0, 0, 0));

	for (auto &wall : walls)
	{
		wall->show();
	}
	SDL_GetMouseState(&mousePos.x,&mousePos.y);
	if (e.type == SDL_MOUSEBUTTONUP)
	{
		if(leftMouseButtonDown&&selectRect)
		leftMouseButtonDown=selectRect=false;
	}
	else if(e.type==SDL_MOUSEBUTTONDOWN){
		leftMouseButtonDown=true;
		SDL_Rect t={particle->pos.x-30,particle->pos.y-30,60,60};
		if(SDL_PointInRect(&mousePos,&t)){
			clickOffset={(int)(mousePos.x-particle->pos.x),(int)(mousePos.y-particle->pos.y)};
			selectRect=true;
		}
	}
	else if(e.type==SDL_MOUSEMOTION){
		if(selectRect&&leftMouseButtonDown)
		particle->update(mousePos.x-clickOffset.x ,mousePos.y-clickOffset.y);
	}
	//particle->update(20, HEIGHT);
	
	particle->look(walls);
particle->show();
	SDL_UpdateWindowSurface(win);
}
int main(int, char**)
{
	srand((unsigned)time(0));

	init();

	for (int i = 0; i < 5; i++)
	{
		int x1 =  rand() % WIDTH;
		int x2 =  rand() % WIDTH;
		int y1 =  rand() % HEIGHT;
		int y2 = rand() % HEIGHT;
		walls.push_back(new Boundary(x1, y1, x2, y2));
	}
	walls.push_back(new Boundary(0, 0, WIDTH, 0));
	walls.push_back(new Boundary(WIDTH, 0, WIDTH, HEIGHT));
	walls.push_back(new Boundary(WIDTH, HEIGHT, 0, HEIGHT));
	walls.push_back(new Boundary(0, HEIGHT, 0, 0));
	particle = new Particle;
	bool quit = false;
	SDL_Event e;

	while (!quit)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				quit = true;
			draw(e);
		}

	}
	close();

	return 0;
}

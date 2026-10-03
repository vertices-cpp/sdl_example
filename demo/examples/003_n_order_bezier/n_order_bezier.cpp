/**********N阶贝塞尔曲线*********/
/**手机c4droid可直接加载运行**/
/**改写来源js,转译c++/SDL2*****/
/****    by 悠然小赐    *********/
 /****qq:108201645       ******/
 /*****需要球体图片cricle.png********/
 /*需要在sd目录下放置msyh.ttf字体*/
 /**************************/
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include "path_head.h"
using namespace std;

#if defined(_WIN32)||defined(_WIN64)||defined(_MSC_VER)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
const int diffLeft = 30, diffTop = 30;
const int fontsize = 12;
#elif defined(__ANDROID__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
const int diffLeft = 70, diffTop = 70;
const int fontsize = 30;
#endif
int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;

const int SCREEN_TICKS_PER_FRAMES = 1000 / 60;
SDL_Window *gWindow;
SDL_Renderer *gRenderer;
SDL_Texture* gTexture, *gRect;
TTF_Font *gFont;
//屏幕高度


 

SDL_Rect clearScr;

typedef struct _Point {
	double x, y;
}Point;

bool isRect(SDL_Point a, SDL_Rect b) {
	return !(a.x  < b.x || a.x > b.x + b.w || a.y < b.y || a.y > b.y + b.h);
}
//清屏
void clearScreen() {
	//SDL_SetRenderTarget(gRenderer, gRect);
	SDL_SetRenderDrawColor(gRenderer, 192, 192, 192, 192);
	SDL_RenderClear(gRenderer);
	//SDL_SetRenderTarget(gRenderer, NULL);
}

void displaybeg();
void displayend();

SDL_Color TextColor[2] = { { 192,192,192,255 }, { 192,192,192,255 } };
class LTexture {
public:
	LTexture() {
		mTexture = NULL;
		mWidth = mHeight = 0;
	}
	~LTexture() {
		free();
	}
	/* 加载图片 */
	LTexture& loadFromFile(const string &path) {
		free();
		SDL_Surface *load = IMG_Load(path.c_str());
		SDL_SetColorKey(load, SDL_TRUE, SDL_MapRGB(load->format, 0, 0, 0));
		mTexture = SDL_CreateTextureFromSurface(gRenderer, load);
		mWidth = load->w, mHeight = load->h;
		SDL_FreeSurface(load);
		return *this;
	}
	/* 加载文本 */
	void loadFromText(const string &text, SDL_Color color) {
		free();
		SDL_Surface *load = TTF_RenderUTF8_Blended(gFont, text.c_str(), color);
		SDL_SetColorKey(load, SDL_TRUE, SDL_MapRGB(load->format, 255, 255, 255));
		mTexture = SDL_CreateTextureFromSurface(gRenderer, load);
		mWidth = load->w, mHeight = load->h;
		SDL_FreeSurface(load);

	}
	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }
	LTexture& setDimension(int w, int h) {
		mWidth = w, mHeight = h;
		return *this;
	}
	LTexture& setBlendMode(SDL_BlendMode blending) {
		SDL_SetTextureBlendMode(mTexture, blending);
		return *this;
	}
	LTexture& setAlpha(int a) {
		SDL_SetTextureAlphaMod(mTexture, a);
		return *this;
	}
	LTexture& setColor(int r, int g, int b) {
		SDL_SetTextureColorMod(mTexture, r, g, b);
		return *this;
	}
	/* 渲染 */
	LTexture& render(int x, int y, SDL_Rect *clip = NULL,
		double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) {
		SDL_Rect renderQuad = { x,y,mWidth,mHeight };
		if (clip != NULL)
		{
			// 裁剪贴图
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
		return *this;
	}
	void free() {
		if (mTexture != NULL)
			SDL_DestroyTexture(mTexture);
	}
private:
	SDL_Texture *mTexture;
	int mWidth, mHeight;
};


/* 文本类 */
class Text {
	int mx, my, mWidth, mHeight, selectRect = 0;
public:
	Text() {
	}
	void SetText(const string &str, int x, int y, int w, int h) {
		mx = x, my = y, mWidth = w, mHeight = h;
		os.str("");
		os << " " << str;
	}
	int handleEvent(SDL_Event &e) {
		if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
			SDL_Point mousePos = { e.motion.x,e.motion.y };
			SDL_Rect thisRect = { mx,my,mWidth,mHeight };
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (isRect(mousePos, thisRect))
					selectRect = 1;
				else
					selectRect = 0;
			}
			else if (e.type == SDL_MOUSEBUTTONUP)
				selectRect = 0;
		}
		return selectRect;
	}
	~Text() { os.str(""); }
	void render();
private:
	stringstream os;
};
//必须为double类型
double t = 0;//贝塞尔函数涉及的占比比例，0<=t<=1

vector< Point> clickNodes;  //点击的控制点对象数组
vector< Point> bezierNodes;//绘制内部控制点的数组
bool isPrinted = false;//当前存在绘制的曲线
bool isPrinting = false; //正在绘制中
int num = 0; //控制点数

LTexture gDot, gTextTexture;

Point bezier(vector<Point>&, double);


stringstream out;

Text drawText, clearText, curMouse, winTitle;

struct MouseLButton {
public:
	MouseLButton& handleEvent(SDL_Event &e) {
		if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEMOTION) {
			/* 获取鼠标坐标 */
			double clientX = e.motion.x, clientY = e.motion.y;

			/* 顶部坐标 */

			double x = clientX - diffLeft, y = clientY - diffTop;

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				isDrag = true;
				clickon = SDL_GetTicks();

				for (int index = 0; index < clickNodes.size(); ++index) {
					double absX = abs(clickNodes[index].x - x);
					double absY = abs(clickNodes[index].y - y);
					//判断是否在触点范围，设定拖拽
					if (absX < 10 && absY < 10) {
						isDragNode = true;
						dragIndex = index;
					}
				}
			}
			//鼠标移动
			else if (e.type == SDL_MOUSEMOTION) {
				displaybeg();

				//cout << isDrag << " :" << isDragNode << endl;
				if (isDrag && isDragNode) {

					clickNodes[dragIndex] = { x,y };

					//清屏

					clearScreen();


					for (int index = 0; index < clickNodes.size(); ++index) {
						double x = clickNodes[index].x;
						double y = clickNodes[index].y;
						int i = index + 1;

						//	displaybeg();
							//填充文本
							/*
							"p" + i, x, y + 20
							"p" + i + ': ('+ x +', '+ y +')', 10, i * 20
							*/
						out.str("");
						out << "p" << i;
						gTextTexture.loadFromText(out.str(), { 0x0,0x69,0x0 });
						gTextTexture.render(x, y + 20);

						out.str("");
						out << "p" << i << ":(x:" << x << ",y: " << y << ",angle: ";

						if (!index)
							out << (int)(atan2(y, x) * 180 / 3.14) << u8"° )";
						else
							out << (int)fmod(360 + atan2(y - clickNodes[index - 1].y, x - clickNodes[index - 1].x) * 180 / 3.14, 360) << u8"° )";

						gTextTexture.loadFromText(out.str(), { 0x69,0x69,0x69 });
						gTextTexture.render(10, i * fontsize);

						//绘制圆x,y   '#696969'
						gDot.setColor(0x69, 0x69, 0x69);
						gDot.render(x - gDot.GetWidth() / 2, y - gDot.GetHeight() / 2);


						if (index)
						{
							double startX = clickNodes[index - 1].x;
							double startY = clickNodes[index - 1].y;

							//displaybeg();
							//绘制线startX,startY->x,y
							SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
							SDL_RenderDrawLine(gRenderer, startX, startY, x, y);
							//displayend();
						}
					}
					if (isPrinted) {
						vector<Point> bezierArr;
						for (double i = 0; i < 1; i += 0.01)
						{
							Point a = bezier(clickNodes, i);
							bezierArr.push_back({ a.x, a.y });
						}
						for (int index = 0; index < bezierArr.size(); ++index) {
							if (index)
							{
								double startX = bezierArr[index - 1].x,
									startY = bezierArr[index - 1].y;
								//绘制线startX,startY->x,y
								double	x = bezierArr[index].x,
									y = bezierArr[index].y;

								//displaybeg();
								SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
								SDL_RenderDrawLine(gRenderer, startX, startY, x, y);
								//displayend();
							}
						}
					}
				}
				displayend();
			}
			else if (e.type == SDL_MOUSEBUTTONUP)//鼠标松开
			{
				displaybeg();
				isDrag = false;
				isDragNode = false;
				clickoff = SDL_GetTicks();
				if (clickoff - clickon < 200) {
					if (!isPrinted && !isDragNode) {
						num++;


						//填充文本
							/*
							"p" + num, x, y + 20
							"p" + num + ': ('+ x +', '+ y +')', 10, num * 20
							*/
						out.str("");
						out << "p" << num;
						gTextTexture.loadFromText(out.str(), { 0x0,0x69,0x0 });
						gTextTexture.render(x, y + 20);

						out.str("");
						out << "p" << num << ":(x:" << x << ",y: " << y << ",angle: ";

						if (clickNodes.size() < 1)
							out << (int)(atan2(y, x) * 180 / 3.14) << u8"° )";
						else
							out << (int)fmod(360 + atan2(y - clickNodes[clickNodes.size() - 1].y, x - clickNodes[clickNodes.size() - 1].x) * 180 / 3.14, 360) << u8"° )";

						gTextTexture.loadFromText(out.str(), { 0x69,0x69,0x69 });
						gTextTexture.render(10, num*fontsize);


						//绘制圆x,y   '#696969'
						gDot.setColor(0x69, 0x69, 0x69);
						gDot.render(x - gDot.GetWidth() / 2, y - gDot.GetHeight() / 2);



						if (clickNodes.size())
						{
							double startX = clickNodes[clickNodes.size() - 1].x;
							double startY = clickNodes[clickNodes.size() - 1].y;


							//绘制线startX,startY->x,y
							SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
							SDL_RenderDrawLine(gRenderer, startX, startY, x, y);

						}
						clickNodes.push_back({ x,y });
					}
				}
				displayend();
			}

		}

		return *this;
	}

	//	void drawnode(vector<Point> &nodes);
private:

	bool isDrag = false;//是否进入拖拽行为
	bool isDragNode = false; //是否点击到了控制点
	int dragIndex = 0; //被拖拽的点的索引
	int clickon = 0; //鼠标按下时间戳
	int clickoff = 0; //鼠标抬起
};



void init() {
	SDL_Init(SDL_INIT_VIDEO);
#if defined(__ANDROID__)
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	SCREEN_WIDTH = dm.w, SCREEN_HEIGHT = dm.h;
#endif
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	gWindow = SDL_CreateWindow("Bezier test", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED/*|SDL_RENDERER_PRESENTVSYNC*/);

	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	gFont = TTF_OpenFont(og::checkPath("003_n_order_bezier/msyh.ttf").c_str(), fontsize);
	//SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);

	clearScr = { (int)diffLeft, (int)diffTop, SCREEN_WIDTH - diffLeft * 2, SCREEN_HEIGHT - diffTop * 2 };
	gTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetTextureBlendMode(gTexture, SDL_BLENDMODE_BLEND);
	gRect = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, clearScr.w, clearScr.h);
}
void close() {
	gDot.free();
	gTextTexture.free();
	TTF_CloseFont(gFont);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void loadMedia() {

	//	gDot.loadFromFile("fire.png");
	gDot.loadFromFile(og::checkPath("003_n_order_bezier/circle.png"));
#if defined(__ANDROID__)
	gDot.setDimension(20, 20);
#else
	gDot.setDimension(10, 10);
#endif
}

void Text::render() {
	SDL_Rect r = { mx, my, mWidth, mHeight };
	SDL_SetRenderDrawColor(gRenderer, TextColor[selectRect].r, TextColor[selectRect].g, TextColor[selectRect].b, 255);
	if (selectRect)
		SDL_RenderFillRect(gRenderer, &r);
	else
		SDL_RenderDrawRect(gRenderer, &r);
	//cout << ":" << os.str() << endl;
	gTextTexture.setAlpha(255);
	gTextTexture.loadFromText(os.str().c_str(), { 0,0,192,255 });

	gTextTexture.setDimension(mWidth, mHeight);
	gTextTexture.render(mx, my);
}


void drawnode(vector<Point>& nodes) {
	if (!nodes.size()) return;
	vector <Point>  _nodes = nodes;
	vector <Point>  next_nodes;
	for (int index = 0; index < _nodes.size(); ++index) {

		double x = _nodes[index].x,
			y = _nodes[index].y;

		//displaybeg();
		if (_nodes.size() == num) {

			int i = index + 1;


			//填充文本
					/*
					"p" + i, x, y + 20
					"p" + i + ': ('+ x +', '+ y +')', 10, i * 20
					*/
			out.str("");
			out << "p" << i;
			gTextTexture.loadFromText(out.str(), { 0x0,0x69,0 });
			gTextTexture.render(x, y + 20);

			out.str("");
			out << "p" << i << ":(x:" << x << ",y: " << y << ",angle: ";

			if (!index)
			out << (int)(atan2(y, x) * 180 / 3.14) << u8"° )";
			else
			out << (int)fmod(360 + atan2(y - _nodes[index - 1].y, x - _nodes[index - 1].x) * 180 / 3.14, 360) << u8"° )";

			gTextTexture.loadFromText(out.str(), { 0x69,0x69,0x69 });
			gTextTexture.render(10, i*fontsize);


		}
		//绘制圆x,y   '#696969'
		gDot.setColor(0, 128, 192);
		gDot.render(x - gDot.GetWidth() / 2, y - gDot.GetHeight() / 2);

		//displayend();

		if (_nodes.size() == 1) {
			bezierNodes.push_back(_nodes[index]);
			if (bezierNodes.size() > 1) {
				for (size_t i = 0; i < bezierNodes.size(); ++i) {
					if (i) {
						double startX = bezierNodes[i - 1].x,
							startY = bezierNodes[i - 1].y,
							x = bezierNodes[i].x,
							y = bezierNodes[i].y;

						//displaybeg();
						//绘制线startX,startY->x,y
						SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
						SDL_RenderDrawLine(gRenderer, startX, startY, x, y);
						//displayend();
					}
				}
			}
		}

		if (index) {
			double startX = _nodes[index - 1].x,
				startY = _nodes[index - 1].y;

			//	绘制线startX,startY->x,y
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
			SDL_RenderDrawLine(gRenderer, startX, startY, x, y);

		}
	}
	if (_nodes.size()) {
		for (int i = 0; i < _nodes.size() - 1; i++) {
			vector<Point> arr;
			arr.push_back(_nodes[i]);
			arr.push_back(_nodes[i + 1]);
			next_nodes.push_back(bezier(arr, t));
		}
		drawnode(next_nodes);
	}

}

void drawBezier(vector<Point> &origin_nodes) {
	if (t > 1) {
		isPrinting = false;
		return;
	}
	isPrinting = true;
	auto nodes = origin_nodes;
	t += 0.01;
	//清屏
	displaybeg();
	clearScreen();

	drawnode(nodes);

	displayend();

	drawBezier(nodes);

}
//必须为double类型
double factorial(double num) { //递归阶乘
	if (num <= 1) {
		return 1;
	}
	else {
		return num * factorial(num - 1);
	}
}
Point bezier(vector<Point> &arr, double t) { //通过各控制点与占比t计算当前贝塞尔曲线上的点坐标
	double x = 0,
		y = 0;
	int n = arr.size() - 1;
	for (int index = 0; index < arr.size(); index++) {

		if (!index) {
			x += (arr[index].x * pow((1 - t), n - index) /** pow(t, index)*/);
			y += (arr[index].y * pow((1 - t), n - index) /** pow(t, index)*/);
			
		}
		else {
			double a = factorial(n);
			double b = factorial(index);
			double c = factorial(n - index);
		//	cout << "n = " << n << " , index = " << index << " , a = " << a << " , b = " << b << " ,c = " << c << endl;
			x += (a / b / c * arr[index].x * pow((1 - t), n - index) * pow(t, index));
			y += (a / b / c * arr[index].y * pow((1 - t), n - index) * pow(t, index));
		}
			
	}
	cout << "............." << endl;
	return { x,y };
}

//3)B(t)=(1−t)³P₀+3t(1−t)²P₁+3t²P₂(1−t)+t³P₃,t∈[0,1]
void clickDraw() {
	if (!num) return;
	if (!isPrinting) {
		isPrinted = true;
		drawBezier(clickNodes);
	}
}
void clickClear() {
	if (!isPrinting) {

		isPrinted = false;
		clearScreen();
		clickNodes.clear();
		bezierNodes.clear();

		t = 0;
		num = 0;
	}
}
//绘制屏幕按钮

void DrawScreenButton() {
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderClear(gRenderer);
	drawText.render();
	clearText.render();
	curMouse.render();
	winTitle.render();
}
int main(int argc, char *argv[]) {

	init();
	loadMedia();

	MouseLButton mouseButton;

	drawText.SetText(u8"绘制", 0, SCREEN_HEIGHT - diffTop, diffLeft, diffTop);
	clearText.SetText(u8"清除", diffLeft, SCREEN_HEIGHT - diffTop, diffLeft, diffTop);
	curMouse.SetText(u8"外窗坐标: x:" + to_string(0) + " y:" + to_string(0) +
		u8" 内窗坐标: x:" + to_string(0) + " y:" + to_string(0), diffLeft * 2, SCREEN_HEIGHT - diffTop, diffLeft * 10, diffTop);
	winTitle.SetText(u8"N阶贝塞尔测试", (SCREEN_WIDTH / 2 - diffLeft * 3) / 2, 0, diffLeft * 3, diffTop);

	bool quit = false;
	SDL_Event e;


	SDL_SetRenderTarget(gRenderer, gRect);
	clearScreen();
	SDL_SetRenderTarget(gRenderer, NULL);

	int  capTimer = 0;

	while (!quit)
	{
		capTimer = SDL_GetTicks();

		SDL_WaitEvent(&e);

		if (e.type == SDL_QUIT)
			quit = true;
		else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEMOTION)
		{
			SDL_Point mousePos = { e.motion.x,e.motion.y };
			curMouse.SetText(u8"外窗坐标: x:" + to_string(mousePos.x) + " y:" + to_string(mousePos.y) +
				u8" 内窗坐标: x:" + to_string(mousePos.x - diffLeft) + " y:" + to_string(mousePos.y - diffTop), diffLeft * 2, SCREEN_HEIGHT - diffTop, diffLeft * 10, diffTop);
			if (SDL_PointInRect(&mousePos, &clearScr)) {

				mouseButton.handleEvent(e);
			}
			else {
				SDL_SetRenderTarget(gRenderer, gRect);
				if (drawText.handleEvent(e))
					clickDraw();
				else if (clearText.handleEvent(e))
					clickClear();
				SDL_SetRenderTarget(gRenderer, NULL);
			}
		}

		clearScreen();
		SDL_SetRenderTarget(gRenderer, gTexture);

		DrawScreenButton();
		SDL_RenderCopy(gRenderer, gRect, NULL, &clearScr);
		SDL_SetRenderTarget(gRenderer, NULL);

		SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
		SDL_RenderPresent(gRenderer);

		double frameTicks = SDL_GetTicks() - capTimer;
		if (frameTicks < SCREEN_TICKS_PER_FRAMES)
			SDL_Delay(SCREEN_TICKS_PER_FRAMES - frameTicks);

	}

	close();

	return 0;
}
void displaybeg() {
	SDL_SetRenderTarget(gRenderer, gRect);
}
void displayend() {
	SDL_SetRenderTarget(gRenderer, NULL);

	SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
	SDL_RenderCopy(gRenderer, gRect, NULL, &clearScr);
	SDL_RenderPresent(gRenderer);
}

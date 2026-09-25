#ifndef _TEXTURE_2D_H_
#define _TEXTURE_2D_H_

//#include "PlatformMacros.h"

#include "SDLView.h"
#include "OGRef.h"
#include <algorithm>

OG_BEGIN


 
 template<class _Ty = float>  
 	void og_swap(_Ty& _Left, _Ty& _Right)
 {	 
 	_Ty _Tmp =  _Left ;
 	_Left =  _Right ;
 	_Right =  _Tmp;
 }
  
 struct  TmpQuad {
 	SDL_Vertex m[4];
 	const int index[6] = { 0,1,2,1,2,3 };
 
 public:
	 TmpQuad() {}
 	//包含纹理，原图大小，当前源/裁剪大小，将渲染目标大小，缩放值，旋转，旋转参照点，颜色，翻转
 	void TextureCvRenderer(SDL_Texture* texture,const  Size &imageSize,
 		const  Rect &clip, const  Rect& dest,const  Vec2& rotate, const  Vec2 &pointer, const Color4B& color, float flipX, float flipY)
 	{
 
 		auto len = sizeof(m);
 		memset(m, 0,len);
 		//裁剪位置
 		float uvX = clip.origin.x / imageSize.width;
 		float uvY = clip.origin.y / imageSize.height;
 		float uvW = (clip.origin.x + clip.size.width) / imageSize.width;
 		float uvH = (clip.origin.y + clip.size.height) / imageSize.height;
  //----------------------
  //|			20           |
  //|20|------------- |20 |
  //|  |              |   |
  //|  |              |   |
  //|	   ---------------   |
  //|          20         |
  // ---------------------- 
 
 		m[0] = {//左上
 			{ dest.origin.x,dest.origin.y }
 			,{color.r,color.g,color.b,color.a}
 			,{uvX,uvY}
 		};
 		m[1] = {//左下
 				{dest.origin.x ,dest.origin.y + dest.size.height}
 				,{color.r,color.g,color.b,color.a}
 				,{uvX,uvH}
 		};
 		m[2] = {//右上
 			{dest.origin.x + dest.size.width ,dest.origin.y}
 			,{color.r,color.g,color.b,color.a}
 			,{uvW,uvY}
 		};
 		m[3] = {//右下
 			{dest.origin.x + dest.size.width ,dest.origin.y + dest.size.height}
 			,{color.r,color.g,color.b,color.a}
 			,{uvW,uvH}
 		};
 
 
 		//采样顺序翻转
 		if (flipX)
 		{
 			og_swap(m[0].tex_coord.x, m[2].tex_coord.x);
  			//og_swap(m[0].tex_coord.y, m[2].tex_coord.y);
  			og_swap(m[1].tex_coord.x, m[3].tex_coord.x);
 			//og_swap(m[1].tex_coord.y, m[3].tex_coord.y);
 		}
 		if (flipY)
 		{
 			//og_swap(m[0].tex_coord.x, m[1].tex_coord.x);
 			og_swap(m[0].tex_coord.y, m[1].tex_coord.y);
 			//og_swap(m[2].tex_coord.x, m[3].tex_coord.x);
 			og_swap(m[2].tex_coord.y, m[3].tex_coord.y);
 		}
 
 
 		//计算旋转
 		// 0.01745329252f = 3.14/180
 		float radiansX = -(rotate.x * 0.01745329252f);
 		float radiansY = -(rotate.y * 0.01745329252f);
 		float rotate_cosX = cosf(radiansX);
 		float rotate_sinX = sinf(radiansX);
 		float rotate_cosY = cosf(radiansY);
 		float rotate_sinY = sinf(radiansY);
 
 		//x = x * cos(θ) - y * sin(θ);
 		// y = x * sin(θ) + y * cos(θ);
 
   		m[0].position = {
   					pointer.x + (m[0].position.x - pointer.x) * rotate_cosY - (m[0].position.y - pointer.y) * rotate_sinX,
   					pointer.y + (m[0].position.x - pointer.x) * rotate_sinY + (m[0].position.y - pointer.y) * rotate_cosX
   		};
   
   		m[1].position = {
   					pointer.x + (m[1].position.x - pointer.x) * rotate_cosY - (m[1].position.y - pointer.y) * rotate_sinX,
   					pointer.y + (m[1].position.x - pointer.x) * rotate_sinY + (m[1].position.y - pointer.y) * rotate_cosX
   		};
   		m[2].position = {
   					pointer.x + (m[2].position.x - pointer.x) * rotate_cosY - (m[2].position.y - pointer.y) * rotate_sinX,
   					pointer.y + (m[2].position.x - pointer.x) * rotate_sinY + (m[2].position.y - pointer.y) * rotate_cosX
   		};
   		m[3].position = {
   					pointer.x + (m[3].position.x - pointer.x) * rotate_cosY - (m[3].position.y - pointer.y) * rotate_sinX,
   					pointer.y + (m[3].position.x - pointer.x) * rotate_sinY + (m[3].position.y - pointer.y) * rotate_cosX
   		};
   
 
 		SDL_RenderGeometry(SDLView::getInstance()->getRender(), texture, m, 4, index, 6);
 	}
 
 };
 
 static TmpQuad tmpQuad;
  
 namespace backend
 {
	 enum class PixelFormat : uint32_t
	 {
		 AUTO = SDL_PIXELFORMAT_UNKNOWN,
		 RGBA8888 = SDL_PIXELFORMAT_ABGR8888,
		 BGRA8888 = SDL_PIXELFORMAT_ARGB8888,
		 RGB888 = SDL_PIXELFORMAT_RGB24,
		 RGB565 = SDL_PIXELFORMAT_RGB565,
		 RGBA4444 = SDL_PIXELFORMAT_ABGR4444,
		 A8 = SDL_PIXELFORMAT_INDEX8
	 };
 }

 typedef struct _MipmapInfo
 {
	 unsigned char* address;
	 int len;
	 _MipmapInfo() :address(NULL), len(0) {}
 }MipmapInfo;
 typedef struct _MipmapInfo MipmapInfo;


 static Uint32 toSDLPixelFormat(backend::PixelFormat fmt)
 {
	 switch (fmt)
	 {
	 case backend::PixelFormat::RGBA8888: return SDL_PIXELFORMAT_ABGR8888;
	 case backend::PixelFormat::BGRA8888: return SDL_PIXELFORMAT_ARGB8888;
	 case backend::PixelFormat::RGB888:   return SDL_PIXELFORMAT_RGB24;
	 case backend::PixelFormat::RGB565:   return SDL_PIXELFORMAT_RGB565;
	 case backend::PixelFormat::A8: return SDL_PIXELFORMAT_INDEX8;
	 case backend::PixelFormat::RGBA4444: return SDL_PIXELFORMAT_ABGR4444;
	 default:                    return SDL_PIXELFORMAT_UNKNOWN;
	 }

 };
class Texture2D :public Ref
{
	//std::string Name;
	SDL_Texture *_texture = nullptr;
	Size _contentSize;
	int mPitch;
	void* mPixels;
	bool _hasPremultipliedAlpha;

	bool _antialiasEnabled;
	int         _pixelsWide = 0;
	int         _pixelsHigh = 0;
	backend::PixelFormat _pixelFormat = backend::PixelFormat::RGBA8888;
public:
	void setMultipliedAlpha(bool malpha) {
		_hasPremultipliedAlpha = malpha;
	}
	bool multipliedAlpha() const {
		return _hasPremultipliedAlpha;
	}
	Texture2D();
	~Texture2D();
	void free();
	void setRGBA(Color4B color);
	void setColor(Uint8 r, Uint8 g, Uint8 b);
	void SetTextureAlphaMod(Uint8 a);
	
	void setBlendMode(SDL_BlendMode blending = SDL_BLENDMODE_BLEND);
  
	//void loadFont(TTF_Font * ttf, const char * text, Color4B color);

	SDL_Texture* getTexture() { return _texture; }
	Size getContentSize() { return _contentSize; }
	bool initWithMipmaps(MipmapInfo * mipmaps, int mipmapsNum, backend::PixelFormat pixelFormat, int pixelsWide, int pixelsHigh);
	void loadFrom(const char *fileName);
	bool loadMemData(unsigned char * data,int len);

	bool createTarget(float w, float h);

	void createTexture(Texture2D* texture,const Rect &r);
 	 void render(const Rect & clip, const Rect & dest,  Vec2 rotate, const Vec2 & pointer, const Color4B &color, bool flipX, bool flipY );

private:
	Texture2D(const Texture2D &) = delete;
	Texture2D &operator =(const Texture2D &) = delete;
};


OG_END

#endif
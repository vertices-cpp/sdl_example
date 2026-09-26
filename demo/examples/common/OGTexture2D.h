#ifndef _TEXTURE_2D_H_
#define _TEXTURE_2D_H_

//#include "PlatformMacros.h"

#include "OGRef.h"
#include "ogTypes.h"
#include "OGGeometry.h"
#include "MathBase.h"
#include "SDLView.h"

#include <algorithm>

OG_BEGIN



enum class BlendFactor :uint32_t {
	ZERO = SDL_BLENDFACTOR_ZERO,
	ONE = SDL_BLENDFACTOR_ONE,
	SRC_COLOR = SDL_BLENDFACTOR_SRC_COLOR,
	ONE_MINUS_SRC_COLOR = SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
	SRC_ALPHA = SDL_BLENDFACTOR_SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA = SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
	DST_COLOR = SDL_BLENDFACTOR_DST_COLOR,
	ONE_MINUS_DST_COLOR = SDL_BLENDFACTOR_ONE_MINUS_DST_COLOR,
	DST_ALPHA = SDL_BLENDFACTOR_DST_ALPHA,
	ONE_MINUS_DST_ALPHA = SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA
};
// 定义自定义的BlendOperation枚举
enum class BlendOperation {
	ADD = SDL_BLENDOPERATION_ADD,
	SUBTRACT = SDL_BLENDOPERATION_SUBTRACT,
	REVERSE_SUBTRACT = SDL_BLENDOPERATION_REV_SUBTRACT,
	MINIMUM = SDL_BLENDOPERATION_MINIMUM,
	MAXIMUM = SDL_BLENDOPERATION_MAXIMUM

};


// 定义BlendFactor枚举到字符串的映射
extern std::map<BlendFactor, std::string> blendFactorToString;

// 定义BlendOperation枚举到字符串的映射
extern std::map<BlendOperation, std::string> blendOperationToString;

class BlendFunc {
public:
	BlendFactor src;
	BlendFactor dst;



	/** Blending disabled. Uses {BlendFactor::ONE, BlendFactor::ZERO} */
	static const BlendFunc DISABLE;
	/** Blending enabled for textures with Alpha premultiplied. Uses {BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA} */
	static const BlendFunc ALPHA_PREMULTIPLIED;
	/** Blending enabled for textures with Alpha NON premultiplied. Uses {BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA} */
	static const BlendFunc ALPHA_NON_PREMULTIPLIED;
	/** Enables Additive blending. Uses {BlendFactor::SRC_ALPHA, BlendFactor::ONE} */
	static const BlendFunc ADDITIVE;

	SDL_BlendMode blendMode;

	BlendFunc() : src(BlendFactor::ZERO), dst(BlendFactor::ZERO), blendMode(SDL_BLENDMODE_NONE) {}
	// 添加接受两个参数的构造函数
	BlendFunc(BlendFactor s, BlendFactor d) : src(s), dst(d), blendMode(SDL_BLENDMODE_NONE) {
		setBlendMode(src, dst);
	}
	// 计算混合模式
// 	void calculateBlendMode() {
// 		blendMode = SDL_ComposeCustomBlendMode(
// 			static_cast<SDL_BlendFactor>(src),
// 			static_cast<SDL_BlendFactor>(dst),
// 			static_cast<SDL_BlendOperation>(BlendOperation::ADD),
// 			static_cast<SDL_BlendFactor>(BlendFactor::SRC_ALPHA),
// 			static_cast<SDL_BlendFactor>(BlendFactor::ONE_MINUS_SRC_ALPHA),
// 			static_cast<SDL_BlendOperation>(BlendOperation::ADD)
// 		);
// 	}

	// 获取 SDL_BlendMode
	SDL_BlendMode getBlendMode() const {
		return blendMode;
	}

	// 设置 SDL_BlendMode
	void setBlendMode(SDL_BlendMode mode) {
		blendMode = mode;
	}

	// 单独设置 SDL_ComposeCustomBlendMode
	void setBlendMode(BlendFactor srcColorFactor,
		BlendFactor dstColorFactor,
		BlendOperation colorOperation,
		BlendFactor srcAlphaFactor,
		BlendFactor dstAlphaFactor,
		BlendOperation alphaOperation) {

		blendMode = SDL_ComposeCustomBlendMode(
			static_cast<SDL_BlendFactor>(srcColorFactor),
			static_cast<SDL_BlendFactor>(dstColorFactor),
			static_cast<SDL_BlendOperation>(colorOperation),
			static_cast<SDL_BlendFactor>(srcAlphaFactor),
			static_cast<SDL_BlendFactor>(dstAlphaFactor),
			static_cast<SDL_BlendOperation>(alphaOperation)
		);
		// 		std::cout << "Custom Blend Mode Set:" << std::endl;
		// 		std::cout << "Src Color Factor: " << blendFactorToString[srcColorFactor] << std::endl;
		// 		std::cout << "Dst Color Factor: " << blendFactorToString[dstColorFactor] << std::endl;
		// 		std::cout << "Color Operation: " << blendOperationToString[colorOperation] << std::endl;
		// 		cout << SDL_GetError() << endl;
		// 		SDL_ClearError();
		// 		std::cout << "Src Alpha Factor: " << blendFactorToString[srcAlphaFactor] << std::endl;
		// 		std::cout << "Dst Alpha Factor: " << blendFactorToString[dstAlphaFactor] << std::endl;
		// 		std::cout << "Alpha Operation: " << blendOperationToString[alphaOperation] << std::endl;
		// 		cout << SDL_GetError() << endl;
		// 		SDL_ClearError();
	}
	// 新增接受两个参数的函数
	void setBlendMode(BlendFactor srcColorFactor, BlendFactor dstColorFactor) {
		// 调用 setCustomBlendMode 并使用默认的颜色和 Alpha 操作
		setBlendMode(srcColorFactor, dstColorFactor, BlendOperation::ADD,
			//OpenGL 默认行为：若仅设置颜色混合因子，Alpha 混合因子会与颜色相同。
			//与 SDL 的差异：SDL 的 SDL_BLENDMODE_BLEND 要求 Alpha 源因子为 ONE，而 OpenGL 默认会使用
			BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendOperation::ADD);
	}
};


template<class _Ty = float>
void og_swap(_Ty& _Left, _Ty& _Right)
{
	_Ty _Tmp = _Left;
	_Left = _Right;
	_Right = _Tmp;
}

struct  TmpQuad {
	SDL_Vertex m[4];
	const int index[6] = { 0,1,2,1,2,3 };

public:
	TmpQuad() {}
	//包含纹理，原图大小，当前源/裁剪大小，将渲染目标大小，缩放值，旋转，旋转参照点，颜色，翻转
	void TextureCvRenderer(SDL_Texture* texture, const  Size &imageSize,
		const  Rect &clip, const  Rect& dest, const  Vec2& rotate, const  Vec2 &pointer, const Color4B& color, float flipX, float flipY)
	{

		auto len = sizeof(m);
		memset(m, 0, len);
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
	// 	 enum class PixelFormat : uint32_t
	// 	 {
	// 		 RGBA8888 = SDL_PIXELFORMAT_ABGR8888,
	// 		 BGRA8888 = SDL_PIXELFORMAT_ARGB8888,
	// 		 RGB888 = SDL_PIXELFORMAT_RGB24,
	// 		 RGB565 = SDL_PIXELFORMAT_RGB565,
	// 		 RGBA4444 = SDL_PIXELFORMAT_ABGR4444,
	// 		 A8 = SDL_PIXELFORMAT_ABGR8888,  // ★ 展开后的目标格式
	// 		 AI88 = SDL_PIXELFORMAT_ABGR8888,   // ★ 展开后的目标格式
	// 	// default:                             return SDL_PIXELFORMAT_UNKNOWN;
	// 	 };
	enum class PixelFormat : uint32_t
	{
		AUTO = 0,
		RGBA8888 = 1,
		BGRA8888 = 2,
		RGB888 = 3,
		RGB565 = 4,
		RGBA4444 = 5,
		A8 = 6,
		AI88 = 7,
	};
}


static Uint32 toSDLPixelFormat(backend::PixelFormat fmt)
{
	switch (fmt)
	{
	case backend::PixelFormat::RGBA8888: return SDL_PIXELFORMAT_ABGR8888;
	case backend::PixelFormat::BGRA8888: return SDL_PIXELFORMAT_ARGB8888;
	case backend::PixelFormat::RGB888:   return SDL_PIXELFORMAT_RGB24;
	case backend::PixelFormat::RGB565:   return SDL_PIXELFORMAT_RGB565;
	case backend::PixelFormat::RGBA4444: return SDL_PIXELFORMAT_ABGR4444;
	case backend::PixelFormat::A8:       return SDL_PIXELFORMAT_ABGR8888;   // ★ 展开后的目标格式
	case backend::PixelFormat::AI88:     return SDL_PIXELFORMAT_ABGR8888;   // ★ 展开后的目标格式
	default:                             return SDL_PIXELFORMAT_UNKNOWN;
	}
}

typedef struct _MipmapInfo
{
	unsigned char* address;
	int len;
	_MipmapInfo() :address(NULL), len(0) {}
}MipmapInfo;
typedef struct _MipmapInfo MipmapInfo;



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
	void setAntiAliasTexParameters();
	void setAliasTexParameters();
	bool  updateWithData(void* data, int x, int y, int w, int h);
	bool initWithData(const void * data, size_t dataLen, backend::PixelFormat pixelFormat, int pixelsWide, int pixelsHigh, const Size & contentSize);
	bool initWithMipmaps(MipmapInfo * mipmaps, int mipmapsNum, backend::PixelFormat pixelFormat, int pixelsWide, int pixelsHigh);
	void loadFrom(const char *fileName);
	bool loadMemData(unsigned char * data, int len);

	bool createTarget(float w, float h);

	void createTexture(Texture2D* texture, const Rect &r);
	void render(const Rect & clip, const Rect & dest, Vec2 rotate, const Vec2 & pointer, const Color4B &color, bool flipX, bool flipY);

private:
	Texture2D(const Texture2D &) = delete;
	Texture2D &operator =(const Texture2D &) = delete;
};


OG_END

#endif
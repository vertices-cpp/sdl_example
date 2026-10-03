// 只在一个 cpp 里定义 STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "OGTexture2D.h"
#include "SDLView.h"

#ifndef OG_ENABLE_PREMULTIPLIED_ALPHA
# define OG_ENABLE_PREMULTIPLIED_ALPHA 1
#endif
#include <string>
#include <map>

//#include "Texture2DManage.h"

//#define TRANSPRENT_BLACK


#include <ft2build.h>
#include FT_FREETYPE_H

OG_BEGIN



// 定义BlendFactor枚举到字符串的映射
std::map<BlendFactor, std::string> blendFactorToString = {
  {BlendFactor::ZERO, "ZERO"},
  {BlendFactor::ONE, "ONE"},
  {BlendFactor::SRC_COLOR, "SRC_COLOR"},
  {BlendFactor::ONE_MINUS_SRC_COLOR, "ONE_MINUS_SRC_COLOR"},
  {BlendFactor::SRC_ALPHA, "SRC_ALPHA"},
  {BlendFactor::ONE_MINUS_SRC_ALPHA, "ONE_MINUS_SRC_ALPHA"},
  {BlendFactor::DST_COLOR, "DST_COLOR"},
  {BlendFactor::ONE_MINUS_DST_COLOR, "ONE_MINUS_DST_COLOR"},
  {BlendFactor::DST_ALPHA, "DST_ALPHA"},
  {BlendFactor::ONE_MINUS_DST_ALPHA, "ONE_MINUS_DST_ALPHA"}
};

// 定义BlendOperation枚举到字符串的映射
std::map<BlendOperation, std::string> blendOperationToString = {
  {BlendOperation::ADD, "ADD"},
  {BlendOperation::SUBTRACT, "SUBTRACT"},
  {BlendOperation::REVERSE_SUBTRACT, "REVERSE_SUBTRACT"},
  {BlendOperation::MINIMUM, "MINIMUM"},
  {BlendOperation::MAXIMUM, "MAXIMUM"}
};


const BlendFunc BlendFunc::DISABLE = { BlendFactor::ONE, BlendFactor::ZERO };
const BlendFunc BlendFunc::ALPHA_PREMULTIPLIED = { BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA };
const BlendFunc BlendFunc::ALPHA_NON_PREMULTIPLIED = { BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA };
const BlendFunc BlendFunc::ADDITIVE = { BlendFactor::SRC_ALPHA, BlendFactor::ONE };


Texture2D::Texture2D()
	: _texture(nullptr)
	, _contentSize(Size::ZERO)
	, mPitch(0)
	, mPixels(nullptr)
	, _hasPremultipliedAlpha(false)   // ★ 加这个
	, _antialiasEnabled(true)
{
}


Texture2D::~Texture2D()
{
	free();
}

void Texture2D::free()
{
	if (_texture != nullptr)
	{
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
	}
}
void Texture2D::setRGBA(Color4B color)
{
	//调制纹理rgb
	SDL_SetTextureColorMod(_texture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(_texture, color.a);
}

void Texture2D::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//调制纹理rgb
	SDL_SetTextureColorMod(_texture, red, green, blue);
}
void Texture2D::SetTextureAlphaMod(Uint8 a) {
	SDL_SetTextureAlphaMod(_texture, a);
}

void Texture2D::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(_texture, blending);
}

// ============================================================
// Texture2D::initWithString
// 用 FreeType 把整段文字渲染成一张 RGBA 纹理
// ============================================================
// 注意：本函数生成的纹理是预乘 alpha 的，
// 调用方必须用 BlendFunc::ALPHA_PREMULTIPLIED
bool Texture2D::initWithString(const char* text, const FontDefinition& textDefinition)
{
	if (!text || strlen(text) == 0) return false;

	free();

	// ============================================================
	// 0. UTF-8 → UTF-32
	// ============================================================
	std::string utf8Text(text);
	std::u32string utf32 = iconv_wrapper_to_utf32le(utf8Text, Utf8ToUtf32le);
	if (utf32.empty()) return false;

	// ============================================================
	// 1. 打开字体（static FT_Library，只初始化一次）
	// ============================================================
	static FT_Library s_ftLib = nullptr;
	if (!s_ftLib) {
		if (FT_Init_FreeType(&s_ftLib)) return false;
	}

	FT_Face face = nullptr;
	if (textDefinition._fontData != nullptr && textDefinition._fontDataSize > 0) {
		if (FT_New_Memory_Face(s_ftLib, (const FT_Byte*)textDefinition._fontData, textDefinition._fontDataSize, 0, &face))
		{
			return false;
		}
	}
	else if (FT_New_Face(s_ftLib, textDefinition._fontName.c_str(), 0, &face)) {
		return false;
	}

	// RAII：保证 face 一定释放
	struct FaceGuard {
		FT_Face f;
		~FaceGuard() { if (f) FT_Done_Face(f); }
	} faceGuard{ face };

	// 字号（26.6 定点数）
	FT_F26Dot6 fontSize26_6 = (FT_F26Dot6)(textDefinition._fontSize * 64.0f);
	if (FT_Set_Char_Size(face, 0, fontSize26_6, 72, 72)) {
		return false;
	}

	// 字体度量
	int ascender = face->size->metrics.ascender >> 6;   // 向上为正
	int descender = face->size->metrics.descender >> 6;   // 向下为负
	int lineHeight = ascender - descender;

	// ============================================================
	// 2. 单遍排版 + 渲染
	//    一次 FT_Load_Char(FT_LOAD_RENDER)，
	//    既拿 metrics 又拿 bitmap，bitmap 直接拷进 GlyphInfo
	// ============================================================
	struct GlyphInfo {
		char32_t cp = 0;
		int x = 0;              // bitmap 左上角 x（相对行首）
		int y = 0;              // bitmap 左上角 y（相对图片顶部）
		int w = 0;
		int h = 0;
		std::vector<uint8_t> bitmap;   // A8 位图
	};
	std::vector<GlyphInfo> glyphs;
	glyphs.reserve(utf32.size());

	// ★ 换行宽度：用显式标志，避免魔法数 1e9f
	const bool  wrapEnabled = textDefinition._enableWrap
		&& textDefinition._dimensions.width > 0;
	const float maxWidth = textDefinition._dimensions.width;

	float penX = 0;
	float penY = 0;          // 当前行顶部 y（相对图片顶部）
	float longestLine = 0;

	// ★ 显式指定 TARGET_NORMAL，保证出 A8 单通道位图
	const FT_Int32 loadFlags = FT_LOAD_RENDER
		| FT_LOAD_NO_AUTOHINT
		| FT_LOAD_TARGET_NORMAL;

	for (size_t i = 0; i < utf32.size(); ++i) {
		char32_t cp = utf32[i];

		// 换行符
		if (cp == '\n') {
			longestLine = std::max(longestLine, penX);
			penX = 0;
			penY += lineHeight;
			continue;
		}

		// 一次加载：既出 metrics，也出 bitmap
		if (FT_Load_Char(face, cp, loadFlags)) {
			penX += 8;
			continue;
		}

		auto& m = face->glyph->metrics;
		auto& bmp = face->glyph->bitmap;

		int adv = m.horiAdvance >> 6;

		// ★ 用 bitmap_left / bitmap_top，而不是 metrics 移位
		//   这两个是 FreeType 专门为位图渲染提供的整数偏移，
		//   与 bmp.width/rows 精确对齐，避免舍入误差
		int bx = face->glyph->bitmap_left;
		int by = face->glyph->bitmap_top;

		int w = bmp.width;
		int h = bmp.rows;

		// 按宽度换行
		if (wrapEnabled && penX + adv > maxWidth && penX > 0) {
			longestLine = std::max(longestLine, penX);
			penX = 0;
			penY += lineHeight;
		}

		GlyphInfo gi;
		gi.cp = cp;
		gi.x = (int)std::round(penX + bx);
		// y 相对图片顶部：字形顶 y = penY + (ascender - by)
		gi.y = (int)std::round(penY + ascender - by);
		gi.w = w;
		gi.h = h;

		// ★ 立即把 bitmap 拷出来
		//   - bmp.pitch 可能 > width（按 4 字节对齐），必须按行拷贝
		//   - bmp.pitch 理论上可能为负（极少见），必须检查，
		//     否则强转 size_t 会变巨值，直接段错误
		if (w > 0 && h > 0 && bmp.pitch > 0) {
			gi.bitmap.resize((size_t)w * (size_t)h);
			for (int row = 0; row < h; ++row) {
				memcpy(gi.bitmap.data() + (size_t)row * (size_t)w,
					bmp.buffer + (size_t)row * (size_t)bmp.pitch,
					(size_t)w);
			}
		}

		glyphs.push_back(std::move(gi));
		penX += adv;
	}
	longestLine = std::max(longestLine, penX);
	float totalHeight = penY + lineHeight;

	// ============================================================
	// 3. 图片尺寸
	// ============================================================
	int imgW = (int)std::ceil(longestLine);
	int imgH = (int)std::ceil(totalHeight);
	if (imgW <= 0) imgW = 1;
	if (imgH <= 0) imgH = 1;

	// ============================================================
	// 4. 对齐偏移
	// ============================================================
	// 水平：只有当 dimensions.width > 0 时才有对齐的意义
	float offsetX = 0;
	if (textDefinition._dimensions.width > 0) {
		switch (textDefinition._alignment) {
		case TextHAlignment::CENTER:
			offsetX = (textDefinition._dimensions.width - longestLine) / 2.0f;
			break;
		case TextHAlignment::RIGHT:
			offsetX = textDefinition._dimensions.width - longestLine;
			break;
		default: break;   // LEFT
		}
	}

	float offsetY = 0;
	switch (textDefinition._vertAlignment) {
	case TextVAlignment::CENTER:
		offsetY = (imgH - totalHeight) / 2.0f;
		break;
	case TextVAlignment::BOTTOM:
		offsetY = imgH - totalHeight;
		break;
	default: break;   // TOP
	}

	// ============================================================
	// 5. 分配 RGBA 位图
	// ============================================================
	std::vector<Uint32> rgba((size_t)imgW * (size_t)imgH, 0);

	Uint8 fr = textDefinition._fontFillColor.r;
	Uint8 fg = textDefinition._fontFillColor.g;
	Uint8 fb = textDefinition._fontFillColor.b;
	Uint8 fa = textDefinition._fontAlpha;

	// ============================================================
	// 6. 第二遍：纯 CPU 画像素，不再碰 FreeType
	// ============================================================
	for (auto& gi : glyphs) {
		if (gi.bitmap.empty()) continue;

		int dstX0 = (int)std::round(gi.x + offsetX);
		int dstY0 = (int)std::round(gi.y + offsetY);

		// ★ 边界外提：先算出实际要画的行/列范围，
		//   内层循环就不用每次都判断了
		int yStart = std::max(0, -dstY0);
		int yEnd = std::min(gi.h, imgH - dstY0);
		int xStart = std::max(0, -dstX0);
		int xEnd = std::min(gi.w, imgW - dstX0);

		if (yStart >= yEnd || xStart >= xEnd) continue;

		for (int y = yStart; y < yEnd; ++y) {
			int dstY = dstY0 + y;

			const uint8_t* srcRow = gi.bitmap.data() + (size_t)y * (size_t)gi.w;
			Uint32*        dstRow = rgba.data() + (size_t)dstY * (size_t)imgW;

			for (int x = xStart; x < xEnd; ++x) {
				Uint8 a = srcRow[x];           // A8
				if (a == 0) continue;

				// 最终 alpha = 文字 alpha × 字形 alpha
				Uint8 finalA = (Uint8)((a * fa) / 255);

				// 预乘 alpha（与 loadFrom 一致）
				Uint8 r = (Uint8)((fr * finalA) / 255);
				Uint8 g = (Uint8)((fg * finalA) / 255);
				Uint8 b = (Uint8)((fb * finalA) / 255);

				// SDL_PIXELFORMAT_ABGR8888：内存顺序 R,G,B,A
				dstRow[dstX0 + x] =
					((Uint32)finalA << 24) |
					((Uint32)b << 16) |
					((Uint32)g << 8) |
					((Uint32)r);
			}
		}
	}

	// ============================================================
	// 7. 上传成 Texture2D
	// ============================================================
	auto renderer = SDLView::getInstance()->getRender();
	if (!renderer) {
		return false;   // FaceGuard 自动释放 face
	}

	SDL_Texture* tex = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STATIC,
		imgW, imgH);
	if (!tex) {
		return false;   // FaceGuard 自动释放 face
	}

	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

	if (SDL_UpdateTexture(tex, nullptr, rgba.data(),
		imgW * (int)sizeof(Uint32)) != 0) {
		SDL_DestroyTexture(tex);
		return false;   // FaceGuard 自动释放 face
	}

	SDL_SetTextureScaleMode(tex, SDL_ScaleModeLinear);

	// 到这里才算真正成功，再写入成员
	_texture = tex;
	_contentSize = Size((float)imgW, (float)imgH);
	_pixelsWide = imgW;
	_pixelsHigh = imgH;
	_pixelFormat = backend::PixelFormat::RGBA8888;
	_hasPremultipliedAlpha = true;

	return true;   // FaceGuard 自动释放 face
}



void Texture2D::setAntiAliasTexParameters()
{
	if (_antialiasEnabled) return;
	_antialiasEnabled = true;

	if (_texture) {
		SDL_SetTextureScaleMode(_texture, SDL_ScaleModeLinear);
	}
}

void Texture2D::setAliasTexParameters()
{
	if (!_antialiasEnabled) return;
	_antialiasEnabled = false;

	if (_texture) {
		SDL_SetTextureScaleMode(_texture, SDL_ScaleModeNearest);
	}
}
bool Texture2D::updateWithData(void* data, int x, int y, int w, int h)
{
	if (!_texture || !data || w <= 0 || h <= 0) return false;
	SDL_Rect region = { x, y, w, h };
	return SDL_UpdateTexture(_texture, &region, data, w * 4) == 0;
}





bool Texture2D::initWithData(const void* data, size_t dataLen,
	backend::PixelFormat pixelFormat,
	int pixelsWide, int pixelsHigh,
	const Size& contentSize, bool preMultipliedAlpha)
{

	free();
	if (!data || dataLen == 0) return false;
	if (pixelsWide <= 0 || pixelsHigh <= 0) return false;

	// ---- A8 特例：先展开成 RGBA8888，再走 initWithMipmaps ----
	if (pixelFormat == backend::PixelFormat::A8 ||
		pixelFormat == backend::PixelFormat::AI88)
	{
		size_t count = (size_t)pixelsWide * pixelsHigh;
		if (dataLen < count) return false;

		std::vector<Uint32> rgba(count);
		const unsigned char* src = (const unsigned char*)data;
		for (size_t i = 0; i < count; ++i) {
			Uint8 a = src[i];
			// ★ 预乘灰：R=G=B=A
			rgba[i] = (Uint32)a << 24
				| (Uint32)a << 16
				| (Uint32)a << 8
				| (Uint32)a;
		}

		MipmapInfo mip;
		mip.address = reinterpret_cast<unsigned char*>(rgba.data());
		mip.len = (int)(count * sizeof(Uint32));

		bool ok = initWithMipmaps(&mip, 1,
			backend::PixelFormat::RGBA8888,
			pixelsWide, pixelsHigh,
			/*preMultipliedAlpha=*/true);   // ★ 改 true
		if (ok) _contentSize = contentSize;
		return ok;
	}

	// ---- 其它格式：直接走 initWithMipmaps ----
	MipmapInfo mip;
	mip.address = const_cast<unsigned char*>(
		reinterpret_cast<const unsigned char*>(data));
	mip.len = (int)dataLen;

	bool ok = initWithMipmaps(&mip, 1, pixelFormat,
		pixelsWide, pixelsHigh,
		preMultipliedAlpha);
	if (ok) _contentSize = contentSize;
	return ok;
}

bool Texture2D::initWithMipmaps(MipmapInfo* mipmaps, int mipmapsNum,
	backend::PixelFormat pixelFormat,
	int pixelsWide, int pixelsHigh,
	bool preMultipliedAlpha)
{
	if (pixelFormat == backend::PixelFormat::A8 ||
		pixelFormat == backend::PixelFormat::AI88) {
		SDL_Log("initWithMipmaps 不支持 A8/AI88");
		return false;   // ← Debug / Release 都生效，优雅失败
	}
	free();
	if (!mipmaps || mipmapsNum <= 0) return false;
	if (pixelsWide <= 0 || pixelsHigh <= 0) return false;

	auto renderer = SDLView::getInstance()->getRender();
	if (!renderer) return false;

	// 只取 level 0
	unsigned char* data = mipmaps[0].address;
	int            len = mipmaps[0].len;
	if (!data || len <= 0) return false;

	// 1. 像素格式映射到 SDL
	// 1. 像素格式映射到 SDL
	Uint32 sdlFmt = toSDLPixelFormat(pixelFormat);
	if (sdlFmt == SDL_PIXELFORMAT_UNKNOWN) {
		SDL_Log("不支持的 PixelFormat");
		return false;
	}

	// 2. 算 pitch
	int bpp = SDL_BITSPERPIXEL(sdlFmt);      // 8/16/24/32
	int pitch = pixelsWide * (bpp / 8);

	// 3. 建纹理 + 上传
	_texture = SDL_CreateTexture(renderer, sdlFmt,
		SDL_TEXTUREACCESS_STATIC, pixelsWide, pixelsHigh);
	if (!_texture) {
		SDL_Log("创建纹理失败: %s", SDL_GetError());
		return false;
	}
	SDL_UpdateTexture(_texture, nullptr, data, pitch);

	// 4. 混合模式 + 采样模式
	SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureScaleMode(_texture,
		_antialiasEnabled ? SDL_ScaleModeLinear : SDL_ScaleModeNearest);

	// 5. 记录属性
	_pixelsWide = pixelsWide;
	_pixelsHigh = pixelsHigh;
	_pixelFormat = pixelFormat;
	_contentSize = Size((float)pixelsWide, (float)pixelsHigh);
	_hasPremultipliedAlpha = preMultipliedAlpha;

	return true;
}



void Texture2D::loadFrom(const char* fileName)
{
	free();

	int w = 0, h = 0, comp = 0;
	unsigned char* pixels = stbi_load(fileName, &w, &h, &comp, 4);
	if (!pixels) {
		printf("stbi_load failed: %s (%s)\n", fileName, stbi_failure_reason());
		return;
	}

#if OG_ENABLE_PREMULTIPLIED_ALPHA != 0
	const int total = w * h;
	for (int i = 0; i < total; ++i) {
		unsigned char* p = pixels + i * 4;
		unsigned char a = p[3];
		p[0] = (unsigned char)((p[0] * a) / 255);
		p[1] = (unsigned char)((p[1] * a) / 255);
		p[2] = (unsigned char)((p[2] * a) / 255);
	}
	_hasPremultipliedAlpha = true;
#else
	_hasPremultipliedAlpha = false;   // ★ 显式标记
#endif

	auto renderer = SDLView::getInstance()->getRender();
	_texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STATIC,
		w, h);
	if (!_texture) {
		stbi_image_free(pixels);
		return;
	}

	SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
	SDL_UpdateTexture(_texture, nullptr, pixels, w * 4);

	_contentSize = Size((float)w, (float)h);

	stbi_image_free(pixels);
}

bool Texture2D::loadMemData(unsigned char* data, int len)
{
	free();

	int w = 0, h = 0, comp = 0;
	// 强制 4 通道 = RGBA
	unsigned char* pixels = stbi_load_from_memory(data, len, &w, &h, &comp, 4);
	if (!pixels) {
		printf("stbi_load_from_memory failed: %s\n", stbi_failure_reason());
		return false;
	}

	// 预乘 alpha（直接操作 RGBA 字节）
#if OG_ENABLE_PREMULTIPLIED_ALPHA != 0
	const int total = w * h;
	for (int i = 0; i < total; ++i) {
		unsigned char* p = pixels + i * 4;   // R,G,B,A
		unsigned char a = p[3];
		p[0] = (unsigned char)((p[0] * a) / 255);
		p[1] = (unsigned char)((p[1] * a) / 255);
		p[2] = (unsigned char)((p[2] * a) / 255);

	}
	_hasPremultipliedAlpha = true;
#else
	_hasPremultipliedAlpha = false;   // ★ 显式标记
#endif

	auto renderer = SDLView::getInstance()->getRender();

	// stb 给的内存顺序是 R,G,B,A —— 对应 SDL 的 ABGR8888
	_texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ABGR8888,
		SDL_TEXTUREACCESS_STATIC,
		w, h);
	if (!_texture) {
		stbi_image_free(pixels);
		return false;
	}

	SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
	SDL_UpdateTexture(_texture, nullptr, pixels, w * 4);

	_contentSize = Size((float)w, (float)h);

	stbi_image_free(pixels);   // 必须用 stbi_image_free，不是 free
	return true;
}
// Texture2D.cpp
bool Texture2D::createTarget(float w, float h)
{
	free();   // 释放旧的

	_texture = SDL_CreateTexture(
		SDLView::getInstance()->getRender(),
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,   // 关键
		w, h);

	if (!_texture) return false;

	SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);
	_contentSize.width = (float)w;
	_contentSize.height = (float)h;
	return true;
}
void Texture2D::createTexture(Texture2D* texture, const Rect & rect)
{
	auto renderer = SDLView::getInstance()->getRender();
	auto getTexture = texture;

	float w = rect.size.width, h = rect.size.height;

	SDL_Texture *tex = SDL_GetRenderTarget(renderer);

	_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)w, (int)h);
	SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND);

	// 1. 保存原渲染目标
	SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);

	SDL_SetRenderTarget(renderer, _texture);
	//创建目标纹理
	Rect dest = { 0,0,w,h };
	getTexture->render(rect, dest, Vec2::ZERO,
		rect.size / 2, Color4B(255, 255, 255, 255), false, false);
	// 	SDL_Rect rrr = { 0,0,w,h };
	// 	SDL_Rect ddd = { 0,0,w,h };
	//	SDL_RenderCopyEx(SDLView::getInstance()->getRender(), getTexture.get()->getTexture(),  &rrr, &ddd, 0, 0,SDL_FLIP_NONE);
	_contentSize.width = w;
	_contentSize.height = h;

	// 4. 切到新目标，把源纹理渲染上来
	SDL_SetRenderTarget(renderer, oldTarget);
}

// void Texture2D::render(int x, int y,SDL_Rect *clip,double degress ,SDL_Point *pointer,SDL_RendererFlip flip)
// {
// 	SDL_Rect renderQuad = { x,y,(int)_contentSize.width,(int)_contentSize.height };
// 	if (clip != nullptr)
// 	{
// 		renderQuad.w = clip->w;
// 		renderQuad.h = clip->h;
// 	}
// 
// 	SDL_RenderCopyEx(SDLView::getInstance()->getRender(), mTexture, clip, &renderQuad, degress, pointer, flip);
// }
// 
// void Texture2D::render(SDL_Rect *dest, SDL_Rect *clip, double degress,SDL_Point *pointer,SDL_RendererFlip flip)
// {
// 
// 	SDL_RenderCopyEx(SDLView::getInstance()->getRender(), mTexture, clip, dest, degress, pointer, flip);
// }
// 
// 
// void Texture2D::render(const Rect& clip, const Rect &dest, double degress, const Vec2 &pointer, bool flipX, bool flipY)
// {
// 	
// 		SDL_RendererFlip  flip = (SDL_RendererFlip)((flipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE) | (flipY ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE));
// 
// 
// 		SDL_Rect src = { (int)clip.origin.x,
// 			(int)clip.origin.y,
// 			(int)clip.size.width,
// 			(int)clip.size.height };
// 
// 		SDL_Rect dst = { (int)dest.origin.x,
// 			(int)dest.origin.y,
// 			(int)dest.size.width,
// 			(int)dest.size.height };
// 
// 		//it->second->setBlendMode(SDL_BLENDMODE_INVALID);
// 		SDL_Point point = { (int)pointer.x,(int)pointer.y };
// 		SDL_RenderCopyEx(SDLView::getInstance()->getRender(), mTexture, nullptr, &dst, degress, &point, flip);
// 	
// }


void Texture2D::render(const Rect& clip, const Rect &dest, Vec2 rotate, const Vec2 &pointer, const Color4B &color, bool flipX, bool flipY)
{
	tmpQuad.TextureCvRenderer(_texture, _contentSize, clip, dest, rotate, pointer, color, flipX, flipY);
}

OG_END

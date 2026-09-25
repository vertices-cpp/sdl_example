
#ifndef _OGFontAtlas_h_
#define _OGFontAtlas_h_

/// @cond DO_NOT_SHOW

#include <string>
#include <unordered_map>

#include "OGPlatformMacros.h"
#include "OGRef.h"
#include "OGTexture2D.h"
#include "OGFont.h"

OG_BEGIN

//--------------------FontAtlas----------------------------------

class Font;
class EventCustom;
class EventListenerCustom;
class FontFreeType;

struct FontLetterDefinition
{
	float U;
	float V;
	float width;
	float height;
	float offsetX;
	float offsetY;
	int textureID;
	bool validDefinition;
	int xAdvance;
};

class  FontAtlas : public Ref
{
public:
	static const int CacheTextureWidth;
	static const int CacheTextureHeight;
	static const char* CMD_PURGE_FONTATLAS;
	static const char* CMD_RESET_FONTATLAS;
	/**
	 * @js ctor
	 */
	FontAtlas(Font &theFont);
	/**
	 * @js NA
	 * @lua NA
	 */
	virtual ~FontAtlas();

	void addLetterDefinition(char32_t utf32Char, const FontLetterDefinition &letterDefinition);
	bool getLetterDefinitionForChar(char32_t utf32Char, FontLetterDefinition &letterDefinition);

	bool prepareLetterDefinitions(const std::u32string& utf16String);

	const std::unordered_map<ssize_t, Texture2D*>& getTextures() const { return _atlasTextures; }
	void  addTexture(Texture2D *texture, int slot);
	float getLineHeight() const { return _lineHeight; }
	void  setLineHeight(float newHeight);

	std::string getFontName() const;

	Texture2D* getTexture(int slot);
	const Font* getFont() const { return _font; }

	/** listen the event that renderer was recreated on Android/WP8
	 It only has effect on Android and WP8.
	 */
	void listenRendererRecreated(EventCustom *event);

	/** Removes textures atlas.
	 It will purge the textures atlas and if multiple texture exist in the FontAtlas.
	 */
	void purgeTexturesAtlas();

	/** sets font texture parameters:
	 - GL_TEXTURE_MIN_FILTER = GL_LINEAR
	 - GL_TEXTURE_MAG_FILTER = GL_LINEAR
	 */
	void setAntiAliasTexParameters();

	/** sets font texture parameters:
	- GL_TEXTURE_MIN_FILTER = GL_NEAREST
	- GL_TEXTURE_MAG_FILTER = GL_NEAREST
	*/
	void setAliasTexParameters();

protected:
	void reset();

	void reinit();

	void releaseTextures();

	void findNewCharacters(const std::u32string& u32Text, std::unordered_map<unsigned int, unsigned int>& charCodeMap);

	void conversionU32TOGB2312(const std::u32string& u32Text, std::unordered_map<unsigned int, unsigned int>& charCodeMap);

	void initTextureWithZeros(Texture2D *texture);

	/**
	 * Scale each font letter by scaleFactor.
	 *
	 * @param scaleFactor A float scale factor for scaling font letter info.
	 */
	void scaleFontLetterDefinition(float scaleFactor);

	void updateTextureContent(backend::PixelFormat format, int startY);

	std::unordered_map<ssize_t, Texture2D*> _atlasTextures;
	std::unordered_map<char32_t, FontLetterDefinition> _letterDefinitions;
	float _lineHeight = 0.f;
	Font* _font = nullptr;
	FontFreeType* _fontFreeType = nullptr;
	void* _iconv = nullptr;

	// Dynamic GlyphCollection related stuff
	int _currentPage = 0;
	unsigned char *_currentPageData = nullptr;
	unsigned char *_currentPageDataRGBA = nullptr;
	int _currentPageDataSize = 0;
	int _currentPageDataSizeRGBA = 0;
	float _currentPageOrigX = 0;
	float _currentPageOrigY = 0;
	int _letterPadding = 0;
	int _letterEdgeExtend = 0;

	int _fontAscender = 0;
	EventListenerCustom* _rendererRecreatedListener = nullptr;
	bool _antialiasEnabled = true;
	int _currLineHeight = 0;

	friend class Label;
};

//--------------------FontAtlasCache----------------------------------
class FontAtlas;
class Texture2D;
struct _ttfConfig;

class   FontAtlasCache
{
public:
	static FontAtlas* getFontAtlasTTF(const _ttfConfig* config);
	static FontAtlas* getFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset = Vec2::ZERO);

	static FontAtlas* getFontAtlasCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
	static FontAtlas* getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
	static FontAtlas* getFontAtlasCharMap(const std::string& plistFile);

	static bool releaseFontAtlas(FontAtlas *atlas);

	/** Removes cached data.
	 It will purge the textures atlas and if multiple texture exist in one FontAtlas.
	 */
	static void purgeCachedData();

	/** Release current FNT texture and reload it.
	 CAUTION : All component use this font texture should be reset font name, though the file name is same!
			   otherwise, it will cause program crash!
	*/
	static void reloadFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset = Vec2::ZERO);

	/** Unload all texture atlas texture create by special file name.
	 CAUTION : All component use this font texture should be reset font name, though the file name is same!
			   otherwise, it will cause program crash!
	*/
	static void unloadFontAtlasTTF(const std::string& fontFileName);

private:
	static std::unordered_map<std::string, FontAtlas *> _atlasMap;
};

OG_END

/// @endcond
#endif 

#ifndef _OG_FONT_ATLAS_H_
#define _OG_FONT_ATLAS_H_

/// @cond DO_NOT_SHOW

#include <string>
#include <unordered_map>

#include "OGPlatformMacros.h"
#include "OGRef.h"
#include "OGTexture2D.h"
 

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
	void listenRendererRecreated(EventCustom *event); 
	void purgeTexturesAtlas(); 
	void setAntiAliasTexParameters();
 

	void setAliasTexParameters();
	FontFreeType* getFontFreeType() const { return _fontFreeType; }
protected:
	void reset();

	void reinit();

	void releaseTextures();

	void findNewCharacters(const std::u32string& u32Text, std::unordered_map<unsigned int, unsigned int>& charCodeMap);

	void conversionU32TOGB2312(const std::u32string& u32Text, std::unordered_map<unsigned int, unsigned int>& charCodeMap);

	void initTextureWithZeros(Texture2D *texture); 
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
//	EventListenerCustom* _rendererRecreatedListener = nullptr;
	bool _antialiasEnabled = true;
	int _currLineHeight = 0;

	friend class Label;

	// 加在 protected 或 public
	Color4B _outlineColor = Color4B(0, 0, 0, 255);      // 默认黑
	Color4B _atlasTextColor = Color4B(255, 255, 255, 255); // 默认白
	
public:
	// FontAtlas.h
	const std::unordered_map<char32_t, FontLetterDefinition>&
		getLetterDefinitions() const { return _letterDefinitions; }
	void setOutlineColor(const Color4B& c) { 
		_outlineColor = c; 
	}
	void setAtlasTextColor(const Color4B& c) { _atlasTextColor = c; }
	const Color4B& getOutlineColor() const { return _outlineColor; }
	const Color4B& getAtlasTextColor() const { return _atlasTextColor; }

};



OG_END

/// @endcond
#endif 
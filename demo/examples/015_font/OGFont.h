
#ifndef _OG_FONT_H_
#define _OG_FONT_H_

/// @cond DO_NOT_SHOW

#include <string>
#include "OGMathGeometry.h"
#include "OGRef.h"
#include "iconv_wrapper.h"

OG_BEGIN



//--------------------Font----------------------------------
class FontAtlas;

class Font : public Ref
{
public:
    virtual FontAtlas* createFontAtlas() = 0;
    virtual int* getHorizontalKerningForTextUTF32(const std::u32string& text, int &outNumLetters) const = 0;
    virtual int getFontMaxHeight() const { return 0; }
};
 
//--------------------FontCharMap----------------------------------

class Texture2D;
class FontCharMap : public Font
{
public:
	static FontCharMap * create(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
	static FontCharMap * create(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
	static FontCharMap * create(const std::string& plistFile);

	virtual int* getHorizontalKerningForTextUTF32(const std::u32string& text, int &outNumLetters) const override;
	virtual FontAtlas *createFontAtlas() override;

protected:
	FontCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
		:_texture(texture)
		, _mapStartChar(startCharMap)
		, _itemWidth(itemWidth)
		, _itemHeight(itemHeight)
	{}
	/**
	 * @js NA
	 * @lua NA
	 */
	virtual ~FontCharMap();

private:
	Texture2D* _texture;
	int _mapStartChar;
	int _itemWidth;
	int _itemHeight;

};

 
//--------------------FontFNT----------------------------------

class BMFontConfiguration;

class  FontFNT : public Font
{

public:

	static FontFNT * create(const std::string& fntFilePath, const Vec2& imageOffset = Vec2::ZERO);
	/** Purges the cached data.
	Removes from memory the cached configurations and the atlas name dictionary.
	*/
	static void purgeCachedData();
	virtual int* getHorizontalKerningForTextUTF32(const std::u32string& text, int &outNumLetters) const override;
	virtual FontAtlas *createFontAtlas() override;
	void setFontSize(float fontSize);
	int getOriginalFontSize()const;

	static void reloadBMFontResource(const std::string& fntFilePath);

protected:

	FontFNT(BMFontConfiguration *theContfig, const Vec2& imageOffset = Vec2::ZERO);
	/**
	 * @js NA
	 * @lua NA
	 */
	virtual ~FontFNT();

private:

	int  getHorizontalKerningForChars(char32_t firstChar, char32_t secondChar) const;

	BMFontConfiguration * _configuration;
	Vec2                   _imageOffset;
	//User defined font size
	float  _fontSize;
};

//--------------------Font----------------------------------
//--------------------Font----------------------------------
//--------------------Font----------------------------------

//--------------------Font----------------------------------
//--------------------Font----------------------------------
OG_END

#endif 



#ifndef _OG_FONT_FNT_H_
#define _OG_FONT_FNT_H_

/// @cond DO_NOT_SHOW

#include <string>
#include "OGFont.h"  


OG_BEGIN

 
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
 
OG_END

#endif 


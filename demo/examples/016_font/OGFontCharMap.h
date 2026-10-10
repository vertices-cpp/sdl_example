
#ifndef _OG_FONT_CHAR_MAP_H_
#define _OG_FONT_CHAR_MAP_H_

/// @cond DO_NOT_SHOW

#include <string> 
#include "OGFont.h" 


OG_BEGIN

 
 
//--------------------FontCharMap----------------------------------

class Texture2D;
class FontCharMap : public Font
{
public:
	static FontCharMap * create(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
	static FontCharMap * create(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
	static FontCharMap * create(const std::string& plistFile);

	virtual int* getHorizontalKerningForTextUTF32(const std::u32string& text, int &outNumLetters) const override;
	virtual FontAtlas *createFontAtlas()  ;

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
 
 
OG_END

#endif 


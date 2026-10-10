
#ifndef _OG_FONT_H_
#define _OG_FONT_H_

/// @cond DO_NOT_SHOW

#include <string>
#include "OGTypes.h"  
#include "OGPlatformMacros.h"
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
  
OG_END

#endif 


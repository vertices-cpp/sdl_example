
#ifndef _OG_FONT_ATLAS_CACHE_H_
#define _OG_FONT_ATLAS_CACHE_H_
 

#include <string>
#include <unordered_map>

#include "ogTypes.h"

OG_BEGIN
 

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
	static void purgeCachedData(); 
	static void reloadFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset = Vec2::ZERO); 
	static void unloadFontAtlasTTF(const std::string& fontFileName);

private:
	static std::unordered_map<std::string, FontAtlas *> _atlasMap;
};

OG_END

/// @endcond
#endif 
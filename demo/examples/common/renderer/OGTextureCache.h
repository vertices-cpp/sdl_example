
#ifndef _OG_TEXTURE_CACHE_H_
#define _OG_TEXTURE_CACHE_H_

#include <string>
#include <unordered_map>
 
#include "base/OGRef.h"
#include "base/OGData.h"

OG_BEGIN

class Data;
class Texture2D;

class TextureCache :public Ref {

	std::unordered_map<std::string, Texture2D *> _texturesCache;
public:
	~TextureCache() {
		removeAllTextures();
	}
	 
	Texture2D*   addImage(const std::string& path);
	Texture2D* addImage(Data * image, const std::string & key);

	Texture2D * addImage(unsigned char * image, int length, const std::string & key, bool rawRGBA = false, int w = 0, int h = 0);

	Texture2D * getTextureForKey(const std::string & textureKeyName) const;

	void removeAllTextures();
	 
	void removeTextureForKey(const std::string & textureKeyName);
	std::unordered_map<std::string, Texture2D *> getTexturesCache()const { return _texturesCache; }
}; 
OG_END

#endif //_TEXTURE_CACHE_H_

#ifndef _TEXTURE_CACHE_H_
#define _TEXTURE_CACHE_H_

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

	Texture2D * addImage(unsigned char * image, int length, const std::string & key);

	Texture2D * getTextureForKey(const std::string & textureKeyName) const;

	void removeAllTextures();
	 
	void removeTextureForKey(const std::string & textureKeyName);
	
}; 
OG_END

#endif //_TEXTURE_CACHE_H_
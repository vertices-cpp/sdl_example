
 

#include "renderer/OGTextureCache.h"

 
#include "platform/OGFileUtils.h"

#include "renderer/OGTexture2D.h" 

OG_BEGIN



Texture2D * TextureCache::addImage(const std::string & path)
{
	std::string curPath = convertPathFormatToUnixStyle(path);

	// 1. 缓存
	auto it = _texturesCache.find(curPath);
	if (it != _texturesCache.end()) return it->second;

	// 2. 从 FileUtils 拿 Data（ZIP / 文件系统，FileUtils 内部处理）
	Data data = FileUtils::getInstance()->getDataFromFile(path);
	if (data.isNull()) return nullptr;

	// 3. 上传纹理
	Texture2D* texture = new Texture2D();
	if (!texture->loadMemData((unsigned char*)data.getBytes(), (int)data.getSize()))
	{
		OG_SAFE_RELEASE(texture);
		return nullptr;
	}

	_texturesCache.emplace(curPath, texture);
	return texture;
}

Texture2D* TextureCache::addImage(Data *image, const std::string &key)
{
	OGASSERT(image != nullptr, "TextureCache: image MUST not be nil");
	OGASSERT(image->getBytes() != nullptr, "TextureCache: image MUST not be nil");

	Texture2D * texture = nullptr;

	do
	{
		auto it = _texturesCache.find(key);
		if (it != _texturesCache.end()) {
			texture = it->second;
			break;
		}

		texture = new (std::nothrow) Texture2D();

		if (texture)
		{
			if (texture->loadMemData(image->getBytes(), image->getSize()))
			{
				_texturesCache.emplace(key, texture);
			}
			else
			{
				OG_SAFE_RELEASE(texture);
				texture = nullptr;
				OGLOG("orange: initWithImage failed!");
			}
		}
		else
		{
			OGLOG("orange: Allocating memory for Texture2D failed!");
		}

	} while (0);

// #if OG_ENABLE_CACHE_TEXTURE_DATA
// 	VolatileTextureMgr::addImage(texture, image);
// #endif

	return texture;
}

Texture2D* TextureCache::addImage(unsigned char *image,int length, const std::string &key)
{
	OGASSERT(image != nullptr, "TextureCache: image MUST not be nil");
  
	Texture2D * texture = nullptr;

	do
	{
		auto it = _texturesCache.find(key);
		if (it != _texturesCache.end()) {
			texture = it->second;
			break;
		}

		texture = new (std::nothrow) Texture2D();

		if (texture)
		{
			if (texture->loadMemData(image, length))
			{
				_texturesCache.emplace(key, texture);
			}
			else
			{
				OG_SAFE_RELEASE(texture);
				texture = nullptr;
				OGLOG("orange: initWithImage failed!");
			}
		}
		else
		{
			OGLOG("orange: Allocating memory for Texture2D failed!");
		}

	} while (0);

	// #if OG_ENABLE_CACHE_TEXTURE_DATA
	// 	VolatileTextureMgr::addImage(texture, image);
	// #endif

	return texture;
}

Texture2D* TextureCache::getTextureForKey(const std::string &textureKeyName) const
{
	std::string key = textureKeyName;
	auto it = _texturesCache.find(key);

	if (it == _texturesCache.end()) {
		key = FileUtils::getInstance()->fullPathForFilename(textureKeyName);
		it = _texturesCache.find(key);
	}

	if (it != _texturesCache.end())
		return it->second;
	return nullptr;
}
void TextureCache::removeAllTextures()
{
	for (auto& i : _texturesCache)
	{
		OG_SAFE_RELEASE(i.second);
	}
	_texturesCache.clear();
}


void TextureCache::removeTextureForKey(const std::string &textureKeyName)
{
	std::string key = textureKeyName;
	auto it = _texturesCache.find(key);

	if (it == _texturesCache.end()) {
		key = FileUtils::getInstance()->fullPathForFilename(textureKeyName);
		it = _texturesCache.find(key);
	}

	if (it != _texturesCache.end()) {
		it->second->release();
		_texturesCache.erase(it);
	}
}
OG_END


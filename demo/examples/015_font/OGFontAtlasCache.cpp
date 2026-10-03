 

#include <string>
#include <unordered_map>

#include "OGFontAtlasCache.h"

#include "OGFontFreeType.h"
#include "OGFontFNT.h"
#include "OGFontCharMap.h"
#include "OGLabel.h"

#include "OGFileUtils.h"
#include "OGDirector.h"

OG_BEGIN
 

//--------------------FontAtlasCache----------------------------------

std::unordered_map<std::string, FontAtlas *> FontAtlasCache::_atlasMap;
#define ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE 255

void FontAtlasCache::purgeCachedData()
{
	auto atlasMapCopy = _atlasMap;
	for (auto&& atlas : atlasMapCopy)
	{
		auto refCount = atlas.second->getReferenceCount();
		atlas.second->release();
		if (refCount != 1)
			atlas.second->purgeTexturesAtlas();
	}
	_atlasMap.clear();
}

FontAtlas* FontAtlasCache::getFontAtlasTTF(const _ttfConfig* config)
{
	auto realFontFilename = FileUtils::getInstance()->getNewFilename(config->fontFilePath);  // resolves real file path, to prevent storing multiple atlases for the same file.
	bool useDistanceField = config->distanceFieldEnabled;
	if (config->outlineSize > 0)
	{
		useDistanceField = false;
	}

	std::string key;
	char keyPrefix[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
	snprintf(keyPrefix, ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE,
		useDistanceField ? "df %.2f %d %d %d %d " : "%.2f %d %d %d %d ",
		config->fontSize,
		config->outlineSize,
		config->outlineColor.r,
		config->outlineColor.g,
		config->outlineColor.b);
	std::string atlasName(keyPrefix);
	atlasName += realFontFilename;

	// 	printf("getFontAtlasTTF: outlineColor=(%d,%d,%d,%d)\n",
	// 		config->outlineColor.r, config->outlineColor.g,
	// 		config->outlineColor.b, config->outlineColor.a);

	auto it = _atlasMap.find(atlasName);

	if (it == _atlasMap.end())
	{
		auto font = FontFreeType::create(realFontFilename, config->fontSize, config->glyphs,
			config->customGlyphs, useDistanceField, (float)config->outlineSize);
		if (font)
		{
			auto tempAtlas = font->createFontAtlas();
			if (tempAtlas)
			{
				tempAtlas->setOutlineColor(config->outlineColor);
				tempAtlas->setAtlasTextColor(Color4B(255, 255, 255, 255));   // 固定白
				_atlasMap[atlasName] = tempAtlas;
				return tempAtlas;
			}
		}
	}
	else
		return it->second;


	return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset /* = Vec2::ZERO */)
{
	auto realFontFilename = FileUtils::getInstance()->getNewFilename(fontFileName);  // resolves real file path, to prevent storing multiple atlases for the same file.
	char keyPrefix[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
	snprintf(keyPrefix, ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE, "%.2f %.2f ", imageOffset.x, imageOffset.y);
	std::string atlasName(keyPrefix);
	atlasName += realFontFilename;

	auto it = _atlasMap.find(atlasName);
	if (it == _atlasMap.end())
	{
		auto font = FontFNT::create(realFontFilename, imageOffset);

		if (font)
		{
			auto tempAtlas = font->createFontAtlas();
			if (tempAtlas)
			{
				_atlasMap[atlasName] = tempAtlas;
				return _atlasMap[atlasName];
			}
		}
	}
	else
		return it->second;

	return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(const std::string& plistFile)
{
	std::string atlasName = plistFile;

	auto it = _atlasMap.find(atlasName);
	if (it == _atlasMap.end())
	{
		auto font = FontCharMap::create(plistFile);

		if (font)
		{
			auto tempAtlas = font->createFontAtlas();
			if (tempAtlas)
			{
				_atlasMap[atlasName] = tempAtlas;
				return _atlasMap[atlasName];
			}
		}
	}
	else
		return it->second;

	return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
	char key[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
	sprintf(key, "name:%p_%d_%d_%d", texture->getTexture(), itemWidth, itemHeight, startCharMap);
	std::string atlasName = key;

	auto it = _atlasMap.find(atlasName);
	if (it == _atlasMap.end())
	{
		auto font = FontCharMap::create(texture, itemWidth, itemHeight, startCharMap);

		if (font)
		{
			auto tempAtlas = font->createFontAtlas();
			if (tempAtlas)
			{
				_atlasMap[atlasName] = tempAtlas;
				return _atlasMap[atlasName];
			}
		}
	}
	else
		return it->second;

	return nullptr;
}

FontAtlas* FontAtlasCache::getFontAtlasCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
	char keyPrefix[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
	snprintf(keyPrefix, ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE, "%d %d %d ", itemWidth, itemHeight, startCharMap);
	std::string atlasName(keyPrefix);
	atlasName += charMapFile;

	auto it = _atlasMap.find(atlasName);
	if (it == _atlasMap.end())
	{
		auto font = FontCharMap::create(charMapFile, itemWidth, itemHeight, startCharMap);

		if (font)
		{
			auto tempAtlas = font->createFontAtlas();
			if (tempAtlas)
			{
				_atlasMap[atlasName] = tempAtlas;
				return _atlasMap[atlasName];
			}
		}
	}
	else
		return it->second;

	return nullptr;
}

bool FontAtlasCache::releaseFontAtlas(FontAtlas *atlas)
{
	if (nullptr != atlas)
	{
		for (auto &item : _atlasMap)
		{
			if (item.second == atlas)
			{
				if (atlas->getReferenceCount() == 1)
				{
					_atlasMap.erase(item.first);
				}

				atlas->release();

				return true;
			}
		}
	}

	return false;
}

void FontAtlasCache::reloadFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset/* = Vec2::ZERO*/)
{
	char keyPrefix[ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE];
	snprintf(keyPrefix, ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE, "%.2f %.2f ", imageOffset.x, imageOffset.y);
	std::string atlasName(keyPrefix);
	atlasName += fontFileName;

	auto it = _atlasMap.find(atlasName);
	if (it != _atlasMap.end())
	{
		OG_SAFE_RELEASE_NULL(it->second);
		_atlasMap.erase(it);
	}
	FontFNT::reloadBMFontResource(fontFileName);
	auto font = FontFNT::create(fontFileName, imageOffset);
	if (font)
	{
		auto tempAtlas = font->createFontAtlas();
		if (tempAtlas)
		{
			_atlasMap[atlasName] = tempAtlas;
		}
	}

}

void FontAtlasCache::unloadFontAtlasTTF(const std::string& fontFileName)
{
	auto item = _atlasMap.begin();
	while (item != _atlasMap.end())
	{
		if (item->first.find(fontFileName) != std::string::npos)
		{
			OG_SAFE_RELEASE_NULL(item->second);
			item = _atlasMap.erase(item);
		}
		else
			item++;
	}
}

OG_END
 


#include "OGFontAtlas.h"
#include "OGFontFreeType.h"
#include "iconv_wrapper.h"
#include "OGDirector.h"
#include "OGLabel.h"
// #include "OGEventListenerCustom.h"
// #include "OGEventDispatcher.h"
// #include "OGEventType.h"

OG_BEGIN

//--------------------FontAtlas----------------------------------
const int FontAtlas::CacheTextureWidth = 512;
const int FontAtlas::CacheTextureHeight = 512;
const char* FontAtlas::CMD_PURGE_FONTATLAS = "__og_PURGE_FONTATLAS";
const char* FontAtlas::CMD_RESET_FONTATLAS = "__og_RESET_FONTATLAS";

FontAtlas::FontAtlas(Font &theFont)
	: _font(&theFont)
{
	_font->retain();

	_fontFreeType = dynamic_cast<FontFreeType*>(_font);
	if (_fontFreeType)
	{
		_lineHeight = (float)_font->getFontMaxHeight();
		_fontAscender = _fontFreeType->getFontAscender();
		_letterEdgeExtend = 2;

		if (_fontFreeType->isDistanceFieldEnabled())
		{
			_letterPadding += 2 * FontFreeType::DistanceMapSpread;
		}

		auto outlineSize = _fontFreeType->getOutlineSize();
		if (outlineSize > 0)
		{
			_lineHeight += 2 * outlineSize;
		}
	}
}

void FontAtlas::reinit()
{
	if (_currentPageData)
	{
		delete[]_currentPageData;
		_currentPageData = nullptr;
	}

	OG_SAFE_DELETE_ARRAY(_currentPageDataRGBA);

	auto texture = new (std::nothrow) Texture2D;

	_currentPageDataSize = CacheTextureWidth * CacheTextureHeight;

	auto outlineSize = _fontFreeType->getOutlineSize();
	if (outlineSize > 0)
	{
		_currentPageDataSize *= 2;

		_currentPageDataSizeRGBA = _currentPageDataSize * 2;
		_currentPageDataRGBA = new (std::nothrow) unsigned char[_currentPageDataSizeRGBA];
		memset(_currentPageDataRGBA, 0, _currentPageDataSizeRGBA);
	}

	_currentPageData = new (std::nothrow) unsigned char[_currentPageDataSize];
	memset(_currentPageData, 0, _currentPageDataSize);

	initTextureWithZeros(texture);

	addTexture(texture, 0);
	texture->release();
}

FontAtlas::~FontAtlas()
{
	_font->release();
	releaseTextures();

	delete[]_currentPageData;

}

void FontAtlas::initTextureWithZeros(Texture2D *texture)
{
	char *zeros = nullptr;
	backend::PixelFormat pixelFormat;
	float outlineSize = _fontFreeType->getOutlineSize();
	size_t zeroBytes = 0;
	if (outlineSize > 0)
	{
		//metal do no support AI88 format
		pixelFormat = backend::PixelFormat::RGBA8888;
		zeroBytes = CacheTextureWidth * CacheTextureWidth * 4;
	}
	else
	{
		pixelFormat = backend::PixelFormat::A8;
		zeroBytes = CacheTextureWidth * CacheTextureWidth;
	}
	zeros = new char[zeroBytes]();
	//std::fill(zeros, zeros + cnt, 0);
	texture->initWithData(zeros, zeroBytes, pixelFormat, CacheTextureWidth, CacheTextureHeight, Size(CacheTextureWidth, CacheTextureHeight));
	delete[] zeros;
}

void FontAtlas::reset()
{
	releaseTextures();

	_currLineHeight = 0;
	_currentPage = 0;
	_currentPageOrigX = 0;
	_currentPageOrigY = 0;
	_letterDefinitions.clear();

	reinit();
}

void FontAtlas::releaseTextures()
{
	for (auto &item : _atlasTextures)
	{
		item.second->release();
	}
	_atlasTextures.clear();
}

void FontAtlas::purgeTexturesAtlas()
{
	if (_fontFreeType)
	{
		reset();
		auto eventDispatcher = Director::getInstance()->getEventDispatcher();
		eventDispatcher->dispatchCustomEvent(CMD_PURGE_FONTATLAS, this);
		eventDispatcher->dispatchCustomEvent(CMD_RESET_FONTATLAS, this);
	}
}

void FontAtlas::listenRendererRecreated(EventCustom * /*event*/)
{
	purgeTexturesAtlas();
}

void FontAtlas::addLetterDefinition(char32_t utf32Char, const FontLetterDefinition &letterDefinition)
{
	_letterDefinitions[utf32Char] = letterDefinition;
}

void FontAtlas::scaleFontLetterDefinition(float scaleFactor)
{
	for (auto&& fontDefinition : _letterDefinitions) {
		auto& letterDefinition = fontDefinition.second;
		letterDefinition.width *= scaleFactor;
		letterDefinition.height *= scaleFactor;
		letterDefinition.offsetX *= scaleFactor;
		letterDefinition.offsetY *= scaleFactor;
		letterDefinition.xAdvance = (int)(letterDefinition.xAdvance * scaleFactor);
	}
}

bool FontAtlas::getLetterDefinitionForChar(char32_t utf32Char, FontLetterDefinition &letterDefinition)
{
	auto outIterator = _letterDefinitions.find(utf32Char);

	if (outIterator != _letterDefinitions.end())
	{
		letterDefinition = (*outIterator).second;
		return letterDefinition.validDefinition;
	}
	else
	{
		return false;
	}
}

void FontAtlas::conversionU32TOGB2312(const std::u32string& u32Text, std::unordered_map<unsigned int, unsigned int>& charCodeMap)
{
	size_t strLen = u32Text.length();
	auto gb2312StrSize = strLen * 2;
	auto gb2312Text = new (std::nothrow) char[gb2312StrSize];
	memset(gb2312Text, 0, gb2312StrSize);

	switch (_fontFreeType->getEncoding())
	{
	case FT_ENCODING_GB2312:
	{
		std::u16string u16Text = iconv_wrapper_from_u32le_to_utf16le(u32Text, Iconv::Utf32leToGbk);
	}
	break;
	default:
		OGLOG("Unsupported encoding:%d", _fontFreeType->getEncoding());
		break;
	}

	unsigned short gb2312Code = 0;
	unsigned char* dst = (unsigned char*)&gb2312Code;
	char32_t u32Code;
	for (size_t index = 0, gbIndex = 0; index < strLen; ++index)
	{
		u32Code = u32Text[index];
		if (u32Code < 256)
		{
			charCodeMap[u32Code] = u32Code;
			gbIndex += 1;
		}
		else
		{
			dst[0] = gb2312Text[gbIndex + 1];
			dst[1] = gb2312Text[gbIndex];
			charCodeMap[u32Code] = gb2312Code;

			gbIndex += 2;
		}
	}

	delete[] gb2312Text;
}

void FontAtlas::findNewCharacters(const std::u32string& u32Text, std::unordered_map<unsigned int, unsigned int>& charCodeMap)
{
	std::u32string newChars;
	FT_Encoding charEncoding = _fontFreeType->getEncoding();

	//find new characters
	if (_letterDefinitions.empty())
	{
		newChars.append(u32Text);
	}
	else
	{
		auto length = u32Text.length();
		newChars.reserve(length);
		for (size_t i = 0; i < length; ++i)
		{
			auto outIterator = _letterDefinitions.find(u32Text[i]);
			if (outIterator == _letterDefinitions.end())
			{
				newChars.push_back(u32Text[i]);
			}
		}
	}

	if (!newChars.empty())
	{
		switch (charEncoding)
		{
		case FT_ENCODING_UNICODE:
		{
			for (auto u32Code : newChars)
			{
				charCodeMap[u32Code] = u32Code;
			}
			break;
		}
		case FT_ENCODING_GB2312:
		{
			conversionU32TOGB2312(newChars, charCodeMap);
			break;
		}
		default:
			//  OGLOG("FontAtlas::findNewCharacters: Unsupported encoding:%d", charEncoding);
			break;
		}
	}
}

bool FontAtlas::prepareLetterDefinitions(const std::u32string& utf32Text)
{
	if (_fontFreeType == nullptr)
	{
		return false;
	}

	if (!_currentPageData)
		reinit();

	std::unordered_map<unsigned int, unsigned int> codeMapOfNewChar;
	findNewCharacters(utf32Text, codeMapOfNewChar);
	if (codeMapOfNewChar.empty())
	{
		return false;
	}

	int adjustForDistanceMap = _letterPadding / 2;
	int adjustForExtend = _letterEdgeExtend / 2;
	long bitmapWidth;
	long bitmapHeight;
	int glyphHeight;
	Rect tempRect;
	FontLetterDefinition tempDef;

	//   auto scaleFactor = OG_CONTENT_SCALE_FACTOR();
	auto  pixelFormat = _fontFreeType->getOutlineSize() > 0 ? backend::PixelFormat::A8 : backend::PixelFormat::A8;//稍后处理

	int startY = (int)_currentPageOrigY;

	for (auto&& it : codeMapOfNewChar)
	{
		auto bitmap = _fontFreeType->getGlyphBitmap(it.second, bitmapWidth, bitmapHeight, tempRect, tempDef.xAdvance);
		if (bitmap && bitmapWidth > 0 && bitmapHeight > 0)
		{
			tempDef.validDefinition = true;
			tempDef.width = tempRect.size.width + _letterPadding + _letterEdgeExtend;
			tempDef.height = tempRect.size.height + _letterPadding + _letterEdgeExtend;
			tempDef.offsetX = tempRect.origin.x - adjustForDistanceMap - adjustForExtend;
			tempDef.offsetY = _fontAscender + tempRect.origin.y - adjustForDistanceMap - adjustForExtend;

			if (_currentPageOrigX + tempDef.width > CacheTextureWidth)
			{
				_currentPageOrigY += _currLineHeight;
				_currLineHeight = 0;
				_currentPageOrigX = 0;
				if (_currentPageOrigY + _lineHeight + _letterPadding + _letterEdgeExtend >= CacheTextureHeight)
				{
					updateTextureContent(pixelFormat, startY);

					startY = 0;

					_currentPageOrigY = 0;
					memset(_currentPageData, 0, _currentPageDataSize);
					_currentPage++;
					auto tex = new (std::nothrow) Texture2D;

					initTextureWithZeros(tex);

					if (_antialiasEnabled)
					{
						tex->setAntiAliasTexParameters();
					}
					else
					{
						tex->setAliasTexParameters();
					}
					addTexture(tex, _currentPage);

					tex->release();
				}
			}
			glyphHeight = static_cast<int>(bitmapHeight) + _letterPadding + _letterEdgeExtend;
			if (glyphHeight > _currLineHeight)
			{
				_currLineHeight = glyphHeight;
			}
			_fontFreeType->renderCharAt(_currentPageData, (int)_currentPageOrigX + adjustForExtend, (int)_currentPageOrigY + adjustForExtend, bitmap, bitmapWidth, bitmapHeight);

			tempDef.U = _currentPageOrigX;
			tempDef.V = _currentPageOrigY;
			tempDef.textureID = _currentPage;
			_currentPageOrigX += tempDef.width + 1;
			// take from pixels to points
//             tempDef.width = tempDef.width / scaleFactor;
//             tempDef.height = tempDef.height / scaleFactor;
//             tempDef.U = tempDef.U / scaleFactor;
//             tempDef.V = tempDef.V / scaleFactor;
		}
		else {
			if (bitmap)
				delete[] bitmap;
			if (tempDef.xAdvance)
				tempDef.validDefinition = true;
			else
				tempDef.validDefinition = false;

			tempDef.width = 0;
			tempDef.height = 0;
			tempDef.U = 0;
			tempDef.V = 0;
			tempDef.offsetX = 0;
			tempDef.offsetY = 0;
			tempDef.textureID = 0;
			_currentPageOrigX += 1;
		}

		_letterDefinitions[it.first] = tempDef;
	}

	updateTextureContent(pixelFormat, startY);
	return true;
}

void FontAtlas::updateTextureContent(backend::PixelFormat format, int startY)
{
	unsigned char *data = nullptr;
	auto outlineSize = _fontFreeType->getOutlineSize();
	if (outlineSize > 0 && format == backend::PixelFormat::AI88)
	{
		int nLen = CacheTextureWidth * ((int)_currentPageOrigY - startY + _currLineHeight);
		data = _currentPageData + CacheTextureWidth * (int)startY * 2;
		memset(_currentPageDataRGBA, 0, 4 * nLen);
		for (auto i = 0; i < nLen; i++)
		{
			_currentPageDataRGBA[i * 4] = data[i * 2];
			_currentPageDataRGBA[i * 4 + 3] = data[i * 2 + 1];
		}
		_atlasTextures[_currentPage]->updateWithData(
			_currentPageDataRGBA, 0, startY,
			CacheTextureWidth,
			(int)_currentPageOrigY - startY + _currLineHeight);
	}
	else
	{
		data = _currentPageData + CacheTextureWidth * (int)startY;
		_atlasTextures[_currentPage]->updateWithData(
			data, 0, startY,
			CacheTextureWidth,
			(int)_currentPageOrigY - startY + _currLineHeight);
	}
}

void FontAtlas::addTexture(Texture2D *texture, int slot)
{
	texture->retain();
	_atlasTextures[slot] = texture;
}

Texture2D* FontAtlas::getTexture(int slot)
{
	return _atlasTextures[slot];
}

void FontAtlas::setLineHeight(float newHeight)
{
	_lineHeight = newHeight;
}

std::string FontAtlas::getFontName() const
{
	std::string fontName = _fontFreeType ? _fontFreeType->getFontName() : "";
	if (fontName.empty()) return fontName;
	auto idx = fontName.rfind('/');
	if (idx != std::string::npos) { return fontName.substr(idx + 1); }
	idx = fontName.rfind('\\');
	if (idx != std::string::npos) { return fontName.substr(idx + 1); }
	return fontName;
}

void FontAtlas::setAliasTexParameters()
{
	if (_antialiasEnabled)
	{
		_antialiasEnabled = false;
		for (const auto & tex : _atlasTextures)
		{
			tex.second->setAliasTexParameters();
		}
	}
}

void FontAtlas::setAntiAliasTexParameters()
{
	if (!_antialiasEnabled)
	{
		_antialiasEnabled = true;
		for (const auto & tex : _atlasTextures)
		{
			tex.second->setAntiAliasTexParameters();
		}
	}
}
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
	snprintf(keyPrefix, ATLAS_MAP_KEY_PREFIX_BUFFER_SIZE, useDistanceField ? "df %.2f %d " : "%.2f %d ", config->fontSize, config->outlineSize);
	std::string atlasName(keyPrefix);
	atlasName += realFontFilename;

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
				_atlasMap[atlasName] = tempAtlas;
				return _atlasMap[atlasName];
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

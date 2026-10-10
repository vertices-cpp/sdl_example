#include "OGLabel.h"

#include <algorithm>
#include <stddef.h>


//#include "SDLView.h"

#include "ogTypes.h"

#include "OGFont.h"
#include "OGFontAtlasCache.h"
#include "OGFontAtlas.h"

#include "OGCamera.h"
#include "ogMacros.h"
#include "OGFileUtils.h" 
#include "OGRenderer.h"
//#include "OGDirector.h"

//#include "OGEventListenerCustom.h"
#include "OGEventDispatcher.h"
#include "OGFontFNT.h"






OG_BEGIN

namespace StringUtils {
	bool isUnicodeNonBreaking(char32_t ch)
	{
		return ch == 0x00A0 || ch == 0x202F || ch == 0x2007 || ch == 0x2060;
	}
	bool isCJKUnicode(char32_t ch)
	{
		return (ch >= 0x4E00 && ch <= 0x9FBF)
			|| (ch >= 0x2E80 && ch <= 0x2FDF)
			|| (ch >= 0x2FF0 && ch <= 0x30FF)
			|| (ch >= 0x3100 && ch <= 0x31BF)
			|| (ch >= 0xAC00 && ch <= 0xD7AF)
			|| (ch >= 0xF900 && ch <= 0xFAFF)
			|| (ch >= 0xFE30 && ch <= 0xFE4F)
			|| (ch >= 0x31C0 && ch <= 0x4DFF)
			|| (ch >= 0x1f004 && ch <= 0x1f682);
	}
	bool isUnicodeSpace(char32_t ch)
	{
		return (ch >= 0x0009 && ch <= 0x000D) || ch == 0x0020 || ch == 0x0085 || ch == 0x00A0 || ch == 0x1680
			|| (ch >= 0x2000 && ch <= 0x200A) || ch == 0x2028 || ch == 0x2029 || ch == 0x202F
			|| ch == 0x205F || ch == 0x3000;
	}
}

// ============================================================
// create
// ============================================================
Label* Label::create()
{
	auto ret = new (std::nothrow) Label;
	if (ret) ret->autorelease();
	return ret;
}

Label* Label::createWithTTF(const std::string& text, const std::string& fontFile, float fontSize,
	const Size& dimensions, TextHAlignment hAlignment, TextVAlignment vAlignment)
{
	auto ret = new (std::nothrow) Label(hAlignment, vAlignment);
	if (ret && ret->initWithTTF(text, fontFile, fontSize, dimensions, hAlignment, vAlignment))
	{
		ret->autorelease();
		return ret;
	}
	OG_SAFE_DELETE(ret);
	return nullptr;
}

Label* Label::createWithTTF(const TTFConfig& ttfConfig, const std::string& text,
	TextHAlignment hAlignment, int maxLineWidth)
{
	auto ret = new (std::nothrow) Label(hAlignment);
	if (ret && ret->initWithTTF(ttfConfig, text, hAlignment, maxLineWidth))
	{
		ret->autorelease();
		return ret;
	}
	OG_SAFE_DELETE(ret);
	return nullptr;
}

Label* Label::createWithBMFont(const std::string& bmfontFilePath, const std::string& text,
	const TextHAlignment& hAlignment, int maxLineWidth, const Vec2& imageOffset)
{
	auto ret = new (std::nothrow) Label(hAlignment);
	if (ret && ret->setBMFontFilePath(bmfontFilePath, imageOffset))
	{
		ret->setMaxLineWidth((float)maxLineWidth);
		ret->setString(text);
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

Label* Label::createWithCharMap(const std::string& plistFile)
{
	auto ret = new (std::nothrow) Label();
	if (ret && ret->setCharMap(plistFile)) { ret->autorelease(); return ret; }
	delete ret;
	return nullptr;
}

Label* Label::createWithCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
	auto ret = new (std::nothrow) Label();
	if (ret && ret->setCharMap(texture, itemWidth, itemHeight, startCharMap)) { ret->autorelease(); return ret; }
	delete ret;
	return nullptr;
}

Label* Label::createWithCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
	auto ret = new (std::nothrow) Label();
	if (ret && ret->setCharMap(charMapFile, itemWidth, itemHeight, startCharMap)) { ret->autorelease(); return ret; }
	delete ret;
	return nullptr;
}

// ============================================================
// setCharMap
// ============================================================
bool Label::setCharMap(const std::string& plistFile)
{
	auto newAtlas = FontAtlasCache::getFontAtlasCharMap(plistFile);
	if (!newAtlas) { reset(); return false; }
	_currentLabelType = LabelType::CHARMAP;
	setFontAtlas(newAtlas);
	return true;
}

bool Label::setCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
	auto newAtlas = FontAtlasCache::getFontAtlasCharMap(texture, itemWidth, itemHeight, startCharMap);
	if (!newAtlas) { reset(); return false; }
	_currentLabelType = LabelType::CHARMAP;
	setFontAtlas(newAtlas);
	return true;
}

bool Label::setCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
	auto newAtlas = FontAtlasCache::getFontAtlasCharMap(charMapFile, itemWidth, itemHeight, startCharMap);
	if (!newAtlas) { reset(); return false; }
	_currentLabelType = LabelType::CHARMAP;
	setFontAtlas(newAtlas);
	return true;
}

// ============================================================
// initWithTTF
// ============================================================
bool Label::initWithTTF(const std::string& text, const std::string& fontFilePath, float fontSize,
	const Size& dimensions, TextHAlignment, TextVAlignment)
{
	if (FileUtils::getInstance()->isFileExist(fontFilePath))
	{
		TTFConfig ttfConfig(fontFilePath, fontSize, GlyphCollection::DYNAMIC);
		if (setTTFConfig(ttfConfig))
		{
			setDimensions(dimensions.width, dimensions.height);
			setString(text);
		}
		return true;
	}
	return false;
}

bool Label::initWithTTF(const TTFConfig& ttfConfig, const std::string& text, TextHAlignment, int maxLineWidth)
{
	if (FileUtils::getInstance()->isFileExist(ttfConfig.fontFilePath) && setTTFConfig(ttfConfig))
	{
		setMaxLineWidth(maxLineWidth);
		setString(text);
		return true;
	}
	return false;
}

// ============================================================
// constructor / destructor
// ============================================================
Label::Label(TextHAlignment hAlignment, TextVAlignment vAlignment)
	: _fontAtlas(nullptr)
	, _horizontalKernings(nullptr)
	, _strikethroughEnabled(false)
{
	_name = "Label";
	reset();
	_hAlignment = hAlignment;
	_vAlignment = vAlignment;

	// 	_purgeTextureListener = EventListenerCustom::create(FontAtlas::CMD_PURGE_FONTATLAS, [this](EventCustom* event) {
	// 		if (_fontAtlas && _currentLabelType == LabelType::TTF && event->getUserData() == _fontAtlas)
	// 		{
	// 			_quads.clear();
	// 			if (_fontAtlas) FontAtlasCache::releaseFontAtlas(_fontAtlas);
	// 		}
	// 	});
	// 	_eventDispatcher->addEventListenerWithFixedPriority(_purgeTextureListener, 1);
	// 
	// 	_resetTextureListener = EventListenerCustom::create(FontAtlas::CMD_RESET_FONTATLAS, [this](EventCustom* event) {
	// 		if (_fontAtlas && _currentLabelType == LabelType::TTF && event->getUserData() == _fontAtlas)
	// 		{
	// 			_fontAtlas = nullptr;
	// 			auto lineHeight = _lineHeight;
	// 			this->setTTFConfig(_fontConfig);
	// 			setLineHeight(lineHeight);
	// 		}
	// 	});
	// 	_eventDispatcher->addEventListenerWithFixedPriority(_resetTextureListener, 2);
}

Label::~Label()
{
	delete[] _horizontalKernings;

	if (_fontAtlas)
	{
		FontAtlasCache::releaseFontAtlas(_fontAtlas);
	}
	// 	_eventDispatcher->removeEventListener(_purgeTextureListener);
	// 	_eventDispatcher->removeEventListener(_resetTextureListener);
}

// ============================================================
// reset
// ============================================================
void Label::reset()
{
	for (auto& i : _letters)
		if (i) delete i;
	_letters.clear();
	_quads.clear();
	_lettersInfo.clear();

	if (_fontAtlas)
	{
		FontAtlasCache::releaseFontAtlas(_fontAtlas);
		_fontAtlas = nullptr;
	}

	_currentLabelType = LabelType::TTF;
	_contentDirty = false;
	_numberOfLines = 0;
	_lengthOfString = 0;
	_utf32Text.clear();
	_utf8Text.clear();

	TTFConfig temp;
	_fontConfig = temp;
	_bmFontPath = "";

	if (_horizontalKernings)
	{
		delete[] _horizontalKernings;
		_horizontalKernings = nullptr;
	}
	_additionalKerning = 0.f;
	_lineHeight = 0.f;
	_lineSpacing = 0.f;
	_maxLineWidth = 0.f;
	_labelDimensions.width = 0.f;
	_labelDimensions.height = 0.f;
	_labelWidth = 0.f;
	_labelHeight = 0.f;
	_lineBreakWithoutSpaces = false;
	_hAlignment = TextHAlignment::LEFT;
	_vAlignment = TextVAlignment::TOP;

	_textColor = Color4B::WHITE;
	_textColorF = Color4F::WHITE;
	setColor(Color3B::WHITE);

	_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
	_isOpacityModifyRGB = false;
	_enableWrap = true;
	_bmFontSize = -1;
	_bmfontScale = 1.0f;
	_overflow = Overflow::NONE;
	_originalFontSize = 0.0f;

	_underlineNode.clear();
	_strikethroughEnabled = false;
	setSkewX(0);
}

// ============================================================
// setFontAtlas
// ============================================================
void Label::setFontAtlas(FontAtlas* atlas, bool, bool)
{
	if (atlas == _fontAtlas) return;

	OG_SAFE_RETAIN(atlas);
	if (_fontAtlas)
	{
		FontAtlasCache::releaseFontAtlas(_fontAtlas);
	}
	_fontAtlas = atlas;

	if (_fontAtlas)
	{
		_lineHeight = _fontAtlas->getLineHeight();
		_contentDirty = true;
	}
}

// ============================================================
// setTTFConfig / setBMFontFilePath
// ============================================================
bool Label::setTTFConfig(const TTFConfig& ttfConfig)
{
	_originalFontSize = ttfConfig.fontSize;
	return setTTFConfigInternal(ttfConfig);
}

bool Label::setBMFontFilePath(const std::string& bmfontFilePath, const Vec2& imageOffset, float fontSize)
{
	FontAtlas* newAtlas = FontAtlasCache::getFontAtlasFNT(bmfontFilePath, imageOffset);
	if (!newAtlas) { reset(); return false; }

	if (std::abs(fontSize) < FLT_EPSILON)
	{
		FontFNT* bmFont = (FontFNT*)newAtlas->getFont();
		if (bmFont) _bmFontSize = (float)bmFont->getOriginalFontSize();
	}
	if (fontSize > 0.0f) _bmFontSize = fontSize;

	_bmFontPath = bmfontFilePath;
	_currentLabelType = LabelType::BMFONT;
	setFontAtlas(newAtlas);
	return true;
}

// ============================================================
// setString / setAlignment / setDimensions
// ============================================================
void Label::setString(const std::string& text)
{
	if (text.compare(_utf8Text))
	{
		_utf8Text = text;
		_contentDirty = true;
		std::u32string utf32String = iconv_wrapper_to_utf32le(_utf8Text, Utf8ToUtf32le);
		if (!utf32String.empty())
			_utf32Text = utf32String;

		// 		printf("setString: size=%zu, bytes=", _utf8Text.size());
		// 		for (unsigned char c : _utf8Text) printf("%02X ", c);
		// 		printf("\n");
	}
}

void Label::setAlignment(TextHAlignment hAlignment, TextVAlignment vAlignment)
{
	if (hAlignment != _hAlignment || vAlignment != _vAlignment)
	{
		_hAlignment = hAlignment;
		_vAlignment = vAlignment;
		_contentDirty = true;
	}
}

void Label::setMaxLineWidth(float maxLineWidth)
{
	if (_labelWidth == 0 && _maxLineWidth != maxLineWidth)
	{
		_maxLineWidth = maxLineWidth;
		_contentDirty = true;
	}
}

void Label::setDimensions(float width, float height)
{
	if (_overflow == Overflow::RESIZE_HEIGHT) height = 0;
	if (height != _labelHeight || width != _labelWidth)
	{
		_labelWidth = width;
		_labelHeight = height;
		_labelDimensions.width = width;
		_labelDimensions.height = height;
		_maxLineWidth = width;
		_contentDirty = true;
		if (_overflow == Overflow::SHRINK && _originalFontSize > 0)
			this->restoreFontSize();
	}
}

void Label::restoreFontSize()
{
	if (_currentLabelType == LabelType::TTF)
	{
		auto ttfConfig = this->getTTFConfig();
		ttfConfig.fontSize = _originalFontSize;
		this->setTTFConfigInternal(ttfConfig);
	}
	else if (_currentLabelType == LabelType::BMFONT)
	{
		this->setBMFontSizeInternal(_originalFontSize);
	}
}

void Label::setLineBreakWithoutSpace(bool breakWithoutSpace)
{
	if (breakWithoutSpace != _lineBreakWithoutSpaces)
	{
		_lineBreakWithoutSpaces = breakWithoutSpace;
		_contentDirty = true;
	}
}

// ============================================================
// updateLabelLetters
// ============================================================
void Label::updateLabelLetters()
{
	if (_letters.empty() || _lengthOfString <= 0) return;

	for (int i = 0; i < _lengthOfString; ++i)
	{
		if (i >= (int)_letters.size()) break;
		auto* letter = _letters[i];
		if (!letter) continue;

		auto& info = _lettersInfo[i];
		if (!info.valid) { letter->_visible = false; continue; }

		auto& def = _fontAtlas->_letterDefinitions[info.utf32Char];
		letter->_clip = Rect(def.U, def.V, def.width, def.height);
		letter->_size = Size(def.width, def.height);
		letter->_rotated = false;

		float px = info.positionX + _linesOffsetX[info.lineIndex];
		float py = info.positionY + _letterOffsetY;
		letter->_offset = Vec2(px, py - def.height);
		letter->_textureID = def.textureID;
		letter->_visible = true;
	}
}

// ============================================================
// alignText
// ============================================================
bool Label::alignText()
{
	if (_fontAtlas == nullptr || _utf32Text.empty())
	{
		setContentSize(Size::ZERO);
		return true;
	}

	bool ret = true;
	do {
		_fontAtlas->prepareLetterDefinitions(_utf32Text);

		// 		_textures.clear();
		// 		auto& textures = _fontAtlas->getTextures();
		// 		for (auto& pair : textures)
		// 			_textures[pair.first] = pair.second;
		// 
		// 		if (_textures.empty()) return true;
		if (_fontAtlas->getTextures().empty()) return true;

		_lengthOfString = static_cast<int>(_utf32Text.size());
		if ((int)_letters.size() < _lengthOfString)
		{
			size_t oldSize = _letters.size();
			_letters.resize(_lengthOfString, nullptr);
			for (size_t i = oldSize; i < _letters.size(); ++i)
				_letters[i] = new LabelLetter();
		}

		_textDesiredHeight = 0.f;
		_linesWidth.clear();

		if (_maxLineWidth > 0.f && !_lineBreakWithoutSpaces)
			multilineTextWrapByWord();
		else
			multilineTextWrapByChar();

		computeAlignmentOffset();

		if (_overflow == Overflow::SHRINK)
		{
			float fontSize = this->getRenderingFontSize();
			if (fontSize > 0 && isVerticalClamp())
				this->shrinkLabelToContentSize(OG_CALLBACK_0(Label::isVerticalClamp, this));
		}

		if (!updateQuads())
		{
			ret = false;
			if (_overflow == Overflow::SHRINK)
				this->shrinkLabelToContentSize(OG_CALLBACK_0(Label::isHorizontalClamp, this));
			break;
		}

		updateLabelLetters();
	} while (0);

	return ret;
}

// ============================================================
// computeHorizontalKernings
// ============================================================
bool Label::computeHorizontalKernings(const std::u32string& stringToRender)
{
	if (_horizontalKernings)
	{
		delete[] _horizontalKernings;
		_horizontalKernings = nullptr;
	}
	int letterCount = 0;
	_horizontalKernings = _fontAtlas->getFont()->getHorizontalKerningForTextUTF32(stringToRender, letterCount);
	return _horizontalKernings != nullptr;
}

bool Label::isHorizontalClamped(float letterPositionX, int lineIndex)
{
	auto wordWidth = this->_linesWidth[lineIndex];
	bool letterOverClamp = (letterPositionX > _contentSize.width || letterPositionX < 0);
	if (!_enableWrap) return letterOverClamp;
	return (wordWidth > this->_contentSize.width && letterOverClamp);
}

// ============================================================
// updateQuads
// ============================================================
bool Label::updateQuads()
{
	bool ret = true;

	_quads.clear();
	_quads.resize(_lengthOfString);

	for (int ctr = 0; ctr < _lengthOfString; ++ctr)
	{
		if (!_lettersInfo[ctr].valid) continue;

		auto& letterDef = _fontAtlas->_letterDefinitions[_lettersInfo[ctr].utf32Char];

		Rect _reusedRect;
		_reusedRect.size.height = letterDef.height;
		_reusedRect.size.width = letterDef.width;
		_reusedRect.origin.x = letterDef.U;
		_reusedRect.origin.y = letterDef.V;

		auto py = _lettersInfo[ctr].positionY + _letterOffsetY;
		if (_labelHeight > 0.f)
		{
			if (py < _tailoredTopY)
			{
				auto clipTop = _tailoredTopY - py;
				_reusedRect.origin.y += clipTop;
				_reusedRect.size.height -= clipTop;
				py += clipTop;
			}
			if (py + letterDef.height * _bmfontScale > _tailoredBottomY)
			{
				_reusedRect.size.height = (py > _tailoredBottomY) ? 0.f : (_tailoredBottomY - py);
			}
		}

		auto lineIndex = _lettersInfo[ctr].lineIndex;
		auto px = _lettersInfo[ctr].positionX + letterDef.width / 2 * _bmfontScale + _linesOffsetX[lineIndex];

		if (_labelWidth > 0.f)
		{
			if (this->isHorizontalClamped(px, lineIndex))
			{
				if (_overflow == Overflow::CLAMP)
				{
					_reusedRect.size.width = 0;
				}
				else if (_overflow == Overflow::SHRINK)
				{
					if (_contentSize.width > letterDef.width) { ret = false; break; }
					else { _reusedRect.size.width = 0; }
				}
			}
		}

		if (_reusedRect.size.height > 0.f && _reusedRect.size.width > 0.f)
		{
			float letterPositionX = _lettersInfo[ctr].positionX + _linesOffsetX[lineIndex];
			float x1 = letterPositionX;
			//float y1 = py - letterDef.height * _bmfontScale;
			float y1 = py;
			float x2 = x1 + letterDef.width * _bmfontScale;
			//float y2 = y1 + letterDef.height * _bmfontScale;
			float y2 = py + letterDef.height * _bmfontScale;

			// 			auto itTex = _textures.find(letterDef.textureID);
			// 			if (itTex == _textures.end() || !itTex->second) continue;
			// 			Texture2D* tex = itTex->second;
			// 			Size texSize = tex->getContentSize();
			const auto& textures = _fontAtlas->getTextures();
			auto itTex = textures.find(letterDef.textureID);
			if (itTex == textures.end() || !itTex->second) continue;
			Texture2D* tex = itTex->second;
			Size texSize = tex->getContentSize();

			float uvX = _reusedRect.origin.x / texSize.width;
			float uvY = _reusedRect.origin.y / texSize.height;
			float uvW = (_reusedRect.origin.x + _reusedRect.size.width) / texSize.width;
			float uvH = (_reusedRect.origin.y + _reusedRect.size.height) / texSize.height;

			Color4B color(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);
			if (_isOpacityModifyRGB)
			{
				color.r *= _displayedOpacity / 255.0f;
				color.g *= _displayedOpacity / 255.0f;
				color.b *= _displayedOpacity / 255.0f;
			}
			// 			printf("updateQuads: displayedColor=(%d,%d,%d,%d)\n",
			// 				_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);
			_quads[ctr].tl = { {x1, y1}, color, {uvX, uvY} };
			_quads[ctr].tr = { {x2, y1}, color, {uvW, uvY} };
			_quads[ctr].bl = { {x1, y2}, color, {uvX, uvH} };
			_quads[ctr].br = { {x2, y2}, color, {uvW, uvH} };
		}
		else
		{
			_quads[ctr].tl.vertices = { 0, 0 };
			_quads[ctr].tr.vertices = { 0, 0 };
			_quads[ctr].bl.vertices = { 0, 0 };
			_quads[ctr].br.vertices = { 0, 0 };
		}
	}

	return ret;
}

// ============================================================
// setTTFConfigInternal  别在 setTTFConfigInternal 
// 里设颜色 Label A 先 setTTFConfig，图集 A 的 _atlasTextColor = _textColor（白）；
//Label B 用同一个图集（同 key），setTTFConfig 又设 _atlasTextColor = _textColor（可能不是白）；
// ============================================================
bool Label::setTTFConfigInternal(const TTFConfig& ttfConfig)
{
	FontAtlas* newAtlas = FontAtlasCache::getFontAtlasTTF(&ttfConfig);
	if (!newAtlas) { reset(); return false; }

	_currentLabelType = LabelType::TTF;
	setFontAtlas(newAtlas, ttfConfig.distanceFieldEnabled, true);
	_fontConfig = ttfConfig;

	if (_fontConfig.underline)     enableUnderline();
	if (_fontConfig.strikethrough) enableStrikethrough();

	return true;
}

void Label::setBMFontSizeInternal(float fontSize)
{
	if (_currentLabelType == LabelType::BMFONT)
	{
		this->setBMFontFilePath(_bmFontPath, Vec2::ZERO, fontSize);
		_contentDirty = true;
	}
}

void Label::scaleFontSizeDown(float fontSize)
{
	bool shouldUpdateContent = true;
	if (_currentLabelType == LabelType::TTF)
	{
		auto ttfConfig = this->getTTFConfig();
		ttfConfig.fontSize = fontSize;
		this->setTTFConfigInternal(ttfConfig);
	}
	else if (_currentLabelType == LabelType::BMFONT)
	{
		if (std::abs(fontSize) < FLT_EPSILON) { fontSize = 0.1f; shouldUpdateContent = false; }
		this->setBMFontSizeInternal(fontSize);
	}

	if (shouldUpdateContent) this->updateContent();
}

// ============================================================
// 下划线 / 删除线
// ============================================================
void Label::enableUnderline()
{
	_underlineEnabled = true;
	_contentDirty = true;
}

void Label::enableStrikethrough()
{
	if (!_strikethroughEnabled)
	{
		enableUnderline();
		_strikethroughEnabled = true;
	}
}

void Label::disableEffect(LabelEffect effect)
{
	switch (effect)
	{
	case LabelEffect::UNDERLINE:
		_underlineNode.clear();
		_contentDirty = true;
		break;
	case LabelEffect::STRIKETHROUGH:
		_strikethroughEnabled = false;
		disableEffect(LabelEffect::UNDERLINE);
		break;
	case LabelEffect::ALL:
		disableEffect(LabelEffect::UNDERLINE);
		disableEffect(LabelEffect::STRIKETHROUGH);
		break;
	default:
		break;
	}
}

// ============================================================
// setCameraMask
// ============================================================
void Label::setCameraMask(unsigned short mask, bool applyChildren)
{
	Node::setCameraMask(mask, applyChildren);
}

// ============================================================
// updateContent
// ============================================================
void Label::updateContent()
{
	bool updateFinished = true;

	if (_fontAtlas)
	{
		std::u32string utf32String = iconv_wrapper_to_utf32le(_utf8Text, Utf8ToUtf32le);
		if (!utf32String.empty()) _utf32Text = utf32String;

		computeHorizontalKernings(_utf32Text);
		updateFinished = alignText();
	}

	_underlineNode.clear();

	if ((_underlineEnabled || _strikethroughEnabled)   // ★ 加条件
		&& _numberOfLines > 0
		&& (int)_linesOffsetX.size() >= _numberOfLines
		&& (int)_linesWidth.size() >= _numberOfLines)
	{
		const float charheight = (_textDesiredHeight / _numberOfLines);
		const float lineWidth = charheight / 6.0f;

		for (int i = 0; i < _numberOfLines; ++i)
		{
			float offsety = _strikethroughEnabled ? charheight / 2.0f : 0.0f;
			// ★ 下划线在基线下，删除线在行中
			float y = i * charheight + charheight * 0.85f + offsety;

			LineRect lr;
			lr.rect.origin.x = _linesOffsetX[i];
			lr.rect.origin.y = y;
			lr.rect.size.width = _linesWidth[i];
			lr.rect.size.height = lineWidth;
			lr.width = lineWidth;
			lr.color = _displayedColor;
			_underlineNode.push_back(lr);
		}
	}

	if (updateFinished) _contentDirty = false;
}

// ============================================================
// setBMFontSize
// ============================================================
void Label::setBMFontSize(float fontSize)
{
	this->setBMFontSizeInternal(fontSize);
	_originalFontSize = fontSize;
}

// ============================================================
// setSystemFontName
// ============================================================
void Label::setSystemFontName(const std::string& font)
{
	if (font != _systemFont) { _systemFont = font; _systemFontDirty = true; }
}

void Label::setSystemFontSize(float fontSize)
{
	if (_systemFontSize != fontSize) { _systemFontSize = fontSize; _systemFontDirty = true; }
}

//----------------描边---------------------

void Label::enableOutline(const Color4B& color, int size)
{
	OGASSERT(_currentLabelType == LabelType::STRING_TEXTURE || _currentLabelType == LabelType::TTF, "Only supported system font and TTF!");
	_outlineSize = size;
	_outlineColor = color;

	if (_currentLabelType == LabelType::TTF)
	{
		TTFConfig config = _fontConfig;
		config.outlineSize = size;
		config.outlineColor = color;
		setTTFConfig(config);
	}

	_contentDirty = true;
}

void Label::disableOutline()
{
	enableOutline(Color4B::BLACK, 0);
}
void Label::enableShadow(const Color4B& color, const Vec2& offset)
{
	// 	printf("enableShadow: r=%d g=%d b=%d a=%d, offset=(%f,%f)\n",
	// 		color.r, color.g, color.b, color.a, offset.x, offset.y);
	_shadowEnabled = true;
	_shadowColor = color;
	_shadowOffset = offset;
	_contentDirty = true;
}

void Label::disableShadow()
{
	_shadowEnabled = false;
	_contentDirty = true;
}

float Label::getBMFontSize() const
{
	return _bmFontSize;
}

bool Label::isQuadVisible(const V2F_C4B_T2F_Quad& q,
	float localMinX, float localMinY,
	float localMaxX, float localMaxY)
{
	float pMinX = std::min(std::min(q.tl.vertices.x, q.tr.vertices.x),
		std::min(q.bl.vertices.x, q.br.vertices.x));
	float pMinY = std::min(std::min(q.tl.vertices.y, q.tr.vertices.y),
		std::min(q.bl.vertices.y, q.br.vertices.y));
	float pMaxX = std::max(std::max(q.tl.vertices.x, q.tr.vertices.x),
		std::max(q.bl.vertices.x, q.br.vertices.x));
	float pMaxY = std::max(std::max(q.tl.vertices.y, q.tr.vertices.y),
		std::max(q.bl.vertices.y, q.br.vertices.y));

	return !(pMaxX < localMinX || pMinX > localMaxX ||
		pMaxY < localMinY || pMinY > localMaxY);
}

// ============================================================
// draw
// ============================================================
void Label::draw(Renderer* renderer, const Mat3& transform, uint32_t flags)
{
	if (!_visible) return;
	if (_quads.empty() || _lengthOfString <= 0) return;
	// if (_textures.empty()) return;
// 换成
	if (!_fontAtlas) return;

	int n = (int)_quads.size();

	_batchVerts.clear();
	_batchIndices.clear();

	// ===== 0. 算可见范围（局部坐标） =====
	float localMinX = -FLT_MAX, localMinY = -FLT_MAX;
	float localMaxX = FLT_MAX, localMaxY = FLT_MAX;
	bool hasCull = false;

	auto camera = Camera::getVisitingCamera();
	if (camera)
	{
		Rect screenRect = camera->getScreenRectWithMargin();
		Mat3 screenToLocal = getModelViewTransformInverse();   // 直接用，零计算//transform.getInversed();

		Vec2 corners[4] = {
			{screenRect.origin.x, screenRect.origin.y},
			{screenRect.origin.x + screenRect.size.width, screenRect.origin.y},
			{screenRect.origin.x, screenRect.origin.y + screenRect.size.height},
			{screenRect.origin.x + screenRect.size.width, screenRect.origin.y + screenRect.size.height}
		};

		localMinX = FLT_MAX; localMinY = FLT_MAX;
		localMaxX = -FLT_MAX; localMaxY = -FLT_MAX;
		for (auto& c : corners)
		{
			float x = c.x, y = c.y;
			screenToLocal.out(x, y);
			localMinX = std::min(localMinX, x);
			localMinY = std::min(localMinY, y);
			localMaxX = std::max(localMaxX, x);
			localMaxY = std::max(localMaxY, y);
		}
		hasCull = true;
	}

	const auto& textures = _fontAtlas->getTextures();
	// ===== 1. 阴影层 =====
	if (_shadowEnabled)
	{
		Mat3 shadowMat = transform * Mat3::createTranslation(_shadowOffset.x, _shadowOffset.y);

		for (int i = 0; i < n; ++i)
		{
			if (i >= (int)_lettersInfo.size()) break;
			if (!_lettersInfo[i].valid) continue;

			const auto& q = _quads[i];

			// ★ 剔除
			if (hasCull && !isQuadVisible(q, localMinX, localMinY, localMaxX, localMaxY))
				continue;

			int texID = _lettersInfo[i].textureID;
			auto itTex = textures.find(texID);
			if (itTex == textures.end() || !itTex->second) continue;

			const V2F_C4B_T2F* src[4] = { &q.tl, &q.tr, &q.bl, &q.br };
			auto& verts = _batchVerts[texID];
			auto& indices = _batchIndices[texID];
			int base = (int)verts.size();

			for (int j = 0; j < 4; ++j)
			{
				float vx = src[j]->vertices.x;
				float vy = src[j]->vertices.y;
				shadowMat.out(vx, vy);

				V2F_C4B_T2F v;
				v.vertices = { vx, vy };
				v.colors = _shadowColor;
				v.texCoords = src[j]->texCoords;
				verts.push_back(v);
			}

			indices.push_back(base + 0);
			indices.push_back(base + 1);
			indices.push_back(base + 2);
			indices.push_back(base + 1);
			indices.push_back(base + 3);
			indices.push_back(base + 2);
		}
	}

	// ===== 2. 正文层 =====
	for (int i = 0; i < n; ++i)
	{
		if (i >= (int)_lettersInfo.size()) break;
		if (!_lettersInfo[i].valid) continue;

		const auto& q = _quads[i];

		// 		printf("quad[%d]: tl.colors=(%d,%d,%d,%d), uv=(%f,%f), v=(%f,%f)\n",
		// 			i, q.tl.colors.r, q.tl.colors.g, q.tl.colors.b, q.tl.colors.a,
		// 			q.tl.texCoords.u, q.tl.texCoords.v,
		// 			q.tl.vertices.x, q.tl.vertices.y);

				// ★ 剔除
		if (hasCull && !isQuadVisible(q, localMinX, localMinY, localMaxX, localMaxY))
			continue;

		int texID = _lettersInfo[i].textureID;
		auto itTex = textures.find(texID);
		if (itTex == textures.end() || !itTex->second) continue;

		const V2F_C4B_T2F* src[4] = { &q.tl, &q.tr, &q.bl, &q.br };
		auto& verts = _batchVerts[texID];
		auto& indices = _batchIndices[texID];
		int base = (int)verts.size();

		for (int j = 0; j < 4; ++j)
		{
			float vx = src[j]->vertices.x;
			float vy = src[j]->vertices.y;
			transform.out(vx, vy);

			V2F_C4B_T2F v;
			v.vertices = { vx, vy };
			v.colors = src[j]->colors;
			v.texCoords = src[j]->texCoords;
			verts.push_back(v);


		}

		indices.push_back(base + 0);
		indices.push_back(base + 1);
		indices.push_back(base + 2);
		indices.push_back(base + 1);
		indices.push_back(base + 3);
		indices.push_back(base + 2);
	}

	// ===== 3. 提交 =====
	_batchCommands.clear();
	_batchCommands.resize(_batchVerts.size());

	int idx = 0;
	for (auto& pair : _batchVerts)
	{
		int texID = pair.first;
		auto& verts = pair.second;
		if (verts.empty()) { idx++; continue; }

		auto itTex = textures.find(texID);
		if (itTex == textures.end() || !itTex->second) { idx++; continue; }

		Texture2D* tex = itTex->second;
		tex->setBlendMode(_blendFunc.getBlendMode());

		auto& cmd = _batchCommands[idx];

#ifdef ORANGE_DEBUG
		cmd.setOwner(this);
#endif 

		cmd.init(_globalZOrder, tex->getTexture(),
			verts.data(),
			_batchIndices[texID].data(),
			(int)verts.size(),
			(int)_batchIndices[texID].size());
		renderer->addCommand(&cmd);
		idx++;
	}

	// ===== 4. 下划线 / 删除线 =====
	if (!_underlineNode.empty())
	{
		_underlineVerts.clear();
		_underlineIndices.clear();

		int vc = 0;
		for (auto& lr : _underlineNode)
		{
			float x1 = lr.rect.origin.x;
			float y1 = lr.rect.origin.y;
			float x2 = x1 + lr.rect.size.width;
			float y2 = y1 + lr.width;

			// ★ 剔除（下划线是轴对齐矩形，简单判断）
			if (hasCull)
			{
				if (x2 < localMinX || x1 > localMaxX ||
					y2 < localMinY || y1 > localMaxY)
					continue;
			}

			float ax = x1, ay = y1; transform.out(ax, ay);
			float bx = x2, by = y1; transform.out(bx, by);
			float cx = x2, cy = y2; transform.out(cx, cy);
			float dx = x1, dy = y2; transform.out(dx, dy);

			Color4B c(lr.color.r, lr.color.g, lr.color.b, _displayedOpacity);

			_underlineVerts.push_back({ {ax, ay}, c, {0, 0} });
			_underlineVerts.push_back({ {bx, by}, c, {0, 0} });
			_underlineVerts.push_back({ {dx, dy}, c, {0, 0} });
			_underlineVerts.push_back({ {cx, cy}, c, {0, 0} });

			int base = vc * 4;
			_underlineIndices.push_back(base + 0);
			_underlineIndices.push_back(base + 1);
			_underlineIndices.push_back(base + 2);
			_underlineIndices.push_back(base + 1);
			_underlineIndices.push_back(base + 3);
			_underlineIndices.push_back(base + 2);
			vc++;
		}

		if (vc > 0)
		{
#ifdef ORANGE_DEBUG
			_underlineCommand.setOwner(this);
#endif 
			_underlineCommand.init(_globalZOrder, nullptr,
				_underlineVerts.data(), _underlineIndices.data(),
				(int)_underlineVerts.size(), (int)_underlineIndices.size());
			renderer->addCommand(&_underlineCommand);
		}
	}
}

void Label::updateBlendState()
{
	setOpacityModifyRGB(_blendFunc != BlendFunc::ALPHA_NON_PREMULTIPLIED);
}

// ============================================================
// visit
// ============================================================
void Label::visit(Renderer* renderer, const Mat3& parentTransform, uint32_t parentFlags)
{
	if (!_visible || (_utf8Text.empty() && _children.empty())) return;

	if (_contentDirty) updateContent();

	uint32_t flags = processParentFlags(parentTransform, parentFlags);
	bool visibleByCamera = isVisitableByVisitingCamera();

	if (!_children.empty())
	{
		sortAllChildren();
		int i = 0;
		for (auto size = _children.size(); i < size; ++i)
		{
			auto node = _children.at(i);
			if (node && node->getLocalZOrder() < 0)
				node->visit(renderer, _modelViewTransform, flags);
			else
				break;
		}
		this->drawSelf(visibleByCamera, renderer, flags);
		for (auto it = _children.cbegin() + i, itCend = _children.cend(); it != itCend; ++it)
			(*it)->visit(renderer, _modelViewTransform, flags);
	}
	else
	{
		this->drawSelf(visibleByCamera, renderer, flags);
	}
}

void Label::drawSelf(bool visibleByCamera, Renderer* renderer, uint32_t flags)
{
	if (visibleByCamera && !_utf8Text.empty())
		draw(renderer, _modelViewTransform, flags);
}

// ============================================================
// getLetter
// ============================================================
LabelLetter* Label::getLetter(int letterIndex)
{
	if (_currentLabelType == LabelType::TTF && _contentDirty)
		updateContent();

	if (letterIndex < 0 || letterIndex >= _lengthOfString) return nullptr;

	const auto& letterInfo = _lettersInfo[letterIndex];
	if (!letterInfo.valid) return nullptr;

	if ((int)_letters.size() <= letterIndex)
		while ((int)_letters.size() <= letterIndex)
			_letters.push_back(new LabelLetter);

	LabelLetter* letter = _letters[letterIndex];
	auto& letterDef = _fontAtlas->_letterDefinitions[letterInfo.utf32Char];

	letter->_clip = Rect(letterDef.U, letterDef.V, letterDef.width, letterDef.height);
	letter->_size = Size(letterDef.width, letterDef.height);
	letter->_offset = Vec2(letterInfo.positionX + _linesOffsetX[letterInfo.lineIndex],
		letterInfo.positionY + _letterOffsetY);
	letter->_textureID = letterDef.textureID;
	letter->_visible = (letterDef.width > 0 && letterDef.height > 0);
	return letter;
}

// ============================================================
// setLineHeight / setLineSpacing / setAdditionalKerning
// ============================================================
void Label::setLineHeight(float height)
{
	if (_lineHeight != height) { _lineHeight = height; _contentDirty = true; }
}

float Label::getLineHeight() const
{
	return _lineHeight * _bmfontScale;
}

void Label::setLineSpacing(float height)
{
	if (_lineSpacing != height) { _lineSpacing = height; _contentDirty = true; }
}

float Label::getLineSpacing() const { return _lineSpacing; }

void Label::setAdditionalKerning(float space)
{
	if (_additionalKerning != space) { _additionalKerning = space; _contentDirty = true; }
}

float Label::getAdditionalKerning() const { return _additionalKerning; }

// ============================================================
// computeStringNumLines / getStringNumLines / getStringLength
// ============================================================
void Label::computeStringNumLines()
{
	int quantityOfLines = 1;
	if (_utf32Text.empty()) { _numberOfLines = 0; return; }

	size_t stringLen = _utf32Text.length();
	for (size_t i = 0; i < stringLen - 1; ++i)
		if (_utf32Text[i] == StringUtils::UnicodeCharacters::NewLine) quantityOfLines++;

	_numberOfLines = quantityOfLines;
}

int Label::getStringNumLines()
{
	if (_contentDirty) updateContent();
	return _numberOfLines;
}

int Label::getStringLength()
{
	_lengthOfString = static_cast<int>(_utf32Text.length());
	return _lengthOfString;
}

// ============================================================
// 颜色
// ============================================================
void Label::setOpacityModifyRGB(bool isOpacityModifyRGB)
{
	if (isOpacityModifyRGB != _isOpacityModifyRGB)
	{
		_isOpacityModifyRGB = isOpacityModifyRGB;
		updateColor();
	}
}

void Label::updateDisplayedColor(const Color3B& parentColor)
{
	Node::updateDisplayedColor(parentColor);
	for (auto&& it : _letters) it->_color = _displayedColor;
	updateColor();
}

void Label::updateDisplayedOpacity(uint8_t parentOpacity)
{
	Node::updateDisplayedOpacity(parentOpacity);
	for (auto&& it : _letters) it->_displayedOpacity = _displayedOpacity;
	updateColor();
}

void Label::setTextColor(const Color4B& color)
{
	_textColor = color;
	_textColorF.r = _textColor.r / 255.0f;
	_textColorF.g = _textColor.g / 255.0f;
	_textColorF.b = _textColor.b / 255.0f;
	_textColorF.a = _textColor.a / 255.0f;
	updateColor();
}

void Label::updateColor()
{
	Color4B color4(_displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity);
	if (_isOpacityModifyRGB)
	{
		color4.r *= _displayedOpacity / 255.0f;
		color4.g *= _displayedOpacity / 255.0f;
		color4.b *= _displayedOpacity / 255.0f;
	}
	for (auto& q : _quads)
	{
		q.bl.colors = color4;
		q.br.colors = color4;
		q.tl.colors = color4;
		q.tr.colors = color4;
	}
}

// ============================================================
// getDescription / getContentSize / getBoundingBox
// ============================================================
std::string Label::getDescription() const
{
	char tmp[50];
	sprintf(tmp, "<Label | Tag = %d, Label = >", _tag);
	std::string ret = tmp;
	ret += _utf8Text;
	return ret;
}

const Size& Label::getContentSize() const
{
	if (_contentDirty) const_cast<Label*>(this)->updateContent();
	return _contentSize;
}

Rect Label::getBoundingBox() const
{
	const_cast<Label*>(this)->getContentSize();
	return Node::getBoundingBox();
}

void Label::setBlendFunc(const BlendFunc& blendFunc)
{
	_blendFunc = blendFunc;
}
// ============================================================
// removeAllChildrenWithCleanup
// ============================================================
void Label::removeAllChildrenWithCleanup(bool cleanup)
{
	for (auto& i : _letters) delete i;
	_letters.clear();
}
void Label::removeChild(LabelLetter* child, bool cleanup)
{
	for (auto it = _letters.begin(); it != _letters.end(); ++it)
		if (*it == child) { delete *it; _letters.erase(it); break; }
}
// ============================================================
// setGlobalZOrder
// ============================================================
void Label::setGlobalZOrder(float globalZOrder)
{
	Node::setGlobalZOrder(globalZOrder);
}

// ============================================================
// getRenderingFontSize / enableWrap / setOverflow / rescale
// ============================================================
float Label::getRenderingFontSize() const
{
	if (_currentLabelType == LabelType::BMFONT) return _bmFontSize;
	if (_currentLabelType == LabelType::TTF)    return this->getTTFConfig().fontSize;
	return this->getLineHeight();
}

void Label::enableWrap(bool enable)
{
	if (enable == _enableWrap || _overflow == Overflow::RESIZE_HEIGHT) return;
	this->_enableWrap = enable;
	this->rescaleWithOriginalFontSize();
	_contentDirty = true;
}

bool Label::isWrapEnabled() const { return this->_enableWrap; }

void Label::setOverflow(Overflow overflow)
{
	if (_overflow == overflow) return;
	if (_currentLabelType == LabelType::CHARMAP && overflow == Overflow::SHRINK) return;

	if (overflow == Overflow::RESIZE_HEIGHT)
	{
		this->setDimensions(_labelDimensions.width, 0);
		this->enableWrap(true);
	}
	_overflow = overflow;
	this->rescaleWithOriginalFontSize();
	_contentDirty = true;
}

void Label::rescaleWithOriginalFontSize()
{
	auto renderingFontSize = this->getRenderingFontSize();
	if (_originalFontSize - renderingFontSize >= 1)
		this->scaleFontSizeDown(_originalFontSize);
}

Label::Overflow Label::getOverflow() const { return _overflow; }

void Label::updateLetterSpriteScale(LabelLetter*) {}

// ============================================================
// computeAlignmentOffset
// ============================================================
void Label::computeAlignmentOffset()
{
	_linesOffsetX.clear();
	switch (_hAlignment)
	{
	case TextHAlignment::LEFT:
		_linesOffsetX.assign(_numberOfLines, 0);
		break;
	case TextHAlignment::CENTER:
		for (auto lineWidth : _linesWidth)
			_linesOffsetX.push_back((_contentSize.width - lineWidth) / 2.f);
		break;
	case TextHAlignment::RIGHT:
		for (auto lineWidth : _linesWidth)
			_linesOffsetX.push_back(_contentSize.width - lineWidth);
		break;
	default: break;
	}

	switch (_vAlignment)
	{
		// 	case TextVAlignment::TOP:    _letterOffsetY = _contentSize.height; break;
		// 	case TextVAlignment::CENTER: _letterOffsetY = (_contentSize.height + _textDesiredHeight) / 2.f; break;
		// 	case TextVAlignment::BOTTOM: _letterOffsetY = _textDesiredHeight; break;
	case TextVAlignment::TOP:    _letterOffsetY = 0; break;
	case TextVAlignment::CENTER: _letterOffsetY = (_contentSize.height - _textDesiredHeight) / 2.f; break;
	case TextVAlignment::BOTTOM: _letterOffsetY = _contentSize.height - _textDesiredHeight; break;
	default: break;
	}
}

int Label::getFirstCharLen(const std::u32string&, int, int) const { return 1; }

int Label::getFirstWordLen(const std::u32string& utf32Text, int startIndex, int textLen) const
{
	int len = 0;
	auto nextLetterX = 0;
	FontLetterDefinition letterDef;

	for (int index = startIndex; index < textLen; ++index)
	{
		char32_t character = utf32Text[index];

		if (character == StringUtils::UnicodeCharacters::NewLine
			|| (!StringUtils::isUnicodeNonBreaking(character)
				&& (StringUtils::isUnicodeSpace(character) || StringUtils::isCJKUnicode(character))))
			break;

		if (!getFontLetterDef(character, letterDef)) break;

		if (_maxLineWidth > 0.f)
		{
			auto letterX = (nextLetterX + letterDef.offsetX * _bmfontScale);
			if (letterX + letterDef.width * _bmfontScale > _maxLineWidth) break;
		}

		nextLetterX += letterDef.xAdvance * _bmfontScale + _additionalKerning;
		len++;
	}

	if (len == 0 && textLen) len = 1;
	return len;
}

bool Label::getFontLetterDef(char32_t character, FontLetterDefinition& letterDef) const
{
	if (character == StringUtils::UnicodeCharacters::NoBreakSpace)
		character = StringUtils::UnicodeCharacters::Space;
	return _fontAtlas->getLetterDefinitionForChar(character, letterDef);
}

void Label::updateBMFontScale()
{
	auto font = _fontAtlas->getFont();
	if (_currentLabelType == LabelType::BMFONT)
	{
		FontFNT* bmFont = (FontFNT*)font;
		float originalFontSize = bmFont->getOriginalFontSize();
		_bmfontScale = _bmFontSize / originalFontSize;
	}
	else
	{
		_bmfontScale = 1.0f;
	}
}

// ============================================================
// multilineTextWrap
// ============================================================
bool Label::multilineTextWrap(const std::function<int(const std::u32string&, int, int)>& nextTokenLen)
{
	int textLen = getStringLength();
	int lineIndex = 0;
	float nextTokenX = 0.f;
	float nextTokenY = 0.f;
	float longestLine = 0.f;
	float letterRight = 0.f;
	float nextWhitespaceWidth = 0.f;
	float lineSpacing = _lineSpacing;
	float highestY = FLT_MAX;    // Y 向下：顶部最小值，初始最大
	float lowestY = -FLT_MAX;    // Y 向下：底部最大值，初始最小
	FontLetterDefinition letterDef;
	Vec2 letterPosition;
	bool nextChangeSize = true;

	this->updateBMFontScale();

	for (int index = 0; index < textLen; )
	{
		char32_t character = _utf32Text[index];
		if (character == StringUtils::UnicodeCharacters::NewLine)
		{
			_linesWidth.push_back(letterRight);
			letterRight = 0.f;
			lineIndex++;
			nextTokenX = 0.f;
			nextTokenY += _lineHeight * _bmfontScale + lineSpacing;
			recordPlaceholderInfo(index, character);
			index++;
			continue;
		}

		auto tokenLen = nextTokenLen(_utf32Text, index, textLen);
		float tokenHighestY = highestY;
		float tokenLowestY = lowestY;
		float tokenRight = letterRight;
		float nextLetterX = nextTokenX;
		float whitespaceWidth = nextWhitespaceWidth;
		bool newLine = false;

		for (int tmp = 0; tmp < tokenLen; ++tmp)
		{
			int letterIndex = index + tmp;
			character = _utf32Text[letterIndex];
			if (character == StringUtils::UnicodeCharacters::CarriageReturn)
			{
				recordPlaceholderInfo(letterIndex, character);
				continue;
			}
			if (character == StringUtils::UnicodeCharacters::NextCharNoChangeX)
			{
				nextChangeSize = false;
				recordPlaceholderInfo(letterIndex, character);
				continue;
			}
			if (!getFontLetterDef(character, letterDef))
			{
				recordPlaceholderInfo(letterIndex, character);
				continue;
			}

			auto letterX = (nextLetterX + letterDef.offsetX * _bmfontScale);
			if (_enableWrap && _maxLineWidth > 0.f && nextTokenX > 0.f
				&& letterX + letterDef.width * _bmfontScale > _maxLineWidth
				&& !StringUtils::isUnicodeSpace(character) && nextChangeSize)
			{
				_linesWidth.push_back(letterRight - whitespaceWidth);
				nextWhitespaceWidth = 0.f;
				letterRight = 0.f;
				lineIndex++;
				nextTokenX = 0.f;
				nextTokenY += (_lineHeight * _bmfontScale + lineSpacing);
				newLine = true;
				break;
			}
			else
			{
				letterPosition.x = letterX;
			}

			letterPosition.y = (nextTokenY + letterDef.offsetY * _bmfontScale);
			recordLetterInfo(letterPosition, character, letterIndex, lineIndex);

			if (nextChangeSize)
			{
				float newLetterWidth = 0.f;
				if (_horizontalKernings && letterIndex < textLen - 1)
					newLetterWidth = _horizontalKernings[letterIndex + 1];
				newLetterWidth += letterDef.xAdvance * _bmfontScale + _additionalKerning;

				nextLetterX += newLetterWidth;
				tokenRight = nextLetterX;

				if (StringUtils::isUnicodeSpace(character))
					nextWhitespaceWidth += newLetterWidth;
				else
					nextWhitespaceWidth = 0;
			}
			nextChangeSize = true;

			// 只累积 token 级：顶部取小，底部取大
			if (tokenHighestY > letterPosition.y)
				tokenHighestY = letterPosition.y;
			if (tokenLowestY < letterPosition.y + letterDef.height * _bmfontScale)
				tokenLowestY = letterPosition.y + letterDef.height * _bmfontScale;
		}

		if (newLine) continue;

		nextTokenX = nextLetterX;
		letterRight = tokenRight;

		// token 级合并到全局：顶部取小，底部取大
		if (highestY > tokenHighestY) highestY = tokenHighestY;
		if (lowestY < tokenLowestY)   lowestY = tokenLowestY;

		index += tokenLen;
	}

	if (_linesWidth.empty())
	{
		_linesWidth.push_back(letterRight);
		longestLine = letterRight;
	}
	else
	{
		_linesWidth.push_back(letterRight - nextWhitespaceWidth);
		for (auto&& lineWidth : _linesWidth)
			if (longestLine < lineWidth) longestLine = lineWidth;
	}

	_numberOfLines = lineIndex + 1;
	_textDesiredHeight = (_numberOfLines * _lineHeight * _bmfontScale);
	if (_numberOfLines > 1) _textDesiredHeight += (_numberOfLines - 1) * _lineSpacing;

	Size contentSize(_labelWidth, _labelHeight);
	if (_labelWidth <= 0.f)  contentSize.width = longestLine;
	if (_labelHeight <= 0.f) contentSize.height = _textDesiredHeight;
	setContentSize(contentSize);

	// Y 向下：上边界是 0，下边界是 contentSize.height
	_tailoredTopY = 0.f;
	_tailoredBottomY = contentSize.height;

	// highestY 是顶部最小值（负数表示字形顶部超出 Label 顶）
	if (highestY < 0.f) _tailoredTopY = highestY;
	// lowestY 是底部最大值（大于 contentHeight 表示字形底部超出 Label 底）
	if (lowestY > contentSize.height) _tailoredBottomY = lowestY;

	return true;
}

bool Label::multilineTextWrapByWord()
{
	return multilineTextWrap(OG_CALLBACK_3(Label::getFirstWordLen, this));
}

bool Label::multilineTextWrapByChar()
{
	return multilineTextWrap(OG_CALLBACK_3(Label::getFirstCharLen, this));
}

// ============================================================
// isVerticalClamp / isHorizontalClamp
// ============================================================
bool Label::isVerticalClamp()
{
	return _textDesiredHeight > _contentSize.height;
}

bool Label::isHorizontalClamp()
{
	bool letterClamp = false;
	for (int ctr = 0; ctr < _lengthOfString; ++ctr)
	{
		if (_lettersInfo[ctr].valid)
		{
			auto& letterDef = _fontAtlas->_letterDefinitions[_lettersInfo[ctr].utf32Char];
			auto px = _lettersInfo[ctr].positionX + letterDef.width / 2 * _bmfontScale;
			auto lineIndex = _lettersInfo[ctr].lineIndex;

			if (_labelWidth > 0.f)
			{
				if (!this->_enableWrap)
				{
					if (px > _contentSize.width) { letterClamp = true; break; }
				}
				else
				{
					auto wordWidth = this->_linesWidth[lineIndex];
					if (wordWidth > this->_contentSize.width && (px > _contentSize.width))
					{
						letterClamp = true; break;
					}
				}
			}
		}
	}
	return letterClamp;
}

// ============================================================
// shrinkLabelToContentSize
// ============================================================
void Label::shrinkLabelToContentSize(const std::function<bool(void)>& lambda)
{
	float fontSize = this->getRenderingFontSize();
	int i = 0;
	auto letterDefinition = _fontAtlas->_letterDefinitions;
	auto tempLetterDefinition = letterDefinition;
	float originalLineHeight = _lineHeight;
	bool flag = true;

	while (lambda())
	{
		++i;
		float newFontSize = fontSize - i;
		flag = false;
		if (newFontSize <= 0) break;

		float scale = newFontSize / fontSize;
		std::swap(_fontAtlas->_letterDefinitions, tempLetterDefinition);
		_fontAtlas->scaleFontLetterDefinition(scale);
		this->setLineHeight(originalLineHeight * scale);

		if (_maxLineWidth > 0.f && !_lineBreakWithoutSpaces)
			multilineTextWrapByWord();
		else
			multilineTextWrapByChar();

		computeAlignmentOffset();
		tempLetterDefinition = letterDefinition;
	}
	this->setLineHeight(originalLineHeight);
	std::swap(_fontAtlas->_letterDefinitions, letterDefinition);

	if (!flag && fontSize - i >= 0)
		this->scaleFontSizeDown(fontSize - i);
}

// ============================================================
// recordLetterInfo / recordPlaceholderInfo
// ============================================================
void Label::recordLetterInfo(const Vec2& point, char32_t utf32Char, int letterIndex, int lineIndex)
{
	if (static_cast<std::size_t>(letterIndex) >= _lettersInfo.size())
	{
		LetterInfo tmpInfo{};
		_lettersInfo.push_back(tmpInfo);
	}
	_lettersInfo[letterIndex].lineIndex = lineIndex;
	_lettersInfo[letterIndex].utf32Char = utf32Char;
	_lettersInfo[letterIndex].valid = _fontAtlas->_letterDefinitions[utf32Char].validDefinition;
	_lettersInfo[letterIndex].positionX = point.x;
	_lettersInfo[letterIndex].positionY = point.y;
	_lettersInfo[letterIndex].atlasIndex = -1;
	_lettersInfo[letterIndex].textureID = _fontAtlas->_letterDefinitions[utf32Char].textureID;
}

void Label::recordPlaceholderInfo(int letterIndex, char32_t utf32Char)
{
	if ((size_t)letterIndex >= _lettersInfo.size())
	{
		LetterInfo tmpInfo{};
		_lettersInfo.push_back(tmpInfo);
	}
	_lettersInfo[letterIndex].utf32Char = utf32Char;
	_lettersInfo[letterIndex].valid = false;
	_lettersInfo[letterIndex].textureID = 0;
}

OG_END
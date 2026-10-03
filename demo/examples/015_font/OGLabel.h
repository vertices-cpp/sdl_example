#ifndef _OG_LABEL_H_
#define _OG_LABEL_H_
 
#include "OGNode.h"
#include "OGFontAtlas.h"
  
OG_BEGIN

namespace StringUtils {
	extern bool isUnicodeNonBreaking(char32_t ch);
	extern bool isCJKUnicode(char32_t ch);
	extern bool isUnicodeSpace(char32_t ch);

	namespace UnicodeCharacters {
		const char32_t NewLine = 0x000A;
		const char32_t CarriageReturn = 0x000D;
		const char32_t NextCharNoChangeX = 0x0008;
		const char32_t Space = 0x0020;
		const char32_t NoBreakSpace = 0x00A0;
	}

	namespace AsciiCharacters {
		const char NewLine = '\n';
		const char CarriageReturn = '\r';
		const char NextCharNoChangeX = '\b';
		const char Space = ' ';
	}
}

class Renderer;

#define OG_DEFAULT_FONT_LABEL_SIZE  12

// ============================================================
// _calcCharCount：TextFieldTTF 用
// ============================================================
static std::size_t _calcCharCount(const char* text)
{
	int n = 0;
	char ch = 0;
	while ((ch = *text))
	{
		if (!ch) break;
		if (0x80 != (0xC0 & ch)) ++n;
		++text;
	}
	return n;
}

// ============================================================
// TTFConfig
// ============================================================
typedef struct _ttfConfig
{
	std::string fontFilePath;
	float fontSize;

	GlyphCollection glyphs;
	const char* customGlyphs;

	bool distanceFieldEnabled;
	int  outlineSize;

	bool italics;
	bool bold;
	bool underline;
	bool strikethrough;

	Color4B outlineColor = Color4B(0, 0, 0, 255);   // ★ 新增

	_ttfConfig(const std::string& filePath = "",
		float size = OG_DEFAULT_FONT_LABEL_SIZE,
		const GlyphCollection& glyphCollection = GlyphCollection::DYNAMIC,
		const char* customGlyphCollection = nullptr,
		bool useDistanceField = false,
		int outline = 0,
		bool useItalics = false,
		bool useBold = false,
		bool useUnderline = false,
		bool useStrikethrough = false)
		: fontFilePath(filePath)
		, fontSize(size)
		, glyphs(glyphCollection)
		, customGlyphs(customGlyphCollection)
		, distanceFieldEnabled(useDistanceField)
		, outlineSize(outline)
		, italics(useItalics)
		, bold(useBold)
		, underline(useUnderline)
		, strikethrough(useStrikethrough)
	{
		if (outline > 0) distanceFieldEnabled = false;
	}
} TTFConfig;

// ============================================================
// LabelLetter
// ============================================================
struct LabelLetter
{
	Rect     _clip;
	Vec2     _offset;
	Size     _size;
	bool     _rotated = false;
	bool     _visible = true;
	Color3B  _color = Color3B::WHITE;
	uint8_t  _displayedOpacity = 255;
	int      _textureID = 0;
};

// ============================================================
// LineRect
// ============================================================
struct LineRect {
	Rect    rect;
	float   width;
	Color3B color;
};

//class EventListenerCustom;
class TextureAtlas;

class Label : public Node
{
public:
	enum class Overflow { NONE, CLAMP, SHRINK, RESIZE_HEIGHT };

	enum class LabelType {
		TTF,
		BMFONT,
		CHARMAP,
		STRING_TEXTURE
	};

	// ---- create ----
	static Label* create();
	static Label* createWithTTF(const std::string& text, const std::string& fontFilePath, float fontSize,
		const Size& dimensions = Size::ZERO,
		TextHAlignment hAlignment = TextHAlignment::LEFT,
		TextVAlignment vAlignment = TextVAlignment::TOP);
	static Label* createWithTTF(const TTFConfig& ttfConfig, const std::string& text,
		TextHAlignment hAlignment = TextHAlignment::LEFT,
		int maxLineWidth = 0);
	static Label* createWithBMFont(const std::string& bmfontPath, const std::string& text,
		const TextHAlignment& hAlignment = TextHAlignment::LEFT,
		int maxLineWidth = 0,
		const Vec2& imageOffset = Vec2::ZERO);
	static Label* createWithCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
	static Label* createWithCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
	static Label* createWithCharMap(const std::string& plistFile);

	// ---- 字体 ----
	virtual bool setTTFConfig(const TTFConfig& ttfConfig);
	virtual const TTFConfig& getTTFConfig() const { return _fontConfig; }
	virtual bool setBMFontFilePath(const std::string& bmfontFilePath, const Vec2& imageOffset = Vec2::ZERO, float fontSize = 0);
	const std::string& getBMFontFilePath() const { return _bmFontPath; }
	virtual bool setCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
	virtual bool setCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
	virtual bool setCharMap(const std::string& plistFile);

	// ---- 系统字体（TextFieldTTF 用，空实现）----
	virtual void setSystemFontName(const std::string& font);
	virtual const std::string& getSystemFontName() const { return _systemFont; }
	virtual void setSystemFontSize(float fontSize); 
	virtual float getSystemFontSize() const { return _systemFontSize; }
	virtual void requestSystemFontRefresh() { _systemFontDirty = true; }

	

	virtual void enableShadow(const Color4B & color, const Vec2 & offset);
	virtual void disableShadow();
	//----------------描边---------------------
	virtual void enableOutline(const Color4B & color, int size);
	virtual void disableOutline();
	// ---- 文本 ----
	virtual void setString(const std::string& text);
	virtual const std::string& getString() const { return _utf8Text; }
	int getStringNumLines();
	int getStringLength();

	// ---- 颜色 ----
	virtual void setTextColor(const Color4B& color);
	const Color4B& getTextColor() const { return _textColor; }
	virtual void setOpacityModifyRGB(bool isOpacityModifyRGB);
	virtual bool isOpacityModifyRGB() const { return _isOpacityModifyRGB; }
	virtual void updateDisplayedColor(const Color3B& parentColor) override;
	virtual void updateDisplayedOpacity(uint8_t parentOpacity) override;

	// ---- 布局 ----
	void setAlignment(TextHAlignment hAlignment) { setAlignment(hAlignment, _vAlignment); }
	TextHAlignment getTextAlignment() const { return _hAlignment; }
	void setAlignment(TextHAlignment hAlignment, TextVAlignment vAlignment);
	void setHorizontalAlignment(TextHAlignment hAlignment) { setAlignment(hAlignment, _vAlignment); }
	TextHAlignment getHorizontalAlignment() const { return _hAlignment; }
	void setVerticalAlignment(TextVAlignment vAlignment) { setAlignment(_hAlignment, vAlignment); }
	TextVAlignment getVerticalAlignment() const { return _vAlignment; }
	void setLineBreakWithoutSpace(bool breakWithoutSpace);
	void setMaxLineWidth(float maxLineWidth);
	float getMaxLineWidth() { return _maxLineWidth; }
	void setBMFontSize(float fontSize);
	float getBMFontSize() const;
	void enableWrap(bool enable);
	bool isWrapEnabled() const;
	void setOverflow(Overflow overflow);
	Overflow getOverflow() const;
	void setWidth(float width) { setDimensions(width, _labelHeight); }
	float getWidth() const { return _labelWidth; }
	void setHeight(float height) { setDimensions(_labelWidth, height); }
	float getHeight() const { return _labelHeight; }
	void setDimensions(float width, float height);
	const Size& getDimensions() const { return _labelDimensions; }
	void setLineHeight(float height);
	float getLineHeight() const;
	void setLineSpacing(float height);
	float getLineSpacing() const;
	LabelType getLabelType() const { return _currentLabelType; }
	float getRenderingFontSize() const;
	void setAdditionalKerning(float space);
	float getAdditionalKerning() const;

	// ---- 下划线 / 删除线 ----
	void enableUnderline();
	void enableStrikethrough();
	virtual void disableEffect(LabelEffect effect);

	// ---- 渲染 ----
	virtual void visit(Renderer* renderer, const Mat3& parentTransform, uint32_t parentFlags) override;
	bool isQuadVisible(const V2F_C4B_T2F_Quad & q, float localMinX, float localMinY, float localMaxX, float localMaxY);
	virtual void draw(Renderer* renderer, const Mat3& transform, uint32_t flags) override;
	void drawSelf(bool visibleByCamera, Renderer* renderer, uint32_t flags);
	void updateBlendState();

	// ---- 更新 ----
	virtual void updateContent();
	virtual bool alignText();
	bool updateQuads();
	void updateLabelLetters();

	// ---- 查询 ----
	virtual const Size& getContentSize() const  ;
	virtual Rect getBoundingBox() const override;
	virtual std::string getDescription() const;

	// ---- 字体图集 ----
	FontAtlas* getFontAtlas() { return _fontAtlas; }
	virtual void setFontAtlas(FontAtlas* atlas, bool distanceFieldEnabled = false, bool useA8Shader = false);

	// ---- 混合 ----
	virtual const BlendFunc& getBlendFunc() const { return _blendFunc; }
	virtual void setBlendFunc(const BlendFunc& blendFunc);

	// ---- 子节点 ----
	virtual void removeAllChildrenWithCleanup(bool cleanup)  ;
	virtual void removeChild(LabelLetter* child, bool cleanup = true);
 	virtual void setGlobalZOrder(float globalZOrder)  ;
	virtual void setCameraMask(unsigned short mask, bool applyChildren = true) ;

	// ---- getLetter ----
	virtual LabelLetter* getLetter(int letterIndex);

public:
	Label(TextHAlignment hAlignment = TextHAlignment::LEFT,
		TextVAlignment vAlignment = TextVAlignment::TOP);
	virtual ~Label();

	bool initWithTTF(const std::string& text, const std::string& fontFilePath, float fontSize,
		const Size& dimensions = Size::ZERO,
		TextHAlignment hAlignment = TextHAlignment::LEFT,
		TextVAlignment vAlignment = TextVAlignment::TOP);
	bool initWithTTF(const TTFConfig& ttfConfig, const std::string& text,
		TextHAlignment hAlignment = TextHAlignment::LEFT,
		int maxLineWidth = 0);

protected:
	struct LetterInfo
	{
		char32_t utf32Char = 0;
		bool     valid = false;
		float    positionX = 0.0f;
		float    positionY = 0.0f;
		int      atlasIndex = -1;
		int      lineIndex = -1;
		int      textureID = 0;
	};

	// ---- 内部 ----
	bool getFontLetterDef(char32_t character, FontLetterDefinition& letterDef) const;
	void computeStringNumLines();
	void computeAlignmentOffset();
	bool computeHorizontalKernings(const std::u32string& stringToRender);

	bool multilineTextWrapByChar();
	bool multilineTextWrapByWord();
	bool multilineTextWrap(const std::function<int(const std::u32string&, int, int)>& lambda);
	void shrinkLabelToContentSize(const std::function<bool(void)>& lambda);
	bool isHorizontalClamp();
	bool isVerticalClamp();
	bool isHorizontalClamped(float letterPositionX, int lineIndex);
	void rescaleWithOriginalFontSize();

	void recordLetterInfo(const Vec2& point, char32_t utf32Char, int letterIndex, int lineIndex);
	void recordPlaceholderInfo(int letterIndex, char32_t utf32Char);

	void updateBMFontScale();
	void scaleFontSizeDown(float fontSize);
	bool setTTFConfigInternal(const TTFConfig& ttfConfig);
	void setBMFontSizeInternal(float fontSize);
	void restoreFontSize();
	void updateLetterSpriteScale(LabelLetter* sprite);
	int  getFirstCharLen(const std::u32string& utf32Text, int startIndex, int textLen) const;
	int  getFirstWordLen(const std::u32string& utf32Text, int startIndex, int textLen) const;

	void reset();
	virtual void updateColor();

	// ---- 文本与字体 ----
	LabelType _currentLabelType = LabelType::TTF;
	bool _contentDirty = false;
	std::u32string _utf32Text;
	std::string _utf8Text;
	int _numberOfLines = 0;

	std::string _bmFontPath;
	TTFConfig _fontConfig;

	// ---- 系统字体（空实现，TextFieldTTF 用）----
	bool _systemFontDirty = false;
	std::string _systemFont;
	float _systemFontSize = 0.f;

	// ---- 图集与排版 ----
	FontAtlas* _fontAtlas = nullptr;
	std::vector<LetterInfo> _lettersInfo;
	std::vector<LabelLetter*> _letters;
	//std::unordered_map<int, Texture2D*> _textures;
	std::vector<V2F_C4B_T2F_Quad> _quads;

	std::unordered_map<int, std::vector<V2F_C4B_T2F>> _batchVerts;
	std::unordered_map<int, std::vector<int>>         _batchIndices;
	std::vector<QuadCommand> _batchCommands;

	int _lengthOfString = 0;

	// ---- 布局 ----
	float _lineHeight = 0.f;
	float _lineSpacing = 0.f;
	float _additionalKerning = 0.f;
	int*  _horizontalKernings = nullptr;
	bool  _lineBreakWithoutSpaces = false;
	float _maxLineWidth = 0.f;
	Size  _labelDimensions;
	float _labelWidth = 0.f;
	float _labelHeight = 0.f;
	TextHAlignment _hAlignment = TextHAlignment::LEFT;
	TextVAlignment _vAlignment = TextVAlignment::TOP;

	float _textDesiredHeight = 0.f;
	std::vector<float> _linesWidth;
	std::vector<float> _linesOffsetX;
	float _letterOffsetY = 0.f;
	float _tailoredTopY = 0.f;
	float _tailoredBottomY = 0.f;

	// ---- 颜色 ----
	Color4B _textColor = Color4B::WHITE;
	Color4F _textColorF;
	bool _isOpacityModifyRGB = false;

	// ---- 混合 ----
	BlendFunc _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

	// ---- 监听器 ----
// 	EventListenerCustom* _purgeTextureListener = nullptr;
// 	EventListenerCustom* _resetTextureListener = nullptr;

	// ---- 换行与字号 ----
	bool  _enableWrap = false;
	float _bmFontSize = 0.f;
	float _bmfontScale = 1.f;
	Overflow _overflow = Overflow::NONE;
	float _originalFontSize = 0.f;

	// ---- 下划线 / 删除线 ----
	// ---- 下划线 / 删除线 ----
	bool _underlineEnabled = false;              // ★ 加
	bool _strikethroughEnabled = false;
	std::vector<LineRect> _underlineNode;

	QuadCommand _underlineCommand;               // ★ 加
	std::vector<V2F_C4B_T2F> _underlineVerts;    // ★ 加
	std::vector<int>         _underlineIndices;  // ★ 加

	bool    _shadowEnabled = false;
	Color4B _shadowColor = Color4B::BLACK;
	Vec2    _shadowOffset = Vec2(2, -2);


	int     _outlineSize = 0;
	Color4B _outlineColor = Color4B::BLACK;
private:
	OG_DISALLOW_COPY_AND_ASSIGN(Label);
};

OG_END

#endif
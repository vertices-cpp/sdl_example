

#ifndef _OG_LABEL_H_
#define _OG_LABEL_H_

#include "OGRef.h"
#include "OGNode.h"
#include "OGPlatformMacros.h"
#include "OGFontAtlas.h"
#include "OGMathGeometry.h"
#include "OGFontFreeType.h"
#include "OGLabelAtlas.h"


OG_BEGIN

class Renderer;




/**
 * @addtogroup _2d
 * @{
 */

#define OG_DEFAULT_FONT_LABEL_SIZE  12


namespace StringUtils {
	namespace AsciiCharacters {
		const char NewLine = '\n';
		const char CarriageReturn = '\r';
		const char NextCharNoChangeX = '\b';
		const char Space = ' ';
	}
}

static std::size_t _calcCharCount(const char * text)
{
	int n = 0;
	char ch = 0;
	while ((ch = *text))
	{
		if (!ch)
			break;

		if (0x80 != (0xC0 & ch))
		{
			++n;
		}
		++text;
	}
	return n;
}
 

/**
 * @struct TTFConfig
 * @see `GlyphCollection`
 */
typedef struct _ttfConfig
{
    std::string fontFilePath;
    float fontSize;

    GlyphCollection glyphs;
    const char *customGlyphs;

    bool distanceFieldEnabled;
    int outlineSize;

    bool italics;
    bool bold;
    bool underline;
    bool strikethrough;

    _ttfConfig(const std::string& filePath = "",float size = OG_DEFAULT_FONT_LABEL_SIZE, const GlyphCollection& glyphCollection = GlyphCollection::DYNAMIC,
        const char *customGlyphCollection = nullptr, bool useDistanceField = false, int outline = 0,
               bool useItalics = false, bool useBold = false, bool useUnderline = false, bool useStrikethrough = false)
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
        if(outline > 0)
        {
            distanceFieldEnabled = false;
        }
    }
} TTFConfig;


//class Sprite;
class SpriteBatchNode;
class DrawNode;
class EventListenerCustom;
class TextureAtlas; 

class LabelLetter;

class Label :   public LabelProtocol ,public Node
{
public:
    enum class Overflow
    {
        //In NONE mode, the dimensions is (0,0) and the content size will change dynamically to fit the label.
        NONE,
        /**
         *In CLAMP mode, when label content goes out of the bounding box, it will be clipped.
         */
        CLAMP,
        /**
         * In SHRINK mode, the font size will change dynamically to adapt the content size.
         */
        SHRINK,
        /**
         *In RESIZE_HEIGHT mode, you can only change the width of label and the height is changed automatically.
         */
        RESIZE_HEIGHT
    };
    
    enum class LabelType {
        TTF,
        BMFONT,
        CHARMAP,
        STRING_TEXTURE
    };
     
    static Label* create(); 
    static Label* createWithSystemFont(const std::string& text, const std::string& font, float fontSize,
        const Size& dimensions = Size::ZERO, TextHAlignment hAlignment = TextHAlignment::LEFT,
        TextVAlignment vAlignment = TextVAlignment::TOP); 
    static Label * createWithTTF(const std::string& text, const std::string& fontFilePath, float fontSize,
        const Size& dimensions = Size::ZERO, TextHAlignment hAlignment = TextHAlignment::LEFT,
        TextVAlignment vAlignment = TextVAlignment::TOP); 
    static Label* createWithTTF(const TTFConfig& ttfConfig, const std::string& text, 
        TextHAlignment hAlignment = TextHAlignment::LEFT, int maxLineWidth = 0); 
    static Label* createWithBMFont(const std::string& bmfontPath, const std::string& text,
        const TextHAlignment& hAlignment = TextHAlignment::LEFT, int maxLineWidth = 0,
        const Vec2& imageOffset = Vec2::ZERO); 
    static Label * createWithCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
	 
    static Label * createWithCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap); 
    static Label * createWithCharMap(const std::string& plistFile); 
    virtual bool setTTFConfig(const TTFConfig& ttfConfig); 
    virtual const TTFConfig& getTTFConfig() const { return _fontConfig;} 
    virtual bool setBMFontFilePath(const std::string& bmfontFilePath, const Vec2& imageOffset = Vec2::ZERO, float fontSize = 0);
	 
    const std::string& getBMFontFilePath() const { return _bmFontPath;} 
    virtual bool setCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap); 
    virtual bool setCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap); 
    virtual bool setCharMap(const std::string& plistFile); 
    virtual void setSystemFontName(const std::string& font); 
    virtual const std::string& getSystemFontName() const { return _systemFont;} 
    virtual void setSystemFontSize(float fontSize); 
    virtual float getSystemFontSize() const { return _systemFontSize;} 
    virtual void requestSystemFontRefresh() { _systemFontDirty = true;} 
    virtual void setString(const std::string& text) override; 
    virtual const std::string& getString() const override {  return _utf8Text; } 
    int getStringNumLines(); 
    int getStringLength(); 
    virtual void setTextColor(const Color4B &color); 
    const Color4B& getTextColor() const { return _textColor;} 
    virtual void enableShadow(const Color4B& shadowColor = Color4B::BLACK,const Size &offset = Size(2,-2), int blurRadius = 0);
	 
    virtual void enableOutline(const Color4B& outlineColor,int outlineSize = -1); 
    virtual void enableGlow(const Color4B& glowColor); 
    void enableItalics(); 
    void enableBold(); 
    void enableUnderline(); 
    void enableStrikethrough(); 
    virtual void disableEffect(); 
    virtual void disableEffect(LabelEffect effect); 
    bool isShadowEnabled() const { return _shadowEnabled; } 
    Size getShadowOffset() const { return _shadowOffset; } 
    float getShadowBlurRadius() const { return _shadowBlurRadius; } 
    Color4F getShadowColor() const { return _shadowColor4F; } 
    float getOutlineSize() const { return _outlineSize; } 
    LabelEffect getLabelEffectType() const { return _currLabelEffect; } 
    Color4F getEffectColor() const { return _effectColorF; } 
    void setAlignment(TextHAlignment hAlignment) { setAlignment(hAlignment,_vAlignment);} 
    TextHAlignment getTextAlignment() const { return _hAlignment;} 
    void setAlignment(TextHAlignment hAlignment,TextVAlignment vAlignment); 
    void setHorizontalAlignment(TextHAlignment hAlignment) { setAlignment(hAlignment,_vAlignment); } 
    TextHAlignment getHorizontalAlignment() const { return _hAlignment; } 
    void setVerticalAlignment(TextVAlignment vAlignment) { setAlignment(_hAlignment,vAlignment); } 
    TextVAlignment getVerticalAlignment() const { return _vAlignment; } 
    void setLineBreakWithoutSpace(bool breakWithoutSpace); 
    void setMaxLineWidth(float maxLineWidth);
    float getMaxLineWidth() { return _maxLineWidth; } 
    void setBMFontSize(float fontSize); 
    float getBMFontSize()const; 
    void enableWrap(bool enable); 
    bool isWrapEnabled()const; 
    void setOverflow(Overflow overflow); 
    Overflow getOverflow()const; 
    void setWidth(float width) { setDimensions(width,_labelHeight);}
    float getWidth() const { return _labelWidth; } 
    void setHeight(float height){ setDimensions(_labelWidth, height); }
    float getHeight() const { return _labelHeight; } 
    void setDimensions(float width, float height);
    const Size& getDimensions() const{ return _labelDimensions;} 
    virtual void updateContent(); 
    virtual LabelLetter * getLetter(int lettetIndex); 
    void setClipMarginEnabled(bool clipEnabled) { _clipEnabled = clipEnabled; } 
    bool isClipMarginEnabled() const { return _clipEnabled; } 
    void setLineHeight(float height); 
    float getLineHeight() const;

    void setLineSpacing(float height);
    float getLineSpacing() const; 
    LabelType getLabelType() const { return _currentLabelType; } 
    float getRenderingFontSize()const; 
    void setAdditionalKerning(float space); 
    float getAdditionalKerning() const; 

   // virtual void setProgramState(backend::ProgramState *programState) override;

    FontAtlas* getFontAtlas() { return _fontAtlas; }

    virtual const BlendFunc& getBlendFunc() const  { return _blendFunc; }
    virtual void setBlendFunc(const BlendFunc &blendFunc) ;

    virtual bool isOpacityModifyRGB() const  { return _isOpacityModifyRGB; }
    virtual void setOpacityModifyRGB(bool isOpacityModifyRGB) ;
    virtual void updateDisplayedColor(const Color3B& parentColor) ;
    virtual void updateDisplayedOpacity(uint8_t parentOpacity) ;

    virtual std::string getDescription() const ;

    virtual const Size& getContentSize() const ;
    virtual Rect getBoundingBox() const ;

    virtual void visit(Renderer *renderer, const Mat3 &parentTransform, uint32_t parentFlags) ;
    virtual void draw(Renderer *renderer, const Mat3 &transform, uint32_t flags) ;

    virtual void setCameraMask(unsigned short mask, bool applyChildren = true) ;

    virtual void removeAllChildrenWithCleanup(bool cleanup) ;
    virtual void removeChild(Node* child, bool cleanup = true) ;
    virtual void setGlobalZOrder(float globalZOrder) ;

public:
    /**
     * Constructor of Label.
     * @js NA
     */
    Label(TextHAlignment hAlignment = TextHAlignment::LEFT,
      TextVAlignment vAlignment = TextVAlignment::TOP);

    /**
     * Destructor of Label.
     * @js NA
     * @lua NA
     */
    virtual ~Label();

    bool initWithTTF(const std::string& text, const std::string& fontFilePath, float fontSize,
                     const Size& dimensions = Size::ZERO, TextHAlignment hAlignment = TextHAlignment::LEFT,
                     TextVAlignment vAlignment = TextVAlignment::TOP);

    bool initWithTTF(const TTFConfig& ttfConfig, const std::string& text,
                     TextHAlignment hAlignment = TextHAlignment::LEFT, int maxLineWidth = 0);

protected:
    struct LetterInfo
    {
        char32_t utf32Char;
        bool valid;
        float positionX;
        float positionY;
        int atlasIndex;
        int lineIndex;
    };

//     struct BatchCommand {
//         BatchCommand();
//         ~BatchCommand();
// 
//         CustomCommand textCommand;
//         CustomCommand outLineCommand;
//         CustomCommand shadowCommand;
// 
//         std::array<CustomCommand*, 3> getCommandArray();
//     };

    virtual void setFontAtlas(FontAtlas* atlas, bool distanceFieldEnabled = false, bool useA8Shader = false);
    bool getFontLetterDef(char32_t character, FontLetterDefinition& letterDef) const;

    void computeStringNumLines();

    void drawSelf(bool visibleByCamera, Renderer* renderer, uint32_t flags);

    bool multilineTextWrapByChar();
    bool multilineTextWrapByWord();
    bool multilineTextWrap(const std::function<int(const std::u32string&, int, int)>& lambda);
    void shrinkLabelToContentSize(const std::function<bool(void)>& lambda);
    bool isHorizontalClamp();
    bool isVerticalClamp();
    void rescaleWithOriginalFontSize();

    void updateLabelLetters();
    virtual bool alignText();
    void computeAlignmentOffset();
    bool computeHorizontalKernings(const std::u32string& stringToRender);

    void recordLetterInfo(const orange::Vec2& point, char32_t utf32Char, int letterIndex, int lineIndex);
    void recordPlaceholderInfo(int letterIndex, char32_t utf16Char);
    
    bool updateQuads();

    void createSpriteForSystemFont(const FontDefinition& fontDef);
    void createShadowSpriteForSystemFont(const FontDefinition& fontDef);

  //  virtual void updateShaderProgram();
    void updateBMFontScale();
    void scaleFontSizeDown(float fontSize);
    bool setTTFConfigInternal(const TTFConfig& ttfConfig);
    void setBMFontSizeInternal(float fontSize);
    bool isHorizontalClamped(float letterPositionX, int lineIndex);
    void restoreFontSize();
    void updateLetterSpriteScale(LabelLetter* sprite);
    int getFirstCharLen(const std::u32string& utf32Text, int startIndex, int textLen) const;
    int getFirstWordLen(const std::u32string& utf32Text, int startIndex, int textLen) const;

    void reset();

    FontDefinition _getFontDefinition() const;

    virtual void updateColor()  ;
    
//     void updateUniformLocations();
//     void setVertexLayout(PipelineDescriptor& vertexLayout);
//     void updateBlendState();
//     void updateEffectUniforms(BatchCommand &batch, TextureAtlas* textureAtlas, Renderer *renderer, const Mat3 &transform);
//     void updateBuffer(TextureAtlas* textureAtlas, CustomCommand& customCommand);
// 
//     void updateBatchCommand(BatchCommand &batch);

    LabelType _currentLabelType;
    bool _contentDirty;
    std::u32string _utf32Text;
    std::string _utf8Text;
    int _numberOfLines;

    std::string _bmFontPath;
    TTFConfig _fontConfig;
    float _outlineSize;

    bool _systemFontDirty;
    std::string _systemFont;
    float _systemFontSize;
	LabelLetter* _textSprite;//(系统字体）
	LabelLetter* _shadowNode;//(阴影）
	LabelLetter *_reusedLetter;//（插 quad 用）


    FontAtlas* _fontAtlas;
//	Vector<SpriteBatchNode*> _batchNodes;
    Vector<V2F_C4B_T2F_Quad> _quads;
    std::vector<LetterInfo> _lettersInfo;

    //! used for optimization
  
    Rect _reusedRect;
    int _lengthOfString;

    //layout relevant properties.
    float _lineHeight;
    float _lineSpacing;
    float _additionalKerning;
    int* _horizontalKernings;
    bool _lineBreakWithoutSpaces;
    float _maxLineWidth;
    Size _labelDimensions;
    float _labelWidth;
    float _labelHeight;
    TextHAlignment _hAlignment;
    TextVAlignment _vAlignment;

    float _textDesiredHeight;
    std::vector<float> _linesWidth;
    std::vector<float> _linesOffsetX;
    float _letterOffsetY;
    float _tailoredTopY;
    float _tailoredBottomY;

    LabelEffect _currLabelEffect;
    Color4F _effectColorF;
    Color4B _textColor;
    Color4F _textColorF;

    QuadCommand _quadCommand;

 //   std::vector<BatchCommand> _batchCommands;
    
    Mat3  _shadowTransform;
    int  _uniformEffectColor;
    int  _uniformEffectType; // 0: None, 1: Outline, 2: Shadow; Only used when outline is enabled.
    int  _uniformTextColor;
    bool _useDistanceField;
    bool _useA8Shader;

    bool _shadowDirty;
    bool _shadowEnabled;
    Size _shadowOffset;
    
    Color4F _shadowColor4F;
    Color3B _shadowColor3B;
    uint8_t _shadowOpacity;
    float _shadowBlurRadius;

    bool _clipEnabled;
    bool _blendFuncDirty;
    BlendFunc _blendFunc;

    /// whether or not the label was inside bounds the previous frame
    bool _insideBounds;

    bool _isOpacityModifyRGB;

    std::unordered_map<int, LabelLetter*> _letters;

    EventListenerCustom* _purgeTextureListener;
    EventListenerCustom* _resetTextureListener;

#if OG_LABEL_DEBUG_DRAW
    DrawNode* _debugDrawNode;
#endif

    bool _enableWrap;
    float _bmFontSize;
    float _bmfontScale;
    Overflow _overflow;
    float _originalFontSize;

    bool _boldEnabled;
    DrawNode* _underlineNode;
    bool _strikethroughEnabled;
    
//     backend::UniformLocation _mvpMatrixLocation;
//     backend::UniformLocation _textureLocation;
//     backend::UniformLocation _alphaTextureLocation;
//     backend::UniformLocation _textColorLocation;
//     backend::UniformLocation _effectColorLocation;
//     backend::UniformLocation _effectTypeLocation;
    
private:
    OG_DISALLOW_COPY_AND_ASSIGN(Label);
};

// end group
/// @}

OG_END

#endif /*__OG_LABEL_H */

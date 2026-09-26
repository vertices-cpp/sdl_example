

#ifndef __OG_TEXT_FIELD_H__
#define __OG_TEXT_FIELD_H__


#include "OGHeader.h"

#include "OGLabel.h"
#include "OGIMEDelegate.h"

/**
 * @addtogroup ui
 * @{
 */
OG_BEGIN

class TextFieldTTF;


/**
 * A input protocol for TextField.
 */
class TextFieldDelegate
{
public: 
    virtual ~TextFieldDelegate() {} 
    virtual bool onTextFieldAttachWithIME(TextFieldTTF* sender); 
    virtual bool onTextFieldDetachWithIME(TextFieldTTF* sender); 
    virtual bool onTextFieldInsertText(TextFieldTTF* sender, const char* text, size_t nLen); 
    virtual bool onTextFieldDeleteBackward(TextFieldTTF* sender, const char* delText, size_t nLen); 
    virtual bool onVisit(TextFieldTTF* sender, Renderer* renderer, const Mat3& transform, uint32_t flags);
}; 
class   TextFieldTTF : public Label, public IMEDelegate
{
public: 
    TextFieldTTF(); 
    virtual ~TextFieldTTF(); 
    static TextFieldTTF * textFieldWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize);
     
    static TextFieldTTF * textFieldWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize);
     
    bool initWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize);
    
    /** Initializes the TextFieldTTF with a font name and font size. */
    bool initWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize);

    /**
     *@brief    Open keyboard and receive input text.
     */
    virtual bool attachWithIME() override;

    /**
     *@brief    End text input and close keyboard.
     */
    virtual bool detachWithIME() override; 
    TextFieldDelegate* getDelegate() const { return _delegate; } 
    void setDelegate(TextFieldDelegate* delegate) { _delegate = delegate; } 
    std::size_t getCharCount() const { return _charCount; }
     
    virtual const Color4B& getColorSpaceHolder();
	 
    virtual void setColorSpaceHolder(const Color3B& color);
	 
    virtual void setColorSpaceHolder(const Color4B& color); 
    virtual void setTextColor(const Color4B& textColor) override;
	 
    virtual void setString(const std::string& text) override; 
    virtual void appendString(const std::string& text); 
    virtual const std::string& getString() const override; 
    virtual void setPlaceHolder(const std::string& text);

    /**
     * Query the placeholder string.
     *@return The placeholder string.
     */
    virtual const std::string& getPlaceHolder() const; 
    virtual void setSecureTextEntry(bool value);
    virtual void setPasswordTextStyle(const std::string& text);
    const std::string& getPasswordTextStyle() const; 
    virtual bool isSecureTextEntry()const;

    virtual void visit(Renderer *renderer, const Mat3 &parentTransform, uint32_t parentFlags) override;

    virtual void update(float delta)  ; 
    void setCursorEnabled(bool enabled); 
    void setCursorChar(char cursor); 
    void setCursorPosition(std::size_t cursorPosition);

    /**
    * Set cursor position to hit letter, if enabled
    * @js NA
    */
    //void setCursorFromPoint(const Vec2 &point, const Camera* camera);

protected: 
    virtual bool canAttachWithIME() override;
    virtual bool canDetachWithIME() override;
    virtual void didAttachWithIME() override;
    virtual void didDetachWithIME() override;
    virtual void insertText(const char * text, size_t len) override;
    virtual void deleteBackward() override;
    virtual const std::string& getContentText() override;
    virtual void controlKey(EventKeyboard::KeyCode keyCode) override;

    TextFieldDelegate * _delegate;
    std::size_t _charCount;

    std::string _inputText;

    std::string _placeHolder;
    Color4B _colorSpaceHolder;
    Color4B _colorText;

    bool _secureTextEntry;
    std::string _passwordStyleText;

    // Need use cursor
    bool _cursorEnabled;
    // Current position cursor
    std::size_t _cursorPosition;
    // Char showing cursor
    char _cursorChar;
    // >0 - show, <0 - hide
    float _cursorShowingTime;

    bool _isAttachWithIME;

    void makeStringSupportCursor(std::string& displayText);
    void updateCursorDisplayText();
    void setAttachWithIME(bool isAttachWithIME);
    void setTextColorInternally(const Color4B& color);

private:
    class LengthStack;
    LengthStack * _lens;
};

OG_END
// end of ui group
/// @}

#endif    // __OG_TEXT_FIELD_H__

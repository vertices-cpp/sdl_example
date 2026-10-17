

#include "ui/UITextBMFont.h"
#include "2d/OGLabel.h"
#include "editor-support/ui-extension/UIExtension.h"

OG_BEGIN

namespace ui {
    
static const int LABELBMFONT_RENDERER_Z = (-1);
    
IMPLEMENT_CLASS_GUI_INFO(TextBMFont)
    
TextBMFont::TextBMFont():
_labelBMFontRenderer(nullptr),
_fntFileName(""),
_stringValue(""),
_labelBMFontRendererAdaptDirty(true)
{
}

TextBMFont::~TextBMFont()
{
    
}

TextBMFont* TextBMFont::create()
{
    TextBMFont* widget = new (std::nothrow) TextBMFont();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}
    
TextBMFont* TextBMFont::create(const std::string &text, const std::string &filename)
{
    TextBMFont* widget = new (std::nothrow) TextBMFont();
    if (widget && widget->init())
    {
        widget->setFntFile(filename);
        widget->setString(text);
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}

void TextBMFont::initRenderer()
{
    _labelBMFontRenderer = orange::Label::create();
    addProtectedChild(_labelBMFontRenderer, LABELBMFONT_RENDERER_Z, -1);
}

void TextBMFont::setFntFile(const std::string& fileName)
{
    if (fileName.empty())
    {
        return;
    }
    _fntFileName = fileName;
    _labelBMFontRenderer->setBMFontFilePath(fileName);
    
    updateContentSizeWithTextureSize(_labelBMFontRenderer->getContentSize());
    _labelBMFontRendererAdaptDirty = true;
}

void TextBMFont::setString(const std::string& value)
{
    if (value == _labelBMFontRenderer->getString())
    {
        return;
    }
    _stringValue = value;
    _labelBMFontRenderer->setString(value);
    updateContentSizeWithTextureSize(_labelBMFontRenderer->getContentSize());
    _labelBMFontRendererAdaptDirty = true;
}

const std::string& TextBMFont::getString()const
{
    return _stringValue;
}
    
ssize_t TextBMFont::getStringLength()const
{
    return _labelBMFontRenderer->getStringLength();
}

void TextBMFont::onSizeChanged()
{
    Widget::onSizeChanged();
    _labelBMFontRendererAdaptDirty = true;
}
    
void TextBMFont::adaptRenderers()
{
    if (_labelBMFontRendererAdaptDirty)
    {
        labelBMFontScaleChangedWithSize();
        _labelBMFontRendererAdaptDirty = false;
    }
}

Size TextBMFont::getVirtualRendererSize() const
{
    return _labelBMFontRenderer->getContentSize();
}

Node* TextBMFont::getVirtualRenderer()
{
    return _labelBMFontRenderer;
}

void TextBMFont::labelBMFontScaleChangedWithSize()
{
    if (_ignoreSize)
    {
        _labelBMFontRenderer->setScale(1.0f);
    }
    else
    {
        Size textureSize = _labelBMFontRenderer->getContentSize();
        if (textureSize.width <= 0.0f || textureSize.height <= 0.0f)
        {
            _labelBMFontRenderer->setScale(1.0f);
            return;
        }
        float scaleX = _contentSize.width / textureSize.width;
        float scaleY = _contentSize.height / textureSize.height;
        _labelBMFontRenderer->setScaleX(scaleX);
        _labelBMFontRenderer->setScaleY(scaleY);
    }
    _labelBMFontRenderer->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
}

std::string TextBMFont::getDescription() const
{
    return "TextBMFont";
}

Widget* TextBMFont::createCloneInstance()
{
    return TextBMFont::create();
}

void TextBMFont::copySpecialProperties(Widget *widget)
{
    TextBMFont* labelBMFont = dynamic_cast<TextBMFont*>(widget);
    if (labelBMFont)
    {
        setFntFile(labelBMFont->_fntFileName);
        setString(labelBMFont->_stringValue);
    }
}

ResourceData TextBMFont::getRenderFile()
{
    ResourceData rData;
    rData.type = 0;
    rData.file = _fntFileName;
    return rData;
}

void TextBMFont::resetRender()
{
    this->removeProtectedChild(_labelBMFontRenderer);
    this->initRenderer();
}
}

OG_END

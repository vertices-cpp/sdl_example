
#include "ui/UIRelativeBox.h"

OG_BEGIN

namespace ui{
    
RelativeBox::RelativeBox()
{
}

RelativeBox::~RelativeBox()
{
}

RelativeBox* RelativeBox::create()
{
    RelativeBox* widget = new (std::nothrow) RelativeBox();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}

RelativeBox* RelativeBox::create(const orange::Size &size)
{
    RelativeBox* widget = new (std::nothrow) RelativeBox();
    if (widget && widget->initWithSize(size))
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}

bool RelativeBox::init()
{
    if (Layout::init())
    {
        setLayoutType(Layout::Type::RELATIVE);
        return true;
    }
    return false;
}

bool RelativeBox::initWithSize(const Size& size)
{
    if (init())
    {
        setContentSize(size);
        return true;
    }
    return false;
}
    
}

OG_END

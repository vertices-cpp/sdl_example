
#include "ui/UIHBox.h"

OG_BEGIN

namespace ui{
    
HBox::HBox()
{
}
    
HBox::~HBox()
{
}
    
HBox* HBox::create()
{
    HBox* widget = new (std::nothrow) HBox();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}
    
HBox* HBox::create(const orange::Size &size)
{
    HBox* widget = new (std::nothrow) HBox();
    if (widget && widget->initWithSize(size))
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}
    
bool HBox::init()
{
    if (Layout::init())
    {
        setLayoutType(Layout::Type::HORIZONTAL);
        return true;
    }
    return false;
}
    
bool HBox::initWithSize(const Size& size)
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

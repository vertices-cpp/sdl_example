

#include "ui/UIVBox.h"

OG_BEGIN

namespace ui{
    
VBox::VBox()
{
}

VBox::~VBox()
{
}

VBox* VBox::create()
{
    VBox* widget = new (std::nothrow) VBox();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}

VBox* VBox::create(const orange::Size &size)
{
    VBox* widget = new (std::nothrow) VBox();
    if (widget && widget->initWithSize(size))
    {
        widget->autorelease();
        return widget;
    }
    OG_SAFE_DELETE(widget);
    return nullptr;
}

bool VBox::init()
{
    if (Layout::init())
    {
        setLayoutType(Layout::Type::VERTICAL);
        return true;
    }
    return false;
}

bool VBox::initWithSize(const Size& size)
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

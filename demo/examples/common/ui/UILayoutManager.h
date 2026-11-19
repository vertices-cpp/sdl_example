

#ifndef _UI_LAYOUT_MANAGER_H_
#define _UI_LAYOUT_MANAGER_H_

#include "base/OGRef.h"
#include "base/OGVector.h"
#include "ui/GUIExport.h"

/**
 * @addtogroup ui
 * @{
 */
OG_BEGIN

namespace ui {
    
class LayoutProtocol;
class Widget;
class RelativeLayoutParameter;

/**
 *@brief Base class for managing layout.
 * All the concrete layout manager should inherit from this class.
 */
class LayoutManager : public Ref
{
public:
    virtual ~LayoutManager(){};
    LayoutManager(){};
    
    /**
     * The interface does the actual layouting work.
     */
    virtual void doLayout(LayoutProtocol *layout) = 0;
    
    friend class Layout;
};

/**
 *@brief Linear vertical layouting class.
 * Note: This class is used only by @see `Layout` class.
 * @lua NA
 * @js NA
 */
class LinearVerticalLayoutManager : public LayoutManager
{
private:
    LinearVerticalLayoutManager(){};
    virtual ~LinearVerticalLayoutManager(){};
    static LinearVerticalLayoutManager* create();
    virtual void doLayout(LayoutProtocol *layout) override;
    
    friend class Layout;
};

/**
 *@brief Linear horizontal layouting class.
 *Note: This class is used only by @see `Layout` class.
 * @lua NA
 * @js NA
 */
class LinearHorizontalLayoutManager : public LayoutManager
{
private:
    LinearHorizontalLayoutManager(){};
    virtual ~LinearHorizontalLayoutManager(){};
    static LinearHorizontalLayoutManager* create();
    virtual void doLayout(LayoutProtocol *layout) override;
    
    friend class Layout;
};

/**
 *@brief Relative layouting class.
 *Note: This class is used only by `Layout` class.
 * @lua NA
 * @js NA
 */
class RelativeLayoutManager : public LayoutManager
{
private:
    RelativeLayoutManager()
    :_unlayoutChildCount(0),
    _widget(nullptr),
    _finalPositionX(0.0f),
    _finalPositionY(0.0f),
    _relativeWidgetLP(nullptr)
    {}
    virtual ~RelativeLayoutManager(){};
    static RelativeLayoutManager* create();
    virtual void doLayout(LayoutProtocol *layout) override;
    
    Vector<Widget*> getAllWidgets(LayoutProtocol *layout);
    Widget* getRelativeWidget(Widget* widget);
    bool calculateFinalPositionWithRelativeWidget(LayoutProtocol *layout);
    void calculateFinalPositionWithRelativeAlign();

    ssize_t _unlayoutChildCount;
    Vector<Widget*> _widgetChildren;
    Widget* _widget;
    float _finalPositionX;
    float _finalPositionY;
    
    RelativeLayoutParameter* _relativeWidgetLP;
    
    friend class Layout;
};

}

OG_END
// end of ui group
/// @}
#endif /* defined(_UI_LAYOUT_MANAGER_H_) */

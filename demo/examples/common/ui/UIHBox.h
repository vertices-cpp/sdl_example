

#ifndef __UIHBox_H__
#define __UIHBox_H__

#include "ui/UILayout.h"
#include "ui/GUIExport.h"

OG_BEGIN

/**
 * @addtogroup ui
 * @{
 */
namespace ui {

/**
 * HBox is just a convenient wrapper class for horizontal layout type.
 * HBox lays out its children in a single horizontal row.
 */
class HBox : public Layout{
public:
    
    /**
     * Default constructor
     * @js ctor
     * @lua new
     */
    HBox();
    
    /**
     * Default destructor
     * @js NA
     * @lua NA
     */
    virtual ~HBox();
    
    /**
     * Create a empty HBox instance.
     * @return A HBox instance pointer.
     */
    static HBox* create();
    
    /**
     *Create a HBox with a certain size.
     *@param size The content size of the layout.
     *@return A HBox instance pointer.
     */
    static HBox* create(const Size& size);
    
OG_CONSTRUCTOR_ACCESS:
    //initializes state of widget.
    virtual bool init() override;
    virtual bool initWithSize(const Size& size);
};
    
}

// end of ui group
/// @}
OG_END

#endif /* defined(__UIHBox__) */

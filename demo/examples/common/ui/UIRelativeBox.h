

#ifndef __UIRelativeBox_H__
#define __UIRelativeBox_H__

#include "ui/UILayout.h"
#include "ui/GUIExport.h"

OG_BEGIN
/**
 * @addtogroup ui
 * @{
 */

namespace ui {
    
/**
 *@brief RelativeBox is just a convenient wrapper class for relative layout type.
 * RelativeBox lays out its children relative to a widget or a position.
 */
class RelativeBox : public Layout{
    
    
public:
    
    /**
     * Default constructor.
     * @js ctor
     * @lua new
     */
    RelativeBox();
    
    /**
     * Default destructor.
     * @js NA
     * @lua NA
     */
    virtual ~RelativeBox();
    
    /**
     * Create an empty RelativeBox instance.
     * @return A RelativeBox instance.
     */
    static RelativeBox* create();
    
    /**
     * @brief Create a RelativeBox with a fixed size.
     *
     * @param size Size in `Size`.
     * @return A RelativeBox instance.
     */
    static RelativeBox* create(const Size& size);
    
OG_CONSTRUCTOR_ACCESS:
    //initializes state of widget.
    virtual bool init() override;
    virtual bool initWithSize(const Size& size);
};
    
}

// end of ui group
/// @}
OG_END

#endif /* defined(__UIRelativeBox__) */



#ifndef _UI_VBox_H_
#define _UI_VBox_H_

#include "ui/UILayout.h"
#include "ui/GUIExport.h"

OG_BEGIN


namespace ui {
    

class VBox : public Layout{
public:
    VBox();
    
    /**
     * Default destructor
     * @js NA
     * @lua NA
     */
    virtual ~VBox();
    
    /**
     * Create a empty VBox instance.
     * @return A VBox instance pointer.
     */
    static VBox* create();
    
    /**
     * Create a VBox with a certain size.
     *@param size The content size of the layout.
     *@return A VBox instance pointer.
     */
    static VBox* create(const Size& size);
public:
    //initializes state of widget.
    virtual bool init() override;
    virtual bool initWithSize(const Size& size);
};
    
}

// end of ui group
/// @}
OG_END

#endif /* defined(_UI_VBox_H_) */

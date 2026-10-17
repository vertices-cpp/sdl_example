

#ifndef __UILABELBMFONT_H__
#define __UILABELBMFONT_H__

#include "ui/UIWidget.h"
#include "ui/GUIExport.h"

/**
 * @addtogroup ui
 * @{
 */
OG_BEGIN

class Label;
struct ResourceData;

namespace ui {
    
/**
 * A widget for displaying BMFont label.
 */    
class TextBMFont : public Widget
{
    
    DECLARE_CLASS_GUI_INFO
    
public:
    /**
     * Default constructor
     * @js ctor
     * @lua new
     */
    TextBMFont();
    
    /**
     * Default destructor
     * @js NA
     * @lua NA
     */
    virtual ~TextBMFont();
    
    /**
     * Allocates and initializes.
     */
    static TextBMFont* create();
    
    static TextBMFont* create(const std::string& text, const std::string& filename);
    
    /** init a bitmap font atlas with an initial string and the FNT file */
    void setFntFile(const std::string& fileName);
    
    /** set string value for labelbmfont*/
    void setString(const std::string& value);
    
    /** get string value for labelbmfont*/
    const std::string& getString()const;
    
    /**
     * Gets the string length of the label.
     * Note: This length will be larger than the raw string length,
     * if you want to get the raw string length, you should call this->getString().size() instead
     *
     * @return  string length.
     */
    ssize_t getStringLength()const;

    virtual Size getVirtualRendererSize() const override;
    virtual Node* getVirtualRenderer() override;
    /**
     * Returns the "class name" of widget.
     */
    virtual std::string getDescription() const override;

    ResourceData getRenderFile();

    /**
    * reset TextBMFont inner label
    */
    void resetRender();
protected:
    virtual void initRenderer() override;
    virtual void onSizeChanged() override;
   
    void labelBMFontScaleChangedWithSize();
    virtual Widget* createCloneInstance() override;
    virtual void copySpecialProperties(Widget* model) override;
    virtual void adaptRenderers() override;
protected:
    Label* _labelBMFontRenderer;
    std::string _fntFileName;
    std::string _stringValue;
    bool _labelBMFontRendererAdaptDirty;
};
    
}
OG_END
// end of ui group
/// @}

#endif /* defined(__LabelBMFont__) */

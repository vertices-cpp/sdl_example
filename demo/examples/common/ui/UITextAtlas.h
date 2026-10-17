

#ifndef __UILABELATLAS_H__
#define __UILABELATLAS_H__

#include "ui/UIWidget.h"
#include "ui/GUIExport.h"

OG_BEGIN

/**
 * @addtogroup ui
 * @{
 */

class Label;
struct ResourceData;

namespace ui {
    
/**
 * @brief UI TextAtlas widget.
 */
class TextAtlas : public Widget
{
    
    DECLARE_CLASS_GUI_INFO
    
public:
    /**
     * Default constructor.
     *
     * @lua new
     */
    TextAtlas();
    
    /**
     * Default destructor.
     *
     * @lua NA
     */
    virtual ~TextAtlas();
    
    /**
     * Create a TexAtlas object.
     *
     * @return An autoreleased TextAtlas object.
     */
    static TextAtlas* create();
    
    /**
     * Create a LabelAtlas from a char map file.
     *
     * @param stringValue A given string needs to be displayed.
     * @param charMapFile A given char map file name.
     * @param itemWidth The element width.
     * @param itemHeight The element height.
     * @param startCharMap The starting char of the atlas.
     * @return An autoreleased TextAtlas object.
     */
    static TextAtlas* create(const std::string& stringValue,
                             const std::string& charMapFile,
                             int itemWidth,
                             int itemHeight,
                             const std::string& startCharMap);
    
    /** Initializes the LabelAtlas with a string, a char map file(the atlas), the width and height of each element and the starting char of the atlas.
     *
     * @param stringValue A given string needs to be displayed.
     * @param charMapFile A given char map file name.
     * @param itemWidth The element width.
     * @param itemHeight The element height.
     * @param startCharMap The starting char of the atlas.
     */
    
    void setProperty(const std::string& stringValue,
                     const std::string& charMapFile,
                     int itemWidth,
                     int itemHeight,
                     const std::string& startCharMap);
    
    /**Set string value for labelatlas.
     *
     * @param value A given string needs to be displayed.
     */
    void setString(const std::string& value);
    
    /**Get string value for labelatlas.
     *
     * @return The string value of TextAtlas.
     */
    const std::string& getString() const;
    
    /**
     * Gets the string length of the label.
     * Note: This length will be larger than the raw string length,
     * if you want to get the raw string length, you should call this->getString().size() instead
     *
     * @return  string length.
     */
    ssize_t getStringLength()const;
    
    //override "getVirtualRendererSize" method of widget.
    virtual Size getVirtualRendererSize() const override;
    
    //override "getVirtualRenderer" method of widget.
    virtual Node* getVirtualRenderer() override;
    
    /**
     * Returns the "class name" of widget.
     */
    virtual std::string getDescription() const override;
    
    /**
     * @js NA
     */
    virtual void adaptRenderers() override;

    ResourceData getRenderFile();

protected:
    virtual void initRenderer() override;
    virtual void onSizeChanged() override;
  
    void labelAtlasScaleChangedWithSize();
    virtual Widget* createCloneInstance() override;
    virtual void copySpecialProperties(Widget* model) override;
protected:
    Label* _labelAtlasRenderer;
    std::string _stringValue;
    std::string _charMapFileName;
    int _itemWidth;
    int _itemHeight;
    std::string _startCharMap;
    bool _labelAtlasRendererAdaptDirty;
};

}

// end of ui group
/// @}

OG_END

#endif /* defined(__Orange=GUI__LabelAtlas__) */

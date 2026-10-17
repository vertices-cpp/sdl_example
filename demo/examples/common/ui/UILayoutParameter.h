

#ifndef __LAYOUTPARMETER_H__
#define __LAYOUTPARMETER_H__

#include <string>
#include "base/OGRef.h"
#include "ui/GUIExport.h"

/**
 * @addtogroup ui
 * @{
 */
OG_BEGIN

namespace ui {

/**
 *@brief Margin of widget's in point. Margin value should be positive.
 *@lua NA
 */
class Margin
{
public:
    /**
     * Left margin.
     */
    float left;
    /**
     * Top margin.
     */
    float top;
    /**
     * Right margin.
     */
    float right;
    /**
     * Bottom margin.
     */
    float bottom;
    
public:
    /**
     * Default constructor.
     */
    Margin();

    /**
     * Construct a Margin instance with left, top, right and bottom margins.
     *@param l Left margin in float.
     *@param t Top margin in float.
     *@param r Right margin in float.
     *@param b Bottom margin in float.
     */
    Margin(float l, float t, float r, float b);

    /**
     * Copy constructor.
     */
    Margin(const Margin& other);

    /**
     * Copy assignment operator.
     */
    Margin& operator= (const Margin& other);

    /**
     * Change margin with left, top, right and bottom margin.
     *@param l Left margin in float.
     *@param t Top margin in float.
     *@param r Right margin in float.
     *@param b Bottom margin in float.
     */
    void setMargin(float l, float t, float r, float b);

    /**
     * Test equality of two margins.
     *@param target A Margin instance.
     *@return True if two margins are equal, false otherwise.
     */
    bool equals(const Margin& target) const;
    
    /**
     * A margin constant with all margins equal zero.
     */
    static const Margin ZERO;

};

/**
 *@brief Base class for various LayoutParameter.
 */
class LayoutParameter : public Ref
{
public:
    /**
     *Layout parameter type.
     * There are mainly two types:
     * - Linear: Elements will  be arranged by margin.
     * - Relative: Elements will be arranged by margin and relative widget name.
     */
    enum class Type
    {
        NONE = 0,
        LINEAR,
        RELATIVE
    };
    /**
     * Default constructor.
     *
     * @lua new
     */
    LayoutParameter() : _margin(Margin())
    {
        _layoutParameterType = Type::NONE;
    }
    
    /**
     * Default destructor.
     * @lua NA
     */
    virtual ~LayoutParameter(){};
    
    /**
     * Create a empty LayoutParameter.
     * @return A autorelease LayoutParameter instance.
     */
    static LayoutParameter* create();
    
    /**
     * Set margin parameter for LayoutParameter.
     * 
     * @see Margin
     * @param margin
     */
    void setMargin(const Margin& margin);
    
    /**
     * Gets margin parameter of LayoutParameter.
     *
     * @see Margin
     * @return Margin of layout parameter.
     */
    const Margin& getMargin() const;
    
    /**
     * Gets LayoutParameterType of LayoutParameter.
     *
     * @see LayoutParameterType.
     * @return LayoutParameterType
     */
    Type getLayoutType() const;
    
    /**
     * Create a copy of original LayoutParameter.
     *@return A LayoutParameter pointer.
     */
    LayoutParameter* clone();

    /**
     * Create a cloned instance of LayoutParameter.
     *@return A LayoutParameter pointer.
     */
    virtual LayoutParameter* createCloneInstance();
    
    /**
     * Copy all the member field from argument LayoutParameter to self.
     *@param model A LayoutParameter instance.
     */
    virtual void copyProperties(LayoutParameter* model);
protected:
    Margin _margin;
    Type _layoutParameterType;
};
    
/**
 * Protocol for getting a LayoutParameter.
 * Every element want to have layout parameter should inherit from this class.
 */
class LayoutParameterProtocol
{
public:
    /**
     * Default destructor.
     */
    virtual ~LayoutParameterProtocol(){}

    /**
     *
     *@return A LayoutParameter and its descendant pointer.
     */
    virtual LayoutParameter* getLayoutParameter() const= 0;
};

    
/**
 * @brief Linear layout parameter.
 * It is used by linear layout manager for arranging elements linearly.
 */
class LinearLayoutParameter : public LayoutParameter
{
public:
    /**
     * Linear gravity.
     */
    enum class LinearGravity
    {
        NONE,
        LEFT,
        TOP,
        RIGHT,
        BOTTOM,
        CENTER_VERTICAL,
        CENTER_HORIZONTAL
    };

    /**
     * Default constructor.
     *
     * @lua new
     */
    LinearLayoutParameter()
    : _linearGravity(LinearGravity::NONE)
    {
        _layoutParameterType = Type::LINEAR;
    }
    
    /**
     * Default destructor.
     *
     * @lua NA
     */
    virtual ~LinearLayoutParameter(){};
    
    /**
     * Create a empty LinearLayoutParameter instance.
     * @return A initialized LayoutParameter which is marked as "autorelease".
     */
    static LinearLayoutParameter* create();
    
    /**
     * Sets LinearGravity parameter for LayoutParameter.
     *
     * @see LinearGravity
     * @param gravity Gravity in LinearGravity.
     */
    void setGravity(LinearGravity gravity);
    
    /**
     * Gets LinearGravity parameter for LayoutParameter.
     *
     * @see LinearGravity
     * @return LinearGravity
     */
    LinearGravity getGravity() const;

    //override functions.
    virtual LayoutParameter* createCloneInstance() override;
    virtual void copyProperties(LayoutParameter* model) override;
protected:
    LinearGravity _linearGravity;
    int i;
};
    
    

/**
 * @brief Relative layout parameter.
 * It is mainly used by  `RelativeLayoutManager`.
 */
class RelativeLayoutParameter : public LayoutParameter
{
public:
    /**
     * Relative Alignment type
     */
    enum class RelativeAlign
    {
        NONE,
        PARENT_TOP_LEFT,
        PARENT_TOP_CENTER_HORIZONTAL,
        PARENT_TOP_RIGHT,
        PARENT_LEFT_CENTER_VERTICAL,
        
        CENTER_IN_PARENT,
        
        PARENT_RIGHT_CENTER_VERTICAL,
        PARENT_LEFT_BOTTOM,
        PARENT_BOTTOM_CENTER_HORIZONTAL,
        PARENT_RIGHT_BOTTOM,
        
        LOCATION_ABOVE_LEFTALIGN,
        LOCATION_ABOVE_CENTER,
        LOCATION_ABOVE_RIGHTALIGN,
        LOCATION_LEFT_OF_TOPALIGN,
        LOCATION_LEFT_OF_CENTER,
        LOCATION_LEFT_OF_BOTTOMALIGN,
        LOCATION_RIGHT_OF_TOPALIGN,
        LOCATION_RIGHT_OF_CENTER,
        LOCATION_RIGHT_OF_BOTTOMALIGN,
        LOCATION_BELOW_LEFTALIGN,
        LOCATION_BELOW_CENTER,
        LOCATION_BELOW_RIGHTALIGN
    };

    /**
     * Default constructor
     *
     * @lua new
     */
    RelativeLayoutParameter()
    : _relativeAlign(RelativeAlign::NONE),
    _relativeWidgetName(""),
    _relativeLayoutName(""),
    _put(false)
    {
        _layoutParameterType = Type::RELATIVE;
    }
    
    /**
     * Default destructor
     *
     * @lua NA
     */
    virtual ~RelativeLayoutParameter(){};
    
    /**
     * Create a RelativeLayoutParameter instance.
     * @return A initialized LayoutParameter which is marked as "autorelease".
     */
    static RelativeLayoutParameter* create();
    
    /**
     * Sets RelativeAlign parameter for LayoutParameter.
     *
     * @see RelativeAlign
     * @param align Relative align in  `RelativeAlign`.
     */
    void setAlign(RelativeAlign align);
    
    /**
     * Get RelativeAlign parameter for LayoutParameter.
     *
     * @see RelativeAlign
     * @return  A RelativeAlign variable.
     */
    RelativeAlign getAlign() const;
    
    /**
     * Set widget name your widget want to relative to.
     *
     * @param name Relative widget name.
     */
    void setRelativeToWidgetName(const std::string& name);
    
    /**
     * Get the relative widget name.
     * @return name A relative widget name in string.
     */
    const std::string& getRelativeToWidgetName() const;
    
    /**
     * Set a name for LayoutParameter in Relative Layout.
     *
     * @param name A string name.
     */
    void setRelativeName(const std::string& name);
    
    /**
     * Get a name of LayoutParameter in Relative Layout.
     *
     * @return name Relative name in string.
     */
    const std::string& getRelativeName() const;
    
    //override functions.
    virtual LayoutParameter* createCloneInstance() override;
    virtual void copyProperties(LayoutParameter* model) override;
protected:
    RelativeAlign _relativeAlign;
    std::string _relativeWidgetName;
    std::string _relativeLayoutName;
    bool _put;
    friend class RelativeLayoutManager;
};

}
OG_END
// end of ui group
/// @}

#endif /* defined(__LayoutParameter__) */

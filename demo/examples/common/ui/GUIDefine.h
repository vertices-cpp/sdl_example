
#ifndef __TestCpp__GUIDefine__
#define __TestCpp__GUIDefine__

#include <string>
#include "base/ObjectFactory.h"

///@cond DO_NOT_SHOW
//
//// Widget macro
//

#define DECLARE_CLASS_GUI_INFO \
    public: \
    static orange::ObjectFactory::TInfo __Type; \
    static orange::Ref* createInstance(); \

#define IMPLEMENT_CLASS_GUI_INFO(className) \
    orange::Ref* className::createInstance() \
    { \
        return className::create(); \
    } \
    orange::ObjectFactory::TInfo className::__Type(#className, &className::createInstance); \

#define CREATE_CLASS_GUI_INFO(className) \
    orange::ObjectFactory::TInfo(#className, &className::createInstance) \


//
//// Reader macro
//

#define DECLARE_CLASS_WIDGET_READER_INFO \
    public: \
    static orange::ObjectFactory::TInfo __Type; \
    static orange::Ref* createInstance(); \

#define IMPLEMENT_CLASS_WIDGET_READER_INFO(className) \
    orange::Ref* className::createInstance() \
    { \
        return className::getInstance(); \
    } \
    orange::ObjectFactory::TInfo className::__Type(#className, &className::createInstance); \

#define CREATE_CLASS_WIDGET_READER_INFO(className) \
    orange::ObjectFactory::TInfo(#className, &className::createInstance) \

#define OG_VIDEOPLAYER_DEBUG_DRAW  0

#define __LAYOUT_COMPONENT_NAME "__ui_layout"
///@endcond

#endif /* defined(__TestCpp__GUIDefine__) */

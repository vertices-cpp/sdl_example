 

#include "ui/UIEditBox/UIEditBox.h"

#if (OG_TARGET_PLATFORM != OG_PLATFORM_ANDROID ) && (OG_TARGET_PLATFORM != OG_PLATFORM_IOS ) && (OG_TARGET_PLATFORM != OG_PLATFORM_WIN32) && (OG_TARGET_PLATFORM != OG_PLATFORM_MAC)

OG_BEGIN

namespace ui {

EditBoxImpl* __createSystemEditBox(EditBox* pEditBox)
{
    return nullptr;
}

}

OG_END

#endif /* #if (..) */

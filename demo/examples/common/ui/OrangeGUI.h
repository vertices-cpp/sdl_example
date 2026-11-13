

#ifndef _ORANGE_GUI_H_
#define _ORANGE_GUI_H_


#include "ui/UIWidget.h"
#include "ui/UILayout.h"
#include "ui/UIButton.h"
#include "ui/UICheckBox.h"
#include "ui/UIRadioButton.h"
#include "ui/UIImageView.h"
#include "ui/UIText.h"
#include "ui/UITextAtlas.h"
#include "ui/UILoadingBar.h"
#include "ui/UIScrollView.h"
#include "ui/UIListView.h"
#include "ui/UISlider.h"
#include "ui/UITextField.h"
#include "ui/UITextBMFont.h"
#include "ui/UIPageView.h"
#include "ui/UIHelper.h"
#include "ui/UIRichText.h"
#include "ui/UIHBox.h"
#include "ui/UIVBox.h"
#include "ui/UIRelativeBox.h"
#if (OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID || OG_TARGET_PLATFORM == OG_PLATFORM_IOS)
#include "ui/UIVideoPlayer.h"
#include "ui/UIWebView/UIWebView.h"
#endif
#include "ui/GUIExport.h"
#include "ui/UIScale9Sprite.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "ui/UILayoutComponent.h"
#include "ui/UITabControl.h"
#include "editor-support/ui-extension/UIExtension.h"

/**
 * @addtogroup ui
 * @{
 */
OG_BEGIN
namespace ui {


/**
 * Get current cocos GUI module version string.
 *@return A string representation of GUI module version number
 */
const char* OrangeGUIVersion();

}

OG_END
// end of ui group
/// @}

#endif /* defined(_ORANGE_GUI_H_) */

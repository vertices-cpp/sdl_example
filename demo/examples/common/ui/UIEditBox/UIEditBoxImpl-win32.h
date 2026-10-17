 

#ifndef __UIEditBoxIMPLWIN_H__
#define __UIEditBoxIMPLWIN_H__

#include "platform/OGPlatformConfig.h"
#include "base/ogMacros.h"
#include "platform/OGPlatformMacros.h"

#if (OG_TARGET_PLATFORM == OG_PLATFORM_WIN32)
//#include "ui/UIEditBox/UIEditBoxImpl-common.h"

OG_BEGIN

namespace ui {

    class EditBox;

    class EditBoxImplWin : public EditBoxImplCommon
    {
    public:

        EditBoxImplWin(EditBox* pEditText);
        virtual ~EditBoxImplWin();

        virtual bool isEditing() override;
        virtual void createNativeControl(const Rect& frame) override;
        virtual void setNativeFont(const char* pFontName, int fontSize) override;
        virtual void setNativeFontColor(const Color4B& color) override;
        virtual void setNativePlaceholderFont(const char* pFontName, int fontSize) override;
        virtual void setNativePlaceholderFontColor(const Color4B& color) override;
        virtual void setNativeInputMode(EditBox::InputMode inputMode) override;
        virtual void setNativeInputFlag(EditBox::InputFlag inputFlag) override;
        virtual void setNativeReturnType(EditBox::KeyboardReturnType returnType) override;
        virtual void setNativeTextHorizontalAlignment(TextHAlignment alignment) override;
        virtual void setNativeText(const char* pText) override;
        virtual void setNativePlaceHolder(const char* pText) override;
        virtual void setNativeVisible(bool visible) override;
        virtual void updateNativeFrame(const Rect& rect) override;
        virtual const char* getNativeDefaultFontName() override;
        virtual void nativeOpenKeyboard() override;
        virtual void nativeCloseKeyboard() override;
        virtual void setNativeMaxLength(int maxLength) override;

    private:
        void createEditCtrl(bool singleLine);
        void cleanupEditCtrl();
        std::string getText() const;
        void _WindowProc(HWND, UINT, WPARAM, LPARAM);

        WNDPROC _prevWndProc;
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK hookGLFWWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        HWND _hwndEdit;
        bool _changedTextManually;
        bool _hasFocus;
        EditBoxDelegate::EditBoxEndAction _endAction;
        static WNDPROC s_prevCocosWndProc;

        static HINSTANCE s_hInstance;
        static HWND s_hwndCocos;
        static HWND s_previousFocusWnd;
        static bool s_isInitialized;
        static int s_editboxChildID;
        static void lazyInit();
    };

}

OG_END

#endif /* (OG_TARGET_PLATFORM == OG_PLATFORM_WIN32) */

#endif /* __UIEditBoxIMPLWIN_H__ */


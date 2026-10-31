 
#ifndef __UIEditBoxIMPL_H__
#define __UIEditBoxIMPL_H__

#include "ui/UIEditBox/UIEditBox.h"


namespace orange {
    namespace ui{
        
        class EditBoxImpl
        {
        public:
            /**
             * @js NA
             */
            EditBoxImpl(EditBox* pEditBox) : _delegate(nullptr),_editBox(pEditBox) {}
            /**
             * @js NA
             * @lua NA
             */
            virtual ~EditBoxImpl() {}
            
            virtual bool initWithSize(const Size& size) = 0;
            virtual void setFont(const char* pFontName, int fontSize) = 0;
            virtual void setFontColor(const Color4B& color) = 0;
            virtual void setPlaceholderFont(const char* pFontName, int fontSize) = 0;
            virtual void setPlaceholderFontColor(const Color4B& color) = 0;
            virtual void setInputMode(EditBox::InputMode inputMode) = 0;
            virtual void setInputFlag(EditBox::InputFlag inputFlag) = 0;
            virtual void setMaxLength(int maxLength) = 0;
            virtual int  getMaxLength() = 0;
            virtual void setTextHorizontalAlignment(TextHAlignment alignment) = 0;
            virtual void setReturnType(EditBox::KeyboardReturnType returnType) = 0;
            virtual bool isEditing() = 0;
            
            virtual void setText(const char* pText) = 0;
            virtual const char* getText() = 0;
            virtual void setPlaceHolder(const char* pText) = 0;
            virtual const char* getPlaceHolder() = 0;

            virtual const char* getFontName() = 0;
            virtual int getFontSize() = 0;
            virtual const Color4B& getFontColor() = 0;

            virtual const char* getPlaceholderFontName() = 0;
            virtual int getPlaceholderFontSize() = 0;
            virtual const Color4B& getPlaceholderFontColor() = 0;

            virtual EditBox::InputMode getInputMode() = 0;
            virtual EditBox::InputFlag getInputFlag() = 0;
            virtual EditBox::KeyboardReturnType getReturnType() = 0;
            virtual TextHAlignment getTextHorizontalAlignment() = 0;

            virtual void doAnimationWhenKeyboardMove(float duration, float distance) = 0;
            
            virtual void openKeyboard() = 0;
            virtual void closeKeyboard() = 0;
            
            virtual void setPosition(const Vec2& pos) = 0;
            virtual void setVisible(bool visible) = 0;
            virtual void setContentSize(const Size& size) = 0;
            virtual void setAnchorPoint(const Vec2& anchorPoint) = 0;
            
            /**
             * check the editbox's position, update it when needed
             */
            virtual void updatePosition(float dt){};
            /**
             * @js NA
             * @lua NA
             */
            virtual void draw(orange::Renderer *renderer, orange::Mat3 const &transform, uint32_t flags) = 0;
            /**
             * @js NA
             * @lua NA
             */
            virtual void onEnter() = 0;
            
            
            void setDelegate(EditBoxDelegate* pDelegate) { _delegate = pDelegate; };
            EditBoxDelegate* getDelegate() { return _delegate; };
            EditBox* getEditBox() { return _editBox; };

        protected:
            EditBoxDelegate* _delegate;
            EditBox* _editBox;
        };
        
        // This method must be implemented at each subclass of EditBoxImpl.
        extern EditBoxImpl* __createSystemEditBox(EditBox* pEditBox);

    }
}

#endif /* __UIEditBoxIMPL_H__ */

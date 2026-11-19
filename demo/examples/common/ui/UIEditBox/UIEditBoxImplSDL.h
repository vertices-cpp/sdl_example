#ifndef __UIEditBoxIMPLSDL_H__
#define __UIEditBoxIMPLSDL_H__

#include "ui/UIEditBox/UIEditBoxImpl.h"
#include "2d/OGLabel.h"
#include "2d/OGDrawNode.h"
#include "platform/SDLView.h"

namespace orange {
	namespace ui {

		class EditBoxImplSDL : public EditBoxImpl
		{
		public:
			EditBoxImplSDL(EditBox* pEditBox);
			virtual ~EditBoxImplSDL();

			virtual bool initWithSize(const Size& size) override;
			virtual void setFont(const char* pFontName, int fontSize) override;
			virtual void setFontColor(const Color4B& color) override;
			virtual void setPlaceholderFont(const char* pFontName, int fontSize) override;
			virtual void setPlaceholderFontColor(const Color4B& color) override;
			virtual void setInputMode(EditBox::InputMode inputMode) override;
			virtual void setInputFlag(EditBox::InputFlag inputFlag) override;
			virtual void setMaxLength(int maxLength) override;
			virtual int  getMaxLength() override;
			virtual void setTextHorizontalAlignment(TextHAlignment alignment) override;
			virtual void setReturnType(EditBox::KeyboardReturnType returnType) override;
			virtual bool isEditing() override;

			virtual void setText(const char* pText) override;
			virtual const char* getText() override;
			virtual void setPlaceHolder(const char* pText) override;
			virtual const char* getPlaceHolder() override;

			virtual const char* getFontName() override;
			virtual int getFontSize() override;
			virtual const Color4B& getFontColor() override;
			virtual const char* getPlaceholderFontName() override;
			virtual int getPlaceholderFontSize() override;
			virtual const Color4B& getPlaceholderFontColor() override;

			virtual EditBox::InputMode getInputMode() override;
			virtual EditBox::InputFlag getInputFlag() override;
			virtual EditBox::KeyboardReturnType getReturnType() override;
			virtual TextHAlignment getTextHorizontalAlignment() override;

			// 纯虚函数必须实现
			virtual void doAnimationWhenKeyboardMove(float duration, float distance) override;
			virtual void openKeyboard() override;
			virtual void closeKeyboard() override;
			virtual void setPosition(const Vec2& pos) override;
			virtual void setVisible(bool visible) override;
			virtual void setContentSize(const Size& size) override;
			virtual void setAnchorPoint(const Vec2& anchorPoint) override;
			virtual void updatePosition(float dt) override;
			virtual void draw(Renderer *renderer, const Mat3 &parentTransform, uint32_t parentFlags) override;
			virtual void onEnter() override;

			// 外部交互接口
			void handleInputText(const std::string& text);
			void handleKeyDown(EventKeyboard::KeyCode keyCode);
			void setCursorByPosition(float localX);

		private:
			void updateUI();
			void updateCursorPosition();
			float calculateWidth(const std::u32string& text);

			Label* _labelRenderer;
			Label* _placeholderRenderer;
			Label* _cursorMeasureLabel;
			DrawNode* _cursorNode;

			std::u32string _text32;
			int _cursorIndex;
			std::string _placeholder;

			bool _isEditing;
			bool _showCursor;
			int _maxLength;

			std::string _fontName;
			int _fontSize;
			Color4B _fontColor;
			std::string _placeholderFontName;
			int _placeholderFontSize;
			Color4B _placeholderFontColor;

			EditBox::InputMode _inputMode;
			EditBox::InputFlag _inputFlag;
			EditBox::KeyboardReturnType _returnType;
			TextHAlignment _alignment;
		};

	}
}
#endif /* __UIEditBoxIMPLSDL_H__ */
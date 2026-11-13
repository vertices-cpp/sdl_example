
//#include "platform/OGInputManager.h"

#include "ui/UIEditBox/UIEditBoxImplSDL.h"
#include "ui/UIEditBox/UIEditBox.h"
#include "base/OGDirector.h"
#include "utils/iconv_wrapper.h"
#include <cmath>

namespace orange {
	namespace ui {

		EditBoxImpl* __createSystemEditBox(EditBox* pEditBox)
		{
			return new (std::nothrow) EditBoxImplSDL(pEditBox);
		}

		static EditBoxImplSDL* _currentEditingBox = nullptr;

		EditBoxImplSDL::EditBoxImplSDL(EditBox* pEditBox)
			: EditBoxImpl(pEditBox)
			, _labelRenderer(nullptr)
			, _placeholderRenderer(nullptr)
			, _cursorMeasureLabel(nullptr)
			, _cursorNode(nullptr)
			, _cursorIndex(0)
			, _isEditing(false)
			, _showCursor(true)
			, _maxLength(-1)
			, _fontName("fonts/Marker Felt.ttf")
			, _fontSize(20)
			, _fontColor(Color4B::WHITE)
			, _placeholderFontName("fonts/Marker Felt.ttf")
			, _placeholderFontSize(20)
			, _placeholderFontColor(Color4B(128, 128, 128, 255))
			, _inputMode(EditBox::InputMode::SINGLE_LINE)
			, _inputFlag(EditBox::InputFlag::SENSITIVE)
			, _returnType(EditBox::KeyboardReturnType::DEFAULT)
			, _alignment(TextHAlignment::LEFT)
		{
		}

		EditBoxImplSDL::~EditBoxImplSDL()
		{
			if (_currentEditingBox == this) {
				_currentEditingBox = nullptr;
			}
		}

		bool EditBoxImplSDL::initWithSize(const Size& size)
		{
			_labelRenderer = Label::createWithTTF("", _fontName, _fontSize);
			_labelRenderer->setTextColor(_fontColor);
			_labelRenderer->setPosition(Vec2(5.0f, 2.0f));
			_editBox->addChild(_labelRenderer, 999);

			_placeholderRenderer = Label::createWithTTF("", _placeholderFontName, _placeholderFontSize);
			_placeholderRenderer->setTextColor(_placeholderFontColor);
			_placeholderRenderer->setPosition(Vec2(5.0f, 2.0f));
			_placeholderRenderer->setVisible(true);
			_editBox->addChild(_placeholderRenderer, 999);

			// ★ 核心修改：测宽节点移到屏幕外，确保它必然参与排版计算
			_cursorMeasureLabel = Label::createWithTTF(" ", _fontName, _fontSize);
			_cursorMeasureLabel->setPosition(Vec2(-2000.0f, -2000.0f));
			_editBox->addChild(_cursorMeasureLabel, -1);

			_cursorNode = DrawNode::create();
			_cursorNode->drawRect(Vec2(0, 0), Vec2(2, _fontSize), Color4F::WHITE);
			_cursorNode->setPosition(Vec2(5.0f, 2.0f));
			_cursorNode->setVisible(false);
			_editBox->addChild(_cursorNode, 1000);

			return true;
		}

		float EditBoxImplSDL::calculateWidth(const std::u32string& text)
		{
			if (text.empty()) return 0.0f;
			std::string utf8 = iconv_wrapper_from_u32le(text, Utf32leToUtf8);

			if (_cursorMeasureLabel) {
				_cursorMeasureLabel->setString(utf8);

				// ★ 核心大杀器 1：强制刷新测宽 Label 的排版！
				_cursorMeasureLabel->updateContent();

				return _cursorMeasureLabel->getContentSize().width;
			}
			return 0.0f;
		}

		void EditBoxImplSDL::updateUI()
		{
			std::string utf8Text = iconv_wrapper_from_u32le(_text32, Utf32leToUtf8);
			_labelRenderer->setString(utf8Text);

			if (_text32.empty() && !_placeholder.empty()) {
				_placeholderRenderer->setString(_placeholder);
				_placeholderRenderer->setVisible(true);
			}
			else {
				_placeholderRenderer->setVisible(false);
			}

			_labelRenderer->getContentSize();
			updateCursorPosition();
		}

		void EditBoxImplSDL::updateCursorPosition()
		{
			if (!_isEditing) return;

			if (_cursorIndex < 0) _cursorIndex = 0;
			if (_cursorIndex > (int)_text32.length()) _cursorIndex = (int)_text32.length();

			// 算光标左侧文字的宽度
			std::u32string subStr = _text32.substr(0, _cursorIndex);
			float cursorTextWidth = calculateWidth(subStr);

			// ★ 核心大杀器 2：强制刷新主 Label 的排版！
			_labelRenderer->updateContent();
			float totalTextWidth = _labelRenderer->getContentSize().width;

			float maxWidth = _editBox->getContentSize().width - 10.0f;

			// ★ 打印 2：看看算出来的宽度对不对！
			OGLOG("【排版】_cursorIndex: %d, 光标左侧宽度: %f, 总宽度: %f",
				_cursorIndex, cursorTextWidth, totalTextWidth);

			float offset = 0;

			// ★ 核心修复 3：如果文字没超出边界，必须把主文字归位到 (5.0, 2.0)
			// 否则可能因为上一次滚动导致文字停留在左边
			if (totalTextWidth <= maxWidth) {
				offset = 0;
				_labelRenderer->setPosition(Vec2(5.0f, 2.0f));
				_cursorNode->setPosition(Vec2(5.0f + cursorTextWidth + 2.0f, 2.0f));
			}
			else {
				// 文字超出边界，进行滚动处理
				if (cursorTextWidth > maxWidth) {
					offset = cursorTextWidth - maxWidth;
				}
				else {
					// 如果光标在最前面，也可以让文字直接贴左边
					// 这里稍微优化一下：只要光标在可视区，就不滚动
					offset = 0;
				}
				_labelRenderer->setPosition(Vec2(5.0f - offset, 2.0f));
				_cursorNode->setPosition(Vec2(5.0f + cursorTextWidth - offset + 2.0f, 2.0f));
			}

		}

		void EditBoxImplSDL::setCursorByPosition(float localX)
		{
			if (!_isEditing) return;

			if (_text32.empty()) {
				_cursorIndex = 0;
				updateCursorPosition();
				return;
			}

			int bestIndex = 0;
			float minDistance = FLT_MAX;

			// 遍历所有字符间隙，找到距离鼠标点击位置最近的一个
			for (int i = 0; i <= (int)_text32.length(); ++i) {
				std::u32string subStr = _text32.substr(0, i);
				float w = calculateWidth(subStr);

				// _labelRenderer 可能因为文字过长而向左偏移了（滚动）
				float currentPosX = _labelRenderer->getPositionX() + w;

				float dist = std::abs(currentPosX - localX);
				if (dist < minDistance) {
					minDistance = dist;
					bestIndex = i;
				}
			}

			_cursorIndex = bestIndex;
			updateCursorPosition();
		}

		void EditBoxImplSDL::handleInputText(const std::string& text)
		{
			if (!_isEditing) return;

			std::u32string input32 = iconv_wrapper_to_utf32le(text, Utf8ToUtf32le);
			if (_maxLength > 0 && (int)(_text32.length() + input32.length()) > _maxLength) {
				return;
			}

			_text32.insert(_cursorIndex, input32);
			_cursorIndex += input32.length();
			updateUI();

			if (_delegate) _delegate->editBoxTextChanged(_editBox, getText());
		}

		void EditBoxImplSDL::handleKeyDown(EventKeyboard::KeyCode keyCode)
		{
			if (!_isEditing) return;

			if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
				if (_cursorIndex > 0 && !_text32.empty()) {
					_text32.erase(_cursorIndex - 1, 1);
					_cursorIndex--;

					// ★ 打印 1：看看退格后，光标索引真的减了吗？
					OGLOG("【退格】_cursorIndex: %d, 总长度: %d, 当前文本: %s",
						_cursorIndex, (int)_text32.length(), getText());

					updateUI();
					if (_delegate) _delegate->editBoxTextChanged(_editBox, getText());
				}
			}
			else if (keyCode == EventKeyboard::KeyCode::KEY_DELETE) {
				if (_cursorIndex < (int)_text32.length()) {
					_text32.erase(_cursorIndex, 1);
					updateUI();
					if (_delegate) _delegate->editBoxTextChanged(_editBox, getText());
				}
			}
			else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW||
				keyCode == EventKeyboard::KeyCode::KEY_KP_LEFT
				) {
				if (_cursorIndex > 0) {
					_cursorIndex--;
					updateCursorPosition();
				}
			}
			else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW||
				keyCode == EventKeyboard::KeyCode::KEY_KP_RIGHT
				) {
				if (_cursorIndex < (int)_text32.length()) {
					_cursorIndex++;
					updateCursorPosition();
				}
			}
			else if (keyCode == EventKeyboard::KeyCode::KEY_ENTER|| keyCode == EventKeyboard::KeyCode::KEY_KP_ENTER) {
				closeKeyboard();
				if (_delegate) {
					_delegate->editBoxEditingDidEndWithAction(_editBox, EditBoxDelegate::EditBoxEndAction::RETURN);
					_delegate->editBoxReturn(_editBox);
				}
			}
		}

		void EditBoxImplSDL::openKeyboard()
		{
			if (_isEditing) return;

			if (_currentEditingBox && _currentEditingBox != this) {
				_currentEditingBox->closeKeyboard();
			}

			_isEditing = true;
			_currentEditingBox = this;

			auto sdlview = SDLView::getInstance();
			if (sdlview)
			{
				sdlview->setIMEKeyboardState(true);
			}

			//_cursorIndex = (int)_text32.length();
			updateCursorPosition();

			_cursorNode->setVisible(true);
			_showCursor = true;

			_editBox->schedule([this](float dt) {
				_showCursor = !_showCursor;
				_cursorNode->setVisible(_showCursor);
			}, 0.5f, "cursor_blink");

			if (_delegate) _delegate->editBoxEditingDidBegin(_editBox);
		}

		void EditBoxImplSDL::closeKeyboard()
		{
			if (!_isEditing) return;
			_isEditing = false;

			if (_currentEditingBox == this) {
				_currentEditingBox = nullptr;
			}

			auto sdlview = SDLView::getInstance();
			if (sdlview)
			{
				sdlview->setIMEKeyboardState(false);
			}
			_cursorNode->setVisible(false);
			_editBox->unschedule("cursor_blink");
		}

		void EditBoxImplSDL::setText(const char* pText)
		{
			std::u32string newText;
			if (pText) {
				newText = iconv_wrapper_to_utf32le(std::string(pText), Utf8ToUtf32le);
			}

			// 如果外部传进来的文本和当前内部文本一样，说明只是状态同步，千万别重置光标！
			if (newText == _text32) {
				return; // ★ 直接返回，保护光标位置
			}

			_text32 = newText;
			_cursorIndex = (int)_text32.length();
			updateUI();
		}

		const char* EditBoxImplSDL::getText()
		{
			static std::string utf8Text;
			utf8Text = iconv_wrapper_from_u32le(_text32, Utf32leToUtf8);
			return utf8Text.c_str();
		}

		void EditBoxImplSDL::setPlaceHolder(const char* pText)
		{
			if (pText) {
				_placeholder = pText;
				updateUI();
			}
		}

		const char* EditBoxImplSDL::getPlaceHolder() { return _placeholder.c_str(); }
		bool EditBoxImplSDL::isEditing() { return _isEditing; }

		void EditBoxImplSDL::setFont(const char* pFontName, int fontSize)
		{
			_fontName = pFontName ? pFontName : "fonts/Marker Felt.ttf";
			_fontSize = fontSize > 0 ? fontSize : 20;

			if (_labelRenderer) {
				_labelRenderer->removeFromParent();
				_labelRenderer = Label::createWithTTF("", _fontName, _fontSize);
				_labelRenderer->setTextColor(_fontColor);
				_labelRenderer->setPosition(Vec2(5.0f, 2.0f));
				_editBox->addChild(_labelRenderer, 999);
			}
			if (_cursorMeasureLabel) {
				_cursorMeasureLabel->removeFromParent();
				_cursorMeasureLabel = Label::createWithTTF(" ", _fontName, _fontSize);
				_cursorMeasureLabel->setPosition(Vec2(-2000.0f, -2000.0f));
				_editBox->addChild(_cursorMeasureLabel, -1);
			}
			if (_cursorNode) {
				_cursorNode->clear();
				_cursorNode->drawRect(Vec2(0, 0), Vec2(2, _fontSize), Color4F::WHITE);
			}
			updateUI();
		}

		void EditBoxImplSDL::setFontColor(const Color4B& color) {
			_fontColor = color;
			if (_labelRenderer) _labelRenderer->setTextColor(_fontColor);
		}

		void EditBoxImplSDL::setPlaceholderFont(const char* pFontName, int fontSize) {
			_placeholderFontName = pFontName ? pFontName : "fonts/Marker Felt.ttf";
			_placeholderFontSize = fontSize;
		}

		void EditBoxImplSDL::setPlaceholderFontColor(const Color4B& color) {
			_placeholderFontColor = color;
			if (_placeholderRenderer) _placeholderRenderer->setTextColor(_placeholderFontColor);
		}

		void EditBoxImplSDL::setInputMode(EditBox::InputMode inputMode) { _inputMode = inputMode; }
		void EditBoxImplSDL::setInputFlag(EditBox::InputFlag inputFlag) { _inputFlag = inputFlag; }
		void EditBoxImplSDL::setMaxLength(int maxLength) { _maxLength = maxLength; }
		int  EditBoxImplSDL::getMaxLength() { return _maxLength; }
		void EditBoxImplSDL::setTextHorizontalAlignment(TextHAlignment alignment) { _alignment = alignment; }
		void EditBoxImplSDL::setReturnType(EditBox::KeyboardReturnType returnType) { _returnType = returnType; }

		const char* EditBoxImplSDL::getFontName() { return _fontName.c_str(); }
		int EditBoxImplSDL::getFontSize() { return _fontSize; }
		const Color4B& EditBoxImplSDL::getFontColor() { return _fontColor; }
		const char* EditBoxImplSDL::getPlaceholderFontName() { return _placeholderFontName.c_str(); }
		int EditBoxImplSDL::getPlaceholderFontSize() { return _placeholderFontSize; }
		const Color4B& EditBoxImplSDL::getPlaceholderFontColor() { return _placeholderFontColor; }
		EditBox::InputMode EditBoxImplSDL::getInputMode() { return _inputMode; }
		EditBox::InputFlag EditBoxImplSDL::getInputFlag() { return _inputFlag; }
		EditBox::KeyboardReturnType EditBoxImplSDL::getReturnType() { return _returnType; }
		TextHAlignment EditBoxImplSDL::getTextHorizontalAlignment() { return _alignment; }

		void EditBoxImplSDL::doAnimationWhenKeyboardMove(float duration, float distance) {}
		void EditBoxImplSDL::setPosition(const Vec2& pos) {}
		void EditBoxImplSDL::setVisible(bool visible)
		{
			if (_labelRenderer) {
				_labelRenderer->setVisible(visible);
			}

			if (_placeholderRenderer) {
				// ★ 核心修复：只有在控件可见、没字、并且设置了占位符的时候，才显示占位符
				bool showPlaceholder = visible && _text32.empty() && !_placeholder.empty();
				_placeholderRenderer->setVisible(showPlaceholder);
			}

			if (_cursorNode) {
				_cursorNode->setVisible(visible && _isEditing && _showCursor);
			}
		}
		void EditBoxImplSDL::setContentSize(const Size& size) {}
		void EditBoxImplSDL::setAnchorPoint(const Vec2& anchorPoint) {}
		void EditBoxImplSDL::updatePosition(float dt) {}
		void EditBoxImplSDL::draw(Renderer *renderer, Mat3 const &transform, uint32_t flags) {}
		void EditBoxImplSDL::onEnter() {}

	}
}
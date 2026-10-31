

#include "ui/UIHelper.h"
#include "ui/UIWidget.h"
#include "ui/UILayoutComponent.h"
#include "base/OGDirector.h"
//#include "base/ogUTF8.h"
#include "utils/iconv_wrapper.h"

OG_BEGIN

namespace ui {

static bool _activeLayout = true;

Widget* Helper::seekWidgetByTag(Widget* root, int tag)
{
    if (!root)
    {
        return nullptr;
    }
    if (root->getTag() == tag)
    {
        return root;
    }
    const auto& arrayRootChildren = root->getChildren();
    ssize_t length = arrayRootChildren.size();
    for (ssize_t i=0;i<length;i++)
    {
        Widget* child = dynamic_cast<Widget*>(arrayRootChildren.at(i));
        if (child)
        {
            Widget* res = seekWidgetByTag(child,tag);
            if (res != nullptr)
            {
                return res;
            }
        }
    }
    return nullptr;
}

Widget* Helper::seekWidgetByName(Widget* root, const std::string& name)
{
    if (!root)
    {
        return nullptr;
    }
    if (root->getName() == name)
    {
        return root;
    }
    const auto& arrayRootChildren = root->getChildren();
    for (auto& subWidget : arrayRootChildren)
    {
        Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            Widget* res = seekWidgetByName(child,name);
            if (res != nullptr)
            {
                return res;
            }
        }
    }
    return nullptr;
}

/*temp action*/
Widget* Helper::seekActionWidgetByActionTag(Widget* root, int tag)
{
	if (!root)
	{
		return nullptr;
	}
	if (root->getActionTag() == tag)
	{
		return root;
	}
    const auto& arrayRootChildren = root->getChildren();
    for (auto& subWidget : arrayRootChildren)
	{
		Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            Widget* res = seekActionWidgetByActionTag(child,tag);
            if (res != nullptr)
            {
                return res;
            }
        }
	}
	return nullptr;
}
    
std::string Helper::getSubStringOfUTF8String(const std::string& str, std::string::size_type start, std::string::size_type length)
{
	std::u32string utf32 = iconv_wrapper_to_utf32le(str, Utf8ToUtf32le);
    if (utf32.empty()) {
        OGLOGERROR("Can't convert string to UTF-32: %s", str.c_str());
        return "";
    }
    if (utf32.size() < start) {
        OGLOGERROR("'start' is out of range: %ld, %s", static_cast<long>(start), str.c_str());
        return "";
    }
    std::string result = iconv_wrapper_from_u32le(utf32.substr(start, length), Utf32leToUtf8);
    if (result.empty()) {
        OGLOGERROR("Can't convert internal UTF-32 string to UTF-8: %s", str.c_str());
        return "";
    }
    return result;
}

void Helper::changeLayoutSystemActiveState(bool bActive)
{
    _activeLayout = bActive;
}
void Helper::doLayout(orange::Node *rootNode)
{
    if(!_activeLayout)
    {
        return;
    }

    for(auto& node : rootNode->getChildren())
    {
        auto com = node->getComponent(__LAYOUT_COMPONENT_NAME);
        Node *parent = node->getParent();
        if (nullptr != com && nullptr != parent) {
            LayoutComponent* layoutComponent = (LayoutComponent*)com;

            layoutComponent->refreshLayout();
        }
    }
}
    
Rect Helper::restrictCapInsetRect(const orange::Rect &capInsets, const Size& textureSize )
{
    float x = capInsets.origin.x;
    float y = capInsets.origin.y;
    float width = capInsets.size.width;
    float height = capInsets.size.height;
    
    if (textureSize.width < width)
    {
        x = textureSize.width / 2.0f;
        width = textureSize.width > 0 ? 1.0f : 0.0f;
    }
    if (textureSize.height < height)
    {
        y = textureSize.height / 2.0f;
        height = textureSize.height > 0 ? 1.0f : 0.0f;
    }
    return Rect(x, y, width, height);
}

Rect Helper::convertBoundingBoxToScreen(Node* node)
{
	if (!node) return Rect::ZERO;

	Vec2 leftBottom = node->convertToWorldSpace(Vec2::ZERO);
	Size contentSize = node->getContentSize();
	Vec2 rightTop = node->convertToWorldSpace(
		Vec2(contentSize.width, contentSize.height));

	float x = leftBottom.x;
	float y = leftBottom.y;
	float w = rightTop.x - leftBottom.x;
	float h = rightTop.y - leftBottom.y;

	return Rect(x, y, w, h);
}
}

OG_END

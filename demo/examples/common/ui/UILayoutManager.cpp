

#include "ui/UILayoutManager.h"
#include "ui/UILayout.h"

OG_BEGIN

namespace ui {

LinearHorizontalLayoutManager* LinearHorizontalLayoutManager::create()
{
    LinearHorizontalLayoutManager* exe = new (std::nothrow) LinearHorizontalLayoutManager();
    if (exe)
    {
        exe->autorelease();
        return exe;
    }
    OG_SAFE_DELETE(exe);
    return nullptr;
}
    
#ifdef USE_ANCHOR_POINT
    
void LinearHorizontalLayoutManager::doLayout(LayoutProtocol* layout)
{
    Size layoutSize = layout->getLayoutContentSize();
    Vector<Node*> container = layout->getLayoutElements();
    float leftBoundary = 0.0f;
    for (auto& subWidget : container)
    {
        Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            LinearLayoutParameter* layoutParameter = dynamic_cast<LinearLayoutParameter*>(child->getLayoutParameter());
            if (layoutParameter)
            {
                LinearLayoutParameter::LinearGravity childGravity = layoutParameter->getGravity();
                Vec2 ap = child->getCenter();
                Size cs = child->getBoundingBox().size;
                float finalPosX = leftBoundary + (ap.x * cs.width);
                float finalPosY = layoutSize.height - (1.0f - ap.y) * cs.height;
                switch (childGravity)
                {
                    case LinearLayoutParameter::LinearGravity::NONE:
                    case LinearLayoutParameter::LinearGravity::TOP:
                        break;
                    case LinearLayoutParameter::LinearGravity::BOTTOM:
                        finalPosY = ap.y * cs.height;
                        break;
                    case LinearLayoutParameter::LinearGravity::CENTER_VERTICAL:
                        finalPosY = layoutSize.height / 2.0f - cs.height * (0.5f - ap.y);
                        break;
                    default:
                        break;
                }
                Margin mg = layoutParameter->getMargin();
                finalPosX += mg.left;
                finalPosY -= mg.top;
                child->setPosition(Vec2(finalPosX, finalPosY));
                leftBoundary = child->getRightBoundary() + mg.right;
            }
        }
    }
}
#else
void LinearHorizontalLayoutManager::doLayout(LayoutProtocol* layout)
{
	Size layoutSize = layout->getLayoutContentSize();
	Vector<Node*> container = layout->getLayoutElements();
	float leftBoundary = 0.0f;
	//OGLOG("==== Horizontal Layout Size: W=%f, H=%f ====", layoutSize.width, layoutSize.height);


	for (auto& subWidget : container)
	{
		Widget* child = dynamic_cast<Widget*>(subWidget);
		if (child)
		{
			LinearLayoutParameter* layoutParameter = dynamic_cast<LinearLayoutParameter*>(child->getLayoutParameter());
			if (layoutParameter)
			{
				LinearLayoutParameter::LinearGravity childGravity = layoutParameter->getGravity();
				Size cs = child->getBoundingBox().size;

				// 无锚点，默认左上角
				float finalPosX = leftBoundary;
				float finalPosY = 0.0f;

				// 处理垂直方向的居中/靠底
				switch (childGravity)
				{
				case LinearLayoutParameter::LinearGravity::NONE:
				case LinearLayoutParameter::LinearGravity::TOP:
					finalPosY = 0.0f;
					break;
				case LinearLayoutParameter::LinearGravity::BOTTOM:
					finalPosY = layoutSize.height - cs.height;
					break;
				case LinearLayoutParameter::LinearGravity::CENTER_VERTICAL:
					finalPosY = (layoutSize.height - cs.height) / 2.0f;
					break;
				default:
					break;
				}

				// 处理 Margin (距左、距上)
				Margin mg = layoutParameter->getMargin();
				finalPosX += mg.left;
				finalPosY += mg.top;

				child->setPosition(Vec2(finalPosX, finalPosY));
				// ★ 加这行日志，看看水平模式下到底算出了什么坐标
				/*OGLOG("H-Layout -> Name: %s, X: %f, Y: %f, Width: %f, Height: %f",
					subWidget->getName().c_str(), finalPosX, finalPosY, cs.width, cs.height);*/
				// 更新下一个元素的左边界
				leftBoundary = finalPosX + cs.width + mg.right;
			}
		}
	}
}
#endif
    
    
//LinearVerticalLayoutManager
LinearVerticalLayoutManager* LinearVerticalLayoutManager::create()
{
    LinearVerticalLayoutManager* exe = new (std::nothrow) LinearVerticalLayoutManager();
    if (exe)
    {
        exe->autorelease();
        return exe;
    }
    OG_SAFE_DELETE(exe);
    return nullptr;
}

#ifdef USE_ANCHOR_POINT
    
void LinearVerticalLayoutManager::doLayout(LayoutProtocol* layout)
{
    Size layoutSize = layout->getLayoutContentSize();
    Vector<Node*> container = layout->getLayoutElements();
    float topBoundary = layoutSize.height;
    
    for (auto& subWidget : container)
    {
        LayoutParameterProtocol* child = dynamic_cast<LayoutParameterProtocol*>(subWidget);
        if (child)
        {
            LinearLayoutParameter* layoutParameter = dynamic_cast<LinearLayoutParameter*>(child->getLayoutParameter());
            
            if (layoutParameter)
            {
                LinearLayoutParameter::LinearGravity childGravity = layoutParameter->getGravity();
                Vec2 ap = subWidget->getCenter();
                Size cs = subWidget->getBoundingBox().size;
                float finalPosX = ap.x * cs.width;
                float finalPosY = topBoundary - ((1.0f-ap.y) * cs.height);
                switch (childGravity)
                {
                    case LinearLayoutParameter::LinearGravity::NONE:
                    case LinearLayoutParameter::LinearGravity::LEFT:
                        break;
                    case LinearLayoutParameter::LinearGravity::RIGHT:
                        finalPosX = layoutSize.width - ((1.0f - ap.x) * cs.width);
                        break;
                    case LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL:
                        finalPosX = layoutSize.width / 2.0f - cs.width * (0.5f-ap.x);
                        break;
                    default:
                        break;
                }
                Margin mg = layoutParameter->getMargin();
                finalPosX += mg.left;
                finalPosY -= mg.top;
                subWidget->setPosition(finalPosX, finalPosY);
				OGLOG("== [Node: %s] ==", subWidget->getName().c_str());
				OGLOG("相对坐标 (传入值): X=%.1f, Y=%.1f", finalPosX, finalPosY);
			 
                topBoundary = subWidget->getPosition().y - subWidget->getCenter().y * subWidget->getBoundingBox().size.height - mg.bottom;
            }
        }
    }
}
#else
void LinearVerticalLayoutManager::doLayout(LayoutProtocol* layout)
{
	Size layoutSize = layout->getLayoutContentSize();
	Vector<Node*> container = layout->getLayoutElements();
	float topBoundary = 0.0f; // 无锚点：Y向下，所以从上往下排

	for (auto& subWidget : container)
	{
		LayoutParameterProtocol* child = dynamic_cast<LayoutParameterProtocol*>(subWidget);
		if (child)
		{
			LinearLayoutParameter* layoutParameter = dynamic_cast<LinearLayoutParameter*>(child->getLayoutParameter());
			if (layoutParameter)
			{
				LinearLayoutParameter::LinearGravity childGravity = layoutParameter->getGravity();
				Size cs = subWidget->getBoundingBox().size;

				float finalPosX = 0.0f;
				float finalPosY = topBoundary;

				// 处理水平方向的居中/靠右
				switch (childGravity)
				{
				case LinearLayoutParameter::LinearGravity::NONE:
				case LinearLayoutParameter::LinearGravity::LEFT:
					finalPosX = 0.0f;
					break;
				case LinearLayoutParameter::LinearGravity::RIGHT:
					finalPosX = layoutSize.width - cs.width;
					break;
				case LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL:
					finalPosX = (layoutSize.width - cs.width) / 2.0f;
					break;
				default:
					break;
				}

				Margin mg = layoutParameter->getMargin();
				finalPosX += mg.left;
				finalPosY += mg.top;

				subWidget->setPosition(Vec2(finalPosX, finalPosY));
				OGLOG("== [Node: %s] ==", subWidget->getName().c_str());
				OGLOG("相对坐标 (传入值): X=%.1f, Y=%.1f", finalPosX, finalPosY);
				// 更新下一个元素的顶边界
				topBoundary = finalPosY + cs.height + mg.bottom;
			}
		}
	}
}
#endif
    
//RelativeLayoutManager

RelativeLayoutManager* RelativeLayoutManager::create()
{
    RelativeLayoutManager* exe = new (std::nothrow) RelativeLayoutManager();
    if (exe)
    {
        exe->autorelease();
        return exe;
    }
    OG_SAFE_DELETE(exe);
    return nullptr;
}




Vector<Widget*> RelativeLayoutManager::getAllWidgets(orange::ui::LayoutProtocol *layout)
{
    Vector<Node*> container = layout->getLayoutElements();
    Vector<Widget*> widgetChildren;
    for (auto& subWidget : container)
    {
        Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            RelativeLayoutParameter* layoutParameter = dynamic_cast<RelativeLayoutParameter*>(child->getLayoutParameter());
            layoutParameter->_put = false;
            _unlayoutChildCount++;
            widgetChildren.pushBack(child);
        }
    }
    return widgetChildren;

}
    
Widget* RelativeLayoutManager::getRelativeWidget(Widget* widget)
{
    Widget* relativeWidget = nullptr;
    RelativeLayoutParameter* layoutParameter = dynamic_cast<RelativeLayoutParameter*>(widget->getLayoutParameter());
    const std::string relativeName = layoutParameter->getRelativeToWidgetName();
    
    if (!relativeName.empty())
    {
        for (auto& sWidget : _widgetChildren)
        {
            if (sWidget)
            {
                RelativeLayoutParameter* rlayoutParameter = dynamic_cast<RelativeLayoutParameter*>(sWidget->getLayoutParameter());
                if (rlayoutParameter &&  rlayoutParameter->getRelativeName() == relativeName)
                {
                    relativeWidget = sWidget;
                    _relativeWidgetLP = rlayoutParameter;
                    break;
                }
            }
        }
    }
    return relativeWidget;
}
#ifdef USE_ANCHOR_POINT

bool RelativeLayoutManager::calculateFinalPositionWithRelativeWidget(LayoutProtocol *layout)
{
    Vec2 ap = _widget->getCenter();
    Size cs = _widget->getBoundingBox().size;
    
    _finalPositionX = 0.0f;
    _finalPositionY = 0.0f;
    
    Widget* relativeWidget = this->getRelativeWidget(_widget);
    
    RelativeLayoutParameter* layoutParameter = dynamic_cast<RelativeLayoutParameter*>(_widget->getLayoutParameter());

    RelativeLayoutParameter::RelativeAlign align = layoutParameter->getAlign();

    Size layoutSize = layout->getLayoutContentSize();

    
    switch (align)
    {
        case RelativeLayoutParameter::RelativeAlign::NONE:
        case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_LEFT:
            _finalPositionX = ap.x * cs.width;
            _finalPositionY = layoutSize.height - ((1.0f - ap.y) * cs.height);
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_CENTER_HORIZONTAL:
            _finalPositionX = layoutSize.width * 0.5f - cs.width * (0.5f - ap.x);
            _finalPositionY = layoutSize.height - ((1.0f - ap.y) * cs.height);
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_RIGHT:
            _finalPositionX = layoutSize.width - ((1.0f - ap.x) * cs.width);
            _finalPositionY = layoutSize.height - ((1.0f - ap.y) * cs.height);
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_CENTER_VERTICAL:
            _finalPositionX = ap.x * cs.width;
            _finalPositionY = layoutSize.height * 0.5f - cs.height * (0.5f - ap.y);
            break;
        case RelativeLayoutParameter::RelativeAlign::CENTER_IN_PARENT:
            _finalPositionX = layoutSize.width * 0.5f - cs.width * (0.5f - ap.x);
            _finalPositionY = layoutSize.height * 0.5f - cs.height * (0.5f - ap.y);
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_RIGHT_CENTER_VERTICAL:
            _finalPositionX = layoutSize.width - ((1.0f - ap.x) * cs.width);
            _finalPositionY = layoutSize.height * 0.5f - cs.height * (0.5f - ap.y);
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_BOTTOM:
            _finalPositionX = ap.x * cs.width;
            _finalPositionY = ap.y * cs.height;
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_BOTTOM_CENTER_HORIZONTAL:
            _finalPositionX = layoutSize.width * 0.5f - cs.width * (0.5f - ap.x);
            _finalPositionY = ap.y * cs.height;
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_RIGHT_BOTTOM:
            _finalPositionX = layoutSize.width - ((1.0f - ap.x) * cs.width);
            _finalPositionY = ap.y * cs.height;
            break;
            
        case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_LEFTALIGN:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                float locationTop = relativeWidget->getTopBoundary();
                float locationLeft = relativeWidget->getLeftBoundary();
                _finalPositionY = locationTop + ap.y * cs.height;
                _finalPositionX = locationLeft + ap.x * cs.width;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_CENTER:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                Size rbs = relativeWidget->getBoundingBox().size;
                float locationTop = relativeWidget->getTopBoundary();
                
                _finalPositionY = locationTop + ap.y * cs.height;
                _finalPositionX = relativeWidget->getLeftBoundary() + rbs.width * 0.5f + ap.x * cs.width - cs.width * 0.5f;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_RIGHTALIGN:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                float locationTop = relativeWidget->getTopBoundary();
                float locationRight = relativeWidget->getRightBoundary();
                _finalPositionY = locationTop + ap.y * cs.height;
                _finalPositionX = locationRight - (1.0f - ap.x) * cs.width;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_TOPALIGN:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                float locationTop = relativeWidget->getTopBoundary();
                float locationLeft = relativeWidget->getLeftBoundary();
                _finalPositionY = locationTop - (1.0f - ap.y) * cs.height;
                _finalPositionX = locationLeft - (1.0f - ap.x) * cs.width;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_CENTER:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                Size rbs = relativeWidget->getBoundingBox().size;
                float locationLeft = relativeWidget->getLeftBoundary();
                _finalPositionX = locationLeft - (1.0f - ap.x) * cs.width;
                
                _finalPositionY = relativeWidget->getBottomBoundary() + rbs.height * 0.5f + ap.y * cs.height - cs.height * 0.5f;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_BOTTOMALIGN:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                float locationBottom = relativeWidget->getBottomBoundary();
                float locationLeft = relativeWidget->getLeftBoundary();
                _finalPositionY = locationBottom + ap.y * cs.height;
                _finalPositionX = locationLeft - (1.0f - ap.x) * cs.width;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_TOPALIGN:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                float locationTop = relativeWidget->getTopBoundary();
                float locationRight = relativeWidget->getRightBoundary();
                _finalPositionY = locationTop - (1.0f - ap.y) * cs.height;
                _finalPositionX = locationRight + ap.x * cs.width;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_CENTER:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                Size rbs = relativeWidget->getBoundingBox().size;
                float locationRight = relativeWidget->getRightBoundary();
                _finalPositionX = locationRight + ap.x * cs.width;
                
                _finalPositionY = relativeWidget->getBottomBoundary() + rbs.height * 0.5f + ap.y * cs.height - cs.height * 0.5f;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_BOTTOMALIGN:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                float locationBottom = relativeWidget->getBottomBoundary();
                float locationRight = relativeWidget->getRightBoundary();
                _finalPositionY = locationBottom + ap.y * cs.height;
                _finalPositionX = locationRight + ap.x * cs.width;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_LEFTALIGN:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                float locationBottom = relativeWidget->getBottomBoundary();
                float locationLeft = relativeWidget->getLeftBoundary();
                _finalPositionY = locationBottom - (1.0f - ap.y) * cs.height;
                _finalPositionX = locationLeft + ap.x * cs.width;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_CENTER:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                Size rbs = relativeWidget->getBoundingBox().size;
                float locationBottom = relativeWidget->getBottomBoundary();
                
                _finalPositionY = locationBottom - (1.0f - ap.y) * cs.height;
                _finalPositionX = relativeWidget->getLeftBoundary() + rbs.width * 0.5f + ap.x * cs.width - cs.width * 0.5f;
            }
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_RIGHTALIGN:
            if (relativeWidget)
            {
                if (_relativeWidgetLP && !_relativeWidgetLP->_put)
                {
                    return false;
                }
                float locationBottom = relativeWidget->getBottomBoundary();
                float locationRight = relativeWidget->getRightBoundary();
                _finalPositionY = locationBottom - (1.0f - ap.y) * cs.height;
                _finalPositionX = locationRight - (1.0f - ap.x) * cs.width;
            }
            break;
        default:
            break;
    }
    return true;
}
    
void RelativeLayoutManager::calculateFinalPositionWithRelativeAlign()
{
    RelativeLayoutParameter* layoutParameter = dynamic_cast<RelativeLayoutParameter*>(_widget->getLayoutParameter());
    
    Margin mg = layoutParameter->getMargin();
   
    
    RelativeLayoutParameter::RelativeAlign align = layoutParameter->getAlign();
    
    //handle margin
    switch (align)
    {
        case RelativeLayoutParameter::RelativeAlign::NONE:
        case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_LEFT:
            _finalPositionX += mg.left;
            _finalPositionY -= mg.top;
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_CENTER_HORIZONTAL:
            _finalPositionY -= mg.top;
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_RIGHT:
            _finalPositionX -= mg.right;
            _finalPositionY -= mg.top;
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_CENTER_VERTICAL:
            _finalPositionX += mg.left;
            break;
        case RelativeLayoutParameter::RelativeAlign::CENTER_IN_PARENT:
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_RIGHT_CENTER_VERTICAL:
            _finalPositionX -= mg.right;
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_BOTTOM:
            _finalPositionX += mg.left;
            _finalPositionY += mg.bottom;
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_BOTTOM_CENTER_HORIZONTAL:
            _finalPositionY += mg.bottom;
            break;
        case RelativeLayoutParameter::RelativeAlign::PARENT_RIGHT_BOTTOM:
            _finalPositionX -= mg.right;
            _finalPositionY += mg.bottom;
            break;
            
        case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_LEFTALIGN:
            _finalPositionY += mg.bottom;
            _finalPositionX += mg.left;
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_RIGHTALIGN:
            _finalPositionY += mg.bottom;
            _finalPositionX -= mg.right;
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_CENTER:
            _finalPositionY += mg.bottom;
            break;
            
        case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_TOPALIGN:
            _finalPositionX -= mg.right;
            _finalPositionY -= mg.top;
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_BOTTOMALIGN:
            _finalPositionX -= mg.right;
            _finalPositionY += mg.bottom;
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_CENTER:
            _finalPositionX -= mg.right;
            break;
            
        case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_TOPALIGN:
            _finalPositionX += mg.left;
            _finalPositionY -= mg.top;
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_BOTTOMALIGN:
            _finalPositionX += mg.left;
            _finalPositionY += mg.bottom;
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_CENTER:
            _finalPositionX += mg.left;
            break;
            
        case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_LEFTALIGN:
            _finalPositionY -= mg.top;
            _finalPositionX += mg.left;
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_RIGHTALIGN:
            _finalPositionY -= mg.top;
            _finalPositionX -= mg.right;
            break;
        case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_CENTER:
            _finalPositionY -= mg.top;
            break;
        default:
            break;
    }
}


#else
 
bool RelativeLayoutManager::calculateFinalPositionWithRelativeWidget(LayoutProtocol *layout)
{
	Size cs = _widget->getBoundingBox().size;
	Size layoutSize = layout->getLayoutContentSize();

	_finalPositionX = 0.0f;
	_finalPositionY = 0.0f;

	Widget* relativeWidget = this->getRelativeWidget(_widget);
	RelativeLayoutParameter* layoutParameter = dynamic_cast<RelativeLayoutParameter*>(_widget->getLayoutParameter());
	RelativeLayoutParameter::RelativeAlign align = layoutParameter->getAlign();

	switch (align)
	{
	case RelativeLayoutParameter::RelativeAlign::NONE:
	case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_LEFT:
		_finalPositionX = 0.0f;
		_finalPositionY = 0.0f;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_CENTER_HORIZONTAL:
		_finalPositionX = layoutSize.width * 0.5f - cs.width * 0.5f;
		_finalPositionY = 0.0f;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_RIGHT:
		_finalPositionX = layoutSize.width - cs.width;
		_finalPositionY = 0.0f;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_CENTER_VERTICAL:
		_finalPositionX = 0.0f;
		_finalPositionY = layoutSize.height * 0.5f - cs.height * 0.5f;
		break;
	case RelativeLayoutParameter::RelativeAlign::CENTER_IN_PARENT:
		_finalPositionX = layoutSize.width * 0.5f - cs.width * 0.5f;
		_finalPositionY = layoutSize.height * 0.5f - cs.height * 0.5f;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_RIGHT_CENTER_VERTICAL:
		_finalPositionX = layoutSize.width - cs.width;
		_finalPositionY = layoutSize.height * 0.5f - cs.height * 0.5f;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_BOTTOM:
		_finalPositionX = 0.0f;
		_finalPositionY = layoutSize.height - cs.height;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_BOTTOM_CENTER_HORIZONTAL:
		_finalPositionX = layoutSize.width * 0.5f - cs.width * 0.5f;
		_finalPositionY = layoutSize.height - cs.height;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_RIGHT_BOTTOM:
		_finalPositionX = layoutSize.width - cs.width;
		_finalPositionY = layoutSize.height - cs.height;
		break;

		// 基于兄弟节点位置
	case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_LEFTALIGN:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			_finalPositionX = relativeWidget->getPosition().x;
			_finalPositionY = relativeWidget->getPosition().y - cs.height;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_CENTER:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x + rbs.width * 0.5f - cs.width * 0.5f;
			_finalPositionY = relativeWidget->getPosition().y - cs.height;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_RIGHTALIGN:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x + rbs.width - cs.width;
			_finalPositionY = relativeWidget->getPosition().y - cs.height;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_TOPALIGN:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			_finalPositionX = relativeWidget->getPosition().x - cs.width;
			_finalPositionY = relativeWidget->getPosition().y;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_CENTER:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x - cs.width;
			_finalPositionY = relativeWidget->getPosition().y + rbs.height * 0.5f - cs.height * 0.5f;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_BOTTOMALIGN:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x - cs.width;
			_finalPositionY = relativeWidget->getPosition().y + rbs.height - cs.height;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_TOPALIGN:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x + rbs.width;
			_finalPositionY = relativeWidget->getPosition().y;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_CENTER:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x + rbs.width;
			_finalPositionY = relativeWidget->getPosition().y + rbs.height * 0.5f - cs.height * 0.5f;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_BOTTOMALIGN:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x + rbs.width;
			_finalPositionY = relativeWidget->getPosition().y + rbs.height - cs.height;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_LEFTALIGN:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x;
			_finalPositionY = relativeWidget->getPosition().y + rbs.height;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_CENTER:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x + rbs.width * 0.5f - cs.width * 0.5f;
			_finalPositionY = relativeWidget->getPosition().y + rbs.height;
		}
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_RIGHTALIGN:
		if (relativeWidget) {
			if (_relativeWidgetLP && !_relativeWidgetLP->_put) return false;
			Size rbs = relativeWidget->getContentSize();
			_finalPositionX = relativeWidget->getPosition().x + rbs.width - cs.width;
			_finalPositionY = relativeWidget->getPosition().y + rbs.height;
		}
		break;
	default:
		break;
	}
	return true;
}

void RelativeLayoutManager::calculateFinalPositionWithRelativeAlign()
{
	RelativeLayoutParameter* layoutParameter = dynamic_cast<RelativeLayoutParameter*>(_widget->getLayoutParameter());
	Margin mg = layoutParameter->getMargin();
	RelativeLayoutParameter::RelativeAlign align = layoutParameter->getAlign();

	// SDL 坐标系下，Left 和 Top 都是正向增加，Right 和 Bottom 是反向减少
	switch (align)
	{
	case RelativeLayoutParameter::RelativeAlign::NONE:
	case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_LEFT:
		_finalPositionX += mg.left;
		_finalPositionY += mg.top;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_CENTER_HORIZONTAL:
		_finalPositionY += mg.top;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_TOP_RIGHT:
		_finalPositionX -= mg.right;
		_finalPositionY += mg.top;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_CENTER_VERTICAL:
		_finalPositionX += mg.left;
		break;
	case RelativeLayoutParameter::RelativeAlign::CENTER_IN_PARENT:
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_RIGHT_CENTER_VERTICAL:
		_finalPositionX -= mg.right;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_BOTTOM:
		_finalPositionX += mg.left;
		_finalPositionY -= mg.bottom;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_BOTTOM_CENTER_HORIZONTAL:
		_finalPositionY -= mg.bottom;
		break;
	case RelativeLayoutParameter::RelativeAlign::PARENT_RIGHT_BOTTOM:
		_finalPositionX -= mg.right;
		_finalPositionY -= mg.bottom;
		break;

	case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_LEFTALIGN:
		_finalPositionY -= mg.bottom;
		_finalPositionX += mg.left;
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_RIGHTALIGN:
		_finalPositionY -= mg.bottom;
		_finalPositionX -= mg.right;
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_CENTER:
		_finalPositionY -= mg.bottom;
		break;

	case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_TOPALIGN:
		_finalPositionX -= mg.right;
		_finalPositionY += mg.top;
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_BOTTOMALIGN:
		_finalPositionX -= mg.right;
		_finalPositionY -= mg.bottom;
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_LEFT_OF_CENTER:
		_finalPositionX -= mg.right;
		break;

	case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_TOPALIGN:
		_finalPositionX += mg.left;
		_finalPositionY += mg.top;
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_BOTTOMALIGN:
		_finalPositionX += mg.left;
		_finalPositionY -= mg.bottom;
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_CENTER:
		_finalPositionX += mg.left;
		break;

	case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_LEFTALIGN:
		_finalPositionY += mg.top;
		_finalPositionX += mg.left;
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_RIGHTALIGN:
		_finalPositionY += mg.top;
		_finalPositionX -= mg.right;
		break;
	case RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_CENTER:
		_finalPositionY += mg.top;
		break;
	default:
		break;
	}
}
#endif
    


void RelativeLayoutManager::doLayout(LayoutProtocol *layout)
{

	_widgetChildren = this->getAllWidgets(layout);

	while (_unlayoutChildCount > 0)
	{
		for (auto& subWidget : _widgetChildren)
		{
			_widget = static_cast<Widget*>(subWidget);

			RelativeLayoutParameter* layoutParameter = dynamic_cast<RelativeLayoutParameter*>(_widget->getLayoutParameter());

			if (layoutParameter)
			{
				if (layoutParameter->_put)
				{
					continue;
				}


				bool ret = this->calculateFinalPositionWithRelativeWidget(layout);
				if (!ret) {
					continue;
				}

				this->calculateFinalPositionWithRelativeAlign();


				_widget->setPosition(Vec2(_finalPositionX, _finalPositionY));

				layoutParameter->_put = true;
			}
		}
		_unlayoutChildCount--;

	}
	_widgetChildren.clear();
}

}

OG_END

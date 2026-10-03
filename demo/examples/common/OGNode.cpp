#include "OGNode.h"
#include "OGDirector.h"
#include "OGEventDispatcher.h"
#include "OGCamera.h"

OG_BEGIN
 

void Node::sortAllChildren()
{
	if (_reorderChildDirty)
	{
		sortNodes(_children);
		_reorderChildDirty = false;
		//_eventDispatcher->setDirtyForNode(this);
	}
}
void Node::setColor(const Color3B& color)
{
	_displayedColor = _realColor = color;

	updateDisplayedColor(_displayedColor);
}
void Node::updateDisplayedColor(const Color3B& parentColor)
{
	_displayedColor.r = _realColor.r * parentColor.r / 255.0;
	_displayedColor.g = _realColor.g * parentColor.g / 255.0;
	_displayedColor.b = _realColor.b * parentColor.b / 255.0;
	updateColor();

// 	if (_cascadeColorEnabled)
// 	{
// 		for (const auto &child : _children)
// 		{
// 			child->updateDisplayedColor(_displayedColor);
// 		}
// 	}
}
void Node::updateDisplayedOpacity(uint8_t parentOpacity)
{
	_displayedOpacity = _realOpacity * parentOpacity / 255.0;
}
const Color3B& Node::getColor() const
{
	return _realColor;
}
void Node::setCameraMask(unsigned short mask, bool applyChildren)
{
	_cameraMask = mask;
// 	if (applyChildren)
// 	{
// 		for (const auto& child : _children)
// 		{
// 			child->setCameraMask(mask, applyChildren);
// 		}
// 	}
}
void Node::setGlobalZOrder(float globalZOrder)
{
	_globalZOrder = globalZOrder;
}

uint32_t Node::processParentFlags(const Mat3& parentTransform, uint32_t parentFlags)
{
	uint32_t flags = parentFlags;
	flags |= (_transformUpdated ? FLAGS_TRANSFORM_DIRTY : 0);

	if (flags & FLAGS_DIRTY_MASK)
	{
		_modelViewTransform = this->transform(parentTransform);
		// true;
	}

	_transformUpdated = false;

	return flags;
}
bool Node::isVisitableByVisitingCamera() const
{
	auto camera = Camera::getVisitingCamera();
	bool visibleByCamera = camera ? ((unsigned short)camera->getCameraFlag() & _cameraMask) != 0 : true;
	return visibleByCamera;
}
void Node::visit(Renderer* ren, const Mat3 & parentTransform, uint32_t parentFlags)
{
	if (!_visible) return;

	std::string debug = _name;

	uint32_t flags = processParentFlags(parentTransform, parentFlags);

	bool visibleByCamera = isVisitableByVisitingCamera();

	int i = 0;

	//后面都是用这个结果去计算_modelViewTransform
	if (!_children.empty())
	{
		sortAllChildren();
		// draw children zOrder < 0  
		for (auto size = _children.size(); i < size; ++i)
		{
			auto node = _children.at(i);

			if (node && node->_localZOrder < 0)
				node->visit(ren, _modelViewTransform, flags);
			else
				break;
		}
		if (visibleByCamera)
			this->draw(ren, _modelViewTransform, flags);

		for (auto it = _children.cbegin() + i, itCend = _children.cend(); it != itCend; ++it)
			(*it)->visit(ren, _modelViewTransform, flags);
	}
	else if (visibleByCamera)
	{
		this->draw(ren, _modelViewTransform, flags);
	}

}
Mat3 Node::transform(const Mat3& parentTransform)
{
	return parentTransform * this->getNodeToParentTransform();
}
Node::Node()
{
	_eventDispatcher = EventDispatcher::getInstance();
	_scheduler = Director::getInstance()->getScheduler();
}
Node::~Node()
{
}
const Mat3 & Node::getNodeToParentTransform() const
{
	if (_transformDirty)
	{
		Mat3 translation = Mat3::createTranslation(_position.x, _position.y);
		//初始化
		_transform.reset();

		// 		if (_scaleX != 1.f)
		// 		{
		// 			_transform.a *= _scaleX;
		// 		}
		// 		if (_scaleY != 1.f)
		// 		{
		// 			_transform.d *= _scaleY;
		// 		}
		if (_scaleX != 1.f || _scaleY != 1.f)
		{
			_transform = _transform.createScale(_scaleX, _scaleY);
		}
		if (_skewX != 0.0f || _skewY != 0.0f)
		{
			_transform = _transform * Mat3::createSkew(_skewX, _skewY);
		}

		Mat3 rotation;
		if (_rotationX != 0.0f || _rotationY != 0.0f)
		{
			rotation = Mat3::createRotationSDL(_center, Vec2(_rotationX, _rotationY));
		}
		//先缩放，倾斜，旋转， 再平移

		_transform = translation * rotation * _transform;
	}

	_transformDirty = false;

	return _transform;
}
OG_END
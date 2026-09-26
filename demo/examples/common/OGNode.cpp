#include "OGNode.h"
#include "OGDirector.h"
#include "OGEventDispatcher.h"

OG_BEGIN

const Mat3 & Node::getNodeToParentTransform() const
{
	if (_transformDirty)
	{
		Mat3 translation = Mat3::createTranslation(_position.x, _position.y);
		//初始化
		_transform.reset();
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

void Node::sortAllChildren()
{
	if (_reorderChildDirty)
	{
		sortNodes(_children);
		_reorderChildDirty = false;
		//_eventDispatcher->setDirtyForNode(this);
	}
}


OG_END
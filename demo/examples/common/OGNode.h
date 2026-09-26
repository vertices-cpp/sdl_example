#ifndef _NODE_H_
#define _NODE_H_ 

#include "OGRenderer.h"
#include "OGRef.h"
#include "ogTypes.h"
#include "Vector.h"
#include "OGGeometry.h"
#include "Mat3.h"
#include "Vec2.h"



OG_BEGIN

template<typename _T> inline
static void sortNodes(orange::Vector<_T*>& nodes)
{
	static_assert(std::is_base_of<Node, _T>::value, "Node::sortNodes: Only accept derived of Node!");
#if OG_64BITS
	std::sort(std::begin(nodes), std::end(nodes), [](_T* n1, _T* n2) {
		return (n1->_localZOrder$Arrival < n2->_localZOrder$Arrival);
	});
#else
	std::sort(std::begin(nodes), std::end(nodes), [](_T* n1, _T* n2) {
		return (n1->_localZOrder == n2->_localZOrder ) || n1->_localZOrder < n2->_localZOrder;
	});
#endif
}

class Director;  
class EventDispatcher; 
struct Color4B;

class Node:public Ref {
protected:
	Color4B  _displayedColor = Color4B::WHITE;
	mutable Mat3 _transform, _modelViewTransform;
	mutable bool _transformDirty = true;

	float _scaleX = 1.0f, _scaleY = 1.0f;
	float _skewX = 0.0f, _skewY = 0.0f;
	float  _rotationX = 0.0f  , _rotationY = 0.0f;
	
	bool _reorderChildDirty;

	Vec2 _position = Vec2::ZERO;
	Size _contentSize = Size::ZERO;
	Vec2 _center = Vec2::ZERO;
	Node *_parent = nullptr;
	
	float _globalZ;
	short _cameraMask = 1;
	Vector<Node*> _children;
public:
	uint32_t _localZOrder = 0;
	CREATE_FUNC(Node);
	virtual bool init() {
		return true;
	}
	virtual void setPosition(const Vec2 &V) {
		_position = V;
	}
	virtual void setPosition(float x,float y) {
		setPosition(Vec2(x, y));
	}
	virtual void setVisible(bool visible) 
	{
		 
	}
	virtual bool isVisible() const  
	{
		return true;
	}
	virtual void draw(Renderer* /*renderer*/, const Mat3 & /*transform*/, uint32_t /*flags*/) 
	{

	}
	virtual Rect getBoundingBox() const
	{ 
		Rect rect(0, 0, _contentSize.width, _contentSize.height);
		return RectApplyMat3(rect, getNodeToParentTransform());
	}
	virtual Node* getParent() {
		return _parent;
	}
	virtual Mat3 getNodeToParentTransform(Node* ancestor) const
	{
		Mat3 t(this->getNodeToParentTransform());

		for (Node *p = _parent; p != nullptr && p != ancestor; p = p->getParent())
		{
			t = p->getNodeToParentTransform() * t;
		}

		return t;
	}
	virtual const Mat3 &  getNodeToParentTransform() const;
	virtual void  sortAllChildren();
	virtual float getGlobalZOrder() { return _globalZ; }
	virtual short getCameraMask() {
		return _cameraMask;
	}
	virtual Size getContentSize() { return _contentSize; }
	virtual void setContentSize(const Size &s) { _contentSize = s; }
	virtual void setRotationSkewX(float skewX)
	{
		_skewX = skewX;
	}
};


OG_END
#endif

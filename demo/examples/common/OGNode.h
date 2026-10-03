#ifndef _NODE_H_
#define _NODE_H_ 

#include "OGRenderer.h"
#include "Vector.h"
#include "Mat3.h"
#include "ogTypes.h" 
#include "OGScheduler.h"



OG_BEGIN



class Director;  
class EventDispatcher; 
struct Color4B;

class Node :public Ref {
protected:
	enum {
		FLAGS_TRANSFORM_DIRTY = (1 << 0),
		FLAGS_CONTENT_SIZE_DIRTY = (1 << 1),
		FLAGS_RENDER_AS_3D = (1 << 3),

		FLAGS_DIRTY_MASK = (FLAGS_TRANSFORM_DIRTY | FLAGS_CONTENT_SIZE_DIRTY),
	};

	  EventDispatcher* _eventDispatcher;
	  Scheduler *_scheduler;
	std::string _name;
	uint32_t _tag = 0;
	bool _visible = true;

	Color3B  _displayedColor = Color3B::WHITE;
	uint8_t _displayedOpacity = 255, _realOpacity =255;
	Color3B _realColor = Color3B::WHITE;
	mutable Mat3 _transform, _modelViewTransform;
	mutable bool _transformUpdated = true;
	mutable bool _transformDirty = true;

	float _scaleX = 1.0f, _scaleY = 1.0f;
	float _skewX = 0.0f, _skewY = 0.0f;
	float  _rotationX = 0.0f, _rotationY = 0.0f;

	bool _reorderChildDirty;

	Vec2 _position = Vec2::ZERO;
	Size _contentSize = Size::ZERO;
	Vec2 _center = Vec2::ZERO;
	Node *_parent = nullptr;

	float _globalZOrder;
	short _cameraMask = 1;
	Vector<Node*> _children;
	uint32_t _localZOrder = 0;

public:
	
	CREATE_FUNC(Node);
	Node();
	~Node();
	virtual bool init() {
		return true;
	}
	virtual void addChild(Node *node) {
		_children.pushBack(node);
	}
	virtual void setPosition(const Vec2 &V) {
		_position = V;
	}
	virtual void setPosition(float x, float y) {
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
	virtual void update(float) {}
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
	virtual void setColor(const Color3B & color);
	virtual void updateDisplayedColor(const Color3B & parentColor);
	virtual void updateDisplayedOpacity(uint8_t parentOpacity);
	virtual const Color3B & getColor() const;
	virtual void updateColor() {}
	virtual float getGlobalZOrder() { return _globalZOrder; }
	virtual short getCameraMask() {
		return _cameraMask;
	}
	virtual Size getContentSize() { return _contentSize; }
	virtual void setContentSize(const Size &s) { _contentSize = s; }
	virtual void setSkewX(float skewX)
	{
		_skewX = skewX;
	}
	virtual void setCameraMask(unsigned short mask, bool applyChildren = true);
	void setGlobalZOrder(float globalZOrder);
	virtual uint32_t processParentFlags(const Mat3 & parentTransform, uint32_t parentFlags);
	virtual bool isVisitableByVisitingCamera() const;
	virtual void visit(Renderer * ren, const Mat3 & parentTransform, uint32_t parentFlags);
 
	virtual Mat3 transform(const Mat3 & parentTransform);
	virtual std::int32_t getLocalZOrder() const { return _localZOrder; }

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
			return (n1->_localZOrder == n2->_localZOrder) || n1->_localZOrder < n2->_localZOrder;
		});
#endif
	}
	virtual void scheduleUpdate() { _scheduler->scheduleUpdate(this); }
	virtual void unscheduleUpdate() { _scheduler->unscheduleUpdate(this); }
};


OG_END
#endif

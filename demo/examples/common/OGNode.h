#ifndef _NODE_H_
#define _NODE_H_ 

#include "OGRenderer.h"
#include "OGVector.h"
#include "Mat3.h"
#include "ogTypes.h" 
#include "OGScheduler.h"



OG_BEGIN

class Action;
class ActionManager;
class Camera;
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
	Director* _director;
	  EventDispatcher* _eventDispatcher;
	  Scheduler *_scheduler;
	  ActionManager *  _actionManager;
	std::string _name;
	uint32_t _tag = 0;
	bool _running = true;
	bool _visible = true;
	bool _cascadeColorEnabled= false,_cascadeOpacityEnabled = false;

	Color3B  _displayedColor = Color3B::WHITE;
	uint8_t _displayedOpacity = 255, _realOpacity =255;
	Color3B _realColor = Color3B::WHITE;
	mutable Mat3 _transform, _modelViewTransform;
	mutable bool _transformUpdated = true;
	mutable bool _transformDirty = true;
	
	Mat3 _modelViewTransformInverse;

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
	virtual void setTransformUpdatedDirty();
	virtual bool init() {
		return true;
	}
	
	virtual void setPosition(const Vec2 &V) {
		_position = V;
		_transformUpdated = _transformDirty = true;
	}
	virtual void setPosition(float x, float y) {
		setPosition(Vec2(x, y));
		_transformUpdated = _transformDirty = true;
	}

	bool Node::isRunning() const
	{
		return _running;
	}
	virtual void setVisible(bool visible)
	{
		_visible = visible;

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

	virtual void setOpacityModifyRGB(bool value) {};
	virtual bool isOpacityModifyRGB() const { return false; };
	virtual void cleanup();
	virtual void removeChild(Node* child, bool cleanup)  ;

	void detachChild(Node * child, size_t childIndex, bool doCleanup);

	void setParent(Node * parent);

	virtual void addChild(Node * child)  ;
	virtual void addChild(Node * child, int zOrder)  ;
	virtual void addChild(Node * child, int zOrder, int tag)  ;
	virtual void addChild(Node * child, int zOrder, const std::string &name)  ;

	virtual void removeFromParentAndCleanup(bool cleanup);

	virtual void onEnter()  ;
	virtual void onExit()  ;

	void setScheduler(Scheduler * scheduler);

	virtual bool isScheduled(SEL_SCHEDULE selector) const;

	virtual  bool isScheduled(const std::string & key) const;

	virtual Vector<Node*>& getChildren() { return _children; }
	virtual Node* getParent() {
		return _parent;
	}
	virtual Mat3 getNodeToParentTransform(Node* ancestor)const;
	virtual void setScale(float scale);
	virtual void setScaleX(float scaleX);
	virtual void setScaleY(float scaleY);

	//添加每级求逆
	virtual const Mat3& getModelViewTransformInverse() const;
	virtual const Mat3 &  getNodeToParentTransform() const;

	virtual Mat3 getWorldToNodeTransform() const;

	Mat3 getNodeToWorldTransform() const;

	Mat3 getNodeTopParent(Node * ancestor) const;
	
	virtual float getScaleX() const;
	virtual float getScaleY() const;
	virtual float getScale() const;
	void stopAction(Action * action);
	void stopAllActions();
	virtual void  sortAllChildren();
	virtual void setCascadeColorEnabled(bool cascadeColorEnabled);
	virtual void updateCascadeColor();
	virtual bool isCascadeColorEnabled() const;
	virtual const Color3B & getDisplayedColor() const;
	virtual void disableCascadeColor();
	virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled);
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

	Action * runAction(Action * action);

	void stopActionByTag(int tag);
	void stopAllActionsByTag(int tag);
	void stopActionsByFlags(unsigned int flags);
	Action * getActionByTag(int tag);
	ssize_t getNumberOfRunningActions() const;
	ssize_t getNumberOfRunningActionsByTag(int tag) const;

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
			return  n1->_localZOrder < n2->_localZOrder;
		});
#endif
	}
	virtual void scheduleUpdate();
	void scheduleUpdateWithPriority(int priority);
	virtual void unscheduleUpdate();
	void schedule(SEL_SCHEDULE selector);
	void schedule(SEL_SCHEDULE selector, float interval);
	void schedule(SEL_SCHEDULE selector, float interval, unsigned int repeat, float delay);
	void schedule(const std::function<void(float)>& callback, const std::string & key);
	void schedule(const std::function<void(float)>& callback, float interval, const std::string & key);
	void schedule(const std::function<void(float)>& callback, float interval, unsigned int repeat, float delay, const std::string & key);
	void scheduleOnce(SEL_SCHEDULE selector, float delay);
	void scheduleOnce(const std::function<void(float)>& callback, float delay, const std::string & key);
	void unschedule(SEL_SCHEDULE selector);
	void unschedule(const std::string & key);
	void unscheduleAllCallbacks();
	void resume();
	void pause();
};

extern bool isScreenPointInRect(const Vec2 &pt,
	const Camera* camera,
	const Mat3& w2l,
	const Rect& rect,
	Vec2 *p);

OG_END
#endif

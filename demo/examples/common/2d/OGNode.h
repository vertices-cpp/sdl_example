#ifndef _NODE_H_
#define _NODE_H_ 

#include "renderer/OGRenderer.h"
#include "base/OGVector.h"
#include "math/Mat3.h"
#include "base/ogTypes.h" 
#include "base/OGScheduler.h"

#include "2d/OGComponentContainer.h"
#include "2d/OGComponent.h"

#if OG_USE_PHYSICS
#include "physics/OGPhysicsBody.h"
#endif

OG_BEGIN

class Action;
class ActionManager;
class Camera;
class Director;  

class Component;
class ComponentContainer;

class EventDispatcher; 
struct Color4B;
class Scene;
//物理
class PhysicsBody;

class EventListener;
class Touch;

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
	size_t _hashOfName;

	uint32_t _tag = 0;
	bool _running;
	bool _visible;
	bool _cascadeColorEnabled= false,_cascadeOpacityEnabled = false;

	Color3B  _displayedColor = Color3B::WHITE;
	uint8_t _displayedOpacity = 255, _realOpacity =255;
	Color3B _realColor = Color3B::WHITE;
	mutable Mat3 _transform, _modelViewTransform;
	mutable bool _transformUpdated = true;
	mutable bool _transformDirty = true;
	mutable bool _inverseDirty = true;

	Mat3 _modelViewTransformInverse;

	float _scaleX = 1.0f, _scaleY = 1.0f;
	float _skewX = 0.0f, _skewY = 0.0f;
	float  _rotationX = 0.0f, _rotationY = 0.0f, _rotationZ = 0.0f;

	bool _reorderChildDirty;
	bool _isTransitionFinished;

	Vec2 _position = Vec2::ZERO;
	Size _contentSize = Size::ZERO;
	Vec2 _anchorPoint = Vec2::ZERO;
	Vec2 _anchorPointInPoints = Vec2::ZERO;
	Node *_parent = nullptr;

	float _globalZOrder;
	short _cameraMask = 1;
	Vector<Node*> _children;
 
	static std::uint32_t s_globalOrderOfArrival;

	


	std::function<void()> _onEnterCallback;
	std::function<void()> _onExitCallback;
	std::function<void()> _onEnterTransitionDidFinishCallback;

	std::function<void()> _onExitTransitionDidStartCallback;
 
public:
#if OG_LITTLE_ENDIAN
	union {
		struct {
			std::uint32_t _orderOfArrival;
			std::int32_t _localZOrder;
		};
		std::int64_t _localZOrder$Arrival;
	};
#else
	union {
		struct {
			std::int32_t _localZOrder;
			std::uint32_t _orderOfArrival;
		};
		std::int64_t _localZOrder$Arrival;
	};
#endif

	void setOnEnterCallback(const std::function<void()>& callback) { _onEnterCallback = callback; }
	const std::function<void()>& getOnEnterCallback() const { return _onEnterCallback; }
	void setOnExitCallback(const std::function<void()>& callback) { _onExitCallback = callback; }
	const std::function<void()>& getOnExitCallback() const { return _onExitCallback; }
	void setOnEnterTransitionDidFinishCallback(const std::function<void()>& callback) { _onEnterTransitionDidFinishCallback = callback; }
	const std::function<void()>& getOnEnterTransitionDidFinishCallback() const { return _onEnterTransitionDidFinishCallback; }
	void setOnExitTransitionDidStartCallback(const std::function<void()>& callback) { _onExitTransitionDidStartCallback = callback; }
	const std::function<void()>& getOnExitTransitionDidStartCallback() const { return _onExitTransitionDidStartCallback; }

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
	virtual Rect getBoundingBox() const
	{
		Rect rect(0, 0, _contentSize.width, _contentSize.height);
		return RectApplyMat3(rect, getNodeToParentTransform());
	}

	virtual void setOpacityModifyRGB(bool value) {};
	virtual bool isOpacityModifyRGB() const { return false; };
	virtual void setRotation(float rotation);
	virtual void cleanup();
	virtual void removeAllChildrenWithCleanup(bool cleanup);
	virtual void removeChild(Node* child, bool cleanup)  ;

	void detachChild(Node * child, size_t childIndex, bool doCleanup);

	void setParent(Node * parent);

	virtual void addChild(Node * child)  ;
	virtual void addChild(Node * child, int zOrder)  ;
	virtual void addChild(Node * child, int zOrder, int tag)  ;
	virtual void addChild(Node * child, int zOrder, const std::string &name)  ;
	void setName(const std::string & name);
	const std::string & getName() const;
	void addChildHelper(Node * child, int localZOrder, int tag, const std::string & name, bool setTag);
	void childrenAlloc();
	void insertChild(Node * child, int z);
	virtual void removeFromParent();
	virtual void removeFromParentAndCleanup(bool cleanup);
	virtual void removeAllChildren();
	virtual void disableCascadeOpacity();
 
	virtual void onEnter()  ;
	virtual void onExit()  ;

	virtual void onEnterTransitionDidFinish();

	virtual void onExitTransitionDidStart();

	Vec2 convertTouchToNodeSpace(Touch * touch) const;

	Vec2 convertToNodeSpace(const Vec2 & worldPoint) const;

	void setLocalZOrder(int z);

	virtual void reorderChild(Node * child, int zOrder);

	virtual void _setLocalZOrder(int z);

	virtual void setScheduler(Scheduler * scheduler);

	virtual bool isScheduled(SEL_SCHEDULE selector) const;

	virtual  bool isScheduled(const std::string & key) const;

	virtual Vector<Node*>& getChildren() { return _children; }
	virtual const Vector<Node*>& getChildren() const { return _children; }
	virtual Node* getParent() {
		return _parent;
	}
	virtual Mat3 getNodeToParentTransform(Node* ancestor)const;
	virtual void setScale(float scale);
	virtual void setScaleX(float scaleX);
	virtual void setScaleY(float scaleY);
	virtual void setScale(float scalex, float scaley);
	//添加每级求逆
	virtual const Mat3& getModelViewTransformInverse() const;
	virtual const Mat3 &  getNodeToParentTransform() const;

	virtual Mat3 getWorldToNodeTransform() const;

	Mat3 getNodeToWorldTransform() const;

	Mat3 getNodeTopParent(Node * ancestor) const;
	
	virtual float getScaleX() const;
	virtual float getScaleY() const;
	virtual float getScale() const;
	virtual float getRotation() const;
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
	virtual void setContentSize(const Size &s) { 
		_contentSize = s;
		_transformUpdated = _transformDirty = true;
	}
 
	virtual void setCameraMask(unsigned short mask, bool applyChildren = true);
	virtual void setGlobalZOrder(float globalZOrder);
	virtual void updateOrderOfArrival();
	virtual void updateCascadeOpacity();
	virtual bool isCascadeOpacityEnabled() const;
	virtual uint32_t processParentFlags(const Mat3 & parentTransform, uint32_t parentFlags);
	virtual bool isVisitableByVisitingCamera() const;
	virtual void visit(Renderer * ren, const Mat3 & parentTransform, uint32_t parentFlags);
 
	virtual Mat3 transform(const Mat3 & parentTransform);
	virtual std::int32_t getLocalZOrder() const { return _localZOrder; }


	virtual void setEventDispatcher(EventDispatcher* dispatcher);
	virtual EventDispatcher* getEventDispatcher() const { return _eventDispatcher; };
	virtual void setActionManager(ActionManager* actionManager);
	virtual ActionManager* getActionManager() { return _actionManager; }
	virtual const ActionManager* getActionManager() const { return _actionManager; }

	Action * runAction(Action * action);

	void stopActionByTag(int tag);
	void stopAllActionsByTag(int tag);
	void stopActionsByFlags(unsigned int flags);
	Action * getActionByTag(int tag);
	ssize_t getNumberOfRunningActions() const;
	ssize_t getNumberOfRunningActionsByTag(int tag) const;

	virtual Scene * getScene() const;

	Vec2 convertToWorldSpace(const Vec2 & nodePoint) const;

	virtual const Vec2 & getAnchorPoint() const;

	virtual void setAnchorPoint(const Vec2 & point);

	virtual const Size & getContentSize() const;

	virtual const Vec2 & getPosition() const;

	virtual const Vec2 & getCenter() const;

	virtual void setSkewX(float skewX);

	virtual void setSkewY(float skewY);

	float getSkewX() const;

	float getSkewY() const;
	 

	virtual float getPositionX() const;

	virtual void setPositionX(float x);

	virtual float getPositionY() const;

	virtual void setPositionY(float y);
	virtual void setCenter(const Vec2 &center);

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

	virtual std::string getDescription() const;

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
	virtual void resume();
	virtual void pause();
 
#if OG_USE_PHYSICS
	
public:
	PhysicsBody* _physicsBody;
	void setPhysicsBody(PhysicsBody* physicsBody);
	PhysicsBody* getPhysicsBody() const { 
		return _physicsBody; 
	}

	friend class PhysicsBody;
#endif
	Component* getComponent(const std::string& name); 
	virtual bool addComponent(Component *component); 
	virtual bool removeComponent(const std::string& name); 
	virtual bool removeComponent(Component *component); 
	virtual void removeAllComponents();
	virtual void update(float fDelta);
	virtual uint8_t getOpacity() const;
	virtual uint8_t getDisplayedOpacity() const;
	virtual void setOpacity(uint8_t opacity);
	int getTag() const;
	void setTag(int tag);
	virtual Node * getChildByName(const std::string & name) const;
	virtual Node * getChildByTag(int tag) const;
 
	virtual size_t getChildrenCount() const;
	ComponentContainer* _componentContainer;        ///< Dictionary of components
};

extern bool isScreenPointInRect(const Vec2 &pt,
	const Camera* camera,
	const Mat3& w2l,
	const Rect& rect,
	Vec2 *p);

OG_END
#endif

#ifndef _OG_NODE_H_
#define _OG_NODE_H_

#include <vector>
#include <string>

#include <cstdint>
#include "base/ogMacros.h"
#include "base/OGVector.h"
#include "base/OGProtocols.h"
// #include "base/OGScriptSupport.h"
// #include "math/OGAffineTransform.h"
//#include "math/OGMath.h"
#include "math/Vec2.h"
#include "math/Mat3.h"
#include "2d/OGComponentContainer.h"
#include "2d/OGComponent.h"

#if OG_USE_PHYSICS
#include "physics/OGPhysicsBody.h"
#endif

//#define USE_ANCHOR_POINT

//#include "OGType.hpp"

OG_BEGIN

//class Scene;
//class GridBase;
class Touch;
class Action;
class LabelProtocol;
class Scheduler;
class ActionManager;

class Component;
class ComponentContainer;

class EventDispatcher;
class Scene;
class Renderer;
class Director;
//class Material;
class Camera;

class PhysicsBody;


class Node :public Ref
{

protected:

	Director* _director;

	Scheduler *_scheduler;

	ActionManager *_actionManager;  ///< a pointer to ActionManager singleton, which is used to handle all the actions

	EventDispatcher* _eventDispatcher;  ///< event dispatcher used to dispatch all kinds of events

	int _tag;
	bool _running;                  ///< is running
	bool _visible;                  ///< is this node visible
	std::string _name;
	size_t _hashOfName;




	float _scaleX;
	float _scaleY;
	float _rotationX;               ///< rotation on the X-axis
	float _rotationY;
	float _rotationZ;

	Vec2 _normalizedPosition;
	bool _usingNormalizedPosition;
	bool _normalizedPositionDirty;

	float _skewX;
	float _skewY;

	//Vec2 _anchorPointInPoints;      ///< anchor point in points
	Vec2 _anchorPoint;

	mutable Vec2 _anchorPointInPoints;
	bool _ignoreAnchorPointForPosition;
	Vec2 _position;
	Size _contentSize;              ///< untransformed size of the node
	bool _contentSizeDirty;

	Vector<Node*> _children;
	Node *_parent;

	static std::uint32_t s_globalOrderOfArrival;

	int _localZOrder;
	float _globalZOrder;
	bool _reorderChildDirty;
	bool _isTransitionFinished;
	//	bool _dirty;


		//矩形
	// 	V2F_C4C_QUAD _transform = { 0 };
	// 	int indices[6] = { 0 };
	//	bool _transformDirty;

	static int __attachedNodeCount;


	mutable Mat3 _transform;        ///< transform
	mutable bool _transformDirty;   ///< transform dirty flag

	mutable Mat3 _inverse;
	mutable bool  _inverseDirty;

	bool _transformUpdated;
	Mat3 _modelViewTransform;
	//增加节点求逆矩形缓存
	Mat3 _modelViewTransformInverse;

	Color3B _displayedColor, _realColor;
	uint8_t     _displayedOpacity;
	uint8_t     _realOpacity;

	unsigned short _cameraMask;

	bool _cascadeColorEnabled;
	bool  _cascadeOpacityEnabled;

	std::function<void()> _onEnterCallback;
	std::function<void()> _onExitCallback;
	std::function<void()> _onEnterTransitionDidFinishCallback;
	std::function<void()> _onExitTransitionDidStartCallback;

public:

	//void setOriginalDestPos(const Vec2 &originalDestPosition) { _originalDestPosition = originalDestPosition; }


	enum {
		FLAGS_TRANSFORM_DIRTY = (1 << 0),
		FLAGS_CONTENT_SIZE_DIRTY = (1 << 1),
		//	FLAGS_RENDER_AS_3D = (1 << 3),

		FLAGS_DIRTY_MASK = (FLAGS_TRANSFORM_DIRTY | FLAGS_CONTENT_SIZE_DIRTY),
	};

	static const int INVALID_TAG = -1;

	// 	bool _contentChange;
	// 	bool _contentSizeDirty;
	// 	Vec2  _render_position;

	Node * create();
	virtual bool init();

	Node();
	virtual ~Node();


	virtual const std::string & getName() const;
	virtual void setName(const std::string & name);
	virtual int getTag() const;
	virtual void setTag(int tag);

	virtual bool isRunning() const;


	virtual void setVisible(bool visible);
	virtual bool isVisible() const;



	virtual Scene * getScene() const;

	void setTransformUpdatedDirty();

	virtual void _setLocalZOrder(int z);
	virtual void updateOrderOfArrival();
	virtual void setLocalZOrder(int z);
	virtual float getGlobalZOrder() const;
	virtual void setGlobalZOrder(float globalZOrder);
	virtual int getLocalZOrder() const { return _localZOrder; }
	virtual void reorderChild(Node * child, int zOrder);

	unsigned short getCameraMask() const { return _cameraMask; }
	virtual void setCameraMask(unsigned short mask, bool applyChildren = true);

	virtual void sortAllChildren();



	virtual void setScaleX(float scaleX);
	virtual void setScaleY(float scaley);
	virtual float getScaleX()const;
	virtual float getScaleY()const;



	virtual void setScale(const Vec2 &);
	virtual void setScale(float scaleX, float scaleY);
	virtual void setScale(float scale);
	virtual float getScale() const;
	virtual Vec2 getScaleVec2() const;

	virtual float getRotation() const;

	virtual void setRotation(float rotation);

	virtual void setRotationX(float rotation);
	virtual void setRotationY(float rotation);

	virtual float getRotationX() const;
	virtual float getRotationY() const;

	virtual void setSkewX(float skewX);
	virtual void setSkewY(float skewY);
	virtual float getSkewX() const;

	virtual float getSkewY() const;


	virtual void setCenter(const Vec2 &center);
	virtual const Vec2& getCenter() const;


	virtual  const Vec2&  getPosition() const;


	virtual void  getPosition(float* x, float* y) const
	{
		*x = _position.x;
		*y = _position.y;
	}

	virtual void setPosition(const Vec2 & position);

	virtual void setPosition(float x, float y);

	virtual void setPositionNormalized(const Vec2 & position);
	virtual void setNormalizedPosition(const Vec2 &position) { setPositionNormalized(position); }

	virtual void setPositionX(float x);
	virtual float getPositionX() const;

	virtual void setPositionY(float y);
	virtual float getPositionY() const;


	virtual void setContentSize(const Size & size);
	virtual const Vec2 & getAnchorPointInPoints() const;

	virtual const Vec2 & getAnchorPoint() const;
	virtual void setAnchorPoint(const Vec2 & point);

	virtual const Size& getContentSize() const;


	virtual Rect getBoundingBox() const;

	virtual uint8_t getOpacity() const;
	virtual uint8_t  getDisplayedOpacity() const;
	virtual void setOpacity(uint8_t opacity);

	virtual void  updateDisplayedOpacity(uint8_t parentOpacity);
	virtual bool isCascadeOpacityEnabled() const;
	virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled);


	virtual void  updateDisplayedColor(const Color3B& parentColor);
	virtual bool isCascadeColorEnabled() const;
	virtual void  setCascadeColorEnabled(bool cascadeColorEnabled);
	virtual void setOpacityModifyRGB(bool value);
	virtual bool isOpacityModifyRGB() const;



	virtual const Color3B getColor() const;
	virtual const Color3B&  getDisplayedColor() const;
	virtual void setColor(const Color3B& color);


	virtual size_t getChildrenCount() const;


	virtual bool isIgnoreAnchorPointForPosition() const;
	virtual void setIgnoreAnchorPointForPosition(bool newValue);
	virtual void setParent(Node * parent);

	virtual Node* getParent() {
		return _parent;
	}
	virtual const Node* getParent() const {
		return _parent;
	}



	virtual Mat3 getNodeTopParent(Node * ancestor) const;
	virtual Mat3 getNodeToParentTransform(Node * ancestor) const;
	Vec2 convertToNodeSpaceAR(const Vec2 & worldPoint) const;
	Vec2 convertToWorldSpaceAR(const Vec2 & nodePoint) const;

	Vec2 convertTouchToNodeSpace(Touch * touch) const;
	Vec2 convertTouchToNodeSpaceAR(Touch * touch) const;

	virtual Mat3 getNodeToWorldTransform() const;
	virtual const Mat3 & getParentToNodeTransform() const;
	virtual Mat3 getWorldToNodeTransform() const;
	//添加每级求逆
	virtual const Mat3& getModelViewTransformInverse() const;



	virtual void updateTransform();

	Vec2 convertToNodeSpace(const Vec2 & worldPoint) const;

	Vec2 convertToWorldSpace(const Vec2 & nodePoint) const;

	virtual Vector<Node*>& getChildren() { return _children; }
	virtual const Vector<Node*>& getChildren() const { return _children; }





	virtual Node * getChildByTag(int tag) const;

	virtual void addChild(Node * child);
	virtual void addChild(Node * child, int localZOrder);
	virtual void addChild(Node * child, int localZOrder, int tag);
	virtual void addChild(Node * child, int localZOrder, const std::string & name);

	void detachChild(Node * child, size_t childIndex, bool doCleanup);



	virtual void onEnter();
	virtual void onExit();

	virtual void onEnterTransitionDidFinish();

	virtual void onExitTransitionDidStart();
	void setOnEnterCallback(const std::function<void()>& callback) { _onEnterCallback = callback; }
	const std::function<void()>& getOnEnterCallback() const { return _onEnterCallback; }
	void setOnExitCallback(const std::function<void()>& callback) { _onExitCallback = callback; }
	const std::function<void()>& getOnExitCallback() const { return _onExitCallback; }
	void setOnEnterTransitionDidFinishCallback(const std::function<void()>& callback) { _onEnterTransitionDidFinishCallback = callback; }
	const std::function<void()>& getOnEnterTransitionDidFinishCallback() const { return _onEnterTransitionDidFinishCallback; }
	void setOnExitTransitionDidStartCallback(const std::function<void()>& callback) { _onExitTransitionDidStartCallback = callback; }
	const std::function<void()>& getOnExitTransitionDidStartCallback() const { return _onExitTransitionDidStartCallback; }


	virtual void cleanup();

	virtual void removeFromParent();
	virtual void removeFromParentAndCleanup(bool cleanup);
	virtual void removeChild(Node * child, bool cleanup = true);

	virtual void removeChildByTag(int tag, bool cleanup = true);

	virtual Node * getChildByName(const std::string & name) const;

	virtual void removeChildByName(const std::string & name, bool cleanup);

	virtual void removeAllChildren();

	virtual void removeAllChildrenWithCleanup(bool cleanup);


	virtual void draw();
	virtual void draw(Renderer *, const Mat3 &, uint32_t);
	virtual void visit();
	virtual  void visit(Renderer * ren, const Mat3 & parentTransform, uint32_t parentFlags);

	virtual const Mat3 & getNodeToParentTransform() const;

	//Schedule部分
	virtual  void setScheduler(Scheduler * scheduler);

	bool isScheduled(SEL_SCHEDULE selector) const;

	bool isScheduled(const std::string & key) const;

	void scheduleUpdate();

	void scheduleUpdateWithPriority(int priority);

	void unscheduleUpdate();

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


	virtual void setEventDispatcher(EventDispatcher* dispatcher);
	virtual EventDispatcher* getEventDispatcher() const { return _eventDispatcher; };
	virtual void setActionManager(ActionManager* actionManager);
	virtual ActionManager* getActionManager() { return _actionManager; }
	virtual const ActionManager* getActionManager() const { return _actionManager; }
	virtual Action * runAction(Action* action);
	void stopAllActions();

	void stopAction(Action * action);

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
			return (n1->_localZOrder == n2->_localZOrder && n1->_orderOfArrival < n2->_orderOfArrival) || n1->_localZOrder < n2->_localZOrder;
		});
#endif
	}
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

	virtual std::string getDescription() const;
protected:
	void childrenAlloc();
	void insertChild(Node * child, int localZOrder);
	void detachChild(Node * child, int childIndex);

	Vec2 convertToWindowSpace(const Vec2 & nodePoint) const;
	Mat3 transform(const Mat3 & parentTransform);
	virtual uint32_t processParentFlags(const Mat3 & parentTransform, uint32_t parentFlags);

	virtual void updateCascadeOpacity();
	virtual void disableCascadeOpacity();
	virtual void  updateCascadeColor();
	virtual void disableCascadeColor();
	virtual void updateColor() {}
	bool doEnumerate(std::string name, std::function<bool(Node *)> callback) const;
	bool doEnumerateRecursive(const Node* node, const std::string &name, std::function<bool(Node *)> callback) const;

	bool isVisitableByVisitingCamera() const;

private:

	Node(const Node &) = delete;
	Node &operator =(const Node &) = delete;

	void addChildHelper(Node * child, int localZOrder, int tag, const std::string & name, bool setTag);

	//物理部分
#if OG_USE_PHYSICS

public:
	PhysicsBody* _physicsBody;
	void setPhysicsBody(PhysicsBody* physicsBody);
	PhysicsBody* getPhysicsBody() const { return _physicsBody; }

	friend class PhysicsBody;
#endif
	Component* getComponent(const std::string& name);
	virtual bool addComponent(Component *component);
	virtual bool removeComponent(const std::string& name);
	virtual bool removeComponent(Component *component);
	virtual void removeAllComponents();
	virtual void update(float fDelta);
	ComponentContainer* _componentContainer;        ///< Dictionary of components
};

extern bool isScreenPointInRect(const Vec2 &pt,
	const Camera* camera,
	const Mat3& w2l,
	const Rect& rect,
	Vec2 *p);

class ScreenNode : public Node
{
public:
	static ScreenNode* create() {
		auto ret = new (std::nothrow) ScreenNode();
		if (ret && ret->init()) { ret->autorelease(); return ret; }
		OG_SAFE_DELETE(ret);
		return nullptr;
	}


	virtual uint32_t processParentFlags(const Mat3 & parentTransform, uint32_t parentFlags);
	virtual  const Mat3 & getNodeToParentTransform() const;
};

OG_END


#endif
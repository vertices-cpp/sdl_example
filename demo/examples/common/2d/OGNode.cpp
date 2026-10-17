#include "2d/OGNode.h"
#include "base/OGDirector.h"
#include "base/OGEventDispatcher.h"
#include "2d/OGCamera.h"
#include "2d/OGActionManager.h"
#include "2d/OGScene.h"

#include "2d/OGComponent.h"

#include "base/OGTouch.h"

#include "base/ogMacros.h"

OG_BEGIN

std::uint32_t Node::s_globalOrderOfArrival = 0;
 
Node::Node():
	 _running(false)
	, _visible(true)
	, _isTransitionFinished(false)
, _componentContainer(nullptr)
#if OG_USE_PHYSICS
, _physicsBody(nullptr)
#endif
{
	_director = Director::getInstance();
	_eventDispatcher = _director->getEventDispatcher();
	_eventDispatcher->retain();
	_scheduler = _director->getScheduler();
	_scheduler->retain();
	_actionManager = _director->getActionManager();
	_actionManager->retain();
}
Node::~Node()
{
	OG_SAFE_RELEASE_NULL(_actionManager);
	OG_SAFE_RELEASE_NULL(_scheduler);
	OG_SAFE_RELEASE_NULL(_eventDispatcher); 
	OG_SAFE_DELETE(_componentContainer);
}
void Node::setTransformUpdatedDirty() {
	_transformUpdated = true;
}

void Node::stopAction(Action* action)
{
	_actionManager->removeAction(action);
}
void Node::stopAllActions()
{
	_actionManager->removeAllActionsFromTarget(this);
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


void Node::setCascadeColorEnabled(bool cascadeColorEnabled)
{
	if (_cascadeColorEnabled == cascadeColorEnabled)
	{
		return;
	}

	_cascadeColorEnabled = cascadeColorEnabled;

	if (_cascadeColorEnabled)
	{
		updateCascadeColor();
	}
	else
	{
		disableCascadeColor();
	}
}

void Node::updateCascadeColor()
{
	Color3B parentColor = Color3B::WHITE;
	if (_parent && _parent->isCascadeColorEnabled())
	{
		parentColor = _parent->getDisplayedColor();
	}

	updateDisplayedColor(parentColor);
}
bool Node::isCascadeColorEnabled() const { return _cascadeColorEnabled; }
const Color3B&  Node::getDisplayedColor() const
{
	return _displayedColor;
}

void Node::disableCascadeColor()
{
	for (const auto& child : _children)
	{
		child->updateDisplayedColor(Color3B::WHITE);
	}
}

void Node::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
	_cascadeOpacityEnabled = cascadeOpacityEnabled;
}
void Node::updateDisplayedOpacity(uint8_t parentOpacity)
{
	_displayedOpacity = _realOpacity * parentOpacity / 255.0;
	updateColor();

	if (_cascadeOpacityEnabled)
	{
		for (const auto& child : _children)
		{
			child->updateDisplayedOpacity(_displayedOpacity);
		}
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
void Node::updateOrderOfArrival()
{
	_orderOfArrival = (++s_globalOrderOfArrival);
}
void Node::updateCascadeOpacity()
{
	uint8_t parentOpacity = 255;

	if (_parent != nullptr && _parent->isCascadeOpacityEnabled())
	{
		parentOpacity = _parent->getDisplayedOpacity();
	}

	updateDisplayedOpacity(parentOpacity);
}
bool Node::isCascadeOpacityEnabled() const
{
	return _cascadeOpacityEnabled;
}
uint32_t Node::processParentFlags(const Mat3& parentTransform, uint32_t parentFlags)
{
	uint32_t flags = parentFlags;
	flags |= (_transformUpdated ? FLAGS_TRANSFORM_DIRTY : 0);

	if (flags & FLAGS_DIRTY_MASK)
	{
		_modelViewTransform = this->transform(parentTransform);
		// true;
		//计算求逆
		_modelViewTransformInverse = _modelViewTransform.getInversed();
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

 
const Mat3 & Node::getModelViewTransformInverse() const
{
	return _modelViewTransformInverse;
}

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
			rotation = Mat3::createRotationSDL(_anchorPointInPoints, Vec2(_rotationX, _rotationY));
		}
		else if (_rotationZ != 0.0f)
		{
			rotation = Mat3::createRotation(_anchorPointInPoints, _rotationZ);
		}
		//先倾斜，缩放，旋转， 再平移

		_transform = translation * rotation * _transform;

#ifdef USE_ANCHOR_POINT
		if (_anchorPointInPoints.isZero())
		{//与2dx行为一致,SDL Y向下所以要+
			_transform.tx += _transform.a * -_anchorPointInPoints.x + _transform.c * -_anchorPointInPoints.y;
			_transform.ty += _transform.b * -_anchorPointInPoints.x + _transform.d * -_anchorPointInPoints.y;
		}
#endif 

	}

	_transformDirty = false;

	return _transform;
}
Mat3 Node::getWorldToNodeTransform() const
{
	//调用求逆矩阵
	return getNodeToWorldTransform().getInversed();
}
Mat3 Node::getNodeToWorldTransform() const
{
	return this->getNodeToParentTransform(nullptr);
}
Mat3 Node::getNodeTopParent(Node* ancestor) const
{
	if (_parent)//相机用
		return _parent->getNodeToParentTransform();
	return Mat3::IDENTITY;
}

Mat3 Node::getNodeToParentTransform(Node* ancestor) const
{
	Mat3 t(this->getNodeToParentTransform());

	for (Node *p = _parent; p != nullptr && p != ancestor; p = p->getParent())
	{
		t = p->getNodeToParentTransform() * t;
	}

	return t;
}

void Node::setScale(float scale)
{
	if (_scaleX == scale && _scaleY == scale)
		return;

	_scaleX = _scaleY = scale;
	_transformUpdated = _transformDirty = true;
}
void Node::setScaleX(float scaleX)
{
	if (_scaleX == scaleX)
		return;
	_scaleX = scaleX;
	_transformUpdated = _transformDirty = true;
}
void Node::setScaleY(float scaleY)
{
	if (_scaleY == scaleY)
		return;
	_scaleY = scaleY;
	_transformUpdated = _transformDirty = true;
}
void Node::setScale(float scaleX, float scaleY)
{
	if (_scaleX == scaleX && _scaleY == scaleY)
		return;

	_scaleX = scaleX;
	_scaleY = scaleY;
	_transformUpdated = _transformDirty = _inverseDirty = true;
}
float Node::getScaleX() const
{
	return _scaleX;
}
float Node::getScaleY() const
{
	return _scaleY;
}
float Node::getScale() const
{
	OGASSERT(_scaleX == _scaleY, "OGNode#scale. ScaleX != ScaleY. Don't know which one to return");
	return _scaleX;
}
/// rotation getter
float Node::getRotation() const
{
	return _rotationZ;
}

void Node::setRotation(float rotation)
{
	if (_rotationZ == rotation)
		return;
	_rotationZ = rotation;
	_transformUpdated = _transformDirty = true;
}
inline void Node::cleanup()
{

} 
/* "add" logic MUST only be on this method
* If a class want's to extend the 'addChild' behavior it only needs
* to override this method
*/
void Node::addChild(Node *child, int localZOrder, int tag)
{
	OGASSERT(child != nullptr, "Argument must be non-nil");
	OGASSERT(child->_parent == nullptr, "child already added. It can't be added again");

	addChildHelper(child, localZOrder, tag, "", true);
}

void Node::addChild(Node* child, int localZOrder, const std::string &name)
{
	OGASSERT(child != nullptr, "Argument must be non-nil");
	OGASSERT(child->_parent == nullptr, "child already added. It can't be added again");

	addChildHelper(child, localZOrder, -1, name, false);
}
void Node::setName(const std::string& name)
{
	_name = name;
	std::hash<std::string> h;
	_hashOfName = h(name);
}
const std::string& Node::getName() const
{
	return _name;
}
 
void Node::addChildHelper(Node* child, int localZOrder, int tag, const std::string &name, bool setTag)
{
	auto assertNotSelfChild
	([this, child]() -> bool
	{
		for (Node* parent(getParent()); parent != nullptr;
			parent = parent->getParent())
			if (parent == child)
				return false;

		return true;
	});
	(void)assertNotSelfChild;

	OGASSERT(assertNotSelfChild(),
		"A node cannot be the child of his own children");

	if (_children.empty())
	{
		this->childrenAlloc();
	}

	this->insertChild(child, localZOrder);

	if (setTag)
		child->setTag(tag);
	else
		child->setName(name);

	child->setParent(this);

	child->updateOrderOfArrival();

	if (_running)
	{
		child->onEnter();
		// prevent onEnterTransitionDidFinish to be called twice when a node is added in onEnter
		if (_isTransitionFinished)
		{
			child->onEnterTransitionDidFinish();
		}
	}

	if (_cascadeColorEnabled)
	{
		updateCascadeColor();
	}

	if (_cascadeOpacityEnabled)
	{
		updateCascadeOpacity();
	}
}
// lazy allocs
void Node::childrenAlloc()
{
	_children.reserve(4);
}
void Node::insertChild(Node* child, int z)
{
#if OG_ENABLE_GC_FOR_NATIVE_OBJECTS
 
#endif // OG_ENABLE_GC_FOR_NATIVE_OBJECTS
	_transformUpdated = true;
	_reorderChildDirty = true;
	_children.pushBack(child);
	//child->_setLocalZOrder(z);
}

void Node::addChild(Node *child, int zOrder)
{
	OGASSERT(child != nullptr, "Argument must be non-nil");
	this->addChild(child, zOrder, child->_name);
}


void Node::addChild(Node *child)
{
	OGASSERT(child != nullptr, "Argument must be non-nil");
	this->addChild(child, child->getLocalZOrder(), child->_name);
}
void Node::removeFromParent()
{
	this->removeFromParentAndCleanup(true);
}
void Node::removeFromParentAndCleanup(bool cleanup)
{
	if (_parent != nullptr)
	{
		_parent->removeChild(this, cleanup);
	}
}
void Node::removeAllChildren()
{
	this->removeAllChildrenWithCleanup(true);
}

void Node::disableCascadeOpacity()
{
	_displayedOpacity = _realOpacity;

	for (const auto& child : _children)
	{
		child->updateDisplayedOpacity(255);
	}
}

void Node::removeAllChildrenWithCleanup(bool cleanup)
{
	// not using detachChild improves speed here
	for (const auto& child : _children)
	{
		// IMPORTANT:
		//  -1st do onExit
		//  -2nd cleanup
		if (_running)
		{
			child->onExitTransitionDidStart();
			child->onExit();
		}

		if (cleanup)
		{
			child->cleanup();
		}
#if OG_ENABLE_GC_FOR_NATIVE_OBJECTS
		// 		auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
		// 		if (sEngine)
		// 		{
		// 			sEngine->releaseScriptObject(this, child);
		// 		}
#endif // OG_ENABLE_GC_FOR_NATIVE_OBJECTS
		// set parent nil at the end
		child->setParent(nullptr);
	}

	_children.clear();
}
void Node::removeChild(Node* child, bool cleanup /* = true */)
{
	// explicit nil handling
	if (_children.empty())
	{
		return;
	}

	size_t index = _children.getIndex(child);
	if (index != OG_INVALID_INDEX)
		this->detachChild(child, index, cleanup);
}
void Node::detachChild(Node *child, size_t childIndex, bool doCleanup)
{
	// IMPORTANT:
	//  -1st do onExit
	//  -2nd cleanup
	if (_running)
	{
		//	child->onExitTransitionDidStart();
		child->onExit();
	}

	// If you don't do cleanup, the child's actions will not get removed and the
	// its scheduledSelectors_ dict will not get released!
	if (doCleanup)
	{
		child->cleanup();
	}


	child->setParent(nullptr);

	_children.erase(childIndex);
}
void Node::setParent(Node * parent)
{
	_parent = parent;
	_transformUpdated = _transformDirty = true;
}
inline void Node::onEnter()
{
	if (_componentContainer && !_componentContainer->isEmpty())
	{
		_componentContainer->onEnter();
	}

	for (const auto &child : _children)
		child->onEnter();

	this->resume();

	_running = true;
}
inline void Node::onExit()
{
	if (_componentContainer && !_componentContainer->isEmpty())
	{
		_componentContainer->onExit();
	}

	this->pause();

	_running = false;

	for (const auto &child : _children)
		child->onExit();
}


void Node::onEnterTransitionDidFinish()
{
#if OG_ENABLE_SCRIPT_BINDING
	// 	if (_scriptType == kScriptTypeJavascript)
	// 	{
	// 		if (ScriptEngineManager::sendNodeEventToJS(this, kNodeOnEnterTransitionDidFinish))
	// 			return;
	// 	}
#endif

	if (_onEnterTransitionDidFinishCallback)
		_onEnterTransitionDidFinishCallback();

	_isTransitionFinished = true;
	for (const auto &child : _children)
		child->onEnterTransitionDidFinish();

#if OG_ENABLE_SCRIPT_BINDING
	// 	if (_scriptType == kScriptTypeLua)
	// 	{
	// 		ScriptEngineManager::sendNodeEventToLua(this, kNodeOnEnterTransitionDidFinish);
	// 	}
#endif
}

void Node::onExitTransitionDidStart()
{
#if OG_ENABLE_SCRIPT_BINDING
	// 	if (_scriptType == kScriptTypeJavascript)
	// 	{
	// 		if (ScriptEngineManager::sendNodeEventToJS(this, kNodeOnExitTransitionDidStart))
	// 			return;
	// 	}
#endif

	if (_onExitTransitionDidStartCallback)
		_onExitTransitionDidStartCallback();

	for (const auto &child : _children)
		child->onExitTransitionDidStart();

#if OG_ENABLE_SCRIPT_BINDING
	// 	if (_scriptType == kScriptTypeLua)
	// 	{
	// 		ScriptEngineManager::sendNodeEventToLua(this, kNodeOnExitTransitionDidStart);
	// 	}
#endif
}
Vec2 Node::convertTouchToNodeSpace(Touch *touch) const
{
	return this->convertToNodeSpace(touch->getLocation());
}

Vec2 Node::convertToNodeSpace(const Vec2& worldPoint) const
{
	Mat3 tmp = getWorldToNodeTransform();
	Vec2 vec2(worldPoint.x, worldPoint.y);
	Vec2 ret;
	tmp.transformPoint(vec2, &ret);
	return Vec2(ret.x, ret.y);
}
void Node::setLocalZOrder(int z)
{
	if (getLocalZOrder() == z)
		return;

	_setLocalZOrder(z);
	if (_parent)
	{
		_parent->reorderChild(this, z);
	}
	// 
	_eventDispatcher->setDirtyForNode(this);
}
void Node::reorderChild(Node *child, int zOrder)
{
	_reorderChildDirty = true;
	child->_setLocalZOrder(zOrder);

}
void Node::_setLocalZOrder(int z)
{
	_localZOrder = z;
	_reorderChildDirty = true;

}
// MARK: Callbacks

void Node::setScheduler(Scheduler* scheduler)
{
	if (scheduler != _scheduler)
	{
		this->unscheduleAllCallbacks();
		OG_SAFE_RETAIN(scheduler);
		OG_SAFE_RELEASE(_scheduler);
		_scheduler = scheduler;
	}
}

bool Node::isScheduled(SEL_SCHEDULE selector) const
{
	return _scheduler->isScheduled(selector, this);
}

bool Node::isScheduled(const std::string &key) const
{
	return _scheduler->isScheduled(key, this);
}

std::string Node::getDescription() const
{
	return StringUtils::format("<Node | Tag = %d", _tag);
}
void Node::scheduleUpdate()
{
	scheduleUpdateWithPriority(0);
}

void Node::scheduleUpdateWithPriority(int priority)
{
	_scheduler->scheduleUpdate(this, priority, !_running);
}


void Node::unscheduleUpdate()
{
	_scheduler->unscheduleUpdate(this);

}

void Node::schedule(SEL_SCHEDULE selector)
{
	this->schedule(selector, 0.0f, OG_REPEAT_FOREVER, 0.0f);
}

void Node::schedule(SEL_SCHEDULE selector, float interval)
{
	this->schedule(selector, interval, OG_REPEAT_FOREVER, 0.0f);
}

void Node::schedule(SEL_SCHEDULE selector, float interval, unsigned int repeat, float delay)
{
	OGASSERT(selector, "Argument must be non-nil");
	OGASSERT(interval >= 0, "Argument must be positive");

	_scheduler->schedule(selector, this, interval, repeat, delay, !_running);
}

void Node::schedule(const std::function<void(float)> &callback, const std::string &key)
{
	_scheduler->schedule(callback, this, 0, !_running, key);
}

void Node::schedule(const std::function<void(float)> &callback, float interval, const std::string &key)
{
	_scheduler->schedule(callback, this, interval, !_running, key);
}

void Node::schedule(const std::function<void(float)>& callback, float interval, unsigned int repeat, float delay, const std::string &key)
{
	_scheduler->schedule(callback, this, interval, repeat, delay, !_running, key);
}

void Node::scheduleOnce(SEL_SCHEDULE selector, float delay)
{
	this->schedule(selector, 0.0f, 0, delay);
}

void Node::scheduleOnce(const std::function<void(float)> &callback, float delay, const std::string &key)
{
	_scheduler->schedule(callback, this, 0, 0, delay, !_running, key);
}

void Node::unschedule(SEL_SCHEDULE selector)
{
	// explicit null handling
	if (selector == nullptr)
		return;

	_scheduler->unschedule(selector, this);
}

void Node::unschedule(const std::string &key)
{
	_scheduler->unschedule(key, this);
}

void Node::unscheduleAllCallbacks()
{
	_scheduler->unscheduleAllForTarget(this);
}

void Node::resume()
{
	_scheduler->resumeTarget(this);
	_actionManager->resumeTarget(this);
	_eventDispatcher->resumeEventListenersForTarget(this);
}

void Node::pause()
{
	_scheduler->pauseTarget(this);
	_actionManager->pauseTarget(this);
	_eventDispatcher->pauseEventListenersForTarget(this);
}
//--------Action相关--------------
void Node::setEventDispatcher(EventDispatcher* dispatcher)
{
	if (dispatcher != _eventDispatcher)
	{
		_eventDispatcher->removeEventListenersForTarget(this);
		OG_SAFE_RETAIN(dispatcher);
		OG_SAFE_RELEASE(_eventDispatcher);
		_eventDispatcher = dispatcher;
	}
}

void Node::setActionManager(ActionManager* actionManager)
{
	if (actionManager != _actionManager)
	{
		this->stopAllActions();
		OG_SAFE_RETAIN(actionManager);
		OG_SAFE_RELEASE(_actionManager);
		_actionManager = actionManager;
	}
}
Action * Node::runAction(Action * action)
{
	_actionManager->addAction(action, this, !_running);
	return action;
}

void Node::stopActionByTag(int tag)
{
	OGASSERT(tag != Action::INVALID_TAG, "Invalid tag");
	_actionManager->removeActionByTag(tag, this);
}

void Node::stopAllActionsByTag(int tag)
{
	OGASSERT(tag != Action::INVALID_TAG, "Invalid tag");
	_actionManager->removeAllActionsByTag(tag, this);
}

void Node::stopActionsByFlags(unsigned int flags)
{
	if (flags > 0)
	{
		_actionManager->removeActionsByFlags(flags, this);
	}
}

Action * Node::getActionByTag(int tag)
{
	OGASSERT(tag != Action::INVALID_TAG, "Invalid tag");
	return _actionManager->getActionByTag(tag, this);
}

ssize_t Node::getNumberOfRunningActions() const
{
	return _actionManager->getNumberOfRunningActionsInTarget(this);
}
ssize_t Node::getNumberOfRunningActionsByTag(int tag) const
{
	return _actionManager->getNumberOfRunningActionsInTargetByTag(this, tag);
}

Scene* Node::getScene() const
{
	if (!_parent)
		return nullptr;

	auto sceneNode = _parent;
	while (sceneNode->_parent)
	{
		sceneNode = sceneNode->_parent;
	}

	return dynamic_cast<Scene*>(sceneNode);
}
Vec2 Node::convertToWorldSpace(const Vec2& nodePoint) const
{
	Mat3 tmp = getNodeToWorldTransform();
	Vec2 vec2(nodePoint.x, nodePoint.y);
	Vec2 ret;
	tmp.transformPoint(vec2, &ret);

	return Vec2(ret.x, ret.y);

}

/// anchorPoint getter
const Vec2& Node::getAnchorPoint() const
{
	return _anchorPoint;
}
void Node::setAnchorPoint(const Vec2& point)
{
	if (!point.equals(_anchorPoint))
	{
		_anchorPoint = point;
		//不影响绘制位置的.
		_anchorPointInPoints.set(_contentSize.width * _anchorPoint.x, _contentSize.height * _anchorPoint.y);
		_transformUpdated = _transformDirty = _inverseDirty = true;
	}
}
const Size& Node::getContentSize() const
{
	return _contentSize;
}
const Vec2 &Node::getPosition() const {
	return _position;
}
const Vec2& Node::getCenter()const
{
	return _anchorPointInPoints;
}
void Node::setSkewX(float skewX)
{
	if (_skewX == skewX)
		return;

	_skewX = skewX;

	_transformUpdated = _transformDirty = true;
}

void Node::setSkewY(float skewY)
{
	if (_skewY == skewY)
		return;

	_skewY = skewY;

	_transformUpdated = _transformDirty = true;
}
float Node::getSkewX() const
{
	return _skewX;
}
float Node::getSkewY() const
{
	return _skewY;
}
float Node::getPositionX() const
{
	return _position.x;
}

void Node::setPositionX(float x)
{
	setPosition(x, _position.y);
}

float Node::getPositionY() const
{
	return  _position.y;
}

void Node::setPositionY(float y)
{
	setPosition(_position.x, y);
}
void Node::setCenter(const Vec2 & center)
{
	if (_anchorPointInPoints == center)
	{
		return;
	}
	_anchorPointInPoints = center;
	_transformUpdated = _transformDirty = true;
}
 
//组件
#if OG_USE_PHYSICS
// MARK: components
void Node::setPhysicsBody(PhysicsBody* physicsBody)
{
	if (_physicsBody != nullptr)
	{
		removeComponent(_physicsBody);
	}

	addComponent(physicsBody);
}
#endif
Component* Node::getComponent(const std::string& name)
{
	if (_componentContainer)
		return _componentContainer->get(name);

	return nullptr;
}


bool Node::addComponent(Component *component)
{
	// lazy alloc
	if (!_componentContainer)
		_componentContainer = new (std::nothrow) ComponentContainer(this);

	// should enable schedule update, then all components can receive this call back
	scheduleUpdate();//注册update调用

	return _componentContainer->add(component);
}

bool Node::removeComponent(const std::string& name)
{
	if (_componentContainer)
		return _componentContainer->remove(name);

	return false;
}

bool Node::removeComponent(Component *component)
{
	if (_componentContainer)
	{
		return _componentContainer->remove(component);
	}

	return false;
}

void Node::removeAllComponents()
{
	if (_componentContainer)
		_componentContainer->removeAll();
}

// override me
void Node::update(float fDelta)
{ 

	if (_componentContainer && !_componentContainer->isEmpty())
	{
		_componentContainer->visit(fDelta);
	}
}

uint8_t Node::getOpacity() const {
	return   _realOpacity;
}
uint8_t  Node::getDisplayedOpacity() const
{
	return _displayedOpacity;
}
void Node::setOpacity(uint8_t opacity) {
	_displayedOpacity = _realOpacity = opacity;
	updateCascadeColor();
}
int Node::getTag() const
{
	return _tag;
}

/// tag setter
void Node::setTag(int tag)
{
	_tag = tag;
}
Node* Node::getChildByName(const std::string& name) const
{
	OGASSERT(!name.empty(), "Invalid name");

	std::hash<std::string> h;
	size_t hash = h(name);

	for (const auto& child : _children)
	{
		// Different strings may have the same hash code, but can use it to compare first for speed
		if (child->_hashOfName == hash && child->_name.compare(name) == 0)
			return child;
	}
	return nullptr;
}
Node* Node::getChildByTag(int tag) const
{
	OGASSERT(tag != -1, "Invalid tag");

	for (const auto child : _children)
	{
		if (child && child->_tag == tag)
			return child;
	}
	return nullptr;
}
//------------------------------
size_t Node::getChildrenCount() const
{
	return _children.size();
}

//屏幕处理
bool isScreenPointInRect(const Vec2 &pt,
	const Camera* camera,
	const Mat3& w2l,
	const Rect& rect,
	Vec2 *p)
{
	if (nullptr == camera || rect.size.width <= 0 || rect.size.height <= 0)
		return false;

	printf("Vec2 isScreenPointInRect原point x = %f, y = %f\n", pt.x, pt.y);
	// 1. 屏幕 -> 世界
	Vec2 world = pt;// Director::getInstance()->convertToWorld(pt);

	// 2. 世界 -> 节点局部
	Vec2 l = { world.x,world.y };
	w2l.transformPoint(&l);   // 或 w2l.out(lx, ly);

	printf("w2l: a=%f b=%f c=%f d=%f tx=%f ty=%f\n",
		w2l.a, w2l.b, w2l.c, w2l.d, w2l.tx, w2l.ty);
	// 3. 判断是否在 rect 里
	Vec2 local(l.x, l.y);
	if (p) *p = local;
	return rect.containsPoint(local);
}
OG_END
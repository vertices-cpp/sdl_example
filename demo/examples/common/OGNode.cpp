#include "OGNode.h"
#include "OGDirector.h"
#include "OGEventDispatcher.h"
#include "OGCamera.h"
#include "OGActionManager.h"

OG_BEGIN
 
Node::Node()
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
inline void Node::cleanup()
{

} 
inline void Node::addChild(Node *child) {
	_children.pushBack(child);
	child->setParent(this);
}
inline void Node::addChild(Node * child, int zOrder)
{
	_children.pushBack(child);
	child->setParent(this);
}
inline void Node::addChild(Node * child, int zOrder, int tag)
{
	_children.pushBack(child);
	child->setParent(this);
}
inline void Node::addChild(Node * child, int zOrder, const std::string & name)
{
	_children.pushBack(child);
	child->setParent(this);
}
void Node::removeFromParentAndCleanup(bool cleanup)
{
	if (_parent != nullptr)
	{
		_parent->removeChild(this, cleanup);
	}
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
}
inline void Node::onExit()
{
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
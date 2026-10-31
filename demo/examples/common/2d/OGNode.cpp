#include "2d/OGNode.h"

#include <algorithm>
#include <string>
#include <regex>

#include "base/OGDirector.h"
#include "base/OGScheduler.h"
#include "base/OGEventDispatcher.h"
//#include "base/OGUTF8.h"
#include "2d/OGCamera.h"
#include "2d/OGActionManager.h"
#include "2d/OGScene.h"
// #include "2d/OGComponent.h"
// #include "renderer/OGMaterial.h"
// #include "math/TransformUtils.h"
#include "platform/OGPlatformMacros.h"

#include "base/OGTouch.h"

//#include "OGTypes.h"


OG_BEGIN

//Director* Node::_director = nullptr;

std::uint32_t Node::s_globalOrderOfArrival = 0;
int Node::__attachedNodeCount = 0;



Node * Node::create()
{
	Node * ret = new (std::nothrow) Node();
	if (ret && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		OG_SAFE_DELETE(ret);
	}
	return ret;
}

bool Node::init()
{
	return true;
}

Node::Node() :
	// 先按声明顺序来
	_tag(Node::INVALID_TAG)
	, _running(false)
	, _visible(true)
	, _name("")
	, _hashOfName(0)


	, _scaleX(1.0f)
	, _scaleY(1.0f)
	, _rotationX(0.0f)
	, _rotationY(0.0f)
	, _rotationZ(0.0f)
	, _usingNormalizedPosition(false)
	, _normalizedPositionDirty(false)
	, _skewX(0.0f)
	, _skewY(0.0f)
	, _anchorPoint(0, 0)
	, _anchorPointInPoints(Vec2::ZERO)
	, _ignoreAnchorPointForPosition(false)
	, _position(Vec2::ZERO)
	, _contentSize(Size::ZERO)
	, _contentSizeDirty(true)

	, _parent(nullptr)
	, _localZOrder(0)
	, _globalZOrder(0)
	, _reorderChildDirty(false)
	, _isTransitionFinished(false)

	, _transformDirty(true)       // _transform 默认构造就行，不用写
	, _transformUpdated(true)
	, _inverseDirty(true)

	, _cameraMask(1)              // ← 挪到这里
	, _displayedOpacity(255)
	, _realOpacity(255)
	, _displayedColor(Color3B::WHITE)
	, _realColor(Color3B::WHITE)

	, _cascadeColorEnabled(false)
	, _cascadeOpacityEnabled(false)

	, _onEnterCallback(nullptr)
	, _onExitCallback(nullptr)
	, _onEnterTransitionDidFinishCallback(nullptr)
	, _onExitTransitionDidStartCallback(nullptr)

	, _componentContainer(nullptr)
#if OG_USE_PHYSICS
	, _physicsBody(nullptr)

#endif

	, _orderOfArrival(0)
{
	_director = Director::getInstance();
	_actionManager = _director->getActionManager();
	_actionManager->retain();

	_scheduler = _director->getScheduler();
	_scheduler->retain();

	_eventDispatcher = _director->getEventDispatcher();
	_eventDispatcher->retain();

	_transform = _inverse = Mat3::IDENTITY;
}



Node::~Node()
{
	OGLOG("~Node() start name:%s, this: %p - tag: %i children num %d ",_name.c_str(), this, _tag, _children.size());
	if (_children.size())
		OGLOG("_children : {");
	for (auto& child : _children)
	{
		OGLOG(" [name: %s this: %p]", child->_name.c_str(), child);
		child->_parent = nullptr;
	}
	if (_children.size())
		OGLOG("}");
	removeAllComponents();
	OG_SAFE_DELETE(_componentContainer);


	stopAllActions();
	unscheduleAllCallbacks();

	OG_SAFE_RELEASE_NULL(_actionManager);
	OG_SAFE_RELEASE_NULL(_scheduler);

	_eventDispatcher->removeEventListenersForTarget(this);

	OGASSERT(!_running, "Node still marked as running on node destruction! Was base class onExit() called in derived class onExit() implementations?");
	OG_SAFE_RELEASE(_eventDispatcher);

	OGLOG("~Node() end name:%s, this: %p - tag: %i children num %d ", _name.c_str(), this, _tag, _children.size());
}




const std::string& Node::getName() const
{
	return _name;
}

void Node::setName(const std::string& name)
{
	_name = name;
	std::hash<std::string> h;
	_hashOfName = h(name);
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

// isRunning getter
bool Node::isRunning() const
{
	return _running;
}

void Node::setVisible(bool visible)
{
	if (visible != _visible)
	{
		_visible = visible;
		if (_visible)
			_transformUpdated = _transformDirty = _inverseDirty = true;
	}
}

bool Node::isVisible() const
{
	return _visible;
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

void Node::setTransformUpdatedDirty() {
	_transformUpdated = true;
}
void Node::_setLocalZOrder(int z)
{
	_localZOrder = z;
	_reorderChildDirty = true;

}
void Node::updateOrderOfArrival()
{
	_orderOfArrival = (++s_globalOrderOfArrival);
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
float Node::getGlobalZOrder() const { return _globalZOrder; }
void Node::setGlobalZOrder(float globalZOrder)
{
	if (_globalZOrder != globalZOrder)
	{
		_globalZOrder = globalZOrder;
		_eventDispatcher->setDirtyForNode(this);
	}
}

void Node::setScaleX(float scaleX)
{
	if (_scaleX == scaleX)
		return;
	_scaleX = scaleX;
	_transformUpdated = _transformDirty = _inverseDirty = true;
}



void Node::setScaleY(float scaleY)
{
	if (_scaleY == scaleY)
		return;
	_scaleY = scaleY;
	_transformUpdated = _transformDirty = _inverseDirty = true;
}
float Node::getScaleX()const
{
	//OGASSERT(_scaleX == _scaleY, "OGNode#scale. ScaleX != ScaleY. Don't know which one to return");
	return _scaleX;
}
float Node::getScaleY()const
{
	return _scaleY;
}


void Node::setScale(const Vec2 &scale)
{
	setScale(scale.x, scale.y);
}
void Node::setScale(float scaleX, float scaleY)
{
	if (_scaleX == scaleX && _scaleY == scaleY)
		return;

	_scaleX = scaleX;
	_scaleY = scaleY;
	_transformUpdated = _transformDirty = _inverseDirty = true;
}
void Node::setScale(float scale)
{
	if (_scaleX == scale && _scaleY == scale)
		return;

	_scaleX = _scaleY = scale;
	_transformUpdated = _transformDirty = _inverseDirty = true;
}
float Node::getScale() const
{
	OGASSERT(_scaleX == _scaleY, "OGNode#scale. ScaleX != ScaleY. Don't know which one to return");
	return _scaleX;
}


Vec2 Node::getScaleVec2()const
{
	return Vec2(_scaleX, _scaleY);
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
	_transformUpdated = _transformDirty = _inverseDirty = true;
}

void Node::setRotationX(float rotateX)
{
	if (_rotationX == rotateX)
		return;

	_rotationX = rotateX;

	_transformUpdated = _transformDirty = _inverseDirty = true;
}

void Node::setRotationY(float rotationY)
{
	if (_rotationY == rotationY)
		return;

	_rotationY = rotationY;

	_transformUpdated = _transformDirty = _inverseDirty = true;
}

float Node::getRotationX() const
{

	return _rotationX;
}

float Node::getRotationY() const
{
	return _rotationY;
}




float Node::getSkewX() const
{
	return _skewX;
}

float Node::getSkewY() const
{
	return _skewY;
}

void Node::setCenter(const Vec2 & center)
{
	if (_anchorPointInPoints == center)
	{
		return;
	}
	_anchorPointInPoints = center;
	_transformUpdated = _transformDirty = _inverseDirty = true;
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

	_transformUpdated = _transformDirty = _inverseDirty = true;
}

void Node::setSkewY(float skewY)
{
	if (_skewY == skewY)
		return;

	_skewY = skewY;

	_transformUpdated = _transformDirty = _inverseDirty = true;
}



const Vec2& Node::getPosition() const
{
	return _position;
}



void Node::setPosition(const Vec2& position)
{
	setPosition(position.x, position.y);
}

void Node::setPosition(float x, float y)
{
	if (_position.x == x && _position.y == y)
		return;

	_position.x = x;
	_position.y = y;

	//processParentFlags();
	_transformUpdated = _transformDirty = _inverseDirty = true;
	_usingNormalizedPosition = false;
}
/// position setter
void Node::setPositionNormalized(const Vec2& position)
{
	if (_normalizedPosition.equals(position))
		return;

	_normalizedPosition = position;
	_usingNormalizedPosition = true;
	_normalizedPositionDirty = true;
	_transformUpdated = _transformDirty = _inverseDirty = true;
}

void Node::setPositionX(float x)
{
	setPosition(x, _position.y);

}


float Node::getPositionX() const
{
	return _position.x;
}



void Node::setPositionY(float y)
{
	setPosition(_position.x, y);
}
float Node::getPositionY() const
{
	return  _position.y;
}
void Node::setContentSize(const Size& size)
{
	if (!size.equals(_contentSize))
	{
		_contentSize = size;
		// ★ 同步 _anchorPointInPoints
		_anchorPointInPoints.set(_contentSize.width * _anchorPoint.x,
			_contentSize.height * _anchorPoint.y);
		_transformUpdated = _transformDirty = _inverseDirty = _contentSizeDirty = true;
	}
}

const Vec2& Node::getAnchorPointInPoints() const
{
	return _anchorPointInPoints;
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

/// contentSize getter 
const Size& Node::getContentSize() const
{
	return _contentSize;
}
Rect Node::getBoundingBox() const
{
	Rect rect(0, 0, _contentSize.width, _contentSize.height);
	return RectApplyMat3(rect, getNodeToParentTransform());
}
//-----------颜色部分--------------


uint8_t Node::getOpacity() const {
	return   _realOpacity;
}
uint8_t  Node::getDisplayedOpacity() const
{
	return _displayedOpacity;
}
void Node::setOpacity(uint8_t opacity) {
	_displayedOpacity = _realOpacity = opacity;
	updateCascadeOpacity();
}
void  Node::updateDisplayedOpacity(uint8_t parentOpacity)
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
bool Node::isCascadeOpacityEnabled() const
{
	return _cascadeOpacityEnabled;
}

void Node::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
	if (_cascadeOpacityEnabled == cascadeOpacityEnabled)
	{
		return;
	}

	_cascadeOpacityEnabled = cascadeOpacityEnabled;

	if (cascadeOpacityEnabled)
	{
		updateCascadeOpacity();
	}
	else
	{
		disableCascadeOpacity();
	}
}



bool Node::isCascadeColorEnabled() const { return _cascadeColorEnabled; }
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
void Node::setOpacityModifyRGB(bool /*value*/)
{}
bool Node::isOpacityModifyRGB() const
{
	return false;
}



void Node::disableCascadeColor()
{
	for (const auto& child : _children)
	{
		child->updateDisplayedColor(Color3B::WHITE);
	}
}

bool Node::doEnumerate(std::string name, std::function<bool(Node *)> callback) const
{
	// name may be xxx/yyy, should find its parent
	size_t pos = name.find('/');
	std::string searchName = name;
	bool needRecursive = false;
	if (pos != name.npos)
	{
		searchName = name.substr(0, pos);
		name.erase(0, pos + 1);
		needRecursive = true;
	}

	bool ret = false;
	for (const auto& child : getChildren())
	{
		if (std::regex_match(child->_name, std::regex(searchName)))
		{
			if (!needRecursive)
			{
				// terminate enumeration if callback return true
				if (callback(child))
				{
					ret = true;
					break;
				}
			}
			else
			{
				ret = child->doEnumerate(name, callback);
				if (ret)
					break;
			}
		}
	}

	return ret;
}
bool Node::doEnumerateRecursive(const Node* node, const std::string &name, std::function<bool(Node *)> callback) const
{
	bool ret = false;

	if (node->doEnumerate(name, callback))
	{
		// search itself
		ret = true;
	}
	else
	{
		// search its children
		for (const auto& child : node->getChildren())
		{
			if (doEnumerateRecursive(child, name, callback))
			{
				ret = true;
				break;
			}
		}
	}

	return ret;
}

void Node::disableCascadeOpacity()
{
	_displayedOpacity = _realOpacity;

	for (const auto& child : _children)
	{
		child->updateDisplayedOpacity(255);
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

void Node::updateDisplayedColor(const Color3B& parentColor)
{
	_displayedColor.r = _realColor.r * parentColor.r / 255.0;
	_displayedColor.g = _realColor.g * parentColor.g / 255.0;
	_displayedColor.b = _realColor.b * parentColor.b / 255.0;

	updateColor();

	if (_cascadeColorEnabled)
	{
		for (const auto &child : _children)
		{
			child->updateDisplayedColor(_displayedColor);
		}
	}
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




const Color3B Node::getColor() const { return _realColor; }
const Color3B&  Node::getDisplayedColor() const
{
	return _displayedColor;
}
void Node::setColor(const Color3B& color) {
	_displayedColor = _realColor = color;

	updateCascadeColor();
}


//------------------------------
size_t Node::getChildrenCount() const
{
	return _children.size();
}












Mat3 Node::getNodeToWorldTransform() const
{
	return this->getNodeToParentTransform(nullptr);
}
const Mat3& Node::getParentToNodeTransform() const
{
	if (_inverseDirty)
	{
		_inverse = getNodeToParentTransform().getInversed();
		_inverseDirty = false;
	}

	return _inverse;
}
Mat3 Node::getWorldToNodeTransform() const
{
	//调用求逆矩阵
	return getNodeToWorldTransform().getInversed();
}

const Mat3 & Node::getModelViewTransformInverse() const
{
	return _modelViewTransformInverse;
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

Vec2 Node::convertToNodeSpaceAR(const Vec2& worldPoint) const
{
	Vec2 nodePoint(convertToNodeSpace(worldPoint));
	return nodePoint - _anchorPointInPoints;
}

Vec2 Node::convertToWorldSpaceAR(const Vec2& nodePoint) const
{
	return convertToWorldSpace(nodePoint + _anchorPointInPoints);
}

Vec2 Node::convertToWindowSpace(const Vec2& nodePoint) const
{
	Vec2 worldPoint(this->convertToWorldSpace(nodePoint));
	return _director->convertToUI(worldPoint);
}
Vec2 Node::convertTouchToNodeSpace(Touch *touch) const
{
	return this->convertToNodeSpace(touch->getLocation());
}


Vec2 Node::convertTouchToNodeSpaceAR(Touch *touch) const
{
	Vec2 point = touch->getLocation();
	return this->convertToNodeSpaceAR(point);
}

void Node::updateTransform()
{
	// Recursively iterate over children
	for (const auto &child : _children)
		child->updateTransform();
}
Vec2 Node::convertToNodeSpace(const Vec2& worldPoint) const
{
	Mat3 tmp = getWorldToNodeTransform();
	Vec2 vec2(worldPoint.x, worldPoint.y);
	Vec2 ret;
	tmp.transformPoint(vec2, &ret);
	return Vec2(ret.x, ret.y);
}
Vec2 Node::convertToWorldSpace(const Vec2& nodePoint) const
{
	Mat3 tmp = getNodeToWorldTransform();
	Vec2 vec2(nodePoint.x, nodePoint.y);
	Vec2 ret;
	tmp.transformPoint(vec2, &ret);

	return Vec2(ret.x, ret.y);

}
/// isRelativeAnchorPoint getter
bool Node::isIgnoreAnchorPointForPosition() const
{
	return _ignoreAnchorPointForPosition;
}

/// isRelativeAnchorPoint setter
void Node::setIgnoreAnchorPointForPosition(bool newValue)
{
	if (newValue != _ignoreAnchorPointForPosition)
	{
		_ignoreAnchorPointForPosition = newValue;
		_transformUpdated = _transformDirty = _inverseDirty = true;
	}
}
/// parent setter
void Node::setParent(Node * parent)
{
	_parent = parent;
	_transformUpdated = _transformDirty = _inverseDirty = true;
}



Node* Node::getChildByTag(int tag) const
{
	OGASSERT(tag != Node::INVALID_TAG, "Invalid tag");

	for (const auto child : _children)
	{
		if (child && child->_tag == tag)
			return child;
	}
	return nullptr;
}


void Node::addChild(Node *child)
{
	OGASSERT(child != nullptr, "Argument must be non-nil");
	this->addChild(child, child->getLocalZOrder(), child->_name);
}
void Node::addChild(Node *child, int zOrder)
{
	OGASSERT(child != nullptr, "Argument must be non-nil");
	this->addChild(child, zOrder, child->_name);
}
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

	addChildHelper(child, localZOrder, INVALID_TAG, name, false);
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

void Node::insertChild(Node* child, int localZOrder)
{
	_children.pushBack(child);
	_reorderChildDirty = true;
	child->updateOrderOfArrival();
	child->_setLocalZOrder(localZOrder);
}
void Node::removeChildByTag(int tag, bool cleanup/* = true */)
{
	OGASSERT(tag != Node::INVALID_TAG, "Invalid tag");

	Node *child = this->getChildByTag(tag);

	if (child == nullptr)
	{
		OGLOG("orange: removeChildByTag(tag = %d): child not found!", tag);
	}
	else
	{
		this->removeChild(child, cleanup);
	}
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
void Node::removeChildByName(const std::string &name, bool cleanup)
{
	OGASSERT(!name.empty(), "Invalid name");

	Node *child = this->getChildByName(name);

	if (child == nullptr)
	{
		OGLOG("orange: removeChildByName(name = %s): child not found!", name.c_str());
	}
	else
	{
		this->removeChild(child, cleanup);
	}
}






void Node::reorderChild(Node *child, int zOrder)
{
	_reorderChildDirty = true;
	child->_setLocalZOrder(zOrder);

}

// MARK: Camera
void Node::setCameraMask(unsigned short mask, bool applyChildren)
{
	_cameraMask = mask;
	if (applyChildren)
	{
		for (const auto& child : _children)
		{
			child->setCameraMask(mask, applyChildren);
		}
	}
}

void Node::sortAllChildren()
{
	if (_reorderChildDirty)
	{
		sortNodes(_children);
		_reorderChildDirty = false;
		_eventDispatcher->setDirtyForNode(this);
	}
}

void Node::onEnter()
{
	OGLOG("Node Name: %s,onEnter this:%.8x",_name.c_str(), this);
	if (!_running)
	{
		++__attachedNodeCount;
	}
#if OG_ENABLE_SCRIPT_BINDING
	// 	if (_scriptType == kScriptTypeJavascript)
	// 	{
	// 		if (ScriptEngineManager::sendNodeEventToJS(this, kNodeOnEnter))
	// 			return;
	// 	}
#endif

	if (_onEnterCallback)
		_onEnterCallback();

	if (_componentContainer && !_componentContainer->isEmpty())
	{
		_componentContainer->onEnter();
	}

	_isTransitionFinished = false;

	for (const auto &child : _children)
		child->onEnter();

	this->resume();

	_running = true;

#if OG_ENABLE_SCRIPT_BINDING
	// 	if (_scriptType == kScriptTypeLua)
	// 	{
	// 		ScriptEngineManager::sendNodeEventToLua(this, kNodeOnEnter);
	// 	}
#endif
}



void Node::onExit()
{
	OGLOG("Node Name: %s,onExit this:%.8x", _name.c_str(), this);
	if (_running)
	{
		--__attachedNodeCount;
	}
#if OG_ENABLE_SCRIPT_BINDING
	// 	if (_scriptType == kScriptTypeJavascript)
	// 	{
	// 		if (ScriptEngineManager::sendNodeEventToJS(this, kNodeOnExit))
	// 			return;
	// 	}
#endif

	if (_onExitCallback)
		_onExitCallback();

	if (_componentContainer && !_componentContainer->isEmpty())
	{
		_componentContainer->onExit();
	}

	this->pause();

	_running = false;

	for (const auto &child : _children)
		child->onExit();

#if OG_ENABLE_SCRIPT_BINDING
	// 	if (_scriptType == kScriptTypeLua)
	// 	{
	// 		ScriptEngineManager::sendNodeEventToLua(this, kNodeOnExit);
	// 	}
#endif
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
void Node::cleanup()
{
	// actions
	this->stopAllActions();
	// timers
	this->unscheduleAllCallbacks();


	for (const auto &child : _children)
		child->cleanup();
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

void Node::removeAllChildren()
{
	this->removeAllChildrenWithCleanup(true);
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

void Node::detachChild(Node *child, int childIndex)
{
	// IMPORTANT:
	//  -1st do onExit
	//  -2nd cleanup
	if (_running)
	{
		child->onExit();
	}
	child->cleanup();

	child->setParent(nullptr);
	_children.erase(childIndex);
}

std::string Node::getDescription() const
{
	return StringUtils::format("<Node | Tag = %d", _tag);
}

void Node::childrenAlloc()
{
	_children.reserve(4);
}


void Node::draw()
{
	auto renderer = _director->getRenderer();
	draw(renderer, _modelViewTransform, FLAGS_TRANSFORM_DIRTY);
}
void Node::draw(Renderer* /*renderer*/, const Mat3 & /*transform*/, uint32_t)
{
}
void Node::visit() {
	auto renderer = _director->getRenderer();
	visit(renderer, Mat3::IDENTITY, Node::FLAGS_TRANSFORM_DIRTY);
}
uint32_t Node::processParentFlags(const Mat3& parentTransform, uint32_t parentFlags)
{
	if (_usingNormalizedPosition)
	{
		OGASSERT(_parent, "setPositionNormalized() doesn't work with orphan nodes");
		if ((parentFlags & FLAGS_CONTENT_SIZE_DIRTY) || _normalizedPositionDirty)
		{
			auto& s = _parent->getContentSize();
			_position.x = _normalizedPosition.x * s.width;
			_position.y = _normalizedPosition.y * s.height;
			_transformUpdated = _transformDirty = _inverseDirty = true;
			_normalizedPositionDirty = false;
		}
	}
	if (!isVisitableByVisitingCamera())
		return parentFlags;

	uint32_t flags = parentFlags;
	flags |= (_transformUpdated ? FLAGS_TRANSFORM_DIRTY : 0);
	flags |= (_contentSizeDirty ? FLAGS_CONTENT_SIZE_DIRTY : 0);

	if (flags & FLAGS_DIRTY_MASK)
	{
		_modelViewTransform = this->transform(parentTransform);
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
const Mat3 & Node::getNodeToParentTransform() const
{
	if (_transformDirty)
	{
		//初始化
		_transform.reset();



		bool needsSkewMatrix = (_skewX || _skewY);

		// Translate values
		float tx = _position.x;
		float ty = _position.y;

#ifndef USE_ANCHOR_POINT
		if (_scaleX < 0) {
			tx += _contentSize.width * std::abs(_scaleX); // 必须乘以缩放绝对值
		}
		if (_scaleY < 0) {
			ty += _contentSize.height * std::abs(_scaleY);
		}


#else
		if (_ignoreAnchorPointForPosition)
		{
			tx += _anchorPointInPoints.x;
			ty += _anchorPointInPoints.y;
		}
#endif 

		Mat3 translation = Mat3::createTranslation(tx, ty);

		if (_scaleX != 1.f || _scaleY != 1.f)
		{
			_transform = _transform.createScale(_scaleX, _scaleY);
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

		if (needsSkewMatrix)
		{
			Mat3 skewMatrix = Mat3::createSkew(_skewX, _skewY);
			_transform = _transform * skewMatrix;
		}

#ifdef USE_ANCHOR_POINT
		if (!_anchorPointInPoints.isZero())
		{//与2dx行为一致,SDL Y向下所以要+
			_transform.tx += _transform.a * -_anchorPointInPoints.x + _transform.c * -_anchorPointInPoints.y;
			_transform.ty += _transform.b * -_anchorPointInPoints.x + _transform.d * -_anchorPointInPoints.y;
		}
#endif 
	}

	_transformDirty = false;

	return _transform;
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

void Node::stopAllActions()
{
	_actionManager->removeAllActionsFromTarget(this);
}
void Node::stopAction(Action* action)
{
	_actionManager->removeAction(action);
}
//--------Action相关--------------
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

//---------------------------------
bool isScreenPointInRect(const Vec2 &pt,
	const Camera* camera,
	const Mat3& w2l,
	const Rect& rect,
	Vec2 *p)
{
	if (nullptr == camera || rect.size.width <= 0 || rect.size.height <= 0)
		return false;

	// 1. 屏幕 -> 世界
	Vec2 world = pt;// camera->screenToWorld(pt);

	// 2. 世界 -> 节点局部
	Vec2 l = { world.x,world.y };
	w2l.transformPoint(&l);   // 或 w2l.out(lx, ly);

	// 3. 判断是否在 rect 里
	Vec2 local(l.x, l.y);
	if (p) *p = local;
	return rect.containsPoint(local);
}



//屏幕节点

uint32_t ScreenNode::processParentFlags(const Mat3& parentTransform, uint32_t parentFlags)
{

	uint32_t flags = parentFlags;
	flags |= (_transformUpdated ? FLAGS_TRANSFORM_DIRTY : 0);

	if (flags & FLAGS_DIRTY_MASK)
	{
		_modelViewTransform = this->transform(parentTransform);
		//计算求逆
		_modelViewTransformInverse = _modelViewTransform.getInversed();
	}

	_transformUpdated = false;

	return flags;
}

const Mat3 & ScreenNode::getNodeToParentTransform() const
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
// 		if (_scaleX != 1.f || _scaleY != 1.f)
// 		{
// 			_transform = _transform.createScale(_scaleX, _scaleY);
// 		}
// 		if (_skewX != 0.0f || _skewY != 0.0f)
// 		{
// 			_transform = _transform * Mat3::createSkew(_skewX, _skewY);
// 		}
// 
// 		Mat3 rotation;
// 		if (_rotationX != 0.0f || _rotationY != 0.0f)
// 		{
// 			rotation = Mat3::createRotationSDL(_anchorPointInPoints, Vec2(_rotationX, _rotationY));
// 		}
// 		//先缩放，倾斜，旋转， 再平移
// 
// 		_transform = translation * rotation * _transform;
	}

	_transformDirty = false;

	return _transform;
}


OG_END

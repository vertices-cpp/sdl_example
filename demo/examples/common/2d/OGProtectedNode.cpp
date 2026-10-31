 

#include "2d/OGProtectedNode.h"

#include "base/OGDirector.h"
#include "2d/OGScene.h"

OG_BEGIN

ProtectedNode::ProtectedNode() : _reorderProtectedChildDirty(false)
{
	_name = "ProtectedNode";
}

ProtectedNode::~ProtectedNode()
{
	OGLOG("~ProtectedNode() start name:%s, this: %p - tag: %i children num %d ", _name.c_str(), this, _tag, _protectedChildren.size());
	if (_children.size())
		OGLOG("protectedChildren : {");
	for (auto& child : _protectedChildren)
	{
		OGLOG(" [name: %s this: %p]",child->getName().c_str(), child);
	}
	if (_children.size())
		OGLOG("}");

	removeAllProtectedChildren();
	OGLOG("~ProtectedNode() end name:%s, this: %p - tag: %i children num %d ", _name.c_str(), this, _tag, _protectedChildren.size());

}

ProtectedNode * ProtectedNode::create()
{
	ProtectedNode * ret = new (std::nothrow) ProtectedNode();
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

void ProtectedNode::cleanup()
{
#if OG_ENABLE_SCRIPT_BINDING
//     if (_scriptType == kScriptTypeJavascript)
//     {
//         if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnCleanup))
//             return;
//     }
#endif // #if OG_ENABLE_SCRIPT_BINDING
    
    Node::cleanup();
    // timers
    for( const auto &child: _protectedChildren)
        child->cleanup();
}

void ProtectedNode::addProtectedChild(orange::Node *child)
{
    addProtectedChild(child, child->getLocalZOrder(), child->getTag());
}

void ProtectedNode::addProtectedChild(orange::Node *child, int localZOrder)
{
    addProtectedChild(child, localZOrder, child->getTag());
}

/* "add" logic MUST only be on this method
 * If a class want's to extend the 'addChild' behavior it only needs
 * to override this method
 */
void ProtectedNode::addProtectedChild(Node *child, int zOrder, int tag)
{
    OGASSERT( child != nullptr, "Argument must be non-nil");
    OGASSERT( child->getParent() == nullptr, "child already added. It can't be added again");
    
    if (_protectedChildren.empty())
    {
        _protectedChildren.reserve(4);
    }
    
    this->insertProtectedChild(child, zOrder);
    
    child->setTag(tag);
    child->setGlobalZOrder(_globalZOrder);
    child->setParent(this);
    child->updateOrderOfArrival();
    
    if( _running )
    {
        child->onEnter();
        // prevent onEnterTransitionDidFinish to be called twice when a node is added in onEnter
        if (_isTransitionFinished) {
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

Node* ProtectedNode::getProtectedChildByTag(int tag)
{
    OGASSERT( tag != -1, "Invalid tag");
    
    for (auto& child : _protectedChildren)
    {
        if(child && child->getTag() == tag)
            return child;
    }
    return nullptr;
}

/* "remove" logic MUST only be on this method
 * If a class want's to extend the 'removeChild' behavior it only needs
 * to override this method
 */
void ProtectedNode::removeProtectedChild(orange::Node *child, bool cleanup)
{
    // explicit nil handling
    if (_protectedChildren.empty())
    {
        return;
    }
    
    ssize_t index = _protectedChildren.getIndex(child);
    if( index != -1 )
    {
        
        // IMPORTANT:
        //  -1st do onExit
        //  -2nd cleanup
        if (_running)
        {
            child->onExitTransitionDidStart();
            child->onExit();
        }
        
        // If you don't do cleanup, the child's actions will not get removed and the
        // its scheduledSelectors_ dict will not get released!
        if (cleanup)
        {
            child->cleanup();
        }
        
        // set parent nil at the end
        child->setParent(nullptr);
        
#if OG_ENABLE_GC_FOR_NATIVE_OBJECTS
        auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
        if (sEngine)
        {
            sEngine->releaseScriptObject(this, child);
        }
#endif // OG_ENABLE_GC_FOR_NATIVE_OBJECTS
        _protectedChildren.erase(index);
    }
}

void ProtectedNode::removeAllProtectedChildren()
{
    removeAllProtectedChildrenWithCleanup(true);
}

void ProtectedNode::removeAllProtectedChildrenWithCleanup(bool cleanup)
{
    // not using detachChild improves speed here
    for (auto& child : _protectedChildren)
    {
        // IMPORTANT:
        //  -1st do onExit
        //  -2nd cleanup
        if(_running)
        {
            child->onExitTransitionDidStart();
            child->onExit();
        }
        
        if (cleanup)
        {
            child->cleanup();
        }
#if OG_ENABLE_GC_FOR_NATIVE_OBJECTS
        auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
        if (sEngine)
        {
            sEngine->releaseScriptObject(this, child);
        }
#endif // OG_ENABLE_GC_FOR_NATIVE_OBJECTS
        // set parent nil at the end
        child->setParent(nullptr);
    }
    
    _protectedChildren.clear();
}

void ProtectedNode::removeProtectedChildByTag(int tag, bool cleanup)
{
    OGASSERT( tag != -1, "Invalid tag");
    
    Node *child = this->getProtectedChildByTag(tag);
    
    if (child == nullptr)
    {
        OGLOG("orange: removeChildByTag(tag = %d): child not found!", tag);
    }
    else
    {
        this->removeProtectedChild(child, cleanup);
    }
}

// helper used by reorderChild & add
void ProtectedNode::insertProtectedChild(orange::Node *child, int z)
{
#if OG_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        sEngine->retainScriptObject(this, child);
    }
#endif // OG_ENABLE_GC_FOR_NATIVE_OBJECTS
    _reorderProtectedChildDirty = true;
    _protectedChildren.pushBack(child);
    child->setLocalZOrder(z);
}

void ProtectedNode::sortAllProtectedChildren()
{
    if( _reorderProtectedChildDirty ) {
        sortNodes(_protectedChildren);
        _reorderProtectedChildDirty = false;
    }
}

void ProtectedNode::reorderProtectedChild(orange::Node *child, int localZOrder)
{
    OGASSERT( child != nullptr, "Child must be non-nil");
    _reorderProtectedChildDirty = true;
    child->updateOrderOfArrival();
    child->setLocalZOrder(localZOrder);
}

void ProtectedNode::visit(Renderer* renderer, const Mat3 &parentTransform, uint32_t parentFlags)
{
    // quick return if not visible. children won't be drawn.
    if (!_visible)
    {
        return;
    }
    
    uint32_t flags = processParentFlags(parentTransform, parentFlags);
    
    // IMPORTANT:
    // To ease the migration to v3.0, we still support the Mat3 stack,
    // but it is deprecated and your code should not rely on it
    Director* director = Director::getInstance();
    OGASSERT(nullptr != director, "Director is null when setting matrix stack");
//     director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
//     director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
    
    int i = 0;      // used by _children
    int j = 0;      // used by _protectedChildren
    
    sortAllChildren();
    sortAllProtectedChildren();
    
    //
    // draw children and protectedChildren zOrder < 0
    //
    for(auto size = _children.size(); i < size; ++i)
    {
        auto node = _children.at(i);
        
        if ( node && node->getLocalZOrder() < 0 )
            node->visit(renderer, _modelViewTransform, flags);
        else
            break;
    }

    for(auto size = _protectedChildren.size(); j < size; ++j)
    {
        auto node = _protectedChildren.at(j);
        
        if ( node && node->getLocalZOrder() < 0 )
            node->visit(renderer, _modelViewTransform, flags);
        else
            break;
    }
    
    //
    // draw self
    //
    if (isVisitableByVisitingCamera())
        this->draw(renderer, _modelViewTransform, flags);
    
    //
    // draw children and protectedChildren zOrder >= 0
    //
    for(auto it=_protectedChildren.cbegin()+j, itCend = _protectedChildren.cend(); it != itCend; ++it)
        (*it)->visit(renderer, _modelViewTransform, flags);

    for(auto it=_children.cbegin()+i, itCend = _children.cend(); it != itCend; ++it)
        (*it)->visit(renderer, _modelViewTransform, flags);
    
    // FIX ME: Why need to set _orderOfArrival to 0??
    //  
    // setOrderOfArrival(0);
    
   // director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void ProtectedNode::onEnter()
{
#if OG_ENABLE_SCRIPT_BINDING
//     if (_scriptType == kScriptTypeJavascript)
//     {
//         if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnter))
//             return;
//     }
#endif
    
    Node::onEnter();
    for( const auto &child: _protectedChildren)
        child->onEnter();
}

void ProtectedNode::onEnterTransitionDidFinish()
{
#if OG_ENABLE_SCRIPT_BINDING
//     if (_scriptType == kScriptTypeJavascript)
//     {
//         if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnterTransitionDidFinish))
//             return;
//     }
#endif
    
    Node::onEnterTransitionDidFinish();
    for( const auto &child: _protectedChildren)
        child->onEnterTransitionDidFinish();
}

void ProtectedNode::onExitTransitionDidStart()
{
#if OG_ENABLE_SCRIPT_BINDING
//     if (_scriptType == kScriptTypeJavascript)
//     {
//         if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExitTransitionDidStart))
//             return;
//     }
#endif
    
    Node::onExitTransitionDidStart();
    for( const auto &child: _protectedChildren)
        child->onExitTransitionDidStart();
}

void ProtectedNode::onExit()
{
#if OG_ENABLE_SCRIPT_BINDING
//     if (_scriptType == kScriptTypeJavascript)
//     {
//         if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExit))
//             return;
//     }
#endif
    
    Node::onExit();
    for( const auto &child: _protectedChildren)
        child->onExit();
}

void ProtectedNode::updateDisplayedOpacity(uint8_t parentOpacity)
{
	_displayedOpacity = _realOpacity * parentOpacity/255.0;
    updateColor();
    
    if (_cascadeOpacityEnabled)
    {
        for(auto child : _children){
            child->updateDisplayedOpacity(_displayedOpacity);
        }
    }
    
    for(auto child : _protectedChildren){
        child->updateDisplayedOpacity(_displayedOpacity);
    }
}

void ProtectedNode::updateDisplayedColor(const Color3B& parentColor)
{
	_displayedColor.r = _realColor.r * parentColor.r/255.0;
	_displayedColor.g = _realColor.g * parentColor.g/255.0;
	_displayedColor.b = _realColor.b * parentColor.b/255.0;
    updateColor();
    
    if (_cascadeColorEnabled)
    {
        for(const auto &child : _children){
            child->updateDisplayedColor(_displayedColor);
        }
    }
    for(const auto &child : _protectedChildren){
        child->updateDisplayedColor(_displayedColor);
    }
}

void ProtectedNode::disableCascadeColor()
{
    for(auto child : _children){
        child->updateDisplayedColor(Color3B::WHITE);
    }
    for(auto child : _protectedChildren){
        child->updateDisplayedColor(Color3B::WHITE);
    }
}

void ProtectedNode::disableCascadeOpacity()
{
    _displayedOpacity = _realOpacity;
    
    for(auto child : _children){
        child->updateDisplayedOpacity(255);
    }
    
    for(auto child : _protectedChildren){
        child->updateDisplayedOpacity(255);
    }
}

void ProtectedNode::setCameraMask(unsigned short mask, bool applyChildren)
{
    Node::setCameraMask(mask, applyChildren);
    if (applyChildren)
    {
        for (auto& iter: _protectedChildren)
        {
            iter->setCameraMask(mask);
        }
    }
    
}

void ProtectedNode::setGlobalZOrder(float globalZOrder)
{
    Node::setGlobalZOrder(globalZOrder);
    for (auto &child : _protectedChildren)
        child->setGlobalZOrder(globalZOrder);
}

OG_END



#include "2d/OGComponent.h"

OG_BEGIN

Component::Component()
: _owner(nullptr)
, _enabled(true)
{
#if OG_ENABLE_SCRIPT_BINDING
  /*  ScriptEngineProtocol* engine = ScriptEngineManager::getInstance()->getScriptEngine();
    _scriptType = engine != nullptr ? engine->getScriptType() : kScriptTypeNone;*/
    
#endif
}

Component::~Component()
{
}

bool Component::init()
{
    return true;
}

#if OG_ENABLE_SCRIPT_BINDING

static bool sendComponentEventToJS(Component* node, int action)
{
   /* auto scriptEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    
    if (scriptEngine->isCalledFromScript())
    {
        scriptEngine->setCalledFromScript(false);
    }
    else
    {
        BasicScriptData data(node,(void*)&action);
        ScriptEvent scriptEvent(kComponentEvent,(void*)&data);
        if (scriptEngine->sendEvent(&scriptEvent))
            return true;
    }*/
    
    return false;
}

#endif

void Component::onEnter()
{
#if OG_ENABLE_SCRIPT_BINDING
//     if (_scriptType == kScriptTypeJavascript)
//     {
//         sendComponentEventToJS(this, kComponentOnEnter);
//     }
#endif
}

void Component::onExit()
{
#if OG_ENABLE_SCRIPT_BINDING
   /* if (_scriptType == kScriptTypeJavascript)
    {
        sendComponentEventToJS(this, kComponentOnExit);
    }*/
#endif
}

void Component::onAdd()
{
#if OG_ENABLE_SCRIPT_BINDING
    /*if (_scriptType == kScriptTypeJavascript)
    {
        sendComponentEventToJS(this, kComponentOnAdd);
    }*/
#endif
}

void Component::onRemove()
{
#if OG_ENABLE_SCRIPT_BINDING
  /*  if (_scriptType == kScriptTypeJavascript)
    {
        sendComponentEventToJS(this, kComponentOnRemove);
    }*/
#endif
}

void Component::update(float /*delta*/)
{
#if OG_ENABLE_SCRIPT_BINDING
    /*if (_scriptType == kScriptTypeJavascript)
    {
        sendComponentEventToJS(this, kComponentOnUpdate);
    }*/
#endif
}

bool Component::serialize(void* /*ar*/)
{
    return true;
}

Component* Component::create()
{
    Component * ret = new (std::nothrow) Component();

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

void Component::setOwner(Node *owner)
{
    _owner = owner;
}

void Component::setEnabled(bool enabled)
{
    _enabled = enabled;
}

OG_END

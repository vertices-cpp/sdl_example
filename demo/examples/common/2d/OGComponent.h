

#ifndef _OG_COMPONENT_H_
#define _OG_COMPONENT_H_

/// @cond DO_NOT_SHOW
#include <string>
#include "base/OGRef.h"
//#include "base/OGScriptSupport.h"

OG_BEGIN

class Node;

enum {
    kComponentOnEnter,
    kComponentOnExit,
    kComponentOnAdd,
    kComponentOnRemove,
    kComponentOnUpdate
};

class Component : public Ref
{
public:
    static Component* create();

    /**
     * @js NA
     * @lua NA
     */
    virtual ~Component();

    virtual bool init();

    bool isEnabled() const { return _enabled; }
    virtual void setEnabled(bool enabled);
    
    const std::string& getName() const { return _name; }
    virtual void setName(const std::string& name) { _name = name; }
    
    Node* getOwner() const { return _owner; }
    virtual void setOwner(Node *owner);

    virtual void update(float delta);
    virtual bool serialize(void* r);

    virtual void onEnter();
    virtual void onExit();
    virtual void onAdd();
    virtual void onRemove();
public:
    /**
    * @js ctor
    */
    Component();

protected:
    Node* _owner;
    std::string _name;
    bool _enabled;
    
#if OG_ENABLE_SCRIPT_BINDING
   // ogScriptType _scriptType;         ///< type of script binding, lua or javascript
#endif
};

OG_END

/// @endcond
#endif  // _OG_COMPONENT_H_

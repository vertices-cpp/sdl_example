

#ifndef _OG_COMPONENT_CONTAINER_H_
#define _OG_COMPONENT_CONTAINER_H_

/// @cond DO_NOT_SHOW

#include "base/OGMap.h"
#include <string>

OG_BEGIN

class Component;
class Node;

class ComponentContainer
{
protected:
    /**
     * @js ctor
     */
    ComponentContainer(Node* node);
    
public:
    /**
     * @js NA
     * @lua NA
     */
    ~ComponentContainer();
    
	/**
     * @js getComponent
     */
	Component* get(const std::string& name) const;

    bool add(Component *com);
    bool remove(const std::string& name);
    bool remove(Component *com);
    void removeAll();
    void visit(float delta);
    
    void onEnter();
    void onExit();
    
    bool isEmpty() const { return _componentMap.empty(); } 
private:
    std::unordered_map<std::string, Component*> _componentMap;
    Node *_owner;
    
    friend class Node;
};

OG_END

/// @endcond
#endif  // _OG_COMPONENT_CONTAINER_H_

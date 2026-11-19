

#include "2d/OGComponentContainer.h"
#include "2d/OGComponent.h"
#include "2d/OGNode.h"

OG_BEGIN

ComponentContainer::ComponentContainer(Node* node)
: _owner(node)
{
}

ComponentContainer::~ComponentContainer()
{  
}

Component* ComponentContainer::get(const std::string& name) const
{
    Component* ret = nullptr;

    auto it = _componentMap.find(name);
    if (it != _componentMap.end())
    {
        ret = it->second;
    }

    return ret;
}

bool ComponentContainer::add(Component *com)
{
    bool ret = false;
    OGASSERT(com != nullptr, "Component must be non-nil");
    OGASSERT(com->getOwner() == nullptr, "Component already added. It can't be added again");
    do
    {
        auto componentName = com->getName();

        if (_componentMap.find(componentName) != _componentMap.end())
        {
            OGASSERT(false, "ComponentContainer already have this kind of component");
            break;
        }
        _componentMap[componentName] = com;
        com->retain();
        com->setOwner(_owner);
        com->onAdd();

        ret = true;
    } while(0);
    return ret;
}

bool ComponentContainer::remove(const std::string& componentName)
{
    bool ret = false;
    do 
    {        
        auto iter = _componentMap.find(componentName);
        OG_BREAK_IF(iter == _componentMap.end());

        auto component = iter->second;
        _componentMap.erase(componentName);

        component->onRemove();
        component->setOwner(nullptr);
        component->release();

        ret = true;
    } while(0);

    return ret;
 }

bool ComponentContainer::remove(Component *com)
{
    return remove(com->getName());
}

void ComponentContainer::removeAll()
{
    if (!_componentMap.empty())
    {
        for (auto& iter : _componentMap)
        {
            iter.second->onRemove();
            iter.second->setOwner(nullptr);
            iter.second->release();
        }
        
        _componentMap.clear();
        _owner->unscheduleUpdate();
    }
}

void ComponentContainer::visit(float delta)
{
    if (!_componentMap.empty())
    {
        OG_SAFE_RETAIN(_owner);
        for (auto& iter : _componentMap)
        {
            iter.second->update(delta);
        }
        OG_SAFE_RELEASE(_owner);
    }
}

void ComponentContainer::onEnter()
{
    for (auto& iter : _componentMap)
    {
        iter.second->onEnter();
    }
}

void ComponentContainer::onExit()
{
    for (auto& iter : _componentMap)
    {
        iter.second->onExit();
    }
}

OG_END

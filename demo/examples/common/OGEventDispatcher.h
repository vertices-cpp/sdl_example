
#ifndef __OG_EVENT_DISPATCHER_H__
#define __OG_EVENT_DISPATCHER_H__

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

#include "OGPlatformMacros.h"
#include "OGEventListener.h"
#include "OGEvent.h"
//#include "OGStdC.h"

/**
 * @addtogroup base
 * @{
 */

OG_BEGIN

class Event;
//class Touch;
class EventTouch;
class Node;
class EventCustom;
class EventListenerCustom;

/** @class EventDispatcher
* @brief This class manages event listener subscriptions
and event dispatching.

The EventListener list is managed in such a way that
event listeners can be added and removed even
from within an EventListener, while events are being
dispatched.
@js NA
*/
class  EventDispatcher : public Ref
{
public: 
    void addEventListenerWithSceneGraphPriority(EventListener* listener, Node* node); 
    void addEventListenerWithFixedPriority(EventListener* listener, int fixedPriority); 
    EventListenerCustom* addCustomEventListener(const std::string &eventName, const std::function<void(EventCustom*)>& callback);
 
    void removeEventListener(EventListener* listener); 
    void removeEventListenersForType(EventListener::Type listenerType); 
    void removeEventListenersForTarget(Node* target, bool recursive = false); 
    void removeCustomEventListeners(const std::string& customEventName); 
    void removeAllEventListeners(); 
    void pauseEventListenersForTarget(Node* target, bool recursive = false); 
    void resumeEventListenersForTarget(Node* target, bool recursive = false); 
    void setPriority(EventListener* listener, int fixedPriority); 
    void setEnabled(bool isEnabled); 
    bool isEnabled() const; 
    void dispatchEvent(Event* event); 
    void dispatchCustomEvent(const std::string &eventName, void *optionalUserData = nullptr); 
    bool hasEventListener(const EventListener::ListenerID& listenerID) const; 
	EventDispatcher();
	~EventDispatcher();

#if OG_NODE_DEBUG_VERIFY_EVENT_LISTENERS && ORANGE_DEBUG > 0
    
    /**
     * To help track down event listener issues in debug builds.
     * Verifies that the node has no event listeners associated with it when destroyed.
     */
    void debugCheckNodeHasNoEventListenersOnDestruction(Node* node);
    
#endif

protected:
    friend class Node;
    
    /** Sets the dirty flag for a node. */
    void setDirtyForNode(Node* node);
    
    /**
     *  The vector to store event listeners with scene graph based priority and fixed priority.
     */
    class EventListenerVector
    {
    public:
        EventListenerVector();
        ~EventListenerVector();
        size_t size() const;
        bool empty() const;
        
        void push_back(EventListener* item);
        void clearSceneGraphListeners();
        void clearFixedListeners();
        void clear();
        
        std::vector<EventListener*>* getFixedPriorityListeners() const { return _fixedListeners; }
        std::vector<EventListener*>* getSceneGraphPriorityListeners() const { return _sceneGraphListeners; }
        ssize_t getGt0Index() const { return _gt0Index; }
        void setGt0Index(ssize_t index) { _gt0Index = index; }
    private:
        std::vector<EventListener*>* _fixedListeners;
        std::vector<EventListener*>* _sceneGraphListeners;
        ssize_t _gt0Index;
    }; 
    void addEventListener(EventListener* listener); 
    void forceAddEventListener(EventListener* listener);
    
    /** Gets event the listener list for the event listener type. */
    EventListenerVector* getListeners(const EventListener::ListenerID& listenerID) const;
    
    /** Update dirty flag */
    void updateDirtyFlagForSceneGraph();
    
    /** Removes all listeners with the same event listener ID */
    void removeEventListenersForListenerID(const EventListener::ListenerID& listenerID);
    
    /** Sort event listener */
    void sortEventListeners(const EventListener::ListenerID& listenerID);
    
    /** Sorts the listeners of specified type by scene graph priority */
    void sortEventListenersOfSceneGraphPriority(const EventListener::ListenerID& listenerID, Node* rootNode);
    
    /** Sorts the listeners of specified type by fixed priority */
    void sortEventListenersOfFixedPriority(const EventListener::ListenerID& listenerID);
    
    /** Updates all listeners
     *  1) Removes all listener items that have been marked as 'removed' when dispatching event.
     *  2) Adds all listener items that have been marked as 'added' when dispatching event.
     */
    void updateListeners(Event* event);

    /** Touch event needs to be processed different with other events since it needs support ALL_AT_ONCE and ONE_BY_NONE mode. */
    void dispatchTouchEvent(EventTouch* event);
    
    /** Associates node with event listener */
    void associateNodeAndEventListener(Node* node, EventListener* listener);
    
    /** Dissociates node with event listener */
    void dissociateNodeAndEventListener(Node* node, EventListener* listener);
    
    /** Dispatches event to listeners with a specified listener type */
    void dispatchEventToListeners(EventListenerVector* listeners, const std::function<bool(EventListener*)>& onEvent); 
    void dispatchTouchEventToListeners(EventListenerVector* listeners, const std::function<bool(EventListener*)>& onEvent);
    
    void releaseListener(EventListener* listener);
    
    /// Priority dirty flag
    enum class DirtyFlag
    {
        NONE = 0,
        FIXED_PRIORITY = 1 << 0,
        SCENE_GRAPH_PRIORITY = 1 << 1,
        ALL = FIXED_PRIORITY | SCENE_GRAPH_PRIORITY
    }; 
    void setDirty(const EventListener::ListenerID& listenerID, DirtyFlag flag);
     
    void visitTarget(Node* node, bool isRootNode);

    /** Remove all listeners in _toRemoveListeners list and cleanup */
    void cleanToRemovedListeners();

    /** Listeners map */
    std::unordered_map<EventListener::ListenerID, EventListenerVector*> _listenerMap;
    
    /** The map of dirty flag */
    std::unordered_map<EventListener::ListenerID, DirtyFlag> _priorityDirtyFlagMap;
    
    /** The map of node and event listeners */
    std::unordered_map<Node*, std::vector<EventListener*>*> _nodeListenersMap;
    
    /** The map of node and its event priority */
    std::unordered_map<Node*, int> _nodePriorityMap;
    
    /** key: Global Z Order, value: Sorted Nodes */
    std::unordered_map<float, std::vector<Node*>> _globalZOrderNodeMap;
    
    /** The listeners to be added after dispatching event */
    std::vector<EventListener*> _toAddedListeners;

    /** The listeners to be removed after dispatching event */
    std::vector<EventListener*> _toRemovedListeners;

    /** The nodes were associated with scene graph based priority listeners */
    std::set<Node*> _dirtyNodes;
    
    /** Whether the dispatcher is dispatching event */
    int _inDispatch;
    
    /** Whether to enable dispatching event */
    bool _isEnabled;
    
    int _nodePriorityIndex;
    
    std::set<std::string> _internalCustomListenerIDs;
};


OG_END

// end of base group
/// @}

#endif // __OG_EVENT_DISPATCHER_H__



#ifndef _OG_SOCKETIO_H_
#define _OG_SOCKETIO_H_

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

#include "base/OGRef.h"
#include "platform/OGPlatformMacros.h"
#include "base/OGMap.h"


/**
 * @addtogroup network
 * @{
 */

OG_BEGIN

namespace network {

	class Ref;

//forward declarations
class SIOClientImpl;
class SIOClient;

/**
 * Singleton and wrapper class to provide static creation method as well as registry of all sockets.
 *
 * @lua NA
 */
class SocketIO
{
public:
    /**
     * Get instance of SocketIO.
     *
     * @return SocketIO* the instance of SocketIO.
     */
    static SocketIO* getInstance();
    static void destroyInstance();

    /**
     * The delegate class to process socket.io events.
     * @lua NA
     */
    class SIODelegate
    {
    public:
        /** Destructor of SIODelegate. */
        virtual ~SIODelegate() {}
        /**
         * This is kept for backwards compatibility, connect is now fired as a socket.io event "connect"
         *
         * This function would be called when the related SIOClient object receive messages that mean it have connected to endpoint successfully.
         *
         * @param client the connected SIOClient object.
         */
        virtual void onConnect(SIOClient* client) { OGLOG("SIODelegate onConnect fired"); };
        /**
         * This is kept for backwards compatibility, message is now fired as a socket.io event "message"
         *
         * This function would be called when the related SIOClient object receive message or json message.
         *
         * @param client the connected SIOClient object.
         * @param data the message,it could be json message
         */
        virtual void onMessage(SIOClient* client, const std::string& data) { OGLOG("SIODelegate onMessage fired with data: %s", data.c_str()); };
        /**
         * Pure virtual callback function, this function should be overridden by the subclass.
         *
         * This function would be called when the related SIOClient object disconnect or receive disconnect signal.
         *
         * @param client the connected SIOClient object.
         */
        virtual void onClose(SIOClient* client) = 0;
        /**
         * Pure virtual callback function, this function should be overridden by the subclass.
         *
         * This function would be called when the related SIOClient object receive error signal or didn't connect the endpoint but do some network operation, eg.,send and emit,etc.
         *
         * @param client the connected SIOClient object.
         * @param data the error message
         */
        virtual void onError(SIOClient* client, const std::string& data) = 0;
        /**
         * Fire event to script when the related SIOClient object receive the fire event signal.
         *
         * @param client the connected SIOClient object.
         * @param eventName the event's name.
         * @param data the event's data information.
         */
        virtual void fireEventToScript(SIOClient* client, const std::string& eventName, const std::string& data) { OGLOG("SIODelegate event '%s' fired with data: %s", eventName.c_str(), data.c_str()); };
    };

    /**
     *  Static client creation method, similar to socketio.connect(uri) in JS.
     *  @param  uri      the URI of the socket.io server.
     *  @param  delegate the delegate which want to receive events from the socket.io client.
     *  @return SIOClient* an initialized SIOClient if connected successfully, otherwise nullptr.
     */
    static SIOClient* connect(const std::string& uri, SocketIO::SIODelegate& delegate);

    /**
     *  Static client creation method, similar to socketio.connect(uri) in JS.
     *  @param  uri      the URI of the socket.io server.
     *  @param  delegate the delegate which want to receive events from the socket.io client.
     *  @param caFilePath The ca file path for wss connection
     *  @return SIOClient* an initialized SIOClient if connected successfully, otherwise nullptr.
     */
    static SIOClient* connect(const std::string& uri, SocketIO::SIODelegate& delegate, const std::string& caFilePath);

private:

    SocketIO();
    virtual ~SocketIO();

    static SocketIO *_inst;

    std::unordered_map<std::string, std::weak_ptr<SIOClientImpl
    >> _sockets;

    std::shared_ptr<SIOClientImpl> getSocket(const std::string& uri);
    void addSocket(const std::string& uri, std::shared_ptr<SIOClientImpl>& socket);
    void removeSocket(const std::string& uri);

    friend class SIOClientImpl;
private:
    OG_DISALLOW_COPY_AND_ASSIGN(SocketIO)
};

//c++11 style callbacks entities will be created using OG_CALLBACK (which uses std::bind)
typedef std::function<void(SIOClient*, const std::string&)> SIOEvent;
//c++11 map to callbacks
typedef std::unordered_map<std::string, SIOEvent> EventRegistry;

/**
 * A single connection to a socket.io endpoint.
 *
 * @lua NA
 */
class SIOClient
    : public orange::Ref
{
private:
    friend class SocketIO; // Only SocketIO class could contruct a SIOClient instance.

    std::string _path, _tag;
    bool _connected;
    std::shared_ptr<SIOClientImpl> _socket;
    
    SocketIO::SIODelegate* _delegate = nullptr;

    EventRegistry _eventRegistry;

    void fireEvent(const std::string& eventName, const std::string& data);

    void onOpen();
    void onConnect();
    void socketClosed();

    friend class SIOClientImpl;

    bool isConnected() const;
    void setConnected(bool);

    /**
     * Constructor of SIOClient class.
     *
     * @param host the string that represent the host address.
     * @param port the int value represent the port number.
     * @param path the string that represent endpoint.
     * @param impl the SIOClientImpl object.
     * @param delegate the SIODelegate object.
     */
    SIOClient(const std::string& path, std::shared_ptr<SIOClientImpl>& impl, SocketIO::SIODelegate& delegate);
    /**
     * Destructor of SIOClient class.
     */
    virtual ~SIOClient();
public:
    /**
     * Get the delegate for the client
     * @return the delegate object for the client
     */
    SocketIO::SIODelegate* getDelegate() { return _delegate; };

    /**
     * Disconnect from the endpoint, onClose will be called for the delegate when complete
     */
    void disconnect();
    /**
     * Send a message to the socket.io server.
     *
     * @param s message.
     */
    void send(const std::string& s);
    void send(const std::vector<std::string>& s);



    /**
     *  Emit the eventname and the args to the endpoint that _path point to.
     * @param eventname
     * @param args
     */
    void emit(const std::string& eventname, const std::string& args);
    void emit(const std::string& eventname, const std::vector<std::string> &args);

    /**
     * Used to register a socket.io event callback.
     * Event argument should be passed using OG_CALLBACK2(&Base::function, this).
     * @param eventName the name of event.
     * @param e the callback function.
     */
    void on(const std::string& eventName, SIOEvent e);

    /**
     * Set tag of SIOClient.
     * The tag is used to distinguish the various SIOClient objects.
     * @param tag string object.
     */
    void setTag(const char* tag);

    /**
     * Get tag of SIOClient.
     * @return const char* the pointer point to the _tag.
     */
    const char* getTag()
    {
        return _tag.c_str();
    }

};

}

OG_END

// end group
/// @}

#endif ///* defined(_OG_SOCKETIO_H_) */


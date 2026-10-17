

#ifndef __OGHTTPCLIENT_H__
#define __OGHTTPCLIENT_H__

#include <atomic>
#include <thread>
#include <condition_variable>
#include "base/OGVector.h"
#include "base/OGScheduler.h"
#include "network/HttpRequest.h"
#include "network/HttpResponse.h"
#include "network/HttpCookie.h"

/**
 * @addtogroup network
 * @{
 */

OG_BEGIN

namespace network {



/** Singleton that handles asynchronous http requests.
 *
 * Once the request completed, a callback will issued in main thread when it provided during make request.
 *
 * @lua NA
 */
class HttpClient
{
	//加一个
	std::atomic<bool> _quitting{ false };
	std::atomic<bool> _threadExited{ false };   // ★ 加
public:
    /**
    * The buffer size of _responseMessage
    */
    static const int RESPONSE_BUFFER_SIZE = 256;

    /**
     * Get instance of HttpClient.
     *
     * @return the instance of HttpClient.
     */
    static HttpClient *getInstance();

    /**
     * Release the instance of HttpClient.
     */
    static void destroyInstance();

    /**
     * Enable cookie support.
     *
     * @param cookieFile the filepath of cookie file.
     */
    void enableCookies(const char* cookieFile);

    /**
     * Get the cookie filename
     *
     * @return the cookie filename
     */
    const std::string& getCookieFilename();

    /**
     * Set root certificate path for SSL verification.
     *
     * @param caFile a full path of root certificate.if it is empty, SSL verification is disabled.
     */
    void setSSLVerification(const std::string& caFile);

    /**
     * Get the ssl CA filename
     *
     * @return the ssl CA filename
     */
    const std::string& getSSLVerification();

    /**
     * Add a get request to task queue
     *
     * @param request a HttpRequest object, which includes url, response callback etc.
                      please make sure request->_requestData is clear before calling "send" here.
     */
    void send(HttpRequest* request);

    /**
     * Immediate send a request
     *
     * @param request a HttpRequest object, which includes url, response callback etc.
                      please make sure request->_requestData is clear before calling "sendImmediate" here.
     */
    void sendImmediate(HttpRequest* request);

    /**
     * Set the timeout value for connecting.
     *
     * @param value the timeout value for connecting.
     */
    void setTimeoutForConnect(int value);

    /**
     * Get the timeout value for connecting.
     *
     * @return int the timeout value for connecting.
     */
    int getTimeoutForConnect();

    /**
     * Set the timeout value for reading.
     *
     * @param value the timeout value for reading.
     */
    void setTimeoutForRead(int value);

    /**
     * Get the timeout value for reading.
     *
     * @return int the timeout value for reading.
     */
    int getTimeoutForRead();

    HttpCookie* getCookie() const {return _cookie; }

    std::mutex& getCookieFileMutex() {return _cookieFileMutex;}

    std::mutex& getSSLCaFileMutex() {return _sslCaFileMutex;}
    
    typedef std::function<bool(HttpRequest*)> ClearRequestPredicate;
    typedef std::function<bool(HttpResponse*)> ClearResponsePredicate;

    /**
     * Clears the pending http responses and http requests
     * If defined, the method uses the ClearRequestPredicate and ClearResponsePredicate
     * to check for each request/response which to delete
     */
    void clearResponseAndRequestQueue(); 

    /**
    * Sets a predicate function that is going to be called to determine if we proceed
    * each of the pending requests
    *
    * @param predicate function that will be called 
    */
    void setClearRequestPredicate(ClearRequestPredicate predicate) { _clearRequestPredicate = predicate; }

    /**
     Sets a predicate function that is going to be called to determine if we proceed
    * each of the pending requests
    *
    * @param cb predicate function that will be called 
    */
    void setClearResponsePredicate(ClearResponsePredicate predicate) { _clearResponsePredicate = predicate; }
	// ★★★ 新增：给 curl progress callback 用
	bool isQuitting() const { return _quitting.load(); }
        
private:
    HttpClient();
    virtual ~HttpClient();
    bool init();

    /**
     * Init pthread mutex, semaphore, and create new thread for http requests
     * @return bool
     */
    bool lazyInitThreadSemaphore();
    void networkThread();
    void networkThreadAlone(HttpRequest* request, HttpResponse* response);
    /** Poll function called from main thread to dispatch callbacks when http requests finished **/
    void dispatchResponseCallbacks();

    void processResponse(HttpResponse* response, char* responseMessage);
    void increaseThreadCount();
    void decreaseThreadCountAndMayDeleteThis();

private:
    bool _isInited;

    int _timeoutForConnect;
    std::mutex _timeoutForConnectMutex;

    int _timeoutForRead;
    std::mutex _timeoutForReadMutex;

    int  _threadCount;
    std::mutex _threadCountMutex;

    Scheduler* _scheduler;
    std::mutex _schedulerMutex;

    Vector<HttpRequest*>  _requestQueue;
    std::mutex _requestQueueMutex;

    Vector<HttpResponse*> _responseQueue;
    std::mutex _responseQueueMutex;

    std::string _cookieFilename;
    std::mutex _cookieFileMutex;

    std::string _sslCaFilename;
    std::mutex _sslCaFileMutex;

    HttpCookie* _cookie;

    std::condition_variable_any _sleepCondition;

    char _responseMessage[RESPONSE_BUFFER_SIZE];

    HttpRequest* _requestSentinel;
    
    ClearRequestPredicate _clearRequestPredicate;
    ClearResponsePredicate _clearResponsePredicate;
};

} // namespace network

OG_END

// end group
/// @}

#endif //__OGHTTPCLIENT_H__


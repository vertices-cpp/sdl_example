


#include <queue>
#include <errno.h>
#include <curl/curl.h>

#include "network/HttpClient.h"
#include "base/OGDirector.h"
#include "platform/OGFileUtils.h"

OG_BEGIN



namespace network {



#if (OG_TARGET_PLATFORM == OG_PLATFORM_WIN32)
typedef int int32_t;
#endif

// Callback function used by libcurl for collect header data
static size_t writeHeaderData(void *ptr, size_t size, size_t nmemb, void *stream)
{
	std::vector<char> *recvBuffer = (std::vector<char>*)stream;
	size_t sizes = size * nmemb;
	recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr + sizes);
	return sizes;
}

// ★★★ 新增：curl 进度回调，用于退出时中断请求
static int progressCallback(void* clientp,
	curl_off_t /*dltotal*/,
	curl_off_t /*dlnow*/,
	curl_off_t /*ultotal*/,
	curl_off_t /*ulnow*/)
{
	HttpClient* client = static_cast<HttpClient*>(clientp);
	if (client && client->isQuitting())
	{
		return 1;   // 非 0 → curl 立即中断，返回 CURLE_ABORTED_BY_CALLBACK
	}
	return 0;
}


static HttpClient* _httpClient = nullptr; // pointer to singleton

typedef size_t (*write_callback)(void *ptr, size_t size, size_t nmemb, void *stream);

// Callback function used by libcurl for collect response data
static size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::vector<char> *recvBuffer = (std::vector<char>*)stream;
    size_t sizes = size * nmemb;
    
    // add data to the end of recvBuffer
    // write data maybe called more than once in a single request
    recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
    
    return sizes;
}

// Callback function used by libcurl for collect header data
// static size_t writeHeaderData(void *ptr, size_t size, size_t nmemb, void *stream)
// {
//     std::vector<char> *recvBuffer = (std::vector<char>*)stream;
//     size_t sizes = size * nmemb;
//     
//     // add data to the end of recvBuffer
//     // write data maybe called more than once in a single request
//     recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
//     
//     return sizes;
// }


static int processGetTask(HttpClient* client, HttpRequest* request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream, char* errorBuffer);
static int processPostTask(HttpClient* client, HttpRequest* request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream, char* errorBuffer);
static int processPutTask(HttpClient* client,  HttpRequest* request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream, char* errorBuffer);
static int processDeleteTask(HttpClient* client,  HttpRequest* request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream, char* errorBuffer);
// int processDownloadTask(HttpRequest *task, write_callback callback, void *stream, int32_t *errorCode);

// Worker thread  原代码
// void HttpClient::networkThread()
// {
//     increaseThreadCount();
// 
//     while (true)
//     {
//         HttpRequest *request;
// 
//         // step 1: send http request if the requestQueue isn't empty
//         {
//             std::lock_guard<std::mutex> lock(_requestQueueMutex);
//             while (_requestQueue.empty())
//             {
//                 _sleepCondition.wait(_requestQueueMutex);
//             }
//             request = _requestQueue.at(0);
//             _requestQueue.erase(0);
//         }
// 
//         if (request == _requestSentinel) {
//             break;
//         }
// 
//         // step 2: libcurl sync access
//         
//         // Create a HttpResponse object, the default setting is http access failed
//         HttpResponse *response = new (std::nothrow) HttpResponse(request);
// 
//         processResponse(response, _responseMessage);
// 
// 
//         // add response packet into queue
//         _responseQueueMutex.lock();
//         _responseQueue.pushBack(response);
//         _responseQueueMutex.unlock();
// 
//         _schedulerMutex.lock();
//         if (nullptr != _scheduler)
//         {
//             _scheduler->performFunctionInOrangeThread(OG_CALLBACK_0(HttpClient::dispatchResponseCallbacks, this));
//         }
//         _schedulerMutex.unlock();
//     }
//     
//     // cleanup: if worker thread received quit signal, clean up un-completed request queue
//     _requestQueueMutex.lock();
//     _requestQueue.clear();
//     _requestQueueMutex.unlock();
// 
//     _responseQueueMutex.lock();
//     _responseQueue.clear();
//     _responseQueueMutex.unlock();
// 
//     decreaseThreadCountAndMayDeleteThis();
// }
void HttpClient::networkThread()
{
	increaseThreadCount();

	while (true)
	{
		HttpRequest* request = nullptr;

		// ★ 用 wait_for + 谓词，退出时能被唤醒
		{
			std::unique_lock<std::mutex> lock(_requestQueueMutex);
			_sleepCondition.wait_for(lock, std::chrono::milliseconds(100),
				[this] {
				return !_requestQueue.empty() || _quitting.load();
			});

			if (_quitting.load())
				break;

			if (!_requestQueue.empty())
			{
				request = _requestQueue.at(0);
				_requestQueue.erase(0);
			}
		}

		if (!request || request == _requestSentinel)
			break;

		HttpResponse* response = new (std::nothrow) HttpResponse(request);
		processResponse(response, _responseMessage);

		_responseQueueMutex.lock();
		_responseQueue.pushBack(response);
		_responseQueueMutex.unlock();

		_schedulerMutex.lock();
		if (nullptr != _scheduler)
			_scheduler->performFunctionInOrangeThread(
				OG_CALLBACK_0(HttpClient::dispatchResponseCallbacks, this));
		_schedulerMutex.unlock();
	}

	// cleanup
	_requestQueueMutex.lock();
	_requestQueue.clear();
	_requestQueueMutex.unlock();

	_responseQueueMutex.lock();
	_responseQueue.clear();
	_responseQueueMutex.unlock();

	_threadExited = true;
	decreaseThreadCountAndMayDeleteThis();
}
// Worker thread
void HttpClient::networkThreadAlone(HttpRequest* request, HttpResponse* response)
{
    increaseThreadCount();

    char responseMessage[RESPONSE_BUFFER_SIZE] = { 0 };
    processResponse(response, responseMessage);

    _schedulerMutex.lock();
    if (nullptr != _scheduler)
    {
        _scheduler->performFunctionInOrangeThread([this, response, request]{
            const ogHttpRequestCallback& callback = request->getCallback();
            Ref* pTarget = request->getTarget();
            SEL_HttpResponse pSelector = request->getSelector();

            if (callback != nullptr)
            {
                callback(this, response);
            }
            else if (pTarget && pSelector)
            {
                (pTarget->*pSelector)(this, response);
            }
            response->release();
            // do not release in other thread
            request->release();
        });
    }
    _schedulerMutex.unlock();

    decreaseThreadCountAndMayDeleteThis();
}

//Configure curl's timeout property
static bool configureCURL(HttpClient* client, CURL* handle, char* errorBuffer)
{
    if (!handle) {
        return false;
    }
    
    int32_t code;
    code = curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errorBuffer);
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_TIMEOUT, HttpClient::getInstance()->getTimeoutForRead());
    if (code != CURLE_OK) {
        return false;
    }
    code = curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, HttpClient::getInstance()->getTimeoutForConnect());
    if (code != CURLE_OK) {
        return false;
    }

    std::string sslCaFilename = client->getSSLVerification();
    if (sslCaFilename.empty()) {
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
    } else {
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(handle, CURLOPT_CAINFO, sslCaFilename.c_str());
    }
    
    // FIXED #3224: The subthread of OGHttpClient interrupts main thread if timeout comes.
    // Document is here: http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTNOSIGNAL 
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);

    curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "");

	// ★★★ 新增：挂上进度回调，让 curl 在退出时能被打断
	curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, progressCallback);
	curl_easy_setopt(handle, CURLOPT_XFERINFODATA, client);

    return true;
}

class CURLRaii
{
    /// Instance of CURL
    CURL *_curl;
    /// Keeps custom header data
    curl_slist *_headers;
public:
    CURLRaii()
        : _curl(curl_easy_init())
        , _headers(nullptr)
    {
    }

    ~CURLRaii()
    {
        if (_curl)
            curl_easy_cleanup(_curl);
        /* free the linked list for header data */
        if (_headers)
            curl_slist_free_all(_headers);
    }

    template <class T>
    bool setOption(CURLoption option, T data)
    {
        return CURLE_OK == curl_easy_setopt(_curl, option, data);
    }

    /**
     * @brief Inits CURL instance for common usage
     * @param request Null not allowed
     * @param callback Response write callback
     * @param stream Response write stream
     */
    bool init(HttpClient* client, HttpRequest* request, write_callback callback, void* stream, write_callback headerCallback, void* headerStream, char* errorBuffer)
    {
        if (!_curl)
            return false;
        if (!configureCURL(client, _curl, errorBuffer))
            return false;

        /* get custom header data (if set) */
        std::vector<std::string> headers=request->getHeaders();
        if(!headers.empty())
        {
            /* append custom headers one by one */
            for (auto& header : headers)
                _headers = curl_slist_append(_headers,header.c_str());
            /* set custom headers for curl */
            if (!setOption(CURLOPT_HTTPHEADER, _headers))
                return false;
        }
        std::string cookieFilename = client->getCookieFilename();
        if (!cookieFilename.empty()) {
            if (!setOption(CURLOPT_COOKIEFILE, cookieFilename.c_str())) {
                return false;
            }
            if (!setOption(CURLOPT_COOKIEJAR, cookieFilename.c_str())) {
                return false;
            }
        }

        return setOption(CURLOPT_URL, request->getUrl())
                && setOption(CURLOPT_WRITEFUNCTION, callback)
                && setOption(CURLOPT_WRITEDATA, stream)
                && setOption(CURLOPT_HEADERFUNCTION, headerCallback)
                && setOption(CURLOPT_HEADERDATA, headerStream);
        
    }

    /// @param responseCode Null not allowed
    bool perform(long *responseCode)
    {
        if (CURLE_OK != curl_easy_perform(_curl))
            return false;
        CURLcode code = curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, responseCode);
        if (code != CURLE_OK || !(*responseCode >= 200 && *responseCode < 300)) {
            OGLOGERROR("Curl curl_easy_getinfo failed: %s", curl_easy_strerror(code));
            return false;
        }
        // Get some mor data.
        
        return true;
    }
};

//Process Get Request
static int processGetTask(HttpClient* client, HttpRequest* request, write_callback callback, void* stream, long* responseCode, write_callback headerCallback, void* headerStream, char* errorBuffer)
{
    CURLRaii curl;
    bool ok = curl.init(client, request, callback, stream, headerCallback, headerStream, errorBuffer)
            && curl.setOption(CURLOPT_FOLLOWLOCATION, true)
            && curl.perform(responseCode);
    return ok ? 0 : 1;
}

//Process POST Request
static int processPostTask(HttpClient* client, HttpRequest* request, write_callback callback, void* stream, long* responseCode, write_callback headerCallback, void* headerStream, char* errorBuffer)
{
    CURLRaii curl;
    bool ok = curl.init(client, request, callback, stream, headerCallback, headerStream, errorBuffer)
            && curl.setOption(CURLOPT_POST, 1)
            && curl.setOption(CURLOPT_POSTFIELDS, request->getRequestData())
            && curl.setOption(CURLOPT_POSTFIELDSIZE, request->getRequestDataSize())
            && curl.perform(responseCode);
    return ok ? 0 : 1;
}

//Process PUT Request
static int processPutTask(HttpClient* client, HttpRequest* request, write_callback callback, void* stream, long* responseCode, write_callback headerCallback, void* headerStream, char* errorBuffer)
{
    CURLRaii curl;
    bool ok = curl.init(client, request, callback, stream, headerCallback, headerStream, errorBuffer)
            && curl.setOption(CURLOPT_CUSTOMREQUEST, "PUT")
            && curl.setOption(CURLOPT_POSTFIELDS, request->getRequestData())
            && curl.setOption(CURLOPT_POSTFIELDSIZE, request->getRequestDataSize())
            && curl.perform(responseCode);
    return ok ? 0 : 1;
}

//Process DELETE Request
static int processDeleteTask(HttpClient* client, HttpRequest* request, write_callback callback, void* stream, long* responseCode, write_callback headerCallback, void* headerStream, char* errorBuffer)
{
    CURLRaii curl;
    bool ok = curl.init(client, request, callback, stream, headerCallback, headerStream, errorBuffer)
            && curl.setOption(CURLOPT_CUSTOMREQUEST, "DELETE")
            && curl.setOption(CURLOPT_FOLLOWLOCATION, true)
            && curl.perform(responseCode);
    return ok ? 0 : 1;
}

// HttpClient implementation
HttpClient* HttpClient::getInstance()
{
    if (_httpClient == nullptr)
    {
        _httpClient = new (std::nothrow) HttpClient();
    }
    
    return _httpClient;
}
//原代码
// void HttpClient::destroyInstance()
// {
//     if (nullptr == _httpClient)
//     {
//         OGLOG("HttpClient singleton is nullptr");
//         return;
//     }
// 
//     OGLOG("HttpClient::destroyInstance begin");
//     auto thiz = _httpClient;
//     _httpClient = nullptr;
// 
//     thiz->_scheduler->unscheduleAllForTarget(thiz);
//     thiz->_schedulerMutex.lock();
//     thiz->_scheduler = nullptr;
//     thiz->_schedulerMutex.unlock();
// 
//     thiz->_requestQueueMutex.lock();
//     thiz->_requestQueue.pushBack(thiz->_requestSentinel);
//     thiz->_requestQueueMutex.unlock();
// 
//     thiz->_sleepCondition.notify_one();
//     thiz->decreaseThreadCountAndMayDeleteThis();
// 
//     OGLOG("HttpClient::destroyInstance() finished!");
// }
void HttpClient::destroyInstance()
{
	if (!_httpClient) return;

	auto thiz = _httpClient;
	_httpClient = nullptr;

	// 1. 先切断 scheduler（防止回调再进主线程，避免 use-after-free）
	{
		std::lock_guard<std::mutex> lk(thiz->_schedulerMutex);
		thiz->_scheduler = nullptr;
	}

	// 2. 设置退出标志（curl progress callback 会看到）
	thiz->_quitting = true;

	// 3. 唤醒线程，清空队列
	{
		std::lock_guard<std::mutex> lk(thiz->_requestQueueMutex);
		while (!thiz->_requestQueue.empty())
		{
			auto req = thiz->_requestQueue.at(0);
			thiz->_requestQueue.erase(0);
			req->release();
		}
		thiz->_requestQueue.pushBack(thiz->_requestSentinel);
	}
	thiz->_sleepCondition.notify_all();

	// 4. 等线程退出（curl 最多 100ms 被中断）
	const int MAX_WAIT_MS = 2000;
	int waited = 0;
	while (!thiz->_threadExited && waited < MAX_WAIT_MS)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		waited += 10;
	}

	// 5. 安全释放
	if (thiz->_threadExited)
	{
		delete thiz;
	}
	else
	{
		OGLOGERROR("HttpClient thread didn't exit in time, leaking instance");
		// 不 delete，泄漏比卡死好
	}
}
void HttpClient::enableCookies(const char* cookieFile)
{
    std::lock_guard<std::mutex> lock(_cookieFileMutex);
    if (cookieFile)
    {
        _cookieFilename = std::string(cookieFile);
    }
    else
    {
        _cookieFilename = (FileUtils::getInstance()->getWritablePath() + "cookieFile.txt");
    }
}
    
void HttpClient::setSSLVerification(const std::string& caFile)
{
    std::lock_guard<std::mutex> lock(_sslCaFileMutex);
    _sslCaFilename = caFile;
}

HttpClient::HttpClient()
: _isInited(false)
, _timeoutForConnect(30)
, _timeoutForRead(60)
, _threadCount(0)
, _cookie(nullptr)
, _requestSentinel(new HttpRequest())
, _clearRequestPredicate(nullptr)
, _clearResponsePredicate(nullptr)
{
    OGLOG("In the constructor of HttpClient!");
    memset(_responseMessage, 0, RESPONSE_BUFFER_SIZE * sizeof(char));
    _scheduler = Director::getInstance()->getScheduler();
    increaseThreadCount();
}

HttpClient::~HttpClient()
{
    OG_SAFE_RELEASE(_requestSentinel);
    OGLOG("HttpClient destructor");
}

//Lazy create semaphore & mutex & thread
bool HttpClient::lazyInitThreadSemaphore()
{
    if (_isInited)
    {
        return true;
    }
    else
    {
        auto t = std::thread(OG_CALLBACK_0(HttpClient::networkThread, this));
        t.detach();
        _isInited = true;
    }
    
    return true;
}

//Add a get task to queue
void HttpClient::send(HttpRequest* request)
{
    if (false == lazyInitThreadSemaphore())
    {
        return;
    }
    
    if (!request)
    {
        return;
    }
        
    request->retain();

    _requestQueueMutex.lock();
    _requestQueue.pushBack(request);
    _requestQueueMutex.unlock();

    // Notify thread start to work
    _sleepCondition.notify_one();
}

void HttpClient::sendImmediate(HttpRequest* request)
{
    if(!request)
    {
        return;
    }

    request->retain();
    // Create a HttpResponse object, the default setting is http access failed
    HttpResponse *response = new (std::nothrow) HttpResponse(request);

    auto t = std::thread(&HttpClient::networkThreadAlone, this, request, response);
    t.detach();
}

// Poll and notify main thread if responses exists in queue
void HttpClient::dispatchResponseCallbacks()
{
    // log("OGHttpClient::dispatchResponseCallbacks is running");
    //occurs when cocos thread fires but the network thread has already quited
    HttpResponse* response = nullptr;

    _responseQueueMutex.lock();
    if (!_responseQueue.empty())
    {
        response = _responseQueue.at(0);
        _responseQueue.erase(0);
    }
    _responseQueueMutex.unlock();
    
    if (response)
    {
        HttpRequest *request = response->getHttpRequest();
        const ogHttpRequestCallback& callback = request->getCallback();
        Ref* pTarget = request->getTarget();
        SEL_HttpResponse pSelector = request->getSelector();

        if (callback != nullptr)
        {
            callback(this, response);
        }
        else if (pTarget && pSelector)
        {
            (pTarget->*pSelector)(this, response);
        }
        
        response->release();
        // do not release in other thread
        request->release();
    }
}

// Process Response
void HttpClient::processResponse(HttpResponse* response, char* responseMessage)
{
    auto request = response->getHttpRequest();
    long responseCode = -1;
    int retValue = 0;

    // Process the request -> get response packet
    switch (request->getRequestType())
    {
    case HttpRequest::Type::GET: // HTTP GET
        retValue = processGetTask(this, request,
            writeData,
            response->getResponseData(),
            &responseCode,
            writeHeaderData,
            response->getResponseHeader(),
            responseMessage);
        break;

    case HttpRequest::Type::POST: // HTTP POST
        retValue = processPostTask(this, request,
            writeData,
            response->getResponseData(),
            &responseCode,
            writeHeaderData,
            response->getResponseHeader(),
            responseMessage);
        break;

    case HttpRequest::Type::PUT:
        retValue = processPutTask(this, request,
            writeData,
            response->getResponseData(),
            &responseCode,
            writeHeaderData,
            response->getResponseHeader(),
            responseMessage);
        break;

    case HttpRequest::Type::DELETE:
        retValue = processDeleteTask(this, request,
            writeData,
            response->getResponseData(),
            &responseCode,
            writeHeaderData,
            response->getResponseHeader(),
            responseMessage);
        break;

    default:
        OGASSERT(false, "OGHttpClient: unknown request type, only GET, POST, PUT or DELETE is supported");
        break;
    }

    // write data to HttpResponse
    response->setResponseCode(responseCode);
    if (retValue != 0)
    {
        response->setSucceed(false);
        response->setErrorBuffer(responseMessage);
    }
    else
    {
        response->setSucceed(true);
    }
}
    
void HttpClient::clearResponseAndRequestQueue()
{
    _requestQueueMutex.lock();
    if (_requestQueue.size())
    {
        for (auto it = _requestQueue.begin(); it != _requestQueue.end();)
        {
            if(!_clearRequestPredicate ||
               _clearRequestPredicate((*it)))
            {
                (*it)->release();
                it =_requestQueue.erase(it);
            }
            else
            {
                it++;
            }
        }
    }
    _requestQueueMutex.unlock();
    
    _responseQueueMutex.lock();
    if (_clearResponsePredicate)
    {
        _responseQueue.erase(std::remove_if(_responseQueue.begin(), _responseQueue.end(), _clearResponsePredicate), _responseQueue.end());
    }
    else
    {
        _responseQueue.clear();
    }
    _responseQueueMutex.unlock();
}

void HttpClient::increaseThreadCount()
{
    _threadCountMutex.lock();
    ++_threadCount;
    _threadCountMutex.unlock();
}
//原始代码
// void HttpClient::decreaseThreadCountAndMayDeleteThis()
// {
//     bool needDeleteThis = false;
//     _threadCountMutex.lock();
//     --_threadCount;
//     if (0 == _threadCount)
//     {
//         needDeleteThis = true;
//     }
// 
//     _threadCountMutex.unlock();
//     if (needDeleteThis)
//     {
//         delete this;
//     }
// }

// 在头文件中把析构函数设为 private 或删除 delete this 逻辑
void HttpClient::decreaseThreadCountAndMayDeleteThis()
{
	_threadCountMutex.lock();
	--_threadCount;
	_threadCountMutex.unlock();

	// ★ 不再 delete this，由 destroyInstance 统一管理
	// 防止线程里 delete 自己、主线程可能还在访问
}

void HttpClient::setTimeoutForConnect(int value)
{
    std::lock_guard<std::mutex> lock(_timeoutForConnectMutex);
    _timeoutForConnect = value;
}
    
int HttpClient::getTimeoutForConnect()
{
    std::lock_guard<std::mutex> lock(_timeoutForConnectMutex);
    return _timeoutForConnect;
}
    
void HttpClient::setTimeoutForRead(int value)
{
    std::lock_guard<std::mutex> lock(_timeoutForReadMutex);
    _timeoutForRead = value;
}
    
int HttpClient::getTimeoutForRead()
{
    std::lock_guard<std::mutex> lock(_timeoutForReadMutex);
    return _timeoutForRead;
}
    
const std::string& HttpClient::getCookieFilename()
{
    std::lock_guard<std::mutex> lock(_cookieFileMutex);
    return _cookieFilename;
}
    
const std::string& HttpClient::getSSLVerification()
{
    std::lock_guard<std::mutex> lock(_sslCaFileMutex);
    return _sslCaFilename;
}

}

OG_END



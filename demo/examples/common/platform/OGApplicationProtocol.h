 

#ifndef __OG_APPLICATION_PROTOCOL_H__
#define __OG_APPLICATION_PROTOCOL_H__

#include "platform/OGPlatformMacros.h"
#include "base/OGAutoreleasePool.h"
#include "base/ogTypes.h"

OG_BEGIN

/**
 * @addtogroup platform
 * @{
 */

class ApplicationProtocol
{
public: 
    enum class Platform
    {
        OS_WINDOWS,     /**< Windows */
        OS_LINUX,       /**< Linux */
        OS_MAC,         /**< Mac OS X*/
        OS_ANDROID,     /**< Android */
        OS_IPHONE,      /**< iPhone */
        OS_IPAD         /**< iPad */
        // OS_BLACKBERRY,  /**< BlackBerry */
        // OS_NACL,        /**< Native Client in Chrome */
        // OS_EMSCRIPTEN,  /**< Emscripten */
        // OS_TIZEN,       /**< Tizen */
        // OS_WINRT,       /**< Windows Runtime Applications */
        // OS_WP8          /**< Windows Phone 8 Applications */
    }; 
	virtual bool init() = 0;
    virtual ~ApplicationProtocol(){
        /** clean auto release pool. */
        PoolManager::destroyInstance();
    } 
//     virtual bool applicationDidFinishLaunching() = 0; 
//     virtual void applicationDidEnterBackground() = 0; 
//     virtual void applicationWillEnterForeground() = 0; 
//     virtual void setAnimationInterval(float interval) = 0; 
//     virtual void initGLContextAttrs() {} 
//  //   virtual LanguageType getCurrentLanguage() = 0; 
//     virtual const char * getCurrentLanguageCode() = 0; 
//     virtual Platform getTargetPlatform() = 0; 
//     virtual std::string getVersion() = 0; 
//     virtual bool openURL(const std::string &url) = 0;
};

// end of platform group
/** @} */

OG_END

#endif    // __OG_APPLICATION_PROTOCOL_H__

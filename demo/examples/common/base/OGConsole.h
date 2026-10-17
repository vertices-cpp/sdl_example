
#ifndef __OGCONSOLE_H__
#define __OGCONSOLE_H__
/// @cond DO_NOT_SHOW

#if defined(_MSC_VER) || defined(__MINGW32__)
 #include <BaseTsd.h>
// #include <WinSock2.h>

#ifndef __SSIZE_T
#define __SSIZE_T
typedef SSIZE_T ssize_t;
#endif // __SSIZE_T

#else
#include <sys/select.h>
#endif

#include <thread>
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include <mutex>
#include <stdarg.h>

#include "base/OGRef.h"
#include "base/ogMacros.h"
#include "platform/OGPlatformMacros.h"

OG_BEGIN

/// The max length of OGLog message.
static const int MAX_LOG_LENGTH = 16*1024;

/**
 @brief Output Debug message.
 */
void   log(const char * format, ...) OG_FORMAT_PRINTF(1, 2);
 

OG_END

/// @endcond
#endif /* defined(__OGCONSOLE_H__) */

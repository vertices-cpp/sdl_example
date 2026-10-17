 

#ifndef __BASE_OG_PLATFORM_CONFIG_H__
#define __BASE_OG_PLATFORM_CONFIG_H__
/// @cond DO_NOT_SHOW

/**
  Config of orange project, per target platform.

  THIS FILE MUST NOT INCLUDE ANY OTHER FILE
*/

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which OG uses.
#define OG_PLATFORM_UNKNOWN            0
#define OG_PLATFORM_IOS                1
#define OG_PLATFORM_ANDROID            2
#define OG_PLATFORM_WIN32              3
// #define OG_PLATFORM_MARMALADE          4
#define OG_PLATFORM_LINUX              5
// #define OG_PLATFORM_BADA               6
// #define OG_PLATFORM_BLACKBERRY         7
#define OG_PLATFORM_MAC                8
// #define OG_PLATFORM_NACL               9
// #define OG_PLATFORM_EMSCRIPTEN        10
// #define OG_PLATFORM_TIZEN             11
// #define OG_PLATFORM_QT5               12
// #define OG_PLATFORM_WINRT             13

// Determine target platform by compile environment macro.
#define OG_TARGET_PLATFORM             OG_PLATFORM_UNKNOWN

// Apple: Mac and iOS
#if defined(__APPLE__) && !defined(ANDROID) // exclude android for binding generator.
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
        #undef  OG_TARGET_PLATFORM
        #define OG_TARGET_PLATFORM         OG_PLATFORM_IOS
    #elif TARGET_OS_MAC
        #undef  OG_TARGET_PLATFORM
        #define OG_TARGET_PLATFORM         OG_PLATFORM_MAC
    #endif
#endif

// android
#if defined(ANDROID)
    #undef  OG_TARGET_PLATFORM
    #define OG_TARGET_PLATFORM         OG_PLATFORM_ANDROID
#endif

// win32
#if defined(_WIN32) && defined(_WINDOWS)
    #undef  OG_TARGET_PLATFORM
    #define OG_TARGET_PLATFORM         OG_PLATFORM_WIN32
#endif

// linux
#if defined(LINUX) && !defined(__APPLE__)
    #undef  OG_TARGET_PLATFORM
    #define OG_TARGET_PLATFORM         OG_PLATFORM_LINUX
#endif


//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if ! OG_TARGET_PLATFORM
    #error  "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif

#if (OG_TARGET_PLATFORM == OG_PLATFORM_WIN32)
#ifndef __MINGW32__
#pragma warning (disable:4127)
#endif
#endif  // OG_PLATFORM_WIN32

#if ((OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID) || (OG_TARGET_PLATFORM == OG_PLATFORM_IOS))
    #define OG_PLATFORM_MOBILE
#else
    #define OG_PLATFORM_PC
#endif

#if (OG_TARGET_PLATFORM == OG_PLATFORM_MAC) || (OG_TARGET_PLATFORM == OG_PLATFORM_IOS)
    #define OG_USE_METAL
#elif (OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID)
    #define OG_USE_GLES
#else
    #define OG_USE_GL
#endif

/// @endcond
#endif  // __BASE_OG_PLATFORM_CONFIG_H__

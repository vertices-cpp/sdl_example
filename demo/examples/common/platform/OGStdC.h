

#ifndef __PLATFORM_OGSTDC_H__
#define __PLATFORM_OGSTDC_H__

#include "platform/OGPlatformConfig.h"

#if defined(_WIN32)||defined(_WIN64)
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Conflicted with ParticleSystem::PositionType::RELATIVE, so we need to undef it.
#ifdef RELATIVE
#undef RELATIVE
#endif

// Conflicted with CCBReader::SizeType::RELATIVE and CCBReader::ScaleType::RELATIVE, so we need to undef it.
#ifdef ABSOLUTE
#undef ABSOLUTE
#endif

// Conflicted with HttpRequest::Type::DELETE, so we need to undef it.
#ifdef DELETE
#undef DELETE
#endif

#undef min
#undef max



#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif  // MAX






#endif
// 
// #if OG_TARGET_PLATFORM == OG_PLATFORM_MAC
// #include "platform/mac/OGStdC-mac.h"
// #elif OG_TARGET_PLATFORM == OG_PLATFORM_IOS
// #include "platform/ios/OGStdC-ios.h"
// #elif OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID
// #include "platform/android/OGStdC-android.h"
// #elif OG_TARGET_PLATFORM == OG_PLATFORM_WIN32
// #include "platform/win32/OGStdC-win32.h"
// #elif OG_TARGET_PLATFORM == OG_PLATFORM_LINUX
// #include "platform/linux/OGStdC-linux.h"
// #endif

#endif /* __PLATFORM_OGSTDC_H__*/

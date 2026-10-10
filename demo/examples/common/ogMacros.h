
#ifndef __BASE_OGMACROS_H__
#define __BASE_OGMACROS_H__

// #ifndef _USE_MATH_DEFINES
// #define _USE_MATH_DEFINES
// #endif  防止冲突
 

// #include "OGConsole.h"
// #include "OGStdC.h"

#ifndef OGASSERT
	#if ORANGE_DEBUG > 0
		//#if OG_ENABLE_SCRIPT_BINDING
		//extern bool   og_assert_script_compatible(const char *msg);
		#define OGASSERT(cond, msg) do {                              \
						if (!(cond)) {                                          \
						if (/*!og_assert_script_compatible(msg) && */strlen(msg)) \
							orange::log("Assert failed: %s", msg);             \
						OG_ASSERT(cond);                                      \
						} \
					} while (0)
// 		#else
// 		#define OGASSERT(cond, msg) OG_ASSERT(cond)
// 		#endif
	#else
	#define OGASSERT(cond, msg)
	#endif

#define GP_ASSERT(cond) OGASSERT(cond, "")

// FIXME:: Backward compatible
#define OGAssert OGASSERT
#endif  // OGASSERT

// #include "ogConfig.h"
// 
// #include "ogRandom.h"

/** @def OGRANDOM_MINUS1_1
 returns a random float between -1 and 1
 */
#define OGRANDOM_MINUS1_1() orange::rand_minus1_1()

 /** @def OGRANDOM_0_1
  returns a random float between 0 and 1
  */
#define OGRANDOM_0_1() orange::rand_0_1()

  /** @def OG_DEGREES_TO_RADIANS
   converts degrees to radians
   */
#define OG_DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) // PI / 180

   /** @def OG_RADIANS_TO_DEGREES
	converts radians to degrees
	*/
#define OG_RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180

#define OG_REPEAT_FOREVER (UINT_MAX -1)
#define kRepeatForever OG_REPEAT_FOREVER

	/** @def OG_BLEND_SRC
	default gl blend src function. Compatible with premultiplied alpha images.
	*/
#define OG_BLEND_SRC orange::backend::BlendFactor::ONE
#define OG_BLEND_DST orange::backend::BlendFactor::ONE_MINUS_SRC_ALPHA


	/** @def OG_NODE_DRAW_SETUP
	 Helpful macro that setups the GL server state, the correct GL program and sets the Model View Projection matrix
	 @since v2.0
	 */
#define OG_NODE_DRAW_SETUP() \
do { \
    OGASSERT(getGLProgram(), "No shader program set for this node"); \
    { \
        getGLProgram()->use(); \
        getGLProgram()->setUniformsForBuiltins(_modelViewTransform); \
    } \
} while(0)


	 /** @def OG_DIRECTOR_END
	  Stops and removes the director from memory.
	  Removes the GLView from its parent

	  @since v0.99.4
	  */
#define OG_DIRECTOR_END()                                       \
do {                                                            \
    Director *__director = orange::Director::getInstance();             \
    __director->end();                                          \
} while(0)

	  /** @def OG_CONTENT_SCALE_FACTOR
	  On Mac it returns 1;
	  On iPhone it returns 2 if RetinaDisplay is On. Otherwise it returns 1
	  */
//#define OG_CONTENT_SCALE_FACTOR() orange::Director::getInstance()->getContentScaleFactor()
//
//	  /****************************/
//	  /** RETINA DISPLAY ENABLED **/
//	  /****************************/
//
//	  /** @def OG_RECT_PIXELS_TO_POINTS
//	   Converts a rect in pixels to points
//	   */
//#define OG_RECT_PIXELS_TO_POINTS(__rect_in_pixels__)                                                                        \
//    orange::Rect( (__rect_in_pixels__).origin.x / OG_CONTENT_SCALE_FACTOR(), (__rect_in_pixels__).origin.y / OG_CONTENT_SCALE_FACTOR(),    \
//            (__rect_in_pixels__).size.width / OG_CONTENT_SCALE_FACTOR(), (__rect_in_pixels__).size.height / OG_CONTENT_SCALE_FACTOR() )
//
//	   /** @def OG_RECT_POINTS_TO_PIXELS
//		Converts a rect in points to pixels
//		*/
//#define OG_RECT_POINTS_TO_PIXELS(__rect_in_points_points__)                                                                        \
//    orange::Rect( (__rect_in_points_points__).origin.x * OG_CONTENT_SCALE_FACTOR(), (__rect_in_points_points__).origin.y * OG_CONTENT_SCALE_FACTOR(),    \
//            (__rect_in_points_points__).size.width * OG_CONTENT_SCALE_FACTOR(), (__rect_in_points_points__).size.height * OG_CONTENT_SCALE_FACTOR() )
//
//		/** @def OG_POINT_PIXELS_TO_POINTS
//		 Converts a rect in pixels to points
//		 */
//#define OG_POINT_PIXELS_TO_POINTS(__pixels__)                                                                        \
//orange::Vec2( (__pixels__).x / OG_CONTENT_SCALE_FACTOR(), (__pixels__).y / OG_CONTENT_SCALE_FACTOR())
//
//		 /** @def OG_POINT_POINTS_TO_PIXELS
//		  Converts a rect in points to pixels
//		  */
//#define OG_POINT_POINTS_TO_PIXELS(__points__)                                                                        \
//orange::Vec2( (__points__).x * OG_CONTENT_SCALE_FACTOR(), (__points__).y * OG_CONTENT_SCALE_FACTOR())
//
//		  /** @def OG_POINT_PIXELS_TO_POINTS
//		   Converts a rect in pixels to points
//		   */
//#define OG_SIZE_PIXELS_TO_POINTS(__size_in_pixels__)                                                                        \
//orange::Size( (__size_in_pixels__).width / OG_CONTENT_SCALE_FACTOR(), (__size_in_pixels__).height / OG_CONTENT_SCALE_FACTOR())
//
//		   /** @def OG_POINT_POINTS_TO_PIXELS
//			Converts a rect in points to pixels
//			*/
//#define OG_SIZE_POINTS_TO_PIXELS(__size_in_points__)                                                                        \
//orange::Size( (__size_in_points__).width * OG_CONTENT_SCALE_FACTOR(), (__size_in_points__).height * OG_CONTENT_SCALE_FACTOR())


#ifndef FLT_EPSILON
#define FLT_EPSILON     1.192092896e-07F
#endif // FLT_EPSILON

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&);\
            void operator=(const TypeName&)

			/**
			Helper macros which converts 4-byte little/big endian
			integral number to the machine native number representation

			It should work same as apples CFSwapInt32LittleToHost(..)
			*/

			/// when define returns true it means that our architecture uses big endian
#define OG_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define OG_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define OG_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define OG_SWAP_INT32_LITTLE_TO_HOST(i) ((OG_HOST_IS_BIG_ENDIAN == true)? OG_SWAP32(i) : (i) )
#define OG_SWAP_INT16_LITTLE_TO_HOST(i) ((OG_HOST_IS_BIG_ENDIAN == true)? OG_SWAP16(i) : (i) )
#define OG_SWAP_INT32_BIG_TO_HOST(i)    ((OG_HOST_IS_BIG_ENDIAN == true)? (i) : OG_SWAP32(i) )
#define OG_SWAP_INT16_BIG_TO_HOST(i)    ((OG_HOST_IS_BIG_ENDIAN == true)? (i):  OG_SWAP16(i) )

/**********************/
/** Profiling Macros **/
/**********************/
#if OG_ENABLE_PROFILERS

#define OG_PROFILER_DISPLAY_TIMERS() NS_OG::Profiler::getInstance()->displayTimers()
#define OG_PROFILER_PURGE_ALL() NS_OG::Profiler::getInstance()->releaseAllTimers()

#define OG_PROFILER_START(__name__) NS_OG::ProfilingBeginTimingBlock(__name__)
#define OG_PROFILER_STOP(__name__) NS_OG::ProfilingEndTimingBlock(__name__)
#define OG_PROFILER_RESET(__name__) NS_OG::ProfilingResetTimingBlock(__name__)

#define OG_PROFILER_START_CATEGORY(__cat__, __name__) do{ if(__cat__) NS_OG::ProfilingBeginTimingBlock(__name__); } while(0)
#define OG_PROFILER_STOP_CATEGORY(__cat__, __name__) do{ if(__cat__) NS_OG::ProfilingEndTimingBlock(__name__); } while(0)
#define OG_PROFILER_RESET_CATEGORY(__cat__, __name__) do{ if(__cat__) NS_OG::ProfilingResetTimingBlock(__name__); } while(0)

#define OG_PROFILER_START_INSTANCE(__id__, __name__) do{ NS_OG::ProfilingBeginTimingBlock( NS_OG::String::createWithFormat("%08X - %s", __id__, __name__)->getCString() ); } while(0)
#define OG_PROFILER_STOP_INSTANCE(__id__, __name__) do{ NS_OG::ProfilingEndTimingBlock(    NS_OG::String::createWithFormat("%08X - %s", __id__, __name__)->getCString() ); } while(0)
#define OG_PROFILER_RESET_INSTANCE(__id__, __name__) do{ NS_OG::ProfilingResetTimingBlock( NS_OG::String::createWithFormat("%08X - %s", __id__, __name__)->getCString() ); } while(0)


#else

#define OG_PROFILER_DISPLAY_TIMERS() do {} while (0)
#define OG_PROFILER_PURGE_ALL() do {} while (0)

#define OG_PROFILER_START(__name__)  do {} while (0)
#define OG_PROFILER_STOP(__name__) do {} while (0)
#define OG_PROFILER_RESET(__name__) do {} while (0)

#define OG_PROFILER_START_CATEGORY(__cat__, __name__) do {} while(0)
#define OG_PROFILER_STOP_CATEGORY(__cat__, __name__) do {} while(0)
#define OG_PROFILER_RESET_CATEGORY(__cat__, __name__) do {} while(0)

#define OG_PROFILER_START_INSTANCE(__id__, __name__) do {} while(0)
#define OG_PROFILER_STOP_INSTANCE(__id__, __name__) do {} while(0)
#define OG_PROFILER_RESET_INSTANCE(__id__, __name__) do {} while(0)

#endif

#if !defined(ORANGE_DEBUG) || ORANGE_DEBUG == 0
#define CHECK_GL_ERROR_DEBUG()
#else
#define CHECK_GL_ERROR_DEBUG() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            orange::log("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
        } \
    } while (false)
#define CHECK_GL_ERROR_ABORT() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            orange::log("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
            assert(false);\
        } \
    } while (false)
#endif


/**
 * GL assertion that can be used for any OpenGL function call.
 *
 * This macro will assert if an error is detected when executing
 * the specified GL code. This macro will do nothing in release
 * mode and is therefore safe to use for realtime/per-frame GL
 * function calls.
 */
#if defined(NDEBUG) || (defined(__APPLE__) && !defined(DEBUG))
#define OG_GL_ASSERT( gl_code ) gl_code
#else
#define OG_GL_ASSERT( gl_code ) do \
{ \
gl_code; \
__gl_error_code = glGetError(); \
OG_ASSERT(__gl_error_code == GL_NO_ERROR, "Error"); \
} while(0)
#endif

 /*********************************/
 /** 64bits Program Sense Macros **/
 /*********************************/
#if defined(_M_X64) || defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(__x86_64) || defined(__arm64__) || defined(__aarch64__)
#define OG_64BITS 1
#else
#define OG_64BITS 0
#endif

 /******************************************************************************************/
 /** LittleEndian Sense Macro, from google protobuf see:                                  **/
 /** https://github.com/google/protobuf/blob/master/src/google/protobuf/io/coded_stream.h **/
 /******************************************************************************************/
#ifdef _MSC_VER
#if defined(_M_IX86)
#define OG_LITTLE_ENDIAN 1
#else
#define OG_LITTLE_ENDIAN 0
#endif
#if _MSC_VER >= 1300 && !defined(__INTEL_COMPILER)
#pragma runtime_checks("c", off)
#endif
#else
#include <sys/param.h>
#if (OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID)
#include <sys/endian.h>
#endif // OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID
#if ((defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)) || \
         (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN)) 
#define OG_LITTLE_ENDIAN 1
#else
#define OG_LITTLE_ENDIAN 0
#endif
#endif

/** @def OG_INCREMENT_GL_DRAWS_BY_ONE
 Increments the GL Draws counts by one.
 The number of calls per frame are displayed on the screen when the Director's stats are enabled.
 */
#define OG_INCREMENT_GL_DRAWS(__n__) orange::Director::getInstance()->getRenderer()->addDrawnBatches(__n__)
#define OG_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(__drawcalls__, __vertices__) \
    do {                                                                \
        auto __renderer__ = orange::Director::getInstance()->getRenderer();     \
        __renderer__->addDrawnBatches(__drawcalls__);                   \
        __renderer__->addDrawnVertices(__vertices__);                   \
    } while(0)

 /*******************/
 /** Notifications **/
 /*******************/
 /** @def AnimationFrameDisplayedNotification
  Notification name when a SpriteFrame is displayed
  */
#define AnimationFrameDisplayedNotification "OGAnimationFrameDisplayedNotification"

  /*******************/
  /** Notifications **/
  /*******************/
  /** @def Animate3DDisplayedNotification
   Notification name when a frame in Animate3D is played
   */
#define Animate3DDisplayedNotification "OGAnimate3DDisplayedNotification"

   // new callbacks based on C++11
#define OG_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define OG_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define OG_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define OG_CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)

#include <assert.h>

//增加
#if OG_DISABLE_ASSERT > 0
#define OG_ASSERT(cond)
#else
#define OG_ASSERT(cond)    assert(cond)
#endif

#endif // __BASE_OGMACROS_H__

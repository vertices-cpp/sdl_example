

#ifndef __OG_PLATFORM_MACROS_H__
#define __OG_PLATFORM_MACROS_H__

 /**
  * Define some platform specific macros.
  */
// #include "ogConfig.h"
// #include "OGPlatformConfig.h"
// #include "OGPlatformDefine.h"
#include "OGPlatformMacros.h"
#include <string>



  /** @def CREATE_FUNC(__TYPE__)
   * Define a create function for a specific type, such as Layer.
   *
   * @param __TYPE__  class type to add create(), such as Layer.
   */
#define CREATE_FUNC(__TYPE__) \
static __TYPE__* create() \
{ \
    __TYPE__ *pRet = new(std::nothrow) __TYPE__(); \
    if (pRet && pRet->init()) \
    { \
        pRet->autorelease(); \
        return pRet; \
    } \
    else \
    { \
        delete pRet; \
        pRet = nullptr; \
        return nullptr; \
    } \
}

   /** @def NODE_FUNC(__TYPE__)
	* Define a node function for a specific type, such as Layer.
	*
	* @param __TYPE__  class type to add node(), such as Layer.
	* @deprecated  This interface will be deprecated sooner or later.
	*/
#define NODE_FUNC(__TYPE__) \
OG_DEPRECATED_ATTRIBUTE static __TYPE__* node() \
{ \
    __TYPE__ *pRet = new(std::nothrow) __TYPE__(); \
    if (pRet && pRet->init()) \
    { \
        pRet->autorelease(); \
        return pRet; \
    } \
    else \
    { \
        delete pRet; \
        pRet = NULL; \
        return NULL; \
    } \
}

	/** @def OG_ENABLE_CACHE_TEXTURE_DATA
	 * Enable it if you want to cache the texture data.
	 * Not enabling for Emscripten any more -- doesn't seem necessary and don't want
	 * to be different from other platforms unless there's a good reason.
	 *
	 * @since v0.99.5
	 */
#if (OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID)
#define OG_ENABLE_CACHE_TEXTURE_DATA       1
#else
#define OG_ENABLE_CACHE_TEXTURE_DATA       0
#endif

#if (OG_TARGET_PLATFORM == OG_PLATFORM_ANDROID) || (OG_TARGET_PLATFORM == OG_PLATFORM_WIN32)
	 /** Application will crash in glDrawElements function on some win32 computers and some android devices.
	  *  Indices should be bound again while drawing to avoid this bug.
	  */
#define OG_REBIND_INDICES_BUFFER  1
#else
#define OG_REBIND_INDICES_BUFFER  0
#endif

	 // Generic macros

	 /// @name namespace orange
	 /// @{
#ifdef __cplusplus
#define OG_BEGIN                     namespace orange {
#define OG_END                       }
#define USING_OG                     using namespace orange
#define OG                           ::orange
#else
#define OG_BEGIN 
#define OG_END 
#define USING_NS_OG 
#define OG
#endif 


OG_BEGIN
//  end of namespace group
/// @}

/** @def OG_PROPERTY_READONLY
 * It is used to declare a protected variable. We can use getter to read the variable.
 *
 * @param varType     the type of variable.
 * @param varName     variable name.
 * @param funName     "get + funName" will be the name of the getter.
 * @warning   The getter is a public virtual function, you should rewrite it first.
 *            The variables and methods declared after OG_PROPERTY_READONLY are all public.
 *            If you need protected or private, please declare.
 */
#define OG_PROPERTY_READONLY(varType, varName, funName)\
protected: varType varName; public: virtual varType get##funName() const;

#define OG_PROPERTY_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName; public: virtual const varType& get##funName() const;

 /** @def OG_PROPERTY
  * It is used to declare a protected variable.
  * We can use getter to read the variable, and use the setter to change the variable.
  *
  * @param varType     The type of variable.
  * @param varName     Variable name.
  * @param funName     "get + funName" will be the name of the getter.
  *                    "set + funName" will be the name of the setter.
  * @warning   The getter and setter are public virtual functions, you should rewrite them first.
  *            The variables and methods declared after OG_PROPERTY are all public.
  *            If you need protected or private, please declare.
  */
#define OG_PROPERTY(varType, varName, funName)\
protected: varType varName; public: virtual varType get##funName() const; virtual void set##funName(varType var);

#define OG_PROPERTY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName; public: virtual const varType& get##funName() const; virtual void set##funName(const varType& var);

  /** @def OG_SYNTHESIZE_READONLY
   * It is used to declare a protected variable. We can use getter to read the variable.
   *
   * @param varType     The type of variable.
   * @param varName     Variable name.
   * @param funName     "get + funName" will be the name of the getter.
   * @warning   The getter is a public inline function.
   *            The variables and methods declared after OG_SYNTHESIZE_READONLY are all public.
   *            If you need protected or private, please declare.
   */
#define OG_SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName; public: virtual inline varType get##funName() const { return varName; }

#define OG_SYNTHESIZE_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName; public: virtual inline const varType& get##funName() const { return varName; }

   /** @def OG_SYNTHESIZE
	* It is used to declare a protected variable.
	* We can use getter to read the variable, and use the setter to change the variable.
	*
	* @param varType     The type of variable.
	* @param varName     Variable name.
	* @param funName     "get + funName" will be the name of the getter.
	*                    "set + funName" will be the name of the setter.
	* @warning   The getter and setter are public inline functions.
	*            The variables and methods declared after OG_SYNTHESIZE are all public.
	*            If you need protected or private, please declare.
	*/
#define OG_SYNTHESIZE(varType, varName, funName)\
protected: varType varName; public: virtual inline varType get##funName() const { return varName; } virtual inline void set##funName(varType var){ varName = var; }

#define OG_SYNTHESIZE_PASS_BY_REF(varType, varName, funName)\
protected: varType varName; public: virtual inline const varType& get##funName() const { return varName; } virtual inline void set##funName(const varType& var){ varName = var; }

#define OG_SYNTHESIZE_RETAIN(varType, varName, funName)    \
private: varType varName; public: virtual inline varType get##funName() const { return varName; } virtual inline void set##funName(varType var) \
{ \
    if (varName != var) \
    { \
        OG_SAFE_RETAIN(var); \
        OG_SAFE_RELEASE(varName); \
        varName = var; \
    } \
} 

#define OG_SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define OG_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define OG_SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define OG_SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define OG_SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define OG_SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)
#define OG_BREAK_IF(cond)           if(cond) break

#define __OGLOGWITHFUNCTION(s, ...) \
    orange::log("%s : %s",__FUNCTION__, orange::StringUtils::format(s, ##__VA_ARGS__).c_str())

	/// @name orange debug
	/// @{
#if !defined(ORANGE_DEBUG) || ORANGE_DEBUG == 0
#define OGLOG(...)       do {} while (0)
#define OGLOGINFO(...)   do {} while (0)
#define OGLOGERROR(...)  do {} while (0)
#define OGLOGWARN(...)   do {} while (0)

#elif ORANGE_DEBUG == 1
#define OGLOG(format, ...)      orange::log(format, ##__VA_ARGS__)
#define OGLOGERROR(format,...)  orange::log(format, ##__VA_ARGS__)
#define OGLOGINFO(format,...)   do {} while (0)
#define OGLOGWARN(...) __OGLOGWITHFUNCTION(__VA_ARGS__)

#elif ORANGE_DEBUG > 1
#define OGLOG(format, ...)      orange::log(format, ##__VA_ARGS__)
#define OGLOGERROR(format,...)  orange::log(format, ##__VA_ARGS__)
#define OGLOGINFO(format,...)   orange::log(format, ##__VA_ARGS__)
#define OGLOGWARN(...) __OGLOGWITHFUNCTION(__VA_ARGS__)
#endif // ORANGE_DEBUG

/** Lua engine debug */
#if !defined(ORANGE_DEBUG) || ORANGE_DEBUG == 0 || OG_LUA_ENGINE_DEBUG == 0
#define LUALOG(...)
#else
#define LUALOG(format, ...)     orange::log(format, ##__VA_ARGS__)
#endif // Lua engine debug

//  end of debug group
/// @}

/** @def OG_DISALLOW_COPY_AND_ASSIGN(TypeName)
 * A macro to disallow the copy constructor and operator= functions.
 * This should be used in the private: declarations for a class
 */
#if defined(__GNUC__) && ((__GNUC__ >= 5) || ((__GNUG__ == 4) && (__GNUC_MINOR__ >= 4))) \
    || (defined(__clang__) && (__clang_major__ >= 3)) || (_MSC_VER >= 1800)
#define OG_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete; \
    TypeName &operator =(const TypeName &) = delete;
#else
#define OG_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &); \
    TypeName &operator =(const TypeName &);
#endif

 /** @def OG_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName)
  * A macro to disallow all the implicit constructors, namely the
  * default constructor, copy constructor and operator= functions.
  *
  * This should be used in the private: declarations for a class
  * that wants to prevent anyone from instantiating it. This is
  * especially useful for classes containing only static methods.
  */
#define OG_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName)    \
    TypeName();                                        \
    OG_DISALLOW_COPY_AND_ASSIGN(TypeName)

  /** @def OG_DEPRECATED_ATTRIBUTE
   * Only certain compilers support __attribute__((deprecated)).
   */
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define OG_DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#elif _MSC_VER >= 1400 //vs 2005 or higher
#define OG_DEPRECATED_ATTRIBUTE __declspec(deprecated) 
#else
#define OG_DEPRECATED_ATTRIBUTE
#endif 

   /** @def OG_DEPRECATED(...)
	* Macro to mark things deprecated as of a particular version
	* can be used with arbitrary parameters which are thrown away.
	* e.g. OG_DEPRECATED(4.0) or OG_DEPRECATED(4.0, "not going to need this anymore") etc.
	*/
#define OG_DEPRECATED(...) OG_DEPRECATED_ATTRIBUTE

	/** @def OG_FORMAT_PRINTF(formatPos, argPos)
	 * Only certain compiler support __attribute__((format))
	 *
	 * @param formatPos 1-based position of format string argument.
	 * @param argPos    1-based position of first format-dependent argument.
	 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define OG_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
#if __has_attribute(format)
#define OG_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#endif // __has_attribute(format)
#else
#define OG_FORMAT_PRINTF(formatPos, argPos)
#endif

#if defined(_MSC_VER)
#define OG_FORMAT_PRINTF_SIZE_T "%08lX"
#else
#define OG_FORMAT_PRINTF_SIZE_T "%08zX"
#endif

#ifdef __GNUC__
#define OG_UNUSED __attribute__ ((unused))
#else
#define OG_UNUSED
#endif

	 /** @def OG_REQUIRES_NULL_TERMINATION
	  *
	  */
#if !defined(OG_REQUIRES_NULL_TERMINATION)
#if defined(__APPLE_OG__) && (__APPLE_OG__ >= 5549)
#define OG_REQUIRES_NULL_TERMINATION __attribute__((sentinel(0,1)))
#elif defined(__GNUC__)
#define OG_REQUIRES_NULL_TERMINATION __attribute__((sentinel))
#else
#define OG_REQUIRES_NULL_TERMINATION
#endif
#endif

//增加

#define Instance(CLASS_TYPE)\
	static CLASS_TYPE *getInstance() {\
		static CLASS_TYPE instance_;\
		return &instance_;\
	}

#if !defined _W64
#define _W64
#endif

#if defined(_WIN64)
typedef __int64 INT_PTR, *PINT_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;

typedef __int64 LONG_PTR, *PLONG_PTR;
typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;

#define __int3264   __int64

#else
typedef _W64 int INT_PTR, *PINT_PTR;
typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;

typedef _W64 long LONG_PTR, *PLONG_PTR;
typedef _W64 unsigned long ULONG_PTR, *PULONG_PTR;

#define __int3264   __int32

#endif

typedef LONG_PTR SSIZE_T, *PSSIZE_T;

#ifndef __SSIZE_T
#define __SSIZE_T


typedef SSIZE_T ssize_t;
#endif // __SSIZE_T

//OG_BEGIN
extern void log(const char * format, ...);
namespace StringUtils {
	extern std::string format(const char* format, ...);
}

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

#define OG_TARGET_PLATFORM             OG_PLATFORM_UNKNOWN

#if defined(_WIN32) && defined(_WINDOWS)
#undef  OG_TARGET_PLATFORM
#define OG_TARGET_PLATFORM         OG_PLATFORM_WIN32
#endif
OG_END

#endif // __OG_PLATFORM_MACROS_H__

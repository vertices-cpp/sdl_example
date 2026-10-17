

#ifndef __OGLIBGUI_H__
#define __OGLIBGUI_H__

#if (defined(WIN32) && defined(_WINDOWS))
    #ifdef __MINGW32__
        #include <string.h>
    #endif

    #if defined(OG_STATIC)
        #define OG_GUI_DLL
    #else
        #if defined(_USEGUIDLL)
            #define OG_GUI_DLL     __declspec(dllexport)
        #else
            #define OG_GUI_DLL     __declspec(dllimport)
        #endif
    #endif

    /* Define NULL pointer value */
    #ifndef NULL
        #ifdef __cplusplus
            #define NULL    0
        #else
            #define NULL    ((void *)0)
        #endif
    #endif
#elif defined(_SHARED_)
    #define OG_GUI_DLL     __attribute__((visibility("default")))
#else
    #define OG_GUI_DLL
#endif

#endif /* __OGEXTENSIONEXPORT_H__*/
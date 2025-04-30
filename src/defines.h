#ifndef EZ_DEFINES_H
#define EZ_DEFINES_H

#include "platform.h"
#include "config.h"

#if defined(EZWIN_IS_BUILT_AS_SHARED_LIB)
    #if defined(COMPILER_GCC)
        // NOTE GCC exports every symbol to the ELF by default, unless -fvisibility=hidden is specified
        #if __GNUC__ >= 4 // NOTE: taken from SDL
            #define EZAPI __attribute__((visibility("default")))
        #else
            #define EZAPI
        #endif
    #elif defined(COMPILER_CLANG)
        #define EZAPI __attribute__((visibility("default")))
    #elif defined(COMPILER_MSVC)
        #define EZAPI __declspec(dllexport)
    #elif defined(COMPILER_MINGW)
        // NOTE apparently you can use both __attribute__() & __declspec() with mingw
        #define EZAPI __declspec(dllexport)
    #elif defined(COMPILER_TCC) && defined(PLATFORM_WIN32)
        #define EZAPI __attribute__(dllexport)
    #endif
#elif defined(EZWIN_IS_BUILT_AS_STATIC_LIB)
    #define EZAPI
#endif



#endif // EZ_DEFINES_H

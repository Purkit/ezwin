#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(_WIN32)
    #define PLATFORM_WIN32
#elif defined(__linux__) // || defined(__gnu_linux__)
    #define PLATFORM_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS
#else
    #error "No supported platform (OS) detected."
# endif

#if defined(__EMSCRIPTEN__)
    #define PLATFORM_WEB
#endif

/* compiler detection */
#if defined(__GNUC__)
    #define COMPILER_GCC
    #ifdef __clang__ // NOTE clang also defines __GNUC__ since it implements the gcc extensions
        #define COMPILER_CLANG
        #undef COMPILER_GCC
    #endif
    #ifdef __MINGW32__ // NOTE mingw also defines __GNUC__
        #define COMPILER_MINGW
        #undef COMPILER_GCC
    #endif

#elif defined(__clang__) // NOTE for clang++ on windows
    #define COMPILER_CLANG
#elif defined(__TINYC__)
    #define COMPILER_TCC
#elif defined(_MSC_VER)
    #define COMPILER_MSVC
#else
    #error "No supported compiler detected."
#endif

#endif // PLATFORM_H

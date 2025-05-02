cmake_minimum_required(VERSION 3.28)

project(ezwin
        VERSION 1.0.0
        LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


function(find_x11_link_dependencies)

    set(ALL_REQUIRED_LIBS
        "X11"
        "GL"
    )

    set(LIBS_PATHS_REQUIRED_FOR_X11_SUPPORT)

    foreach(LIB IN LISTS ALL_REQUIRED_LIBS)
        find_library(${LIB}_PATH ${LIB})
        if (${LIB}_PATH)
            message(STATUS "${${LIB}_PATH} found!")
            list(APPEND LIBS_PATHS_REQUIRED_FOR_X11_SUPPORT ${${LIB}_PATH})
        else()
            message(WARNING "${LIB} not found!")
            set(X11_LINK_DEPENDENCY_MISSING ON PARENT_SCOPE)
        endif()
    endforeach()

    set(LIBS_PATHS_REQUIRED_FOR_X11_SUPPORT
        ${LIBS_PATHS_REQUIRED_FOR_X11_SUPPORT}
        PARENT_SCOPE
    )

endfunction()


function(find_wayland_link_dependencies)

    set(ALL_REQUIRED_LIBS
        "wayland-client"
        "wayland-egl"
        "EGL"
        "rt"
        "xkbcommon"
        "m"
    )

    set(LIBS_PATHS_REQUIRED_FOR_WAYLAND_SUPPORT)

    foreach(LIB IN LISTS ALL_REQUIRED_LIBS)
        message(STATUS "list elements: ${LIB}")
        find_library(${LIB}_PATH ${LIB})
        if (${LIB}_PATH)
            message(STATUS "${${LIB}_PATH} found!")
            list(APPEND LIBS_PATHS_REQUIRED_FOR_WAYLAND_SUPPORT ${${LIB}_PATH})
        else()
            message(WARNING "${LIB} not found!")
            set(WAYLAND_LINK_DEPENDENCY_MISSING ON PARENT_SCOPE)
        endif()
    endforeach()

    set(LIBS_PATHS_REQUIRED_FOR_WAYLAND_SUPPORT
        ${LIBS_PATHS_REQUIRED_FOR_WAYLAND_SUPPORT}
        PARENT_SCOPE
    )

endfunction()

function(get_library_paths LIBS_PATHS LIST_OF_LIBRARY_NAMES)

    set(LIBS_PATHS)

    foreach(LIB IN LISTS LIST_OF_LIBRARY_NAMES)
        find_library(${LIB}_PATH ${LIB})
        if (${LIB}_PATH)
            message(STATUS "${${LIB}_PATH} found!")
            list(APPEND LIBS_PATHS ${${LIB}_PATH})
        else()
            message(WARNING "${LIB} not found!")
            set(LIBS_PATHS 0 PARENT_SCOPE)
            break()
        endif()
    endforeach()

    set(LIBS_PATHS
        ${LIBS_PATHS}
        PARENT_SCOPE
    )

endfunction()

function(find_wayland_protocols)
endfunction()

function(generate_wayland_protocol_code)
    find_program(HAS_wayland-scanner wayland-scanner)
    if (NOT HAS_wayland-scanner)
        message(FATAL_ERROR "wayland-scanner not found!")
    endif()
    # TODO: generate wayland protocol headers and source in here
endfunction()

option(EZWIN_BUILD_SHARED_LIBRARY "Build libezwin as shared library (.dll/.so)" ON)
option(EZWIN_BUILD_STATIC_LIBRARY "Build libezwin as static library (.lib/.a)" OFF)
if (EZWIN_BUILD_SHARED_LIBRARY AND EZWIN_BUILD_STATIC_LIBRARY)
    set(EZWIN_BUILD_SHARED_LIBRARY OFF)
    set(LIBRARY_BUILD_TYPE STATIC)
elseif(EZWIN_BUILD_SHARED_LIBRARY)
    set(LIBRARY_BUILD_TYPE SHARED)
elseif(EZWIN_BUILD_STATIC_LIBRARY)
    set(LIBRARY_BUILD_TYPE STATIC)
endif()

option(COMPILE_FOR_BUILD_HOST_ONLY "Compile for build host only" ON)
option(CROSS_COMPILE_FOR_WINDOWS "Cross compile for Windows platform" OFF)
option(CROSS_COMPILE_FOR_MACOS "Cross compile for MacOS platform" OFF)
option(CROSS_COMPILE_FOR_ANDROID "Cross compile for android platforms" OFF)
option(CROSS_COMPILE_FOR_ALL_PLATFORMS "Cross compile for all platforms" OFF)

option(SUPPORT_X11_ONLY "Build with X11 support only under Linux" OFF)
option(SUPPORT_WAYLAND_ONLY "Build with Wayland support only under Linux" OFF)
option(SUPPORT_BOTH_X11_AND_WAYLAND "Build with both X11 and Wayland support under Linux" ON)

# Setting things required to export to config.h
set(EZWIN_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(EZWIN_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(EZWIN_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Detect the platform
if(CMAKE_HOST_WIN32)
    set(EZWIN_PLATFORM_WINDOWS 1)
    set(BUILD_HOST "WINDOWS")
elseif(CMAKE_HOST_APPLE)
    set(BUILD_HOST "MACOS")
    set(EZWIN_PLATFORM_MACOS 1)
elseif(CMAKE_HOST_LINUX)
    set(EZWIN_PLATFORM_LINUX 1)
    set(BUILD_HOST "LINUX")
elseif(CMAKE_ANDROID_NDK_VERSION)
    set(EZWIN_PLATFORM_ANDROID 1)
    set(BUILD_HOST "ANDROID")
else()
    set(BUILD_HOST "UNKNOWN")
endif()
if(CMAKE_HOST_UNIX)
    set(EZWIN_PLATFORM_UNIX 1)
endif()

if (CROSS_COMPILE_FOR_WINDOWS)
    set(EZWIN_PLATFORM_WINDOWS 1)
endif()
if (CROSS_COMPILE_FOR_MACOS)
    set(EZWIN_PLATFORM_WINDOWS 1)
endif()
if (CROSS_COMPILE_FOR_ANDROID OR CMAKE_ANDROID_NDK_VERSION)
    set(EZWIN_PLATFORM_ANDROID 1)
endif()

cmake_host_system_information(RESULT PLATFORM_NAME_STRING QUERY OS_NAME)
cmake_host_system_information(RESULT DISTRO_NAME_STRING QUERY DISTRIB_PRETTY_NAME)
cmake_host_system_information(RESULT DISTRO_BASED_ON_STRING QUERY DISTRIB_ID_LIKE)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(EZWIN_COMPILER_GCC 1)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(EZWIN_COMPILER_CLANG 1)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(EZWIN_COMPILER_MSVC 1)
endif()

if(LIBRARY_BUILD_TYPE STREQUAL "SHARED")
    set(EZWIN_SHARED_BUILD 1)
    set(EZWIN_STATIC_BUILD 0)
elseif(LIBRARY_BUILD_TYPE STREQUAL "STATIC")
    set(EZWIN_SHARED_BUILD 0)
    set(EZWIN_STATIC_BUILD 1)
endif()

if(SUPPORT_X11_ONLY)
    set(EZWIN_SUPPORT_X11 1)
    set(EZWIN_SUPPORT_WAYLAND 0)
elseif(SUPPORT_WAYLAND_ONLY)
    set(EZWIN_SUPPORT_X11 1)
    set(EZWIN_SUPPORT_WAYLAND 0)
elseif(SUPPORT_BOTH_X11_AND_WAYLAND)
    set(EZWIN_SUPPORT_X11 1)
    set(EZWIN_SUPPORT_WAYLAND 1)
endif()

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/config.h.in
    ${PROJECT_SOURCE_DIR}/include/ezwin/config.h
)


file(GLOB_RECURSE EZWIN_LIBRARY_SOURCES ${PROJECT_SOURCE_DIR}/src/**/*.c)
add_library(${PROJECT_NAME} ${LIBRARY_BUILD_TYPE} ${EZWIN_LIBRARY_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")

if (COMPILE_FOR_BUILD_HOST_ONLY)

    if(BUILD_HOST STREQUAL "WINDOWS")
        message(STATUS "Building on Windows...")
        if (MSVC)
            set_target_properties(${PROJECT_NAME} PROPERTIES
                LINK_FLAGS "/SUBSYSTEM:WINDOWS"
            )
        endif() # MSVC
        target_link_libraries(${PROJECT_NAME} PRIVATE user32 gdi32 kernel32)

    elseif(BUILD_HOST STREQUAL "MACOS")
        message(STATUS "Building on MacOS...")
        target_link_libraries(${PROJECT_NAME} PRIVATE 
            "-framework Cocoa"
            "-framework CoreFoundation"
        )

    elseif(BUILD_HOST STREQUAL "LINUX")
        message(STATUS "Building on Linux...")

        if (SUPPORT_X11_ONLY)
            find_x11_link_dependencies()
            if (NOT X11_LINK_DEPENDENCY_MISSING)
                target_link_libraries(${PROJECT_NAME} PRIVATE LIBS_PATHS_REQUIRED_FOR_X11_SUPPORT)
            else()
                message(FATAL_ERROR "X11 link dependency missing!")
            endif()
        elseif (SUPPORT_WAYLAND_ONLY)
            find_wayland_link_dependencies()
            if (NOT WAYLAND_LINK_DEPENDENCY_MISSING)
                target_link_libraries(${PROJECT_NAME} PRIVATE LIBS_PATHS_REQUIRED_FOR_WAYLAND_SUPPORT)
            else()
                message(FATAL_ERROR "Wayland link dependency missing!")
            endif()
        elseif (SUPPORT_BOTH_X11_AND_WAYLAND)
            find_x11_link_dependencies()
            find_wayland_link_dependencies()
            if (NOT X11_LINK_DEPENDENCY_MISSING
                AND
                NOT WAYLAND_LINK_DEPENDENCY_MISSING
            )
                target_link_libraries(${PROJECT_NAME}
                                        PRIVATE
                                        LIBS_PATHS_REQUIRED_FOR_X11_SUPPORT
                                        LIBS_PATHS_REQUIRED_FOR_WAYLAND_SUPPORT
                )
            else()
                message(FATAL_ERROR "Link dependency missing!")
            endif()
        endif()

    endif() # BUILD_HOST

endif() # COMPILE_FOR_BUILD_HOST_ONLY



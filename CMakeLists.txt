include(CheckLibraryExists)

cmake_minimum_required(VERSION 3.14)

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
        message(STATUS "list elements: ${LIB}")
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
        message(STATUS "list elements: ${LIB}")
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

option(COMPILE_FOR_BUILD_HOST_ONLY "Compile for build host only" ON)
option(CROSS_COMPILE_FOR_WINDOWS "Cross compile for Windows platform" OFF)
option(CROSS_COMPILE_FOR_MACOS "Cross compile for MacOS platform" OFF)
option(CROSS_COMPILE_FOR_ANDROID "Cross compile for android platforms" OFF)
option(CROSS_COMPILE_FOR_ALL_PLATFORMS "Cross compile for all platforms" OFF)

option(SUPPORT_X11_ONLY "Build with X11 support only under Linux" OFF)
option(SUPPORT_WAYLAND_ONLY "Build with Wayland support only under Linux" OFF)
option(SUPPORT_BOTH_X11_AND_WAYLAND "Build with both X11 and Wayland support under Linux" ON)

# Detect the platform
if(WIN32)
    set(BUILD_HOST "WINDOWS")
elseif(APPLE)
    set(BUILD_HOST "MACOS")
elseif(UNIX)
    set(BUILD_HOST "LINUX")
else()
    set(BUILD_HOST "UNKNOWN")
endif()

file(GLOB_RECURSE EZWIN_LIBRARY_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
add_library(${PROJECT_NAME} STATIC EZWIN_LIBRARY_SOURCES)

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
        endif()

        if (SUPPORT_WAYLAND_ONLY)
            find_wayland_link_dependencies()
            if (NOT WAYLAND_LINK_DEPENDENCY_MISSING)
                target_link_libraries(${PROJECT_NAME} PRIVATE LIBS_PATHS_REQUIRED_FOR_WAYLAND_SUPPORT)
            else()
                message(FATAL_ERROR "Wayland link dependency missing!")
            endif()
        endif()

        if (SUPPORT_BOTH_X11_AND_WAYLAND)
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



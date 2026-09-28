# CMake build for the Aphelion styling C/C++ library.
#
# Builds a dependency-free shared (and optionally static) library from
# aphelion_styling.c that reproduces the aphelion_styling Python package's
# theme tokens and QSS generation. It links against nothing but the C
# runtime, so it can be embedded in any C or C++ host.
#
# Usage:
#   cmake -S native -B native/build
#   cmake --build native/build --config Release
#   cmake --install native/build --prefix <dest>
#
# Consumers can also skip CMake entirely and add aphelion_styling.c to their
# own build: it is a single, self-contained translation unit.

cmake_minimum_required(VERSION 3.20)
project(aphelion_styling LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(STYLING_BUILD_STATIC "Also build a static library" ON)

# Shared library ("C lib binary").
add_library(aphelion_styling SHARED aphelion_styling.c)
target_include_directories(aphelion_styling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)
target_compile_definitions(aphelion_styling PRIVATE STYLING_BUILD)
set_target_properties(aphelion_styling PROPERTIES
    OUTPUT_NAME "aphelion_styling"
    VERSION 0.1.0
    SOVERSION 0
    PUBLIC_HEADER "aphelion_styling.h"
)

if(STYLING_BUILD_STATIC)
    add_library(aphelion_styling_static STATIC aphelion_styling.c)
    target_include_directories(aphelion_styling_static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)
    target_compile_definitions(aphelion_styling_static PUBLIC STYLING_STATIC)
    # Keep the static target's distinct name: on Windows the shared target's
    # import library and a static library with the same OUTPUT_NAME collide.
    set_target_properties(aphelion_styling_static PROPERTIES
        PUBLIC_HEADER "aphelion_styling.h")
endif()

if(MSVC)
    target_compile_options(aphelion_styling PRIVATE /O2 /W3)
    target_compile_definitions(aphelion_styling PRIVATE _CRT_SECURE_NO_WARNINGS)
    if(STYLING_BUILD_STATIC)
        target_compile_options(aphelion_styling_static PRIVATE /O2 /W3)
        target_compile_definitions(aphelion_styling_static PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
else()
    target_compile_options(aphelion_styling PRIVATE
        -O2 -Wall -Wextra -Wno-unused-parameter)
    if(STYLING_BUILD_STATIC)
        target_compile_options(aphelion_styling_static PRIVATE
            -O2 -Wall -Wextra -Wno-unused-parameter)
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS aphelion_styling
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
if(STYLING_BUILD_STATIC)
    install(TARGETS aphelion_styling_static
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()

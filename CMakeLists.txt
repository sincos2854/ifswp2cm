# Copyright (c) 2023 - 2024 sincos2854
# Licensed under the MIT License

cmake_minimum_required(VERSION 3.22...3.29)

project(ifswp2cm LANGUAGES CXX VERSION 0.1.12)
set(CMAKE_CXX_STANDARD 20)

# Write ifswp2cm version number in version.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

set(CMAKE_SHARED_LIBRARY_PREFIX "") # Don't prefix file names with "lib"
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "Use /MT option instead of /MD option.")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Make libwebp2 a static build.")
set(WP2_BUILD_TESTS OFF CACHE BOOL "")
set(WP2_ENABLE_TESTS OFF CACHE BOOL "")
set(WP2_BUILD_EXAMPLES OFF CACHE BOOL "")
set(WP2_BUILD_EXTRAS OFF CACHE BOOL "")

set(LIBWEBP2_NAME "libwebp2")
set(LIBWEBP2_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/${LIBWEBP2_NAME})

message(STATUS "CMAKE_HOST_SYSTEM_NAME = ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "${LIBWEBP2_NAME} SOURCE DIR = ${LIBWEBP2_SRC_DIR}")

# Since Linux can't read "Windows.h", replace it with "windows.h".
if(NOT CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
  message(STATUS "Replacing Windows.h with windows.h")
  execute_process(
    COMMAND find ${LIBWEBP2_SRC_DIR} -name "*.h" -type f -exec sed -i "s/Windows\.h/windows.h/g" {} +
  )
endif()

# Add libwebp2 directory
add_subdirectory(${LIBWEBP2_SRC_DIR})

set(DLL_NAME "ifswp2cm")

# Creating ifswp2cm
add_library(${DLL_NAME} SHARED spi00in.cpp ifswp2cm.cpp spi00in.def)
target_include_directories(${DLL_NAME} PRIVATE ${LIBWEBP2_SRC_DIR}/src)
target_link_libraries(${DLL_NAME} webp2)

# .spi extension for 32bit version, .sph extension for 64bit version
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(STATUS "Build target is 32-bit.")
  set_target_properties(${DLL_NAME} PROPERTIES SUFFIX ".spi")
else()
  message(STATUS "Build target is 64-bit.")
  set_target_properties(${DLL_NAME} PROPERTIES SUFFIX ".sph")
endif()

target_compile_definitions(${DLL_NAME} PRIVATE _USRDLL UNICODE _UNICODE)

if(MSVC)
  target_compile_options(${DLL_NAME} PRIVATE /W3)
else()
  target_compile_options(${DLL_NAME} PRIVATE -Wall -Wextra)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(${DLL_NAME} PRIVATE -s)
  endif()
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(${DLL_NAME} PRIVATE -static)
  endif()
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(${DLL_NAME} PRIVATE -static-libstdc++)
  endif()
endif()

install(TARGETS ${DLL_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})

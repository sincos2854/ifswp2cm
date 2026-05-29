# Copyright (c) 2023 - 2026 sincos2854
# Licensed under the MIT License

cmake_minimum_required(VERSION 3.24...4.2)

project(ifswp2cm LANGUAGES C CXX VERSION 0.4.0)

# Write ifswp2cm version number in version.h.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)

# Don't prefix file names with "lib".
set(CMAKE_SHARED_LIBRARY_PREFIX "")

# Use /MT option instead of /MD.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Build libwebp2 as a static library.
set(BUILD_SHARED_LIBS OFF)

# libwebp2 options
option(WP2_BUILD_TESTS "" OFF)
option(WP2_ENABLE_TESTS "" OFF)
option(WP2_BUILD_EXAMPLES "" OFF)
option(WP2_BUILD_EXTRAS "" OFF)

# Link Time Optimization(LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output LANGUAGES C CXX)
if(result)
  message(STATUS "IPO/LTO is supported.")
  option(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE "" ON)
else()
  message(STATUS "IPO/LTO is not supported: ${output}")
endif()

set(LIBWEBP2_NAME "libwebp2")
set(LIBWEBP2_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LIBWEBP2_NAME}")

set(LIBWEBP2_GIT_URL "https://chromium.googlesource.com/codecs/libwebp2")
set(LIBWEBP2_GIT_TAG "d5920d8fe6abb5b3ba9b9bef3924f18433dd6d37")

# Fetch libwebp2.
if(NOT EXISTS "${LIBWEBP2_SRC_DIR}/CMakeLists.txt")
  find_package(Git REQUIRED)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} clone ${LIBWEBP2_GIT_URL} "${LIBWEBP2_SRC_DIR}"
    COMMAND_ERROR_IS_FATAL ANY
  )
  execute_process(
    COMMAND ${GIT_EXECUTABLE} -C "${LIBWEBP2_SRC_DIR}" checkout ${LIBWEBP2_GIT_TAG}
    COMMAND_ERROR_IS_FATAL ANY
  )
endif()

message(STATUS "CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "${LIBWEBP2_NAME} SOURCE DIR: ${LIBWEBP2_SRC_DIR}")

# Add the libwebp2 directory.
add_subdirectory(${LIBWEBP2_SRC_DIR} EXCLUDE_FROM_ALL)

set(DLL_NAME "ifswp2cm")

# Create ifswp2cm.
add_library(${DLL_NAME} SHARED spi00in.cpp ifswp2cm.cpp spi00in.def)
target_compile_features(${DLL_NAME} PRIVATE cxx_std_20)
target_include_directories(${DLL_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${DLL_NAME} SYSTEM PRIVATE ${LIBWEBP2_SRC_DIR}/src)
target_link_libraries(${DLL_NAME} PRIVATE webp2)

# .spi for the 32-bit version, .sph for the 64-bit version
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(STATUS "Build target: 32-bit")
  set_target_properties(${DLL_NAME} PROPERTIES SUFFIX ".spi")
else()
  message(STATUS "Build target: 64-bit")
  set_target_properties(${DLL_NAME} PROPERTIES SUFFIX ".sph")
endif()

target_compile_definitions(${DLL_NAME} PRIVATE NOMINMAX)

if(MSVC)
  target_compile_options(${DLL_NAME} PRIVATE /W4 /external:anglebrackets /external:W0)
else()
  target_compile_options(${DLL_NAME} PRIVATE -Wall -Wextra -Wno-multichar)
  target_link_options(${DLL_NAME} PRIVATE -static)
  target_link_options(${DLL_NAME} PRIVATE $<$<CONFIG:Release>:-s>)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(${DLL_NAME} PRIVATE -static-libstdc++)
  endif()
endif()

install(TARGETS ${DLL_NAME} RUNTIME DESTINATION .)

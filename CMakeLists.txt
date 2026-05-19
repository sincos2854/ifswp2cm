# Copyright (c) 2023 - 2026 sincos2854
# Licensed under the MIT License

cmake_minimum_required(VERSION 3.24...4.2)

project(ifswp2cm LANGUAGES C CXX VERSION 0.3.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Write ifswp2cm version number in version.h.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

# Don't prefix file names with "lib".
set(CMAKE_SHARED_LIBRARY_PREFIX "")

# Use /MT option instead of /MD.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Make libwebp2 a static build.
set(BUILD_SHARED_LIBS OFF)

# libwebp2 options
option(WP2_BUILD_TESTS "" OFF)
option(WP2_ENABLE_TESTS "" OFF)
option(WP2_BUILD_EXAMPLES "" OFF)
option(WP2_BUILD_EXTRAS "" OFF)

# Position-independent code(PIC)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Link Time Optimization(LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result)
  message(STATUS "IPO/LTO is supported.")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON CACHE BOOL "")
else()
  message(STATUS "IPO/LTO is not supported: ${output}")
endif()

set(LIBWEBP2_NAME "libwebp2")
set(LIBWEBP2_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/${LIBWEBP2_NAME})
if(NOT EXISTS ${LIBWEBP2_SRC_DIR}/CMakeLists.txt)
  message(FATAL_ERROR "libwebp2 submodule not found. Run: git submodule update --init --recursive")
endif()

message(STATUS "CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "${LIBWEBP2_NAME} SOURCE DIR: ${LIBWEBP2_SRC_DIR}")

# Since Linux can't read "Windows.h", replace it with "windows.h".
if(NOT CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
  message(STATUS "Replacing Windows.h with windows.h")
  execute_process(
    COMMAND find ${LIBWEBP2_SRC_DIR} -name "*.h" -type f -exec sed -i "s/Windows\.h/windows.h/g" {} +
  )
endif()

# Add libwebp2 directory.
add_subdirectory(${LIBWEBP2_SRC_DIR})

set(DLL_NAME "ifswp2cm")

# Create ifswp2cm.
add_library(${DLL_NAME} SHARED spi00in.cpp ifswp2cm.cpp spi00in.def)
target_include_directories(${DLL_NAME} SYSTEM PRIVATE ${LIBWEBP2_SRC_DIR}/src)
target_link_libraries(${DLL_NAME} PRIVATE webp2)

# .spi extension for 32bit version, .sph extension for 64bit version
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(STATUS "Build target is 32-bit.")
  set_target_properties(${DLL_NAME} PROPERTIES SUFFIX ".spi")
else()
  message(STATUS "Build target is 64-bit.")
  set_target_properties(${DLL_NAME} PROPERTIES SUFFIX ".sph")
endif()

if(MSVC)
  target_compile_options(${DLL_NAME} PRIVATE /W4 /external:anglebrackets /external:W0)
else()
  target_compile_options(${DLL_NAME} PRIVATE -Wall -Wextra -Wno-multichar)
  target_link_options(${DLL_NAME} PRIVATE $<$<CONFIG:Release>:-s>)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(${DLL_NAME} PRIVATE -static)
  endif()
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(${DLL_NAME} PRIVATE -static-libstdc++)
  endif()
endif()

install(TARGETS ${DLL_NAME} RUNTIME DESTINATION .)

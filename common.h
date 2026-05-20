// Copyright (c) 2023 - 2026 sincos2854
// Licensed under the MIT License

#pragma once

#include <windows.h>

constexpr int SPI_NO_FUNCTION       = -1;    // Function is not implemented
constexpr int SPI_ALL_RIGHT         =  0;    // Normal termination
constexpr int SPI_ABORT             =  1;    // Decoding was aborted because the callback function returned non-zero
constexpr int SPI_NOT_SUPPORT       =  2;    // Unknown format
constexpr int SPI_OUT_OF_ORDER      =  3;    // Data is corrupt
constexpr int SPI_NO_MEMORY         =  4;    // Cannot allocate memory
constexpr int SPI_MEMORY_ERROR      =  5;    // Memory error
constexpr int SPI_FILE_READ_ERROR   =  6;    // File read error
constexpr int SPI_WINDOW_ERROR      =  7;    // Cannot open window (Non-public error code)
constexpr int SPI_OTHER_ERROR       =  8;    // Internal error
constexpr int SPI_FILE_WRITE_ERROR  =  9;    // Write error (Non-public error code)
constexpr int SPI_END_OF_FILE       = 10;    // End of file (Non-public error code)

#pragma pack(push, 1)
struct PictureInfo
{
    long left, top;
    long width;
    long height;
    WORD x_density;
    WORD y_density;
    short colorDepth;
#ifdef _WIN64
    char dummy[2];
#endif
    HLOCAL hInfo;
};
#pragma pack(pop)

using SUSIE_PROGRESS = int(__stdcall*)(int, int, LONG_PTR);

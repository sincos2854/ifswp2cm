// Copyright (c) 2003 - 2024 sincos2854
// Licensed under the MIT License

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

struct FileHandleDeleter
{
    void operator()(HANDLE handle)
    {
        if (handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(handle);
        }
    }
};

using FileHandle = std::unique_ptr<std::remove_pointer<HANDLE>::type, FileHandleDeleter>;

struct PictureHandleDeleter
{
    void operator()(HANDLE handle)
    {
        LocalUnlock(handle);
        LocalFree(handle);
    }
};

using PictureHandle = std::unique_ptr<std::remove_pointer<HANDLE>::type, PictureHandleDeleter>;

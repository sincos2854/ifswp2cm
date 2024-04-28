// Copyright (c) 2003 - 2024 sincos2854
// Licensed under the MIT License

#pragma once

#include <windows.h>

struct FileHandleDeleter
{
    using pointer = HANDLE;
    void operator()(HANDLE handle)
    {
        if (handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(handle);
        }
    }
};

struct PictureHandleDeleter
{
    using pointer = HANDLE;
    void operator()(HANDLE handle)
    {
        LocalUnlock(handle);
        LocalFree(handle);
    }
};

// Copyright (c) 2023 - 2026 sincos2854
// Licensed under the MIT License

#pragma once

#include <windows.h>
#include <memory>

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

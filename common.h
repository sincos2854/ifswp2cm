// Copyright (c) 2023 - 2026 sincos2854
// Licensed under the MIT License

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
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

struct PictureHandleDeleter
{
    void operator()(HANDLE handle)
    {
        LocalFree(handle);
    }
};

using PictureHandle = std::unique_ptr<std::remove_pointer<HANDLE>::type, PictureHandleDeleter>;

class AutoUnlockBitmapHeader
{
public:
    AutoUnlockBitmapHeader(HANDLE handle) : handle_(handle), locked_header_(nullptr), locked_v5_(nullptr)
    {
        if (handle_ != nullptr)
        {
            locked_header_ = reinterpret_cast<LPBITMAPINFOHEADER>(LocalLock(handle_));
            if (locked_header_ != nullptr && locked_header_->biSize == sizeof(BITMAPV5HEADER))
            {
                locked_v5_ = reinterpret_cast<LPBITMAPV5HEADER>(locked_header_);
            }
        }
    }

    ~AutoUnlockBitmapHeader()
    {
        if (locked_header_ != nullptr)
        {
            LocalUnlock(handle_);
        }
    }

    bool MakeV5Header(void)
    {
        if (locked_v5_ != nullptr)
        {
            return true;
        }
        else if (locked_header_ != nullptr)
        {
            locked_v5_ = reinterpret_cast<LPBITMAPV5HEADER>(locked_header_);
            if (locked_v5_ != nullptr)
            {
                return true;
            }
        }

        return false;
    }

    LPBITMAPINFOHEADER GetBitmapHeader(void) const
    {
        return locked_header_;
    }

    LPBITMAPV5HEADER GetV5Header(void) const
    {
        return locked_v5_;
    }

private:
    HANDLE handle_;
    LPBITMAPINFOHEADER locked_header_;
    LPBITMAPV5HEADER locked_v5_;
};

class AutoUnlockBitmap
{
public:
    AutoUnlockBitmap(HANDLE handle) : handle_(handle), locked_bitmap_(nullptr)
    {
        if (handle_ != nullptr)
        {
            locked_bitmap_ = reinterpret_cast<BYTE*>(LocalLock(handle_));
        }
    }

    ~AutoUnlockBitmap()
    {
        if (locked_bitmap_ != nullptr)
        {
            LocalUnlock(handle_);
        }
    }

    BYTE* GetBitmap(void) const
    {
        return locked_bitmap_;
    }

private:
    HANDLE handle_;
    BYTE* locked_bitmap_;
};

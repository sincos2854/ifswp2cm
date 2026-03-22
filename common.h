// Copyright (c) 2023 - 2026 sincos2854
// Licensed under the MIT License

#pragma once

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
    void operator()(HLOCAL handle)
    {
        LocalFree(handle);
    }
};

using PictureHandle = std::unique_ptr<std::remove_pointer<HLOCAL>::type, PictureHandleDeleter>;

class AutoUnlockBitmapHeader
{
public:
    AutoUnlockBitmapHeader(HLOCAL handle) : handle_(handle)
    {
        if (handle_)
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
        if (locked_header_)
        {
            LocalUnlock(handle_);
        }
    }

    bool MakeV5Header(void)
    {
        if (locked_v5_)
        {
            return true;
        }
        else if (locked_header_)
        {
            locked_v5_ = reinterpret_cast<LPBITMAPV5HEADER>(locked_header_);
            if (locked_v5_)
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
    HLOCAL handle_ = nullptr;
    LPBITMAPINFOHEADER locked_header_ = nullptr;
    LPBITMAPV5HEADER locked_v5_ = nullptr;
};

class AutoUnlockBitmap
{
public:
    AutoUnlockBitmap(HLOCAL handle) : handle_(handle)
    {
        if (handle_)
        {
            locked_bitmap_ = reinterpret_cast<LPBYTE>(LocalLock(handle_));
        }
    }

    ~AutoUnlockBitmap()
    {
        if (locked_bitmap_)
        {
            LocalUnlock(handle_);
        }
    }

    LPBYTE GetBitmap(void) const
    {
        return locked_bitmap_;
    }

private:
    HLOCAL handle_ = nullptr;
    LPBYTE locked_bitmap_ = nullptr;
};

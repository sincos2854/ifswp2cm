// Copyright (c) 2023 - 2026 sincos2854
// Licensed under the MIT License

#pragma once

#include <windows.h>
#include <memory>

struct LocalMemHandleDeleter
{
    void operator()(HLOCAL handle)
    {
        LocalFree(handle);
    }
};

using LocalMemHandle = std::unique_ptr<std::remove_pointer<HLOCAL>::type, LocalMemHandleDeleter>;

class LockedBitmapHeader
{
public:
    LockedBitmapHeader(HLOCAL handle) : handle_(handle)
    {
        if (handle_)
        {
            locked_header_ = reinterpret_cast<LPBITMAPINFOHEADER>(LocalLock(handle_));
            if (locked_header_ && locked_header_->biSize == sizeof(BITMAPV5HEADER))
            {
                locked_v5_ = reinterpret_cast<LPBITMAPV5HEADER>(locked_header_);
            }
        }
    }

    ~LockedBitmapHeader()
    {
        if (locked_header_)
        {
            LocalUnlock(handle_);
        }
    }

    LockedBitmapHeader(const LockedBitmapHeader&) = delete;
    LockedBitmapHeader& operator=(const LockedBitmapHeader&) = delete;

    bool InitializeAsV5(void)
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

class LockedBitmap
{
public:
    LockedBitmap(HLOCAL handle) : handle_(handle)
    {
        if (handle_)
        {
            locked_bitmap_ = reinterpret_cast<LPBYTE>(LocalLock(handle_));
        }
    }

    ~LockedBitmap()
    {
        if (locked_bitmap_)
        {
            LocalUnlock(handle_);
        }
    }

    LockedBitmap(const LockedBitmap&) = delete;
    LockedBitmap& operator=(const LockedBitmap&) = delete;

    LPBYTE GetBitmap(void) const
    {
        return locked_bitmap_;
    }

private:
    HLOCAL handle_ = nullptr;
    LPBYTE locked_bitmap_ = nullptr;
};

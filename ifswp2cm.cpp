// Copyright (c) 2003 - 2025 sincos2854
// Licensed under the MIT License

#include <memory>
#include "common.h"
#include "ifswp2cm.h"
#include "wp2/decode.h"

bool IsSupportedEx(LPCWSTR filename, const LPBYTE data)
{
    if (!data)
    {
        return false;
    }

    // Check the file header
    if (memcmp(data, "\xF4\xFF\x6F", 3) == 0)
    {
        return true;
    }

#if 0
    if (!filename || !filename[0])
    {
        return false;
    }

    std::wstring_view name = filename;

    // Check the file extension
    for (const std::wstring_view& ext : extensions)
    {
        auto dot = name.find_last_of(L'.');
        if (dot != std::wstring_view::npos && _wcsicmp(name.data() + dot, ext.data()) == 0)
        {
            return true;
        }
    }
#endif

    return false;
}

int GetPictureInfoEx(LPCWSTR file_name, const LPBYTE data, size_t size, PictureInfo* lpInfo)
{
    if (!IsSupportedEx(file_name, data))
    {
        return SPI_NOT_SUPPORT;
    }

    WP2::BitstreamFeatures features{};

    if (features.Read(data, size) != WP2_STATUS_OK)
    {
        return SPI_OUT_OF_ORDER;
    }

    *lpInfo = {};
    lpInfo->width = features.raw_width;
    lpInfo->height = features.raw_height;
    lpInfo->colorDepth = 32;

    return SPI_ALL_RIGHT;
}

int GetPictureEx(LPCWSTR file_name, const LPBYTE data, size_t size, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData)
{
    if (!IsSupportedEx(file_name, data))
    {
        return SPI_NOT_SUPPORT;
    }

    if (lpPrgressCallback)
    {
        if (lpPrgressCallback(0, 100, lData))
        {
            return SPI_ABORT;
        }
    }

    PictureHandle h_bitmap_info;
    PictureHandle h_bitmap;
    LPBITMAPINFOHEADER bitmap_header = nullptr;
    LPBYTE bitmap = nullptr;

    WP2::BitstreamFeatures features{};
    WP2::DecoderConfig config;
    WP2::ArgbBuffer output_buffer;
    WP2::MemoryWriter writer;

    if (features.Read(data, size) != WP2_STATUS_OK)
    {
        return SPI_OUT_OF_ORDER;
    }

    LONG width = features.raw_width;
    LONG height = features.raw_height;
    DWORD stride = width * 4;
    size_t bitmap_size = static_cast<size_t>(height) * stride;

    // Get the ICC Profile
    bool got_icc_profile = false;
    if (features.has_icc)
    {
        if (WP2::GetChunk(data, size, WP2::ChunkType::kIcc, &writer) != WP2_STATUS_OK)
        {
            return SPI_OUT_OF_ORDER;
        }

        h_bitmap_info = PictureHandle(
            LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof(BITMAPV5HEADER) + writer.size_)
        );
        if (!h_bitmap_info)
        {
            return SPI_NO_MEMORY;
        }

        LPBITMAPV5HEADER v5 = reinterpret_cast<LPBITMAPV5HEADER>(LocalLock(h_bitmap_info.get()));
        if (!v5)
        {
            return SPI_MEMORY_ERROR;
        }

        v5->bV5Size = sizeof(BITMAPV5HEADER);
        v5->bV5CSType = PROFILE_EMBEDDED;
        v5->bV5ProfileData = sizeof(BITMAPV5HEADER);
        v5->bV5ProfileSize = static_cast<DWORD>(writer.size_);
        memcpy(reinterpret_cast<LPBYTE>(v5) + v5->bV5ProfileData, writer.mem_, v5->bV5ProfileSize);
        got_icc_profile = true;

        LocalUnlock(h_bitmap_info.get());
    }

    if (!got_icc_profile)
    {
        h_bitmap_info = PictureHandle(
            LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof(BITMAPINFO))
        );
        if (!h_bitmap_info)
        {
            return SPI_NO_MEMORY;
        }
    }

    bitmap_header = reinterpret_cast<LPBITMAPINFOHEADER>(LocalLock(h_bitmap_info.get()));
    if (!bitmap_header)
    {
        return SPI_MEMORY_ERROR;
    }

    if (!got_icc_profile)
    {
        bitmap_header->biSize = sizeof(BITMAPINFOHEADER);
    }
    bitmap_header->biWidth = width;
    bitmap_header->biHeight = height;
    bitmap_header->biPlanes = 1;
    bitmap_header->biBitCount = 32;
    bitmap_header->biSizeImage = static_cast<DWORD>(bitmap_size);

    // The number of logical processors in the current group.
    SYSTEM_INFO info{};
    GetSystemInfo(&info);

    config.thread_level = info.dwNumberOfProcessors - 1;

    h_bitmap = PictureHandle(
        LocalAlloc(LMEM_MOVEABLE, bitmap_size)
    );
    if (!h_bitmap)
    {
        return SPI_NO_MEMORY;
    }
    bitmap = reinterpret_cast<LPBYTE>(LocalLock(h_bitmap.get()));
    if(!bitmap)
    {
        return SPI_MEMORY_ERROR;
    }

    if (output_buffer.SetFormat(WP2_BGRA_32) != WP2_STATUS_OK)
    {
        return SPI_OTHER_ERROR;
    }
    if (output_buffer.SetExternal(width, height, bitmap, stride) != WP2_STATUS_OK)
    {
        return SPI_OTHER_ERROR;
    }
    if (WP2::Decode(data, size, &output_buffer, config) != WP2_STATUS_OK)
    {
        return SPI_OUT_OF_ORDER;
    }

    // Flip the bitmap
    size_t half_height = static_cast<size_t>(height) / 2;
    auto line = std::make_unique<BYTE[]>(stride);
    for (size_t i = 0; i < half_height; i++)
    {
        memcpy(line.get(), bitmap + stride * i, stride);
        memcpy(bitmap + stride * i, bitmap + stride * (height - i - 1), stride);
        memcpy(bitmap + stride * (height - i - 1), line.get(), stride);
    }

    if (lpPrgressCallback)
    {
        if (lpPrgressCallback(100, 100, lData))
        {
            return SPI_ABORT;
        }
    }

    LocalUnlock(h_bitmap.get());
    LocalUnlock(h_bitmap_info.get());

    *pHBInfo = h_bitmap_info.get();
    *pHBm = h_bitmap.get();

    h_bitmap_info.release();
    h_bitmap.release();

    return SPI_ALL_RIGHT;
}

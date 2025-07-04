// Copyright (c) 2023 - 2025 sincos2854
// Licensed under the MIT License

#include "common.h"
#include "ifswp2cm.h"
#include "wp2/decode.h"

bool IsSupportedEx(LPCWSTR filename, const BYTE* data)
{
    if (!data)
    {
        return false;
    }

    // Check the file header
    if (std::memcmp(data, "\xF4\xFF\x6F", 3) == 0)
    {
        return true;
    }

    return false;
}

int GetPictureInfoEx(LPCWSTR file_name, const BYTE* data, size_t size, PictureInfo* lpInfo)
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

int GetPictureEx(LPCWSTR file_name, const BYTE* data, size_t size, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData)
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
    if (features.has_icc)
    {
        if (WP2::GetChunk(data, size, WP2::ChunkType::kIcc, &writer) != WP2_STATUS_OK)
        {
            return SPI_OUT_OF_ORDER;
        }

        h_bitmap_info = PictureHandle(LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof(BITMAPV5HEADER) + writer.size_));

        if (!h_bitmap_info)
        {
            return SPI_NO_MEMORY;
        }

        auto auto_unlock_header = std::make_unique<AutoUnlockBitmapHeader>(h_bitmap_info.get());

        if (!auto_unlock_header->MakeV5Header())
        {
            return SPI_MEMORY_ERROR;
        }

        auto v5 = auto_unlock_header->GetV5Header();

        v5->bV5Size = sizeof(BITMAPV5HEADER);
        v5->bV5CSType = PROFILE_EMBEDDED;
        v5->bV5ProfileData = sizeof(BITMAPV5HEADER);
        v5->bV5ProfileSize = static_cast<DWORD>(writer.size_);

        std::memcpy(reinterpret_cast<BYTE*>(v5) + v5->bV5ProfileData, writer.mem_, v5->bV5ProfileSize);
    }

    if (!h_bitmap_info)
    {
        h_bitmap_info = PictureHandle(LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof(BITMAPINFO)));

        if (!h_bitmap_info)
        {
            return SPI_NO_MEMORY;
        }
    }

    auto auto_unlock_header = std::make_unique<AutoUnlockBitmapHeader>(h_bitmap_info.get());
    auto bitmap_header = auto_unlock_header->GetBitmapHeader();

    if (!bitmap_header)
    {
        return SPI_MEMORY_ERROR;
    }

    if (!auto_unlock_header->GetV5Header())
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

    h_bitmap = PictureHandle(LocalAlloc(LMEM_MOVEABLE, bitmap_size));

    if (!h_bitmap)
    {
        return SPI_NO_MEMORY;
    }
    
    auto auto_unlock_bitmap = std::make_unique<AutoUnlockBitmap>(h_bitmap.get());
    auto bitmap = auto_unlock_bitmap->GetBitmap();

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
    auto line = std::make_unique_for_overwrite<BYTE[]>(stride);
    for (size_t i = 0; i < half_height; i++)
    {
        std::memcpy(line.get(), bitmap + stride * i, stride);
        std::memcpy(bitmap + stride * i, bitmap + stride * (height - i - 1), stride);
        std::memcpy(bitmap + stride * (height - i - 1), line.get(), stride);
    }

    if (lpPrgressCallback)
    {
        if (lpPrgressCallback(100, 100, lData))
        {
            return SPI_ABORT;
        }
    }

    auto_unlock_header.reset();
    auto_unlock_bitmap.reset();

    *pHBInfo = h_bitmap_info.get();
    *pHBm = h_bitmap.get();

    h_bitmap_info.release();
    h_bitmap.release();

    return SPI_ALL_RIGHT;
}

// Copyright (c) 2023 sincos2854
// Licensed under the MIT License

#include <memory>
#include "ifswp2cm.h"

EXTERN_C int __stdcall GetPluginInfo(int infono, LPSTR buf, int buflen)
{
    wchar_t unicode[BUF_SIZE_S];

    if (GetPluginInfoW(infono, unicode, BUF_SIZE_S) == 0)
    {
        return 0;
    }

    return UnicodeToAnsi(unicode, buf, buflen);
}

EXTERN_C int __stdcall GetPluginInfoW(int infono, LPWSTR buf, int buflen)
{
    if (buflen <= 1 || infono < 0 || (sizeof(plugin_info) / sizeof(plugin_info[0])) <= (size_t)infono)
    {
        return 0;
    }

    auto str = plugin_info[infono];
    int len = (int)wcslen(str) + 1;
    if (buflen < len)
    {
        len = buflen;
    }
    wcsncpy_s(buf, len, str, _TRUNCATE);

    return (int)wcslen(buf);
}

bool IsSupportedEx(LPCWSTR filename, const LPBYTE data)
{
    if (!data)
    {
        return false;
    }

    if (memcmp(data, "\xF4\xFF\x6F", 3) == 0)
    {
        return true;
    }

    if (!filename || !filename[0])
    {
        return false;
    }

    size_t len = wcslen(filename);
    if ( 4 < len &&
        filename[len - 4] == L'.' &&
        filename[len - 3] == L'w' &&
        filename[len - 2] == L'p' &&
        filename[len - 1] == L'2')
    {
        return true;
    }

    return false;
}

EXTERN_C int __stdcall IsSupported(LPCSTR filename, DWORD_PTR dw)
{
    std::wstring unicode;
    AnsiToUnicode(filename, unicode);
    return IsSupportedW(unicode.c_str(), dw);
}

EXTERN_C int __stdcall IsSupportedW(LPCWSTR filename, DWORD_PTR dw)
{
    std::unique_ptr<BYTE[]> buf;
    LPBYTE data = nullptr;
    DWORD read_size = 0;

    if ((dw & 0xFFFF0000) == 0)
    {
        buf = std::make_unique<BYTE[]>(BUF_SIZE_HEADER);
        if (!ReadFile((HANDLE)dw, buf.get(), BUF_SIZE_HEADER, &read_size, NULL))
        {
            return 0;
        }
        if (read_size <= WP2_HEADER_MIN_SIZE)
        {
            return 0;
        }
        data = buf.get();
    }
    else
    {
        data = (LPBYTE)dw;
    }

    if (IsSupportedEx(filename, data))
    {
        return 1;
    }

    return 0;
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

int ReadDataFromFile(LPCWSTR file_name, LONG_PTR macbin_offset, std::unique_ptr<BYTE[]>& file_data, size_t& file_size)
{
    auto handle = std::unique_ptr<HANDLE, FileHandleDeleter>(
        CreateFileW(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL), FileHandleDeleter()
    );
    if (handle.get() == INVALID_HANDLE_VALUE)
    {
        return SPI_FILE_READ_ERROR;
    }

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(handle.get(), &size))
    {
        return SPI_FILE_READ_ERROR;
    }

#ifndef _WIN64
    if (size.HighPart)
    {
        return SPI_FILE_READ_ERROR;
    }
#endif

    file_size = (size_t)size.QuadPart - macbin_offset;
    if (file_size <= WP2_HEADER_MIN_SIZE)
    {
        return SPI_NOT_SUPPORT;
    }

    if (SetFilePointer(handle.get(), (LONG)macbin_offset, NULL, FILE_BEGIN) != (DWORD)macbin_offset)
    {
        return SPI_FILE_READ_ERROR;
    }

    file_data = std::make_unique<BYTE[]>(file_size);

    size_t remain = file_size, offset = 0;
    DWORD to_read = 0, read = 0;
    BOOL ret;

    while (0 < remain)
    {
        to_read = (ULONG_MAX < remain) ? ULONG_MAX : (DWORD)remain;

        ret = ReadFile(handle.get(), file_data.get() + offset, to_read, &read, NULL);
        if (!ret || to_read != read)
        {
            return SPI_FILE_READ_ERROR;
        }

        remain -= read;
        offset += read;
    }

    return SPI_ALL_RIGHT;
}

EXTERN_C int __stdcall GetPictureInfo(LPCSTR buf, LONG_PTR len, unsigned int flag, PictureInfo* lpInfo)
{
    std::wstring unicode;
    if ((flag & 7) == 0)
    {
        AnsiToUnicode(buf, unicode);
    }
    return GetPictureInfoW(unicode.c_str(), len, flag, lpInfo);
}

EXTERN_C int __stdcall GetPictureInfoW(LPCWSTR buf, LONG_PTR len, unsigned int flag, PictureInfo* lpInfo)
{
    if (!lpInfo)
    {
        return SPI_OTHER_ERROR;
    }

    if ((flag & 7) == 0)
    {
        std::unique_ptr<BYTE[]> file_data;
        size_t file_size = 0;

        int ret = ReadDataFromFile(buf, len, file_data, file_size);
        if (ret)
        {
            return ret;
        }

        return GetPictureInfoEx(buf, file_data.get(), file_size, lpInfo);
    }

    return GetPictureInfoEx(NULL, (LPBYTE)buf, len, lpInfo);
}

int GetPictureEx(LPCWSTR file_name, const LPBYTE data, size_t size, HANDLE* pHBInfo, HANDLE* pHBm, SPI_PROGRESS lpPrgressCallback, LONG_PTR lData)
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

    std::unique_ptr<HANDLE, PictureHandleDeleter> h_bitmap_info;
    std::unique_ptr<HANDLE, PictureHandleDeleter> h_bitmap;
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
    size_t bitmap_size = (size_t)height * stride;

    // The number of logical processors in the current group.
    SYSTEM_INFO info{};
    GetSystemInfo(&info);

    config.thread_level = info.dwNumberOfProcessors - 1;
    config.exact = true;

    h_bitmap = std::unique_ptr<HANDLE, PictureHandleDeleter>(
        LocalAlloc(LMEM_MOVEABLE, bitmap_size), PictureHandleDeleter()
    );
    if (!h_bitmap)
    {
        return SPI_NO_MEMORY;
    }
    bitmap = (LPBYTE)LocalLock(h_bitmap.get());
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
    auto line = std::make_unique<BYTE[]>(stride);
    for (size_t i = 0; i < (size_t)height / 2; i++)
    {
        memcpy(line.get(), bitmap + stride * i, stride);
        memcpy(bitmap + stride * i, bitmap + stride * (height - i - 1), stride);
        memcpy(bitmap + stride * (height - i - 1), line.get(), stride);
    }

    // Get the ICC Profile
    bool got_icc_profile = false;
    if (features.has_icc)
    {
        if (WP2::GetChunk(data, size, WP2::ChunkType::kIcc, &writer) != WP2_STATUS_OK)
        {
            return SPI_OUT_OF_ORDER;
        }

        h_bitmap_info = std::unique_ptr<HANDLE, PictureHandleDeleter>(
            LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof(BITMAPV5HEADER) + writer.size_), PictureHandleDeleter()
        );
        if (!h_bitmap_info)
        {
            return SPI_NO_MEMORY;
        }

        LPBITMAPV5HEADER v5 = (LPBITMAPV5HEADER)LocalLock(h_bitmap_info.get());
        if (!v5)
        {
            return SPI_MEMORY_ERROR;
        }

        v5->bV5Size = sizeof(BITMAPV5HEADER);
        v5->bV5CSType = PROFILE_EMBEDDED;
        v5->bV5ProfileData = sizeof(BITMAPV5HEADER);
        v5->bV5ProfileSize = (DWORD)writer.size_;
        memcpy((LPBYTE)v5 + v5->bV5ProfileData, writer.mem_, v5->bV5ProfileSize);
        got_icc_profile = true;

        LocalUnlock(h_bitmap_info.get());
    }

    if (!got_icc_profile)
    {
        h_bitmap_info = std::unique_ptr<HANDLE, PictureHandleDeleter>(
            LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, sizeof(BITMAPINFO)), PictureHandleDeleter()
        );
        if (!h_bitmap_info)
        {
            return SPI_NO_MEMORY;
        }
    }

    bitmap_header = (LPBITMAPINFOHEADER)LocalLock(h_bitmap_info.get());
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
    bitmap_header->biSizeImage = (DWORD)bitmap_size;

    LocalUnlock(h_bitmap.get());
    LocalUnlock(h_bitmap_info.get());

    *pHBInfo = h_bitmap_info.get();
    *pHBm = h_bitmap.get();

    h_bitmap_info.release();
    h_bitmap.release();

    if (lpPrgressCallback)
    {
        if (lpPrgressCallback(100, 100, lData))
        {
            return SPI_ABORT;
        }
    }

    return SPI_ALL_RIGHT;
}

EXTERN_C int __stdcall GetPicture(LPCSTR buf, LONG_PTR len, unsigned int flag, HANDLE* pHBInfo, HANDLE* pHBm, SPI_PROGRESS lpPrgressCallback, LONG_PTR lData)
{
    std::wstring unicode;
    if ((flag & 7) == 0)
    {
        AnsiToUnicode(buf, unicode);
    }
    return GetPictureW(unicode.c_str(), len, flag, pHBInfo, pHBm, lpPrgressCallback, lData);
}

EXTERN_C int __stdcall GetPictureW(LPCWSTR buf, LONG_PTR len, unsigned int flag, HANDLE* pHBInfo, HANDLE* pHBm, SPI_PROGRESS lpPrgressCallback, LONG_PTR lData)
{
    if (!pHBInfo || !pHBm)
    {
        return SPI_OTHER_ERROR;
    }

    if ((flag & 7) == 0)
    {
        std::unique_ptr<BYTE[]> file_data;
        size_t file_size = 0;

        int ret = ReadDataFromFile(buf, len, file_data, file_size);
        if (ret)
        {
            return ret;
        }

        return GetPictureEx(buf, file_data.get(), file_size, pHBInfo, pHBm, lpPrgressCallback, lData);
    }

    return GetPictureEx(NULL, (LPBYTE)buf, len, pHBInfo, pHBm, lpPrgressCallback, lData);
}

EXTERN_C int __stdcall GetPreview(LPCSTR buf, LONG_PTR len, unsigned int flag, HANDLE* pHBInfo, HANDLE* pHBm, SPI_PROGRESS lpPrgressCallback, LONG_PTR lData)
{
    return GetPicture(buf, len, flag, pHBInfo, pHBm, lpPrgressCallback, lData);
}

EXTERN_C int __stdcall GetPreviewW(LPCWSTR buf, LONG_PTR len, unsigned int flag, HANDLE* pHBInfo, HANDLE* pHBm, SPI_PROGRESS lpPrgressCallback, LONG_PTR lData)
{
    return GetPictureW(buf, len, flag, pHBInfo, pHBm, lpPrgressCallback, lData);
}
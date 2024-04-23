// Copyright (c) 2003 - 2024 sincos2854
// Licensed under the MIT License

#include <memory>
#include <string>
#include "common.h"
#include "ifswp2cm.h"

#define BUF_SIZE_HEADER 2048

int UnicodeToAnsi(LPCWSTR unicode, LPSTR ansi, int size)
{
    if (!unicode || !unicode[0])
    {
        return 0;
    }

    int len = WideCharToMultiByte(CP_ACP, 0, unicode, -1, NULL, 0, NULL, NULL);
    if (len == 0)
    {
        return 0;
    }

    // When the buffer size is insufficient, WideCharToMultiByte returns 0, so it is stored once in std::string.
    std::string buf(static_cast<size_t>(len) - 1, '\0');
    len = WideCharToMultiByte(CP_ACP, 0, unicode, -1, buf.data(), len, NULL, NULL);
    if (len == 0)
    {
        return 0;
    }
    else if (size < len)
    {
        len = size;
    }

    strncpy_s(ansi, len, buf.c_str(), _TRUNCATE);

    return static_cast<int>(strlen(ansi));
}

int AnsiToUnicode(LPCSTR ansi, std::wstring& unicode)
{
    unicode.clear();

    if (!ansi || !ansi[0])
    {
        return 0;
    }

    int len = MultiByteToWideChar(CP_ACP, 0, ansi, -1, NULL, 0);
    if (len == 0)
    {
        return 0;
    }

    unicode.resize(static_cast<size_t>(len) - 1);
    len = MultiByteToWideChar(CP_ACP, 0, ansi, -1, unicode.data(), len);
    if (len == 0)
    {
        unicode.clear();
        return 0;
    }

    return static_cast<int>(unicode.size());
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

    file_size = static_cast<size_t>(size.QuadPart) - macbin_offset;
    if (file_size < HEADER_MIN_SIZE)
    {
        return SPI_NOT_SUPPORT;
    }

    if (SetFilePointer(handle.get(), static_cast<LONG>(macbin_offset), NULL, FILE_BEGIN) != static_cast<DWORD>(macbin_offset))
    {
        return SPI_FILE_READ_ERROR;
    }

    file_data = std::make_unique<BYTE[]>(file_size);

    size_t remain = file_size, offset = 0;
    DWORD to_read = 0, read = 0;
    BOOL ret;

    while (0 < remain)
    {
        to_read = (ULONG_MAX < remain) ? ULONG_MAX : static_cast<DWORD>(remain);

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

int __stdcall GetPluginInfo(int infono, LPSTR buf, int buflen)
{
    wchar_t unicode[MAX_PATH];

    if (GetPluginInfoW(infono, unicode, MAX_PATH) == 0)
    {
        return 0;
    }

    return UnicodeToAnsi(unicode, buf, buflen);
}

int __stdcall GetPluginInfoW(int infono, LPWSTR buf, int buflen)
{
    if (buflen <= 1 || infono < 0 || (sizeof(plugin_info) / sizeof(plugin_info[0])) <= static_cast<size_t>(infono))
    {
        return 0;
    }

    auto str = plugin_info[infono];
    int len = static_cast<int>(wcslen(str)) + 1;
    if (buflen < len)
    {
        len = buflen;
    }
    wcsncpy_s(buf, len, str, _TRUNCATE);

    return static_cast<int>(wcslen(buf));
}

int __stdcall IsSupported(LPCSTR filename, DWORD_PTR dw)
{
    std::wstring unicode;
    AnsiToUnicode(filename, unicode);
    return IsSupportedW(unicode.c_str(), dw);
}

int __stdcall IsSupportedW(LPCWSTR filename, DWORD_PTR dw)
{
    std::unique_ptr<BYTE[]> buf;
    LPBYTE data = nullptr;
    DWORD read_size = 0;

    if ((dw & 0xFFFF0000) == 0)
    {
        buf = std::make_unique<BYTE[]>(BUF_SIZE_HEADER);
        if (!ReadFile(reinterpret_cast<HANDLE>(dw), buf.get(), BUF_SIZE_HEADER, &read_size, NULL))
        {
            return 0;
        }
        if (read_size < HEADER_MIN_SIZE)
        {
            return 0;
        }
        data = buf.get();
    }
    else
    {
        data = reinterpret_cast<LPBYTE>(dw);
    }

    if (IsSupportedEx(filename, data))
    {
        return 1;
    }

    return 0;
}

int __stdcall GetPictureInfo(LPCSTR buf, LONG_PTR len, UINT flag, PictureInfo* lpInfo)
{
    std::wstring unicode;
    if ((flag & 7) == 0)
    {
        AnsiToUnicode(buf, unicode);
    }
    return GetPictureInfoW(unicode.c_str(), len, flag, lpInfo);
}

int __stdcall GetPictureInfoW(LPCWSTR buf, LONG_PTR len, UINT flag, PictureInfo* lpInfo)
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

    return GetPictureInfoEx(NULL, reinterpret_cast<LPBYTE>(const_cast<LPWSTR>(buf)), len, lpInfo);
}

int __stdcall GetPicture(LPCSTR buf, LONG_PTR len, UINT flag, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData)
{
    std::wstring unicode;
    if ((flag & 7) == 0)
    {
        AnsiToUnicode(buf, unicode);
    }
    return GetPictureW(unicode.c_str(), len, flag, pHBInfo, pHBm, lpPrgressCallback, lData);
}

int __stdcall GetPictureW(LPCWSTR buf, LONG_PTR len, UINT flag, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData)
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

    return GetPictureEx(NULL, reinterpret_cast<LPBYTE>(const_cast<LPWSTR>(buf)), len, pHBInfo, pHBm, lpPrgressCallback, lData);
}

int __stdcall GetPreview(LPCSTR buf, LONG_PTR len, UINT flag, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData)
{
    return GetPicture(buf, len, flag, pHBInfo, pHBm, lpPrgressCallback, lData);
}

int __stdcall GetPreviewW(LPCWSTR buf, LONG_PTR len, UINT flag, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData)
{
    return GetPictureW(buf, len, flag, pHBInfo, pHBm, lpPrgressCallback, lData);
}
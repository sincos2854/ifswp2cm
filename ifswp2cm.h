// Copyright (c) 2023 sincos2854
// Licensed under the MIT License

#pragma once

#include <string>
#include "config.h"

#define COPYRIGHT L"WebP 2 Susie Plug-in Ver." PROJECT_VERSION L" (c) 2023 sincos2854"

static const wchar_t* plugin_info[4] = {
    L"00IN",
    COPYRIGHT,
    L"*.wp2",
    L"WebP 2 file(*.wp2)",
};

#define WP2_HEADER_MIN_SIZE 3
#define BUF_SIZE_S 256
#define BUF_SIZE_HEADER 2048

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
    std::string buf((size_t)len - 1, '\0');
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

    return (int)strlen(ansi);
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

    unicode.resize((size_t)len - 1);
    len = MultiByteToWideChar(CP_ACP, 0, ansi, -1, unicode.data(), len);
    if (len == 0)
    {
        unicode.clear();
        return 0;
    }

    return (int)unicode.size();
}
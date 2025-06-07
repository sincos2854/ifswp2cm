// Copyright (c) 2003 - 2025 sincos2854
// Licensed under the MIT License

#pragma once

#include <string>
#include "spi00in.h"
#include "version.h"

#define COPYRIGHT L"WebP 2 Susie Plug-in Ver." PROJECT_VERSION L" (c) 2003 - 2025 sincos2854"

#define EXTENSION1 L".wp2"

#define PLUGIN_INFO3 L"*" EXTENSION1
#define PLOGIN_INFO4 L"WebP 2 file(*" EXTENSION1 L")"

static const wchar_t* plugin_info[]{
    L"00IN",
    COPYRIGHT,
    PLUGIN_INFO3,
    PLOGIN_INFO4,
};

constexpr size_t HEADER_MIN_SIZE = 3;

constexpr static std::wstring_view extensions[]{
    EXTENSION1
};

bool IsSupportedEx(LPCWSTR filename, const BYTE* data);
int GetPictureInfoEx(LPCWSTR file_name, const BYTE* data, size_t size, PictureInfo* lpInfo);
int GetPictureEx(LPCWSTR file_name, const BYTE* data, size_t size, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData);

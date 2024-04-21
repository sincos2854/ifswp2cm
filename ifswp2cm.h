// Copyright (c) 2003 - 2024 sincos2854
// Licensed under the MIT License

#pragma once

#include "spi00in.h"
#include "version.h"

#define COPYRIGHT L"WebP 2 Susie Plug-in Ver." PROJECT_VERSION L" (c) 2003 - 2024 sincos2854"

static const wchar_t* plugin_info[4] = {
    L"00IN",
    COPYRIGHT,
    L"*.wp2",
    L"WebP 2 file(*.wp2)",
};

#define HEADER_MIN_SIZE 3

bool IsSupportedEx(LPCWSTR filename, const LPBYTE data);
int GetPictureInfoEx(LPCWSTR file_name, const LPBYTE data, size_t size, PictureInfo* lpInfo);
int GetPictureEx(LPCWSTR file_name, const LPBYTE data, size_t size, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData);

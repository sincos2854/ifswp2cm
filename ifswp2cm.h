// Copyright (c) 2023 - 2025 sincos2854
// Licensed under the MIT License

#pragma once

#include <string>
#include "spi00in.h"
#include "version.h"

constexpr LPCWSTR API_VER = L"00IN";
constexpr LPCWSTR EXTENSION = L"*.wp2";
constexpr LPCWSTR FILE_TYPE = L"WebP 2 file(*.wp2)";

constexpr std::wstring_view PLUGIN_INFO[]{
    API_VER,
    PLUGIN_NAME,
    EXTENSION,
    FILE_TYPE
};

constexpr size_t HEADER_MIN_SIZE = 3;

bool IsSupportedEx(LPCWSTR filename, const BYTE* data);
int GetPictureInfoEx(LPCWSTR file_name, const BYTE* data, size_t size, PictureInfo* lpInfo);
int GetPictureEx(LPCWSTR file_name, const BYTE* data, size_t size, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData);

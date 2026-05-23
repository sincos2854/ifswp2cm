// Copyright (c) 2023 - 2026 sincos2854
// Licensed under the MIT License

#pragma once

#include "common.h"
#include "version.h"
#include <string>

inline constexpr LPCWSTR API_VER = L"00IN";
inline constexpr LPCWSTR EXTENSION = L"*.wp2";
inline constexpr LPCWSTR FILE_TYPE = L"WebP 2 file(*.wp2)";

inline constexpr std::wstring_view PLUGIN_INFO[]{
    API_VER,
    PLUGIN_NAME,
    EXTENSION,
    FILE_TYPE
};

inline constexpr size_t SIGN_OFFSET = 0;
inline constexpr char SIGN_BYTES[]{ '\xF4', '\xFF', '\x6F' };
inline constexpr auto SIGN_SIZE = std::size(SIGN_BYTES);
inline constexpr auto MIN_HEADER_SIZE = SIGN_OFFSET + SIGN_SIZE;

bool IsSupportedEx(LPCWSTR fil_ename, LPCBYTE file_data);
int GetPictureInfoEx(LPCWSTR file_name, LPCBYTE file_data, size_t file_size, PictureInfo* lp_info);
int GetPictureEx(LPCWSTR file_name, LPCBYTE file_data, size_t file_size, HLOCAL* out_bitmap_info, HLOCAL* out_bitmap, SUSIE_PROGRESS lp_callback, LONG_PTR lp_data);

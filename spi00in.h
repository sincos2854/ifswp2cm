// Copyright (c) 2023 - 2026 sincos2854
// Licensed under the MIT License

#pragma once

#include "common.h"

EXTERN_C int __stdcall GetPluginInfo(int infono, LPSTR buf, int buflen);
EXTERN_C int __stdcall GetPluginInfoW(int infono, LPWSTR buf, int buflen);
EXTERN_C int __stdcall IsSupported(LPCSTR filename, LPCVOID dw);
EXTERN_C int __stdcall IsSupportedW(LPCWSTR filename, LPCVOID dw);
EXTERN_C int __stdcall GetPictureInfo(LPCSTR buf, LONG_PTR len, UINT flag, PictureInfo* lpInfo);
EXTERN_C int __stdcall GetPictureInfoW(LPCWSTR buf, LONG_PTR len, UINT flag, PictureInfo* lpInfo);
EXTERN_C int __stdcall GetPicture(LPCSTR buf, LONG_PTR len, UINT flag, HLOCAL* pHBInfo, HLOCAL* pHBm, SUSIE_PROGRESS lpProgressCallback, LONG_PTR lData);
EXTERN_C int __stdcall GetPictureW(LPCWSTR buf, LONG_PTR len, UINT flag, HLOCAL* pHBInfo, HLOCAL* pHBm, SUSIE_PROGRESS lpProgressCallback, LONG_PTR lData);
EXTERN_C int __stdcall GetPreview(LPCSTR buf, LONG_PTR len, UINT flag, HLOCAL* pHBInfo, HLOCAL* pHBm, SUSIE_PROGRESS lpProgressCallback, LONG_PTR lData);
EXTERN_C int __stdcall GetPreviewW(LPCWSTR buf, LONG_PTR len, UINT flag, HLOCAL* pHBInfo, HLOCAL* pHBm, SUSIE_PROGRESS lpProgressCallback, LONG_PTR lData);

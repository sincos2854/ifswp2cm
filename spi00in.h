// Copyright (c) 2003 - 2025 sincos2854
// Licensed under the MIT License

#pragma once

#include <windows.h>

#define SPI_NO_FUNCTION         -1    // Unimplemented function
#define SPI_ALL_RIGHT            0    // Ended without error
#define SPI_ABORT                1    // Callback function returned non-zero, so expansion was aborted
#define SPI_NOT_SUPPORT          2    // Unsupported format
#define SPI_OUT_OF_ORDER         3    // Broken data
#define SPI_NO_MEMORY            4    // Memory can't be allocated
#define SPI_MEMORY_ERROR         5    // Memory error
#define SPI_FILE_READ_ERROR      6    // Can't read file
#define SPI_WINDOW_ERROR         7    // Can't open window (Non-public error code)
#define SPI_OTHER_ERROR          8    // Error of some kind
#define SPI_FILE_WRITE_ERROR     9    // Can't write file (Non-public error code)
#define SPI_END_OF_FILE         10    // End of file (Non-public error code)

#pragma pack(push, 1)
struct PictureInfo {
    long left, top;
    long width;
    long height;
    WORD x_density;
    WORD y_density;
    short colorDepth;
#ifdef _WIN64
    char dummy[2];
#endif
    HLOCAL hInfo;
};
#pragma pack(pop)

typedef int(CALLBACK* ProgressCallback)(int, int, LONG_PTR);
EXTERN_C int __stdcall GetPluginInfo(int infono, LPSTR buf, int buflen);
EXTERN_C int __stdcall GetPluginInfoW(int infono, LPWSTR buf, int buflen);
EXTERN_C int __stdcall IsSupported(LPCSTR filename, DWORD_PTR dw);
EXTERN_C int __stdcall IsSupportedW(LPCWSTR filename, DWORD_PTR dw);
EXTERN_C int __stdcall GetPictureInfo(LPCSTR buf, LONG_PTR len, UINT flag, PictureInfo* lpInfo);
EXTERN_C int __stdcall GetPictureInfoW(LPCWSTR buf, LONG_PTR len, UINT flag, PictureInfo* lpInfo);
EXTERN_C int __stdcall GetPicture(LPCSTR buf, LONG_PTR len, UINT flag, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData);
EXTERN_C int __stdcall GetPictureW(LPCWSTR buf, LONG_PTR len, UINT flag, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData);
EXTERN_C int __stdcall GetPreview(LPCSTR buf, LONG_PTR len, UINT flag, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData);
EXTERN_C int __stdcall GetPreviewW(LPCWSTR buf, LONG_PTR len, UINT flag, HANDLE* pHBInfo, HANDLE* pHBm, ProgressCallback lpPrgressCallback, LONG_PTR lData);

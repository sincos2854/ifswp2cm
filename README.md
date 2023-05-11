# ifswp2cm
Susie Plug-in for [WebP 2](https://chromium.googlesource.com/codecs/libwebp2/)(*.wp2) files.

## Download
From [Releases page](https://github.com/sincos2854/ifswp2cm/releases)

## Features
- 32bit(.spi) and 64bit(.sph)
- ANSI and Unicode (e.g. GetPicture and GetPictureW)
- ICC Profile (Need a viewer that supports color management like [susico](http://www.vector.co.jp/soft/dl/winnt/art/se515212.html))

## Build
Get tht source code
```
git clone https://github.com/sincos2854/ifswp2cm.git --recursive --shallow-submodules
```
Visual Studio 32bit
```
cmake -G "Visual Studio 17 2022" -A Win32 -B build_32_release -DCMAKE_INSTALL_PREFIX=out_32_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_32_release --config Release --target install
```
Visual Studio 64bit
```
cmake -G "Visual Studio 17 2022" -A x64 -B build_64_release -DCMAKE_INSTALL_PREFIX=out_64_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_64_release --config Release --target install
```
Cross-compilation environment
```
sudo apt install cmake pkg-config ninja-build mingw-w64
sudo update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix
sudo update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix
sudo update-alternatives --set i686-w64-mingw32-g++ /usr/bin/i686-w64-mingw32-g++-posix
sudo update-alternatives --set i686-w64-mingw32-gcc /usr/bin/i686-w64-mingw32-gcc-posix
```
Cross-compilation 32bit
```
cmake -G Ninja -B build_32_release -DCMAKE_INSTALL_PREFIX=out_32_release -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++
cmake --build build_32_release --config Release --target install
```
Cross-compilation 64bit
```
cmake -G Ninja -B build_64_release -DCMAKE_INSTALL_PREFIX=out_64_release -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
cmake --build build_64_release --config Release --target install
```
## Inspired by
- [TORO's Software library(Win32/Win64 Plugin)](http://toro.d.dooo.jp/slplugin.html)
- [JPEG XL plugin for Susie Image Viewer](https://github.com/uyjulian/ifjxl)
- [HEIF/AVIF Susie plugin](https://github.com/Mr-Ojii/ifheif)
- [BPGファイル用Susieプラグインを埋め込みプロファイル対応にする : やんま まのblog（仮）](http://blog.livedoor.jp/yamma_ma/archives/44473876.html)

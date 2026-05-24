# ifswp2cm

A Susie Plug-in for [WebP 2](https://chromium.googlesource.com/codecs/libwebp2/) (`*.wp2`) files.

## Download

Download from the [Releases](https://github.com/sincos2854/ifswp2cm/releases) page.

## Features

- 32-bit (`.spi`) and 64-bit (`.sph`) support
- ANSI and Unicode support (e.g., `GetPicture` and `GetPictureW`)
- Always returns a 32-bit BGRA bitmap
- ICC profile support (requires a viewer that supports color management, such as [susico](http://www.vector.co.jp/soft/dl/winnt/art/se515212.html))

## Building

### Checking out the source code

```bash
git clone https://github.com/sincos2854/ifswp2cm.git
```

`libwebp2` is automatically fetched into the `libwebp2` directory during the initial CMake configuration. Subsequent builds reuse the existing checkout.

### Windows

Building with Visual Studio (32-bit, `.spi`)

```bat
cmake -G "Visual Studio 18 2026" -A Win32 -B build_32_release -DCMAKE_INSTALL_PREFIX=out_32_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_32_release --config Release --target install
```

Building with Visual Studio (64-bit, `.sph`)

```bat
cmake -G "Visual Studio 18 2026" -A x64 -B build_64_release -DCMAKE_INSTALL_PREFIX=out_64_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_64_release --config Release --target install
```

### Cross-compilation

Setting up the cross-compilation environment

```bash
sudo apt install cmake ninja-build mingw-w64
sudo update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix
sudo update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix
sudo update-alternatives --set i686-w64-mingw32-g++ /usr/bin/i686-w64-mingw32-g++-posix
sudo update-alternatives --set i686-w64-mingw32-gcc /usr/bin/i686-w64-mingw32-gcc-posix
```

Cross-compiling (32-bit, `.spi`)

```bash
cmake -G Ninja -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ -DCMAKE_SYSTEM_NAME=Windows -B build_32_release -DCMAKE_INSTALL_PREFIX=out_32_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_32_release --config Release --target install
```

Cross-compiling (64-bit, `.sph`)

```bash
cmake -G Ninja -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_SYSTEM_NAME=Windows -B build_64_release -DCMAKE_INSTALL_PREFIX=out_64_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_64_release --config Release --target install
```

## Inspired by

- [Susieプラグインの制作方法](https://www.asahi-net.or.jp/~kh4s-smz/spi/make_spi.html)
- [TORO's Software library(Win32/Win64 Plugin)](http://toro.d.dooo.jp/slplugin.html)
- [BPGファイル用Susieプラグインを埋め込みプロファイル対応にする : やんま まのblog（仮）](http://blog.livedoor.jp/yamma_ma/archives/44473876.html)
- [uyjulian/ifjxl: JPEG XL plugin for Susie Image Viewer](https://github.com/uyjulian/ifjxl)
- [Mr-Ojii/ifheif: HEIF/AVIF Susie plugin](https://github.com/Mr-Ojii/ifheif)

# gif图片编辑器

## ffmpeg 编译

### 桌面端编译配置

msys2环境需预先安装  

pacman -S mingw-w64-x86_64-libwebp pkg-config

```shell
./configure \
  --prefix=/build2 \
  --target-os=mingw32 \
  --arch=x86_64 \
  --enable-gpl \
  --enable-version3 \
  --enable-libwebp \
  --enable-libx264 \
  --enable-sdl2 \
  --enable-ffplay \
  --disable-static \
  --enable-shared \
  --disable-debug 

make -j
make install
```

### Android端

## TODO

- 时间轴
- 编辑后导出 


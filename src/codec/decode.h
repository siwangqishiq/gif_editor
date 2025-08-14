#pragma once

#include <string>
#include <functional>
#include <cstdint>

const unsigned int MAX_FRAME_COUNT = 64 * 4; //读取的最大帧图像数量

const int DECODE_SUCCESS = 0;
const int DECODE_ERR = -1;

int DecodeGifFile(
    std::string path,
    std::function<void(uint8_t *, int, int, double)> onGetFrameImageFunc
);
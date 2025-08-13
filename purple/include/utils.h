#pragma once

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "render/common.h"


namespace purple{

    constexpr double PI = 3.14159265358979323846;
    
    std::string date_time(std::time_t posix);

    long long currentTimeMillis();

    long long currentTimeMicro();

    float currentTimeMillisFloat();

    void ScaleWithPoint(Point &point , float scale, Point center);
    
    void Rotate(Point &point, float cx , float cy , float angle);

    float RoundToTwoDecimal(float num);

    bool PointInCircle(float x , float y , float cx , float cy , float radius);

    float Clamp(float x ,float min , float max);

    int RndInt(int max, int min);

    float RndFloat(float max , float min);

    float Min(float v1, float v2);

    float Max(float v1, float v2);
}



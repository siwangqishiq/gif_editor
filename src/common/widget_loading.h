#pragma once

#include <string>
#include "purple.h"

class WidgetLoading{
public:
    WidgetLoading();
    void render();
    ~WidgetLoading();
private:
    float time = 0.0f;
};
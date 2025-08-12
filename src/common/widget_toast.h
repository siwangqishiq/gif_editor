#pragma once

#include <string>
#include "purple.h"


class Toast{
public:
    Toast(std::wstring content_);

    void render();

    ~Toast();
private:
    std::wstring content;
};
#pragma once

#include "purple.h"
#include "input/input_common.h"



class InputAction{
public:
    virtual purple::Rect getHitRect() = 0;

    virtual bool onTouchEvent(purple::InputEvent &e);

    virtual void setOnClickListener(std::function<void()> click);
private:
    std::function<void()> onClick = nullptr;
};



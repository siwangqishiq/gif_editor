
#pragma once

#include "input_action.h"
#include "purple.h"

class GifEditorApp;

class BaseWidget: public InputAction{
public:
    virtual void init(GifEditorApp *appContext_) = 0;

    virtual purple::Rect getHitRect() override;

    purple::Rect viewRect;
protected:
    GifEditorApp *appContext;
};





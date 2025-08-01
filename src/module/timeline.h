#pragma once

#include "purple.h"
#include "base_widget.h"

class GifEditorApp;

class TimeLine : public BaseWidget{
public:
    TimeLine();

    virtual void init(GifEditorApp *appContext_) override;

    void tick();

    virtual ~TimeLine();

private:
    void prepare();
    int offset = 0;
};
#pragma once

#include "purple.h"
#include "input_action.h"
#include "base_widget.h"

class SaveButton : public BaseWidget{
public:
    SaveButton();

    virtual void init(GifEditorApp *appContext_) override;

    virtual ~SaveButton();

    void tick();

    void onClick();
private:
    GifEditorApp *appContext;
};
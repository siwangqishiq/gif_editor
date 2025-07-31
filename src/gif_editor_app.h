#pragma once

#include <memory>
#include "purple.h"

class GifEditorApp : public purple::IApp{
public:
    virtual void onInit() override;
    virtual void onTick() override;
    virtual void onDispose() override;

    virtual void onResize(int w , int h) override;
};
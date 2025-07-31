#pragma once

#include "purple.h"

class GifEditorApp;

class MainView{
public:
    MainView();

    void init(GifEditorApp *appContext_);

    void render();

    void onResize();

    ~MainView();
private:
    GifEditorApp *appContext;

    purple::Rect viewRect;

    void resetViewRect();
};

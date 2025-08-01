#pragma once

#include "purple.h"
#include "input_action.h"

class GifEditorApp;

enum MainViewState{
    Pause = 0,
    Play = 1,
};

class MainView : public InputAction{
public:
    MainViewState state = Pause;
    uint32_t curFrameIndex = 0;
    long long curFramePlayTime = 0;

    MainView();

    void init(GifEditorApp *appContext_);

    void render();

    void onResize();

    void updateNewState(MainViewState newState);

    virtual purple::Rect getHitRect() override;

    ~MainView();
private:
    GifEditorApp *appContext;

    purple::Rect viewRect;

    void resetViewRect();

    void trySkipNextFrame();
};

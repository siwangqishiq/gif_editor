#pragma once

#include "purple.h"
#include "input_action.h"
#include "base_widget.h"

class GifEditorApp;

enum MainViewState{
    Pause = 0,
    Play = 1,
};

class MainView : public BaseWidget{
public:
    MainViewState state = Pause;
    uint32_t curFrameIndex = 0;
    long long curFramePlayTime = 0;

    MainView();

    virtual void init(GifEditorApp *appContext_) override;

    void tick();

    void onResize();

    void updateNewState(MainViewState newState);

    virtual ~MainView();
private:
    void resetViewRect();

    void trySkipNextFrame();
};

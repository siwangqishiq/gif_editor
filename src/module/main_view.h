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
    long long curFramePlayTime = 0;

    MainView();

    virtual void init(GifEditorApp *appContext_) override;

    void tick();

    void onResize();

    void updateNewState(MainViewState newState);

    virtual ~MainView();

    void updateCurrentFrame(uint32_t newFrame);

    uint32_t getCurrentFrame(){
        return curFrameIndex;
    }

    void onClickPlayButton();
private:
    uint32_t curFrameIndex = 0;

    bool isLoopPlay = false;

    void resetViewRect();

    void trySkipNextFrame();
};

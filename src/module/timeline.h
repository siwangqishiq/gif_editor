#pragma once

#include "purple.h"
#include "base_widget.h"
#include <memory>

class GifEditorApp;

enum TimeLineWidgetState{
    Idle,
    Move
};

class TimeLine : public BaseWidget{
public:
    TimeLineWidgetState widgetState = Idle;

    const std::string TIME_LINE_TEX_NAME = "_timeline_tex";

    const int EXTRACT_FRAME_COUNT = 8;

    TimeLine();

    virtual void init(GifEditorApp *appContext_) override;

    void tick();

    int findTimelineTexOffset(uint32_t curFrameIndex);
    uint32_t findFrameIndexByTimelineOffset(int offset);

    virtual bool onTouchEvent(purple::InputEvent &e) override;

    virtual ~TimeLine();
private:
    purple::Rect timelineRect;
    purple::Rect bottomTimeRect;

    float scrollBeginX;
    float timelineRectBeiginX;
    
    void prepare();

    void selectFramesToTimeline();

    void renderTimelineFrames();

    void renderMiddleLine();

    void renderTimeStr();

    void updateTimelineRect();

    void releaseTimelineAllTextures();

    void onTimelineScrollBegin(float x, float y);
    void onTimelineScrollMove(float x, float y);
    void onTimelineScrollEnd(float x, float y);

    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t totalFrameCount= 0;

    int thumbImageHeight = 1.0f;
    int thumbImageWidth = 1.0f;

    int timelineWidgetTop = 0;
    int timelineWidgetLeft = 0;

    std::vector<int> frameTexIndexArray;

    int offset = 0;

    unsigned int buildVirtualTexIndex = 0;
    std::vector<std::shared_ptr<purple::TextureInfo>> timelineTexInfoList;
};
#pragma once

#include "purple.h"
#include "base_widget.h"
#include <memory>

class GifEditorApp;

class TimeLine : public BaseWidget{
public:
    const std::string TIME_LINE_TEX_NAME = "_timeline_tex";

    const int EXTRACT_FRAME_COUNT = 1;

    TimeLine();

    virtual void init(GifEditorApp *appContext_) override;

    void tick();

    int findTimelineTexOffset(uint32_t curFrameIndex);

    virtual ~TimeLine();
private:
    void prepare();

    void selectFramesToTimeline();

    void renderTimelineFrames();

    void renderMiddleLine();

    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t totalFrameCount= 0;

    int thumbImageHeight;
    int thumbImageWidth;

    int timelineWidgetTop = 0;
    int timelineWidgetLeft = 0;

    std::vector<int> frameTexIndexArray;

    int offset = 0;

    std::shared_ptr<purple::TextureInfo> timelineTexInfo = nullptr;
    std::shared_ptr<purple::TextureImage> timelineImage = nullptr;
};
#pragma once

#include <memory>
#include "purple.h"
#include <vector>
#include "main_view.h"

struct ImageFrameData{
    std::shared_ptr<purple::TextureImage> tex;
    float pts;
};

const unsigned int MAX_FRAME_COUNT = 128;

class GifEditorApp : public purple::IApp{
public:
    std::vector<std::unique_ptr<ImageFrameData>> frameList;
    uint32_t curFrameIndex = 0;

    virtual void onInit() override;
    virtual void onTick() override;
    virtual void onDispose() override;

    virtual void onResize(int w , int h) override;

    void onGetFrameImage(uint8_t *data, int w, int h , double pts);

    purple::Rect findCenterModeRect(purple::Rect srcRect,purple::Rect viewRect,bool isCrop);

    long long getLastFrameDeltaTime();
private:
    MainView mMainView;
    long long timeMs = -1;

    int decodeGifFile(const char* filepath); //decode gif file 
};
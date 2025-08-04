#pragma once

#include <memory>
#include "purple.h"
#include <vector>
#include "main_view.h"
#include "timeline.h"

struct ImageFrameData{
    std::shared_ptr<purple::TextureImage> tex;
    float pts;
};

const unsigned int MAX_FRAME_COUNT = 128 * 8; //读取的最大帧图像数量

class InputAction;

class GifEditorApp : public purple::IApp{
public:
    GifEditorApp(std::vector<std::string> params);

    // image data
    std::vector<std::unique_ptr<ImageFrameData>> frameList;

    virtual void onInit() override;
    virtual void onTick() override;
    virtual void onDispose() override;

    virtual void onResize(int w , int h) override;

    void onGetFrameImage(uint8_t *data, int w, int h , double pts);

    void registerInputWidget(InputAction *widget);
    void unRegisterInputWidget(InputAction *widget);

    long long getLastFrameDeltaTime();

    std::string filePath;

    MainView mMainView;
    TimeLine mTimeline;
private:
    long long timeMs = -1;

    bool hasDecodeGifImage = false;

    InputAction *catchedInputWidget = nullptr;

    std::vector<InputAction *> inputWidgets;

    int decodeGifFile(const char* filepath); //decode gif file 

    void handleInputAction(purple::InputEvent &e);
};
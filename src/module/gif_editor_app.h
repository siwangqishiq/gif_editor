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

class InputAction;
class SaveButton;
class Toast;

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

    void showToast(std::wstring content, long showTimeMils);

    std::string filePath;

    std::shared_ptr<MainView> mMainView = nullptr;
    std::shared_ptr<TimeLine> mTimeline = nullptr;
    std::shared_ptr<SaveButton> saveButton = nullptr;
    
    std::shared_ptr<Toast> toastWidget = nullptr;
    int toastScheduleId = -1;

    std::unique_ptr<purple::ThreadPool> threadPool;
private:
    long long timeMs = -1;
    
    bool hasDecodeGifImage = false;

    InputAction *catchedInputWidget = nullptr;

    std::vector<InputAction *> inputWidgets;

    int decodeGifFile(const char* filepath); //decode gif file 

    void handleInputAction(purple::InputEvent &e);
};
#include "gif_editor_app.h"
#include "app_util.h"
#include "utils.h"
#include "input/input_manager.h"
#include "input_action.h"
#include "decode.h"
#include "glheader.h"
#include "save_button.h"
#include "widget_toast.h"
#include "widget_loading.h"

// const char *FILE_PATH = "../assets/sharongzi.gif";
const char *FILE_PATH = "../assets/gakki.gif";
// const char *FILE_PATH = "../assets/nezha.gif";
// const char *FILE_PATH = "../assets/loading.gif";
// const char *FILE_PATH = "../assets/test.gif";
// const char *FILE_PATH = "../assets/wanzi.mp4";

const char *TAG = "gif_editor";
const std::string GIF_EDITOR_INPUT = "gif_editor_input";

GifEditorApp::GifEditorApp(std::vector<std::string> params){
    if(params.size() >= 2){
        filePath = params[1];
    }else{
        filePath = FILE_PATH;
    }//end if
}

void GifEditorApp::onInit(){
    purple::Log::i(TAG,"onInit");


    if(purple::Engine::ScreenWidth == 0 || purple::Engine::ScreenHeight == 0){
        purple::Log::e(TAG,"screen size %d %d not need to init!" , 
            purple::Engine::ScreenWidth,purple::Engine::ScreenHeight);
        return;
    }

    threadPool = std::make_unique<purple::ThreadPool>(1);

    showLoading();
    threadPool->enqueue([this](){
        decodeGifFile(filePath.c_str());
        purple::Engine::getTimer()->schedule([this](void *p){
            onDecodeFileFinished();
        },0);
    });

    // decodeGifFile(filePath.c_str());

    if(saveButton == nullptr){
        saveButton = std::make_shared<SaveButton>();
    }
    saveButton->init(this);

    if(mMainView == nullptr){
        mMainView = std::make_shared<MainView>();
    }
    
    if(mTimeline == nullptr){
        mTimeline = std::make_shared<TimeLine>();
    }

    mMainView->init(this);
    mTimeline->init(this);

    purple::InputManager::getInstance()->removeEventListener(GIF_EDITOR_INPUT);
    purple::InputManager::getInstance()->addEventListener(GIF_EDITOR_INPUT,[this](purple::InputEvent e){
        // purple::Log::i("input_action" , "%d, (%f, %f)", e.action, e.x, e.y);
        handleInputAction(e);
        return true;
    });
    
    registerInputWidget(static_cast<InputAction *>(mMainView.get())); 
    registerInputWidget(static_cast<InputAction *>(mTimeline.get())); 
    registerInputWidget(static_cast<InputAction *>(saveButton.get()));

    purple::Log::w(TAG, "inputWidgets count:%d", inputWidgets.size());
}

int GifEditorApp::decodeGifFile(const char* filepath){
    if(hasDecodeGifImage){
        return 0;
    }
    
    int ret = DecodeGifFile(filePath, [this](uint8_t *data,int w,int h ,double pts){
        purple::Engine::getTimer()->schedule([this,data,w,h,pts](void *){
            onGetFrameImage(data, w, h, pts);
            delete[] data;
        },0);
    });

    if(ret == DECODE_SUCCESS){
        hasDecodeGifImage = true;
    }
    return 0;
}

void GifEditorApp::onDecodeFileFinished(){
    purple::Log::i(TAG, "load file decode finished");

    dismissLoading();
    
    mTimeline->init(this);
}

void GifEditorApp::onGetFrameImage(uint8_t *data, int w, int h , double pts){
    // purple::Log::w(TAG, "onGetFrameImage");
    auto tex = purple::BuildImageByPixlData(data, w, h, GL_RGBA);
    // purple::Log::w(TAG, "onGetFrameImage frameData");
    std::unique_ptr<ImageFrameData> frameData = std::make_unique<ImageFrameData>();
    frameData->pts = static_cast<float>(pts);
    frameData->tex = tex;
    
    this->frameList.push_back(std::move(frameData));
}

void GifEditorApp::registerInputWidget(InputAction *widget){
    unRegisterInputWidget(widget);
    inputWidgets.emplace_back(widget);
}

void GifEditorApp::unRegisterInputWidget(InputAction *widget){
    auto it = std::find(inputWidgets.begin(), inputWidgets.end(), widget);
    if (it != inputWidgets.end()) {
        inputWidgets.erase(it);
    }
}

void GifEditorApp::handleInputAction(purple::InputEvent &e){
    if(catchedInputWidget != nullptr){
        catchedInputWidget->onTouchEvent(e);
        if(e.action == purple::EVENT_ACTION_END || e.action == purple::EVENT_ACTION_CANCEL){
            catchedInputWidget = nullptr;
            return;
        }
    }

    for(auto it = inputWidgets.rbegin(); it != inputWidgets.rend();++it){
        auto widget = *it;
        if(widget->onTouchEvent(e)){
            catchedInputWidget = widget;
            break;
        }
    }//end for 
}

long long GifEditorApp::getLastFrameDeltaTime(){
    if(timeMs < 0){
        return 0;
    }
    return purple::currentTimeMillis() - timeMs;
}

void GifEditorApp::onResize(int w , int h){
    onInit();
}

void GifEditorApp::onTick(){
    mMainView->tick();
    mTimeline->tick();

    if(saveButton != nullptr){
        saveButton->tick();
    }


    updateUIWidgets();
    timeMs = purple::currentTimeMillis();
}

void GifEditorApp::updateUIWidgets(){
    if(loadingWidget != nullptr){
        loadingWidget->render();
    }

    if(toastWidget != nullptr){
        toastWidget->render();
    }
}

void GifEditorApp::showToast(std::wstring content, long showTimeMils){
    if(toastWidget != nullptr && toastScheduleId != -1){
        purple::Engine::getTimer()->removeScheduleTask(toastScheduleId);
    }

    toastWidget = std::make_shared<Toast>(content);
    toastScheduleId = purple::Engine::getTimer()->schedule([this](void *){
        toastWidget = nullptr;
    }, static_cast<long long>(showTimeMils));
}

void GifEditorApp::showLoading(){
    loadingWidget = std::make_shared<WidgetLoading>();
}

void GifEditorApp::dismissLoading(){
    loadingWidget = nullptr;
}

void GifEditorApp::onDispose(){
    if(threadPool != nullptr){
        threadPool->shutdown();
    }
    purple::InputManager::getInstance()->removeEventListener(GIF_EDITOR_INPUT);
    purple::Log::i("GifEditorApp","TestTextUi::onDispose");
}


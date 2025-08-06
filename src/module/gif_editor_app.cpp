#include "gif_editor_app.h"
#include "app_util.h"
#include "utils.h"
#include "input/input_manager.h"
#include "input_action.h"
#include "decode.h"

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
    decodeGifFile(filePath.c_str());

    mMainView.init(this);
    mTimeline.init(this);

    purple::InputManager::getInstance()->removeEventListener(GIF_EDITOR_INPUT);
    purple::InputManager::getInstance()->addEventListener(GIF_EDITOR_INPUT,[this](purple::InputEvent e){
        // purple::Log::i("input_action" , "%d, (%f, %f)", e.action, e.x, e.y);
        handleInputAction(e);
        return true;
    });

    registerInputWidget(static_cast<InputAction *>(&mMainView)); 
    registerInputWidget(static_cast<InputAction *>(&mTimeline)); 
}

int GifEditorApp::decodeGifFile(const char* filepath){
    if(hasDecodeGifImage){
        return 0;
    }
    
    int ret = DecodeGifFile(filePath, [this](uint8_t *data,int w,int h ,double pts){
        onGetFrameImage(data, w, h, pts);
    });

    if(ret == DECODE_SUCCESS){
        hasDecodeGifImage = true;
    }
    return 0;
}

void GifEditorApp::onGetFrameImage(uint8_t *data, int w, int h , double pts){
    purple::Log::w(TAG, "onGetFrameImage");
    auto tex = purple::BuildImageByPixlData(data, w, h, GL_BGRA);
    purple::Log::w(TAG, "onGetFrameImage frameData");
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
    for(auto it = inputWidgets.begin() ; it != inputWidgets.end(); it++){
        if(*it == widget){
            inputWidgets.erase(it);
            break;
        }
    }//end for each
}

void GifEditorApp::handleInputAction(purple::InputEvent &e){
    if(catchedInputWidget != nullptr){
        catchedInputWidget->onTouchEvent(e);
        if(e.action == purple::EVENT_ACTION_END || e.action == purple::EVENT_ACTION_CANCEL){
            catchedInputWidget = nullptr;
            return;
        }
    }

    for(auto &widget : inputWidgets){
        if(widget->onTouchEvent(e)){
            catchedInputWidget = widget;
            break;
        }
    }//end for each;
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
    mMainView.tick();
    mTimeline.tick();

    timeMs = purple::currentTimeMillis();
}

void GifEditorApp::onDispose(){
    purple::InputManager::getInstance()->removeEventListener(GIF_EDITOR_INPUT);
    purple::Log::i("GifEditorApp","TestTextUi::onDispose");
}


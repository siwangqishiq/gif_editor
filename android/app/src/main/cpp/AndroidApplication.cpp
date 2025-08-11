#include "AndroidApplication.h"

#include "input/input_manager.h"
#include <thread>
#include <string>
#include <vector>

#include "gif_editor_app.h"

void AndroidApplication::init(){
    purple::InitParams purpleEngineParams;
    purpleEngineParams.clearColor = glm::vec4(0.0f , 0.0f ,0.0f , 1.0f);
    purple::Engine::init(width , height , purpleEngineParams);
    purple::Engine::getRenderEngine()->getTextRender()->loadFontRes("youyuan","text/youyuan.ttf",true);

    std::vector<std::string> filePaths;
    appInstanceList.push_back(std::make_shared<GifEditorApp>(params));

    for(std::shared_ptr<purple::IApp> &app : appInstanceList){
        app->onInit();
    }

    purple::Engine::getTimer()->scheduleAtFixedRate([this](void *app){
        std::string fpsStr = "fps : " + std::to_string(fps);
        purple::Log::w("fps" , fpsStr);
        fps = 0;
    } , 1000L);
}

void AndroidApplication::tick(){
    purple::Engine::tick();
    for(std::shared_ptr<purple::IApp> &app : appInstanceList){
        app->onTick();
    }
    fps++;
}

void AndroidApplication::dispose(){
    for(std::shared_ptr<purple::IApp> &app : appInstanceList){
        app->onDispose();
    }
    purple::Engine::dispose();
}

void AndroidApplication::resize(int w , int h) {
    this->width = w;
    this->height = h;
    purple::Engine::init(w , h);
}

void AndroidApplication::onTouchEvent(int action , float x , float y){
    int inputAction = 0;
    switch (action) {
        case 0://down
            inputAction = purple::EVENT_ACTION_BEGIN;
            break;
        case 1:
            inputAction = purple::EVENT_ACTION_END;
            break;
        case 2://move
            inputAction = purple::EVENT_ACTION_MOVE;
            break;
        case 3://cancel
            inputAction = purple::EVENT_ACTION_CANCEL;
            break;
        default:
            break;
    }

    purple::InputEvent event;
    event.action = inputAction;
    event.x = x;
    event.y = static_cast<float>(purple::Engine::ScreenHeight) - y;

    purple::InputManager::getInstance()->onEvent(event);
}
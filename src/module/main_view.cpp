#include "gif_editor_app.h"
#include "main_view.h"
#include "app_util.h"
#include "purple.h"

MainView::MainView(){
}

void MainView::init(GifEditorApp *appContext_){
    appContext = appContext_;
    resetViewRect();
    updateNewState(Pause);

    this->setOnClickListener([this](){
        purple::Log::i("MainView", "on clicked");
        onClickPlayButton();
    });
}

void MainView::onClickPlayButton(){
    if(state == Play){
        updateNewState(Pause);
    }else if(state == Pause){
        if(curFrameIndex + 1 >= appContext->frameList.size()){
            curFrameIndex = 0;
            curFramePlayTime = 0;
        }
        
        updateNewState(Play);
    }//end if
}

void MainView::tick(){
    if(appContext->frameList.empty()){
        return;
    }

    auto image = appContext->frameList[curFrameIndex]->tex;
    auto srcRect = image->getRect();
    purple::Rect dstRect = FindCenterModeRect(srcRect, viewRect, false);

    // purple::Engine::getRenderEngine()->beginScissor(dstRect.left,dstRect.getBottom(),
    //     dstRect.width / 2, dstRect.height / 2);
    
    auto batch = purple::Engine::getRenderEngine()->getSpriteBatch();

    purple::Paint framePaint;
    // framePaint.texFlip = true;
    
    batch->begin();
    batch->renderImage(image,srcRect, dstRect);
    batch->end(framePaint);

    // purple::Engine::getRenderEngine()->endScissor();

    if(state == Play){
        trySkipNextFrame();
    }
}

void MainView::trySkipNextFrame(){
    auto deltaTime = appContext->getLastFrameDeltaTime();

    if(curFrameIndex + 1 >= appContext->frameList.size()){
        curFramePlayTime += deltaTime;
        if(curFramePlayTime > 25){

            if(isLoopPlay){
                updateCurrentFrame(0);
                curFramePlayTime = 0;
            }else{
                updateNewState(Pause);
            }
        }
    }else{
        const float curPtsF = appContext->frameList[curFrameIndex]->pts;
        long long curPts = curPtsF * 1000;

        const float nextPtsF = appContext->frameList[curFrameIndex + 1]->pts;
        long long nextPts = nextPtsF * 1000;
        
        curFramePlayTime += deltaTime;
        if(curFramePlayTime > nextPts - curPts){
            updateCurrentFrame(curFrameIndex + 1);
            curFramePlayTime = 0;
        }
    }
}

void MainView::updateCurrentFrame(uint32_t newFrame){
    curFrameIndex = newFrame;
}

void MainView::onResize(){
    resetViewRect();
}

void MainView::updateNewState(MainViewState newState){
    if(newState != state){
        state = newState;
    }
}

void MainView::resetViewRect(){
    viewRect.left = 0.0f;
    viewRect.top = purple::Engine::ScreenHeight;
    viewRect.width = purple::Engine::ScreenWidth;
    viewRect.height = purple::Engine::ScreenHeight / 2.0f;
    
    purple::Log::w("MainView" , "size %f, %f", viewRect.width , viewRect.height);
}

MainView::~MainView(){
    purple::Log::w("MainView", "~MainView");
}
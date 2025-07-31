#include "gif_editor_app.h"
#include "main_view.h"
#include "purple.h"

MainView::MainView(){
}

void MainView::init(GifEditorApp *appContext_){
    appContext = appContext_;

    resetViewRect();
}

void MainView::render(){
    purple::Paint bgPaint;
    bgPaint.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // auto batch = purple::Engine::getRenderEngine()->getShapeBatch();
    // batch->begin();
    // batch->renderRect(viewRect, bgPaint);
    // batch->end();

    // int frameSize = this->appContext->frameList.size();
    // purple::Log::e("MainView" , "frameSize count %d", frameSize);

    if(appContext->frameList.size() > 0){
        auto image = appContext->frameList[appContext->curFrameIndex]->tex;
        auto batch = purple::Engine::getRenderEngine()->getSpriteBatch();

        purple::Paint framePaint;
        framePaint.texFlip = true;

        batch->begin();
        auto srcRect = image->getRect();

        purple::Rect dstRect = appContext->findCenterModeRect(srcRect, viewRect, false);
        batch->renderImage(image,srcRect, dstRect);
        batch->end(framePaint);

        float pts = appContext->frameList[appContext->curFrameIndex]->pts;
        long long ptsMls = pts * 1000;

        auto deltaTime = appContext->getLastFrameDeltaTime();
        purple::Log::e("test", "deltaTime = %ld ptsMls = %ld", deltaTime, ptsMls);
        if(deltaTime > ptsMls){
            appContext->curFrameIndex = (appContext->curFrameIndex + 1) % appContext->frameList.size();
        }
    }
}

void MainView::onResize(){
    resetViewRect();
}

void MainView::resetViewRect(){
    viewRect.left = 0.0f;
    viewRect.top = purple::Engine::ScreenHeight;
    viewRect.width = purple::Engine::vpTopx(600);
    viewRect.height = purple::Engine::vpTopx(500);

    purple::Log::w("MainView" , "size %f, %f", viewRect.width , viewRect.height);
}

MainView::~MainView(){
    purple::Log::w("MainView", "~MainView");
}
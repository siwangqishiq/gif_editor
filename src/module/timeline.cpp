#include "timeline.h"
#include "purple.h"
#include "purple_ui.h"
#include "input_action.h"

TimeLine::TimeLine(){
}

void TimeLine::init(GifEditorApp *appContext_){
    appContext = appContext_;

    viewRect.left = 0.0f;
    viewRect.top = purple::Engine::ScreenHeight / 4.0f;
    viewRect.width = purple::Engine::ScreenWidth;
    viewRect.height = viewRect.top - purple::Engine::vpTopx(8.0f);

    // this->setOnClickListener([](){
    //     purple::Log::i("timeline","clicked");
    // });

    prepare();
}

void TimeLine::prepare(){
    
}

void TimeLine::tick(){
    purple::Paint bgPaint;
    bgPaint.color = purple::ConverColorValue(purple::Color::Gray);

    auto shapeBatch = purple::Engine::getRenderEngine()->getShapeBatch();
    shapeBatch->begin();
    shapeBatch->renderRoundRect(viewRect, purple::Engine::vpTopx(4.0f), bgPaint);
    shapeBatch->end();
}

TimeLine::~TimeLine(){

}
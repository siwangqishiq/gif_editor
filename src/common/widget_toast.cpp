#include "widget_toast.h"
#include "colors.h"


Toast::Toast(std::wstring content_){
    this->content = content_;
}

void Toast::render(){
    purple::TextPaint textPaint;
    textPaint.textColor = Colors::TOAST_TEXT;
    textPaint.setTextSize(purple::Engine::vpTopx(32.0f));
    textPaint.textGravity = purple::TextGravity::BottomCenter;

    purple::Paint paint;
    paint.color = Colors::TOAST_BACKGROUND;
    
    purple::Rect outRect;
    purple::Engine::getRenderEngine()->preCalculateTextSize(this->content,
        textPaint, purple::Engine::ScreenWidth * 0.8f, outRect);
    outRect.width = 1.2f * outRect.width;
    outRect.height = 1.2f * outRect.height;

    auto shapeBatch = purple::Engine::getRenderEngine()->getShapeBatch();
    shapeBatch->begin();
    outRect.left = purple::Engine::ScreenWidth / 2.0f - outRect.width / 2.0f;
    outRect.top = purple::Engine::ScreenHeight / 2.0f + outRect.height / 2.0f;
    shapeBatch->renderRoundRect(outRect, purple::Engine::vpTopx(8.0f),paint);
    shapeBatch->end();
    purple::Engine::getRenderEngine()->renderTextWithRect(content, outRect, textPaint, nullptr);
}

Toast::~Toast(){
}


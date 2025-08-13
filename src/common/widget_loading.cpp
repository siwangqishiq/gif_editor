#include "widget_loading.h"
#include "colors.h"
#include <cmath>
#include "utils.h"

WidgetLoading::WidgetLoading(){
}

WidgetLoading::~WidgetLoading(){
}

void WidgetLoading::render(){
    auto shapeBatch = purple::Engine::getRenderEngine()->getShapeBatch();
    shapeBatch->begin();

    purple::Paint bgPaint;
    bgPaint.color = Colors::WINDOW_BACKGROUND;
    purple::Rect bgRect(0, 
        purple::Engine::ScreenHeight, 
        purple::Engine::ScreenWidth,
        purple::Engine::ScreenHeight);
    shapeBatch->renderRect(bgRect, bgPaint);

    float w = purple::Engine::vpTopx(120.0f);
    float A = w / 3.0f;
    float lineWidth = w / 16.0f;
    float startX = purple::Engine::ScreenWidth / 2.0f - w / 2.0f;
    float startY = purple::Engine::ScreenHeight / 2.0f;

    purple::Paint shapePaint;
    shapePaint.color = Colors::LOADING_COLOR;

    purple::Rect rect;
    for(int i = 0 ; i < 8; i++){
        float height = A * sinf((time + i * lineWidth * 2.0f) * (2 * purple::PI / w)); //i * lineWidth * 2.0f * 
        rect.left = startX + i * lineWidth * 2.0f;
        rect.width = lineWidth;
        rect.height = height;
        rect.top = startY + height / 2.0f;
        shapeBatch->renderRoundRect(rect, 0.0f, shapePaint);
    }//end for i

    shapeBatch->end();
    time += 1.2f;
}
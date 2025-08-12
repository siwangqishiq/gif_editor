#include "save_button.h"
#include "colors.h"
#include "gif_editor_app.h"

SaveButton::SaveButton(){
}

void SaveButton::init(GifEditorApp *appContext_) {
    this->appContext = appContext_;

    viewRect.width = purple::Engine::vpTopx(80.0f);
    viewRect.height = purple::Engine::vpTopx(32.0f);
    viewRect.left = purple::Engine::ScreenWidth - viewRect.width;
    viewRect.top = purple::Engine::ScreenHeight;

    setOnClickListener([this](){
        this->onClick();
    });
}

void SaveButton::onClick(){
    purple::Log::i("SaveButton" , "Save Button Clicked!");
    appContext->showToast(L"保存文件", 3000);
}

void SaveButton::tick(){
    purple::Paint paint;
    paint.color = Colors::BUTTON_BACKGROUND;

    auto batch = purple::Engine::getRenderEngine()->getShapeBatch();
    batch->begin();
    batch->renderRoundRect(viewRect, purple::Engine::vpTopx(4.0f), paint);
    batch->end();

    purple::TextPaint textPaint;
    textPaint.setTextSize(viewRect.height);
    textPaint.textColor = Colors::BUTTON_TEXT;
    textPaint.textGravity = purple::TextGravity::Center; 
    purple::Engine::getRenderEngine()->renderTextWithRect(L"保存", viewRect, textPaint, nullptr);
}

SaveButton::~SaveButton(){

}
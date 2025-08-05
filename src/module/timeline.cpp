#include "timeline.h"
#include "purple.h"
#include "purple_ui.h"
#include "input_action.h"
#include "gif_editor_app.h"
#include "main_view.h"
#include "time_util.h"


TimeLine::TimeLine(){
}

void TimeLine::init(GifEditorApp *appContext_){
    appContext = appContext_;

    viewRect.left = 0.0f;
    viewRect.top = purple::Engine::ScreenHeight / 4.0f;
    viewRect.width = purple::Engine::ScreenWidth;
    viewRect.height = viewRect.top - purple::Engine::vpTopx(18.0f);

    // this->setOnClickListener([](){
    //     purple::Log::i("timeline","clicked");
    // });

    bottomTimeRect.left = viewRect.left;
    bottomTimeRect.top = viewRect.getBottom();
    bottomTimeRect.width = viewRect.width;
    bottomTimeRect.height = bottomTimeRect.top;

    prepare();
}

void TimeLine::prepare(){
    if(appContext->frameList.size() == 0){
        purple::Log::e("TimeLine", "frame list is empty");
        return;
    }

    imageWidth = appContext->frameList[0]->tex->getWidth();
    imageHeight = appContext->frameList[0]->tex->getHeight();
    totalFrameCount = appContext->frameList.size();

    thumbImageHeight = (2 * viewRect.height) / 3;
    const float ratio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);
    thumbImageWidth = thumbImageHeight * ratio;

    timelineWidgetTop = viewRect.center().y + thumbImageHeight / 2;
    timelineWidgetLeft = viewRect.center().x;

    purple::Log::i("TimeLine", "thumbImageWidth %d x %d thumbImageWidth", thumbImageWidth, thumbImageHeight);


    selectFramesToTimeline();
}

void TimeLine::selectFramesToTimeline(){
    frameTexIndexArray.clear();
    for(size_t i = 0 ; i < this->appContext->frameList.size(); i++){
        if(i % EXTRACT_FRAME_COUNT == 0){
            frameTexIndexArray.push_back(i);
        }
    }//end for i
    
    if(timelineTexInfo != nullptr){
        purple::TextureManager::getInstance()->freeTexture(*timelineTexInfo);
    }

    timelineTexInfo = purple::Engine::getRenderEngine()->buildVirtualTexture(
        TIME_LINE_TEX_NAME,
        thumbImageWidth * frameTexIndexArray.size(),
        thumbImageHeight,
        [this](int w,int h){
            int x = 0;
            purple::Rect dst;
            dst.top = thumbImageHeight;
            dst.width = thumbImageWidth;
            dst.height = thumbImageHeight;
            
            for(size_t i = 0; i < frameTexIndexArray.size() ;i++){
                dst.left =x;
                
                // purple::Paint paint;
                // paint.texFlip = true;
                auto batch = purple::Engine::getRenderEngine()->getSpriteBatch();
                batch->begin();
                auto image = appContext->frameList[frameTexIndexArray[i]]->tex;
                auto srcRect = image->getRect();
                batch->renderImage(image, srcRect, dst);
                batch->end();

                x += thumbImageWidth;
            }//end for i
           
        }
    );
    timelineImage = std::make_shared<purple::TextureImage>(timelineTexInfo);
}


void TimeLine::tick(){
    // purple::Engine::getRenderEngine()->beginScissor(
    //     static_cast<int>(viewRect.left), 
    //     static_cast<int>(viewRect.getBottom()),
    //     static_cast<int>(viewRect.width), static_cast<int>(viewRect.height));

    purple::Paint bgPaint;
    bgPaint.color = purple::ConverColorValue(purple::Color::Gray);
    auto shapeBatch = purple::Engine::getRenderEngine()->getShapeBatch();
    shapeBatch->begin();
    shapeBatch->renderRoundRect(viewRect, purple::Engine::vpTopx(0.0f), bgPaint);
    shapeBatch->end();

    renderTimelineFrames();
    
    renderMiddleLine();

    renderTimeStr();

    // purple::Engine::getRenderEngine()->endScissor();
}

int TimeLine::findTimelineTexOffset(uint32_t curFrameIndex){
    if(curFrameIndex >= totalFrameCount || timelineTexInfo == nullptr){
        purple::Log::e("TimeLine" , "findTimelineTexOffset error");
        return -1;
    }

    const float select2OriginScale = static_cast<float>(totalFrameCount) / static_cast<float>(frameTexIndexArray.size());
    const float frameIndexInSelected = curFrameIndex / select2OriginScale;

    float offsetFactor = frameIndexInSelected/ static_cast<float>(frameTexIndexArray.size());
    int offset = offsetFactor * timelineTexInfo->width;
    return offset;
}


void TimeLine::renderTimelineFrames(){
    if(timelineImage == nullptr){
        return;
    }


    purple::Rect srcRect = timelineImage->getRect();
    purple::Rect dstRect;
    dstRect = srcRect;
    // purple::Log::e("timeline" , "offset = %d", findTimelineTexOffset(appContext->mMainView.getCurrentFrame()));
    dstRect.left = viewRect.width / 2 - findTimelineTexOffset(appContext->mMainView.getCurrentFrame());
    dstRect.top = timelineWidgetTop;

    auto batch = purple::Engine::getRenderEngine()->getSpriteBatch();
    batch->begin();
    batch->renderImage(*timelineImage, srcRect, dstRect);
    batch->end();
}

void TimeLine::renderMiddleLine(){
    purple::Paint linePaint;
    linePaint.color = purple::ConverColorValue(purple::Color::White);
    
    purple::Rect lineRect;

    float roundRadius = purple::Engine::vpTopx(2);
    lineRect.left = viewRect.center().x - roundRadius;
    lineRect.top = viewRect.top;
    lineRect.width = roundRadius + roundRadius;
    lineRect.height = viewRect.height;

    auto batch = purple::Engine::getRenderEngine()->getShapeBatch();
    batch->begin();
    batch->renderRoundRect(lineRect, roundRadius, linePaint);
    batch->end();
}

void TimeLine::renderTimeStr(){
    // purple::Paint linePaint;
    // linePaint.color = purple::ConverColorValue(purple::Color::Red);
    // auto batch = purple::Engine::getRenderEngine()->getShapeBatch();
    // batch->begin();
    // batch->renderRect(bottomTimeRect, linePaint);
    // batch->end();

    purple::TextPaint textPaint;
    textPaint.textColor = purple::ConverColorValue(purple::Color::White);
    textPaint.fontName = "youyuan";
    textPaint.textGravity = purple::TextGravity::BottomCenter;
    textPaint.fontWeight = 40.0f;
    textPaint.setTextSize(bottomTimeRect.height * 1.1f);

    auto textRender = purple::Engine::getRenderEngine()->getTextRender();
    auto textRect = bottomTimeRect;
    textRect.top -= 8;

    uint32_t currentFrame = appContext->mMainView.getCurrentFrame();
    
    if(currentFrame >= 0 && currentFrame < appContext->frameList.size()){
        float pts = appContext->frameList[currentFrame]->pts;
        long long timePts = pts * 1000;
        textRender->renderTextWithRect(TimeUtil::timeMillisToStr(timePts), 
            textRect, textPaint, nullptr);
    }

    if(appContext->frameList.size() > 0){
        textPaint.textGravity = purple::TextGravity::BottomRight;
        float pts = appContext->frameList[appContext->frameList.size() - 1]->pts;
        long long timePts = pts * 1000;
        textRender->renderTextWithRect(TimeUtil::timeMillisToStr(timePts), 
            textRect, textPaint, nullptr);
    }
}

TimeLine::~TimeLine(){
    if(timelineTexInfo != nullptr){
        purple::TextureManager::getInstance()->freeTexture(*timelineTexInfo);
    }
}
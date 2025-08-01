#include "app_util.h"
#include "utils.h"

purple::Rect FindCenterModeRect(purple::Rect srcRect,purple::Rect viewRect,bool isCrop){
    purple::Rect dstRect;
    dstRect.left = viewRect.left;
    dstRect.top = viewRect.top;
    dstRect.width = srcRect.width;
    dstRect.height = srcRect.height;

    auto dstCenter = dstRect.center();
    auto viewCenter = viewRect.center();
    float tranX = dstCenter.x - viewCenter.x;
    float tranY = dstCenter.y - viewCenter.y;

    dstRect.left -= tranX;
    dstRect.top -= tranY;
    
    const float scaleWidth = viewRect.width / dstRect.width;
    purple::Point pw{dstRect.left , dstRect.top};
    purple::ScaleWithPoint(pw , scaleWidth , viewCenter);
    int wLeft = pw.x;
    int wTop = pw.y;

    const float scaleHeight = viewRect.height / dstRect.height;
    purple::Point ph{dstRect.left , dstRect.top};
    purple::ScaleWithPoint(ph , scaleHeight , viewCenter);
    int hLeft = ph.x;
    int hTop = ph.y;
    
    float scale = 1.0f;

    const int viewLeft = viewRect.left;
    const int viewTop = viewRect.top;

    if(isCrop){
        dstRect= viewRect;
        // std::cout << "scale Width " << scaleWidth  
        //     << "  scale Height: " << scaleHeight << std::endl;
        // std::cout << "Width wleft " << wLeft << " wTop: " << wTop  
        //     << " Height hLeft: " << hLeft << "  hTop: " << hTop << std::endl;

        if(wLeft >= viewLeft && wTop < viewTop){
            scale = scaleHeight;
        }else if(hLeft > viewRect.left && hTop >= viewTop) {
            scale = scaleWidth;
        }
        // std::cout << "use scale = " << scale << std::endl;

        purple::Rect copySrcRect;
        copySrcRect.width = scale * srcRect.width;
        copySrcRect.height = scale * srcRect.height;
        copySrcRect.left = 0.0f;
        copySrcRect.top = copySrcRect.height;

        float deltaW = (copySrcRect.width - dstRect.width) / 2.0f;
        float deltaH = (copySrcRect.height - dstRect.height) / 2.0f;
        copySrcRect.left = copySrcRect.left + deltaW;
        copySrcRect.top = copySrcRect.top - deltaH;
        copySrcRect.width = copySrcRect.width - 2.0f * deltaW;
        copySrcRect.height = copySrcRect.height - 2.0f * deltaH;

        const float inverseScale = 1.0f / scale;
        srcRect.left = copySrcRect.left * inverseScale;
        srcRect.top = copySrcRect.top * inverseScale;
        srcRect.width = copySrcRect.width * inverseScale;
        srcRect.height = copySrcRect.height * inverseScale;
    }else{
        if(wLeft >= viewLeft && wTop < viewTop){
            scale = scaleWidth;
            dstRect.left = wLeft;
            dstRect.top = wTop;
        }else if(hLeft > viewRect.left && hTop >= viewTop){
            scale = scaleHeight;
            dstRect.left = hLeft;
            dstRect.top = hTop;
        }else{
            // std::cout << "set scale 0" << std::endl;
            scale = 0.0f;
        }

        dstRect.width = dstRect.width * scale;
        dstRect.height = dstRect.height * scale;
    }

    return dstRect;
}



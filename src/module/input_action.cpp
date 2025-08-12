#include "input_action.h"

void InputAction::setOnClickListener(std::function<void()> click){
    this->onClick = click;
}

bool InputAction::onTouchEvent(purple::InputEvent &e){
    float x = e.x;
    float y = e.y;

    bool ret = false;
    purple::Rect detectRect = this->getHitRect();
    // purple::Log::w("InputAction","detectRect(%f,%f,%f,%f)  x,y %f,%f",
        // detectRect.left,detectRect.top , detectRect.width , detectRect.height
        // , x, y); 
    switch(e.action){
        case purple::EVENT_ACTION_BEGIN:
            if(detectRect.isPointInRect(x,y)){
                // std::cout << "detectRect is in point" << std::endl;
                ret = true;
            }
            break;
        case purple::EVENT_ACTION_END:
            if(detectRect.isPointInRect(x,y)){
                ret = true;
                if(onClick != nullptr){
                    onClick();
                }
            }
            break;
        default:
        break;
    }//end switch

    // std::cout << "InputAction::onTouchEvent end" << std::endl;
    return ret;
}


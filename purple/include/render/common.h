#pragma once

#include "glm/vec4.hpp"
#include <cmath>
#include <string>

namespace purple{

    const float FONT_DEFAULT_SIZE = 64.0f;
    // const float FONT_DEFAULT_SIZE = 32.0f;
    
    //
    struct Point{
        float x;
        float y;

        Point(){
            x = 0.0f;
            y = 0.0f;
        }

        Point(float _x , float _y):x(_x) , y(_y){}
    };

    enum ShapeType{
        ShapeCircle = 0,
        ShapeRect = 1,
        ShapeRoundRect = 2,
        ShapeOval = 3,
        ShapeBlurCircle = 4,
        ShapeLinearGradientRect = 5,
        ShapeBlurRect = 6,
        ShapeBlurRoundRect = 7,
        ShapeTriangle = 8,
    };

    enum TextStyle{
        normal = 0,//普通
        italic = 1 //斜体
    };
    
    enum TextGravity{
        TopLeft,
        TopCenter,
        TopRight,
        BottomLeft,
        BottomCenter,
        BottomRight,
        CenterLeft,
        CenterRight,
        Center
    };

    typedef TextGravity LayoutGravity;

    enum FillStyle{
        Filled = 0,
        Stroken = 1
    };


    //矩形
    struct Rect{
        float left;
        float top;
        float width;
        float height;
        
        Rect(float l , float t , float w , float h){
            left = l;
            top = t;
            width = w;
            height = h;
        }
        Rect(){
            left = 0.0f;
            top = 0.0f;
            width = 0.0f;
            height = 0.0f;
        }

        inline float getTop(){
            return top;
        }

        inline float getLeft(){
            return left;
        }

        inline float getRight(){
            return left + width;
        }

        inline float getBottom(){
            return top - height;
        }

        inline Point center() {
            Point p;
            p.x = (left + getRight()) / 2.0f;
            p.y = (getBottom() + top) / 2.0f;
            return p;
        }

        inline bool isPointInRect(float x , float y){
            return x >= left && x <= left + width && y <= top && y >= top - height;
        }
    };

    //矩形
    struct RectI{
        int left;
        int top;
        int width;
        int height;

        inline Rect toRectF(){
            Rect rect;
            rect.left = static_cast<float>(left);
            rect.top = static_cast<float>(top);
            rect.width = static_cast<float>(width);
            rect.height = static_cast<float>(height);
            return rect;
        }
        
        inline bool isPointInRect(float x , float y){
            return x >= left && x <= left + width && y <= top && y >= top - height;
        }
    };

    //文本绘制 配置
    struct TextPaint{
        float textSizeScale = 1.0f;
        float gapSize = 0.0f;
        glm::vec4 textColor = glm::vec4(1.0f , 1.0f , 1.0f , 1.0f);
        TextStyle textStyle = normal;
        TextGravity textGravity = TopLeft;
        std::string fontName;
        float fontWeight = 50.0f;

        inline void setTextSize(float size){
            textSizeScale = (size / FONT_DEFAULT_SIZE);
        }
        
        float getTextFontHeight(){
            return textSizeScale * FONT_DEFAULT_SIZE;
        }
    };

    //形状绘制 配置参数
    struct Paint{
        glm::vec4 color = glm::vec4(1.0f , 1.0f , 1.0f , 1.0f);
        FillStyle fillStyle = Filled;
        
        float stokenWidth = 1.0f;
        float shadowRadius = 0.0f;//阴影半径
        
        //阴影偏移
        float shadowDx = 0.0f;
        float shadowDy = 0.0f;

        //
        bool texFlip = false;
    };

    inline bool isPointInRect(Rect &rect,float x , float y){
        return x >= rect.left && x <= rect.getRight()
             && y <= rect.top && y >= rect.getBottom();
    }

    const float SPACE_WIDTH = 16.0f;

    //字符信息
    struct CharInfo{
        std::wstring value; 
        float width;
        float height;
        float bearingX;
        float bearingY;
        float textureCoords[5];
        unsigned int textureId;
    };

    typedef glm::vec4 ColorType;
}



  

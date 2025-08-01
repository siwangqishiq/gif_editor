#include "render/render.h"
#include "log.h"
#include "render/vram.h"
#include "render/texture.h"
#include "resource/asset_manager.h"
#include "json.h"
#include "glheader.h"
#include "purple.h"

#include "render/render_batch.h"
#include "render/vram.h"
#include "render/render.h"
#include "render/text_render.h"
#include "render/cmd/command.h"
#include "render/cmd/cmd_arc.h"
#include "render/cmd/cmd_custom_shader.h"
#include "render/cmd/cmd_custom_texture_shader.h"
#include "render/cmd/cmd_lines_smooth.h"
#include "render/cmd/cmd_lines.h"
#include "render/cmd/cmd_rect.h"
#include "render/cmd/cmd_shape.h"
#include "render/cmd/cmd_text.h"
#include "render/cmd/cmd_triangles.h"
#include "render/cmd/cmd_round_rect.h"
#include "render/cmd/cmd_circle.h"
#include "render/cmd/cmd_circle_shadow.h"


namespace purple{
    std::unordered_map<wchar_t , wchar_t> SymbolMap;

    void RenderEngine::free(){
        if(shapeBatch_ != nullptr){
            shapeBatch_->dispose();
        }
        // VRamManager::getInstance()->clear();
        vramManager_->clear();
        TextureManager::getInstance()->clear();
        ShaderManager::getInstance()->clear();

        AudioManager::getInstance()->dispose();
        
    }

    void RenderEngine::clearRenderCommands(){
        resetDepth();
        renderCommandList_.clear();
    }

    void RenderEngine::onScreenResize(){
    //    viewWidth_ = appContext_->viewWidth_;
    //    viewHeight_ = appContext_->viewHeight_;
        resetNormalMat(purple::Engine::ScreenWidth , purple::Engine::ScreenHeight);
        glViewport(0 , 0 , purple::Engine::ScreenWidth , purple::Engine::ScreenHeight);
        
        //打开混合模式 文字有透明度
        glContextInit();
    }

    void RenderEngine::glContextInit(){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        // glDepthRangef(0.0f, 10.0f);
        // glDepthFunc(GL_GREATER);
        // glClearDepthf(1.0f);
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    }

    void RenderEngine::init(InitParams &params){
        Log::i(TAG , "render engine init start");
        
        clearColor = params.clearColor;

        vramManager_ = std::make_shared<VRamManager>();
        
        loadShapeShader();
        shapeBatch_ = std::make_shared<ShapeBatch>(this);//std::shared_ptr<RenderEngine>(this)
        shapeBatch_->init();

        spriteBatch_ = std::make_shared<SpriteBatch>(this);
        spriteBatch_->init();

        clearRenderCommands();
        
        loadTextRenderResource();//text render init
        // Logi(TAG , "render engine init end");
    }

    void RenderEngine::loadTextRenderResource(){
        Log::i(TAG , "render init loadTextRenderResource");
        // textRenderHelper_ = std::make_shared<TextRenderHelper>();
        // textRenderHelper_->loadRes(*this);
        // TextRenderHelper::loadSymbolMap();

        //new add text render
        // loadTextRender("text/heiti.ttf");
        // loadTextRender("text/zhuhai.ttf");
        loadTextRender("text/fangzheng.ttf");
    }

    void RenderEngine::loadShapeShader(){
        Log::i(TAG , "render init loadShapeShader");
        
        //实心矩形
        ShaderManager::getInstance()->loadAssetShader("primitive_rect" , 
            "shader/shape/primitive_vert.glsl", "shader/shape/primitive_rect_frag.glsl");

        // GL_LINES 方式画线
        ShaderManager::getInstance()->loadAssetShader("primitive_lines" , 
            "shader/shape/primitive_lines_vert.glsl", "shader/shape/primitive_lines_frag.glsl");
        //render smooth lines
        ShaderManager::getInstance()->loadAssetShader("primitive_lines_smooth" , 
            "shader/shape/primitive_smooth_lines_vert.glsl", "shader/shape/primitive_smooth_lines_frag.glsl");
        
        //arc
        ShaderManager::getInstance()->loadAssetShader("primitive_arc" , 
            "shader/shape/primitive_arc_vert.glsl", "shader/shape/primitive_arc_frag.glsl");

        //triangle
        ShaderManager::getInstance()->loadAssetShader("primitive_triangles",
            "shader/shape/primitive_triangles_vert.glsl", "shader/shape/primitive_triangles_frag.glsl");
    }

    void RenderEngine::resetNormalMat(float w , float h){
        // Log::i(TAG , "reset normal matrix %f , %f" , w , h);
        normalMatrix_[0][0] = 2.0f / w;
        normalMatrix_[0][1] = 0.0f;
        normalMatrix_[0][2] = 0.0f;

        normalMatrix_[1][0] = 0.0f;
        normalMatrix_[1][1] = 2.0f / h;
        normalMatrix_[1][2] = 0.0f;

        normalMatrix_[2][0] = -1.0f;
        normalMatrix_[2][1] = -1.0f;
        normalMatrix_[2][2] =  1.0f;
    }

    std::shared_ptr<TextRender> RenderEngine::getTextRenderByName(std::string name){
        std::shared_ptr<TextRender> result = textRenderMap_[name];
        if(result == nullptr){
            result = textRenderMap_[DEFAULT_TEXT_RENDER_NAME];
        }
        return result;
    }

    bool RenderEngine::loadTextRender(std::string assetFontFile){
        std::shared_ptr<TextRender> defaultTextRender = std::make_shared<TextRender>(this);
        int ret = defaultTextRender->loadFontRes(DEFAULT_TEXT_RENDER_NAME,assetFontFile , true);
        if(ret){
            textRenderMap_[DEFAULT_TEXT_RENDER_NAME] = defaultTextRender;
            return true;
        }
        return false;
    }


    void RenderEngine::submitRenderCommand(std::shared_ptr<RenderCommand> cmd) {
        renderCommandList_.push_back(cmd);
    }

    //绘制自定义shader
    void RenderEngine::renderShader(Shader &shader , 
                                    Rect &showRect , 
                                    std::function<void(void)> preRenderCallback){
        auto cmd = fetchShaderRenderCommand(this);
        cmd->putParams(shader ,showRect);
        cmd->setPreRenderCallback(preRenderCallback);

        cmd->runCommands();
    }

    void RenderEngine::renderText(std::wstring &text , 
            float left , float bottom , TextPaint &paint){
        renderTextV2(text, left , bottom , paint);
    }

    void RenderEngine::renderTextWithRect(std::wstring &text , Rect &showRect , 
            TextPaint &paint, TextRenderOutInfo *wrapContentRect){
        renderTextWithRectV2(text , showRect , paint , wrapContentRect);
    }

    std::shared_ptr<TextRenderCommand> RenderEngine::fetchTextRenderCommand(RenderEngine *engine){
        // new a new instace 
        // later use pool to reuse
        auto newCmd = 
            std::make_shared<TextRenderCommand>(this);
        newCmd->used = true;
        return newCmd;
    }

    std::shared_ptr<ShapeBatch> RenderEngine::getShapeBatch(){
        return shapeBatch_;
    }

    std::shared_ptr<ShaderRenderCommand> RenderEngine::fetchShaderRenderCommand(RenderEngine *engine){
        auto newCmd = 
            std::make_shared<ShaderRenderCommand>(this);
        newCmd->used = true;
        return newCmd;
    }

    void TextRenderHelper::loadRes(RenderEngine &engine){
        textRenderShader_ = ShaderManager::getInstance()->loadAssetShader("text_render" 
                                    , "shader/render_text_vert.glsl"
                                    , "shader/render_text_frag.glsl");
        
        buildTextCharConfig();
    }

    std::shared_ptr<CharInfo> TextRenderHelper::findCharInfo(wchar_t &ch){
        auto result = charInfoMaps_[ch];
        if(result == nullptr){
            // Logi("debug" , "in charinfos  %d not found" , ch);
            result = charInfoMaps_[L'*'];
        }
        return result;
    }

    //读取字符配置
    void TextRenderHelper::buildTextCharConfig(){
        
        std::string fontFoldName = "font1/";

        std::wstring charConfigStr = AssetManager::getInstance()->readAssetTextFile("text/"+ fontFoldName +"char_config.json");

        if(charConfigStr.empty()){
            return;
        }
        
        JsonParser parser;
        auto configJson = parser.parseJsonObject(charConfigStr);
        auto charJsonList = configJson->getJsonArray("list");
        Log::i("text_render" , "charJsonList size : %d" , charJsonList->size());
        
        //load texture
        auto textureFileList = configJson->getJsonArray("textureFiles");
        std::vector<std::string> filelist;
        for(int i = 0 ; i < textureFileList->size() ;i++){
            std::string filename = ToByteString(textureFileList->getString(i));
            Log::i("debug" , "filename: %s" , filename.c_str());
            filelist.push_back("text/" + fontFoldName +filename);
        }//end for i
        // auto textureInfo = TextureManager::getInstance()->acquireTexture("text/" + textureName);
        auto textureInfo = TextureManager::getInstance()->loadTextureArray(filelist);
        Log::i("debug" , "load texture %d" , glGetError());
        if(textureInfo != nullptr){
            mainTextureId_ = textureInfo->textureId;
        }else{
            Log::e("TextRenderHelper" , "error to load textureInfo");
            return;
        }

        Log::i("TextRenderHelper" , "load texture name : %s , width : %d , height : %d" , 
            textureInfo->name.c_str(),
            textureInfo->width,
            textureInfo->height);

        charInfoMaps_.clear();
        for(int i = 0 ;  i< charJsonList->size();i++){
            auto itemJson = charJsonList->getJsonObject(i);
            std::shared_ptr<CharInfo> info = std::make_shared<CharInfo>();
            info->value = itemJson->getString("value");
            info->width = itemJson->getInt("width");
            info->height = itemJson->getInt("height");
            info->bearingX = itemJson->getInt("bearingX");
            info->bearingY = itemJson->getInt("bearingY");
            auto texCoordsArray = itemJson->getJsonArray("texCoords");
            for(int i = 0 ; i < texCoordsArray->size(); i++){
                info->textureCoords[i] = texCoordsArray->getFloat(i);
            }//end for i

            //load texture
            // auto textureName = itemJson->getString("texture");
            // std::string textureName = ToByteString(itemJson->getString("texture"));
            // Logi("text_render" , "textureName coord: %f , %f , %f , %f" , 
            //     info->textureCoords[0] ,info->textureCoords[1],
            //     info->textureCoords[2] ,info->textureCoords[3] );
            // Logi("text_render" , "texture value size %d" , info->value.size());
            // Logi("text_render" , "texture name  %s" , textureName.c_str());
            // mainTextureId_ = info->textureId;
            // // Logi("text_render" , "textureId : %d" , info->textureId);
            charInfoMaps_.insert(std::make_pair<>(info->value[0] , info));
        }//end for i

        Log::i("text_render" , "charInfoMaps size : %d" , charInfoMaps_.size());
        Log::i("text_render" , "texture : %s" , 
                TextureManager::getInstance()->allTextureInfos().c_str());

        addSpecialTextCharInfo();
    }

    //添加特殊字符配置
    void TextRenderHelper::addSpecialTextCharInfo(){
        auto blankCharInfo = std::make_shared<CharInfo>();
        blankCharInfo->textureId = 0;
        blankCharInfo->value = L" ";
        blankCharInfo->width = SPACE_WIDTH;
        blankCharInfo->height = 0.0f;
        blankCharInfo->textureCoords[0] = 0.0f;
        blankCharInfo->textureCoords[1] = 0.0f;
        blankCharInfo->textureCoords[2] = 0.0f;
        blankCharInfo->textureCoords[3] = 0.0f;
        charInfoMaps_.insert(std::make_pair<>(blankCharInfo->value[0] , blankCharInfo));

        auto tabCharInfo = std::make_shared<CharInfo>();
        tabCharInfo->textureId = 0;
        tabCharInfo->value = L"\t";
        tabCharInfo->width = 4*SPACE_WIDTH;
        tabCharInfo->height = 0.0f;
        tabCharInfo->textureCoords[0] = 0.0f;
        tabCharInfo->textureCoords[1] = 0.0f;
        tabCharInfo->textureCoords[2] = 0.0f;
        tabCharInfo->textureCoords[3] = 0.0f;
        charInfoMaps_.insert(std::make_pair<>(tabCharInfo->value[0] , tabCharInfo));

        auto rCharInfo = std::make_shared<CharInfo>();
        rCharInfo->textureId = 0;
        rCharInfo->value = L"\r";
        rCharInfo->width = 0.0f;
        rCharInfo->height = 0.0f;
        rCharInfo->textureCoords[0] = 0.0f;
        rCharInfo->textureCoords[1] = 0.0f;
        rCharInfo->textureCoords[2] = 0.0f;
        rCharInfo->textureCoords[3] = 0.0f;
        charInfoMaps_.insert(std::make_pair<>(rCharInfo->value[0] , rCharInfo));

        auto nCharInfo = std::make_shared<CharInfo>();
        nCharInfo->textureId = 0;
        nCharInfo->value = L"\n";
        nCharInfo->width = 0.0f;
        nCharInfo->height = 0.0f;
        nCharInfo->textureCoords[0] = 0.0f;
        nCharInfo->textureCoords[1] = 0.0f;
        nCharInfo->textureCoords[2] = 0.0f;
        nCharInfo->textureCoords[3] = 0.0f;
        charInfoMaps_.insert(std::make_pair<>(nCharInfo->value[0] , nCharInfo));
    }

    void TextRenderHelper::loadSymbolMap(){
        std::wstring symbolStr = L"1234567890!@#$%^&*()_[]{}:;,.<>/?abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ，";
        for(wchar_t ch : symbolStr){
            SymbolMap.insert({ch , ch});
        }
        Log::i("isymbol" , "map size = %d " , SymbolMap.size());
    }

    bool TextRenderHelper::isSymbol(std::wstring &ch){
        return SymbolMap.find(ch[0]) != SymbolMap.end();
    }

    std::shared_ptr<TextRenderCommand> RenderCommandCache::acquireTextRender(std::wstring &content , Rect &rect ,TextPaint &paint){
        return nullptr;
    }

    std::shared_ptr<ShapeRenderCommand> RenderEngine::fetchShaderShapeRenderCommand(RenderEngine *engine){
        auto newCmd = 
            std::make_shared<ShapeRenderCommand>(this);
        newCmd->used = true;
        return newCmd;
    }


    ////绘制圆形
    //void RenderEngine::renderCircle(float cx , float cy , float radius , Paint &paint){
    //    Rect rect;
    //    rect.left = cx - radius;
    //    rect.top = cy + radius;
    //    rect.width = 2 * radius;
    //    rect.height = 2 * radius;
    //
    //    long long t1 = currentTimeMicro();
    //    auto cmd = fetchShaderShapeRenderCommand(this);
    //    cmd->putParams(rect , paint, ShapeType::ShapeCircle);
    //    long long t2 = currentTimeMicro();
    //     // Logi("renderCircle" , "put params time: %lld", (t2 - t1));
    //    submitRenderCommand(cmd);
    //}

    ////绘制矩形
    //void RenderEngine::renderRect(Rect &rectangle ,Paint &paint){
    //    auto cmd = fetchShaderShapeRenderCommand(this);
    //    cmd->putParams(rectangle , paint, ShapeType::ShapeRect);
    //    submitRenderCommand(cmd);
    //}

    //精灵类批量渲染
    std::shared_ptr<SpriteBatch> RenderEngine::getSpriteBatch(){
        return spriteBatch_;
    }

    float RenderEngine::getAndChangeDepthValue(){
        depthValue -= 0.00001f;
        // const float result = depthValue;
        return depthValue;    
    }

    void RenderEngine::resetDepth(){
        depthValue = 1.0f;
    }

    //绘制文字
    void RenderEngine::renderTextV2(
        const wchar_t *text , 
        float left , 
        float bottom , 
        TextPaint &paint) {
        auto textRender = getTextRenderByName(paint.fontName);
        if(textRender != nullptr){
            auto str = std::wstring(text);
            textRender->renderText(str , left , bottom , paint);
        }
    }

    void RenderEngine::renderTextV2(
            std::wstring &text , 
            float left , 
            float bottom , 
            TextPaint &paint){
        auto textRender = getTextRenderByName(paint.fontName);
        if(textRender != nullptr){
            textRender->renderText(text , left , bottom , paint);
        }
    }

    void RenderEngine::renderTextWithRectV2(std::wstring &text, 
            Rect &showRect, 
            TextPaint &paint, 
            TextRenderOutInfo *outInfo) {
        auto textRender = getTextRenderByName(paint.fontName);
        if(textRender != nullptr){
            textRender->renderTextWithRect(text , showRect , paint , outInfo);
        }
    } 

    void RenderEngine::preCalculateTextSize(std::wstring &text, 
            TextPaint &paint, int maxWidth, Rect &outInfo){
        auto textRender = getTextRenderByName(paint.fontName);
        if(textRender != nullptr){
            textRender->preCalTextRect(text , paint , maxWidth , outInfo);
        }
    }


    /**
     * @brief  text layout calculate
     *
     * @param content
     * @param renderCmd
     * @param outRect
     * @param buf
     */
    void TextRenderHelper::layoutText(std::wstring &content, 
            TextRenderCommand *renderCmd,
            Rect &outRect,
            std::vector<float> &buf){
        
        TextPaint paint = renderCmd->paint_;
        Rect limitRect = renderCmd->limitRect_;
        
        outRect.left = limitRect.left;
        outRect.top = limitRect.top;
        outRect.width = 0.0f;
        outRect.height =(FONT_DEFAULT_SIZE + paint.gapSize) * paint.textSizeScale;

        float maxBaselineY = 0.0f;
        float lineWidth = 0.0f;
        bool isFirstLine = true;

        float x = limitRect.left;
        float y = limitRect.top;
        float depthValue = renderCmd->engine_->getAndChangeDepthValue();

        int index = 0;
        int realRenderCharCount = 0;
        const int size = content.length();
        while(index < size){
            wchar_t ch = content[index];

            auto charInfoPtr = findCharInfo(ch);

            if(isFirstLine){
                float bearingY = charInfoPtr->bearingY * paint.textSizeScale;
                if(maxBaselineY <= bearingY){
                    maxBaselineY = bearingY;
                }
            }

            float charRealWidth = (charInfoPtr->width + paint.gapSize) * paint.textSizeScale;

            if(x + charRealWidth <= limitRect.getRight() && ch != L'\n'){
                renderCmd->putVertexDataToBuf(buf , index , x , y ,depthValue,
                    charInfoPtr , paint);
                
                x += charRealWidth;
                lineWidth += charRealWidth;
                if(outRect.width < lineWidth){
                    outRect.width = lineWidth;
                }
                index++;
                realRenderCharCount = index;
            }else{// change a new line
                isFirstLine = false;

                x = limitRect.left;
                y -= (FONT_DEFAULT_SIZE + paint.gapSize) * paint.textSizeScale;
                if(y - maxBaselineY < limitRect.getBottom()){
                    break;
                }

                outRect.height += (FONT_DEFAULT_SIZE + paint.gapSize) * paint.textSizeScale;
                lineWidth = 0.0f;
                
                if(ch == L'\n'){
                    index++;
                }
            }
        }//end while

        // if(realRenderCharCount < content.size()){
        //     buf.resize(realRenderCharCount * renderCmd->vertCountPerChar_);
        // }

        float translateX = limitRect.left - outRect.left;
        float translateY = -maxBaselineY;
        
        switch(paint.textGravity){
            case TopLeft:
                break;
            case TopCenter:
                translateX += (limitRect.width / 2.0f - outRect.width / 2.0f);
                outRect.left += translateX;
                break;
            case TopRight:
                translateX += (limitRect.width - outRect.width);
                outRect.left += translateX;
                break;
            case BottomLeft:
                translateY -= (limitRect.height - outRect.height);
                outRect.top += translateY + maxBaselineY;
                break;
            case BottomCenter:
                translateX += (limitRect.width / 2.0f - outRect.width / 2.0f);
                outRect.left += translateX;
                translateY -= (limitRect.height - outRect.height);
                outRect.top += translateY + maxBaselineY;
                break;
            case BottomRight:
                translateX += (limitRect.width - outRect.width);
                outRect.left += translateX;
                translateY -= (limitRect.height - outRect.height);
                outRect.top += translateY + maxBaselineY;
                break;
            case CenterLeft:
                translateY -= (limitRect.height / 2.0f - outRect.height / 2.0f);
                outRect.top += translateY + maxBaselineY;
                break;
            case CenterRight:
                translateX += (limitRect.width - outRect.width);
                outRect.left += translateX;
                translateY -= (limitRect.height / 2.0f - outRect.height / 2.0f);
                outRect.top += translateY + maxBaselineY;
                break;
            case Center:
                translateX += (limitRect.width / 2.0f - outRect.width / 2.0f);
                outRect.left += translateX;
                translateY -= (limitRect.height / 2.0f - outRect.height / 2.0f);
                outRect.top += translateY + maxBaselineY;
                break;
        }//end switch

        for(int i = 0 ; i < realRenderCharCount ;i++){
            renderCmd->updateVertexPositionData(buf , i , translateX , translateY);
        }//end for i
    }

    void RenderEngine::renderRect(Rect &rect , glm::mat4 &transMat , 
            Paint &paint){
        if(paint.fillStyle == Stroken){
            std::vector<float> points(2 * 5);
            fillLinesFromRect(rect , transMat, points);
            renderLines(points , paint);
        }else{
            RectRenderCommand cmd(this);
            Shader rectShader = ShaderManager::getInstance()->getShaderByName("primitive_rect");
            cmd.putParams(rectShader , rect ,transMat, paint);
            cmd.runCommands();
        }
    }
    
    void RenderEngine::renderRoundRect(
            Rect &rect , 
            float leftTopRadius,
            float rightTopRadius,
            float leftBottomRadius,
            float rightBottomRadius,
            glm::mat4 &transMat , 
            Paint &paint){
        RoundRectRenderCommand cmd(this);
        cmd.putParams(rect ,transMat, paint);
        cmd.runCommands();
    }

    void RenderEngine::renderCircle(float cx , float cy , float radius, 
        Paint &paint){
        CircleRenderCommand cmd(this);
        cmd.putParams(cx, cy, radius , paint);
        cmd.runCommands();
    }

    void RenderEngine::renderCircleShadow(float cx , float cy, float radius, 
        Paint &paint){
        CircleShadowRenderCommand cmd(this);
        cmd.putParams(cx, cy, radius , paint);
        cmd.runCommands();
    }

    void RenderEngine::renderArc(float cx , float cy , float radius , 
                float beginAngle ,float endAngle , bool isReverse , Paint &paint){
        ArcRenderCommand cmd(this);
        Shader arcShader = ShaderManager::getInstance()->getShaderByName("primitive_arc");
        if(paint.fillStyle == Stroken){
            radius += (paint.stokenWidth * 0.5f);
        }
        Rect rect(cx - radius , cy + radius , radius * 2.0f , radius * 2.0f);
        cmd.putParams(arcShader , rect,beginAngle , endAngle, isReverse , paint);
        cmd.runCommands();        
    }

    void RenderEngine::renderLines(std::vector<float> &points , Paint &paint){
        if(paint.stokenWidth < 1.0f){
            return;
        }
        
        LinesRenderCommand cmd(this);
        Shader shader = ShaderManager::getInstance()->getShaderByName("primitive_lines");
        cmd.putParams(shader , points , paint);
        cmd.runCommands();
    }

    void RenderEngine::renderSmoothLines(std::vector<float> &points , Paint &paint){
        if(paint.stokenWidth < 1.0f){
            return;
        }

        LinesSmoothRenderCommand cmd(this);
        Shader shader = ShaderManager::getInstance()->getShaderByName("primitive_lines_smooth");
        cmd.putParams(shader , points , paint);
        cmd.runCommands();
    }


    //三角形绘制
    void RenderEngine::renderTriangle(
                        float p1x,  float p1y, 
                        float p2x , float p2y,
                        float p3x , float p3y,
                        glm::mat4 &mat, Paint &paint){
        std::vector<float> posVec = {p1x , p1y , p2x , p2y , p3x , p3y};
        renderTriangles(posVec , mat , paint);
    }

    //三角形绘制
    void RenderEngine::renderTriangles(std::vector<float> &vertex ,glm::mat4 &mat, Paint &paint){
        TrianglesRenderCommand cmd(this);
        Shader trianglesShader = ShaderManager::getInstance()->getShaderByName("primitive_triangles");
        // std::cout << "vertex " << vertex[0]<< ", " << vertex[1]
        //     << "  , "<< vertex[2]<< ", " << vertex[3]
        //     << "  , "<< vertex[4]<< ", " << vertex[5] << std::endl;
        cmd.putParams(trianglesShader , vertex , paint , mat);
        cmd.runCommands();
    }

    //自定义带纹理的shader
    void RenderEngine::renderTextureShader(
            Shader &shader , 
            Rect &showRect , 
            unsigned int textureId,
            std::function<void(void)> preRenderCallback){
        CustomTextureShaderRenderCommand cmd(this);
        cmd.putParams(shader , showRect , textureId);
        cmd.setPreRenderCallback(preRenderCallback);
        cmd.runCommands();
    }

    void RenderEngine::fillLinesFromRect(Rect &rect , glm::mat4 &transMat, std::vector<float> &buf){
        glm::vec4 value;

        value[0] = rect.left;
        value[1] = rect.top;
        value[2] = 0.0f;
        value[3] = 1.0f;
        value = transMat * value;
        buf[0] = value[0];
        buf[1] = value[1];


        value[0] = rect.getRight();
        value[1] = rect.top;
        value[2] = 0.0f;
        value[3] = 1.0f;
        value = transMat * value;
        buf[2] = value[0];
        buf[3] = value[1];

        value[0] = rect.getRight();
        value[1] = rect.getBottom();
        value[2] = 0.0f;
        value[3] = 1.0f;
        value = transMat * value;
        buf[4] = value[0];
        buf[5] = value[1];

        value[0] = rect.left;
        value[1] = rect.getBottom();
        value[2] = 0.0f;
        value[3] = 1.0f;
        value = transMat * value;
        buf[6] = value[0];
        buf[7] = value[1];

        buf[8] = buf[0];
        buf[9] = buf[1];
    }

    std::shared_ptr<TextureInfo> RenderEngine::buildVirtualTexture(
            std::string texName,
            int texWidth , 
            int texHeight ,
            std::function<void(int , int)> renderFn){
        //craete a virtual texture
        std::shared_ptr<TextureInfo> ret = TextureManager::getInstance()->createEmptyTexture(texName , texWidth , texHeight , GL_RGBA);
        if(ret == nullptr){
            Log::e("buildVirtualTexture" , "createEmptyTexture error");
            return nullptr;
        }

        ret->category = TextureCategory::VIRTUAL_TEX;
        const int viewWidth = texWidth;
        const int viewHeight = texHeight;

        int errCode = createFrameBufferForVirtualTexture(ret);
        if(errCode < 0){
            Log::e("buildVirtualTexture" , "createFrameBufferForVirtualTexture error");
            return nullptr;
        }

        glBindFramebuffer(GL_FRAMEBUFFER , ret->framebufferId);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            Log::e("buildVirtualTexture" , "framebuffer state is error");
            glBindFramebuffer(GL_FRAMEBUFFER , 0);
            return nullptr;
        }
            
        glClearColor(0.0f , 0.0f, 0.0f , 0.0f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0 , 0 , viewWidth , viewHeight);
        resetNormalMat(viewWidth , viewHeight);
        
        if(renderFn != nullptr){
            renderFn(viewWidth , viewHeight); // user custom render function callback    
        }

        glBindTexture(GL_TEXTURE_2D , ret->textureId);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D , 0);

        glBindFramebuffer(GL_FRAMEBUFFER , 0);
        onScreenResize();
        return ret;
    }

    int RenderEngine::createFrameBufferForVirtualTexture(std::shared_ptr<TextureInfo> texInfo){
        if(texInfo == nullptr || texInfo->textureId == 0){
            return -1;
        }

        GLuint frameBufferIds[1];
        glGenFramebuffers(1 , frameBufferIds);
        texInfo->framebufferId = frameBufferIds[0];
        glBindFramebuffer(GL_FRAMEBUFFER , texInfo->framebufferId);

        GLuint  renderBufferIds[1];
        glGenRenderbuffers(1 , renderBufferIds);
        texInfo->renderBufferId = renderBufferIds[0];

        glBindRenderbuffer(GL_RENDERBUFFER , texInfo->renderBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8 
            , texInfo->width , texInfo->height);
        glBindRenderbuffer(GL_RENDERBUFFER , 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER , GL_COLOR_ATTACHMENT0 ,
                            GL_TEXTURE_2D , texInfo->textureId , 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER , GL_DEPTH_STENCIL_ATTACHMENT ,
                                GL_RENDERBUFFER , texInfo->renderBufferId);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) !=GL_FRAMEBUFFER_COMPLETE){
            Log::e("createFrameBuffer" , "createFrameBuffer error.");
            glBindFramebuffer(GL_FRAMEBUFFER , 0);
            return -1;
        }

        glBindFramebuffer(GL_FRAMEBUFFER , 0);
        return 0;
    }

    //更新虚拟纹理 
    void RenderEngine::updateVirtualTexture(std::shared_ptr<TextureInfo> texInfo ,std::function<void(int , int)> renderFn){
        if(texInfo == nullptr || texInfo->category != TextureCategory::VIRTUAL_TEX){
            Log::e("updateVirtualTexture" , "texture state error");
            return;
        }

        const int viewWidth = texInfo->width;
        const int viewHeight = texInfo->height;
        
        glBindFramebuffer(GL_FRAMEBUFFER , texInfo->framebufferId);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            Log::e("updateVirtualTexture" , "framebuffer state is error");
            glBindFramebuffer(GL_FRAMEBUFFER , 0);
            return;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
        
        glClearColor(0.0f , 0.0f, 0.0f , 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0 , 0 , viewWidth , viewHeight);
        resetNormalMat(viewWidth , viewHeight);

        renderFn(viewWidth , viewHeight); // user custom render function callback
        
        glBindTexture(GL_TEXTURE_2D , texInfo->textureId);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D , 0);

        glBindFramebuffer(GL_FRAMEBUFFER , 0);
        onScreenResize();
    }

    /**
     * @brief  载入字体 
     * 
     * @param fontName  字体名称  与TextPaint 中的名称对应
     * @param fontPath  字体文件路径 
     * 
     * @return true 
     * @return false 
     */
    bool RenderEngine::loadTextFontRes(std::string fontName, std::string fontPath , bool isAssetRes) {
        std::shared_ptr<TextRender> result = textRenderMap_[fontName];
        if(result != nullptr){
            return true;
        }

        std::shared_ptr<TextRender> theNewTextRender = std::make_shared<TextRender>(this);
        int ret = theNewTextRender->loadFontRes(fontName, fontPath , isAssetRes);
        if(ret >= 0){
            textRenderMap_[fontName] = theNewTextRender;
            return true;
        }
        Log::i(TAG , "load font file %s failed code: %d" , fontPath.c_str() , ret);
        return false;
    }

    void RenderEngine::beginScissor(int x,int y, int width, int height){
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, y , width , height);
    }

    void RenderEngine::endScissor(){
        glDisable(GL_SCISSOR_TEST);
    }
}


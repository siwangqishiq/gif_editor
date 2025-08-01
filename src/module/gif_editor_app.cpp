#include "gif_editor_app.h"
#include "app_util.h"
#include "utils.h"
#include "input/input_manager.h"
#include "input_action.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

// const char *FILE_PATH = "../assets/sharongzi.gif";
const char *FILE_PATH = "../assets/gakki.gif";
// const char *FILE_PATH = "../assets/test.gif";
// const char *FILE_PATH = "../assets/yaoren.mp4";

const char *TAG = "gif_editor";
const std::string GIF_EDITOR_INPUT = "gif_editor_input";

void GifEditorApp::onInit(){
    purple::Log::i("GifEditorApp","onInit");
    decodeGifFile(FILE_PATH);

    mMainView.init(this);

    purple::InputManager::getInstance()->removeEventListener(GIF_EDITOR_INPUT);
    purple::InputManager::getInstance()->addEventListener(GIF_EDITOR_INPUT,[this](purple::InputEvent e){
        // purple::Log::i("input_action" , "%d, (%f, %f)", e.action, e.x, e.y);
        handleInputAction(e);
        return true;
    });

    registerInputWidget(static_cast<InputAction *>(&mMainView)); 
}

int GifEditorApp::decodeGifFile(const char* filepath){
    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;

    const AVCodec *decoder = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;

    int videoSteamIdx = -1;

    avformat_network_init();

    if(avformat_open_input(&formatContext, filepath, nullptr, nullptr) < 0){
        purple::Log::e(TAG, "open file %s failed", filepath);
        return -1;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        purple::Log::e(TAG, "find steam info error");
        return -1;
    }

    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        purple::Log::w(TAG, "codec type : %d" , formatContext->streams[i]->codecpar->codec_type);
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoSteamIdx = i;
            break;
        }
    }//end for i

    if (videoSteamIdx == -1) {
        purple::Log::e(TAG, "Not found steam");
        return -1;
    }

    AVStream *stream = formatContext->streams[videoSteamIdx];
    AVCodecParameters *codecpar = stream->codecpar;
    
    decoder = avcodec_find_decoder(codecpar->codec_id);
    if (!decoder) {
        purple::Log::e(TAG, "Not found decoder");
        return -1;
    }

    codecContext = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(codecContext, codecpar);

    if (avcodec_open2(codecContext, decoder, nullptr) < 0) {
        purple::Log::e(TAG, "open decode error");
        return -1;
    }
    
    packet = av_packet_alloc();
    frame = av_frame_alloc();

    unsigned int frameCount = 0;
    bool isBreak = false;
    while(av_read_frame(formatContext, packet) >= 0){
        if (packet->stream_index == videoSteamIdx) {
            int ret = avcodec_send_packet(codecContext, packet);
            if(ret < 0){
                purple::Log::e(TAG, "send packet failed");
                break;
            }
            
            while ((ret = avcodec_receive_frame(codecContext, frame)) >= 0) {
                auto ptsTime = av_q2d(stream->time_base) * frame->pts;

                purple::Log::w(TAG, "decode image size %d,%d #%d pts:%f dts:%d pictype:%c pixformat:%s  lineSize:%d", 
                    frame->width, frame->height,
                    frameCount,
                    ptsTime,
                    frame->pkt_dts,
                    av_get_picture_type_char(frame->pict_type),
                    av_get_pix_fmt_name((AVPixelFormat)frame->format),
                    frame->linesize[0]);
                
                uint32_t width = frame->width;
                uint32_t height = frame->height;
                int src_stride = frame->linesize[0];
                
                const unsigned int colorSize = 4;
                uint8_t* dst = new uint8_t[width * height * colorSize];
                for (uint32_t y = 0; y < height; y++) {
                    memcpy(dst + y * width * colorSize, 
                            frame->data[0] + y * src_stride, 
                            width * colorSize);
                }//end for y

                onGetFrameImage(dst, frame->width, frame->height, ptsTime);
                delete[] dst;

                frameCount++;

                if(frameCount >= MAX_FRAME_COUNT){
                    isBreak = true;
                    break;
                }
            }//end while

            if(isBreak){
                break;
            }
        }//end if

        av_packet_unref(packet);
    }//end while

    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);

    return 0;
}

void GifEditorApp::onGetFrameImage(uint8_t *data, int w, int h , double pts){
    purple::Log::w(TAG, "onGetFrameImage");
    auto tex = purple::BuildImageByPixlData(data, w, h, GL_BGRA);
    purple::Log::w(TAG, "onGetFrameImage frameData");
    std::unique_ptr<ImageFrameData> frameData = std::make_unique<ImageFrameData>();
    frameData->pts = static_cast<float>(pts);
    frameData->tex = tex;

    this->frameList.push_back(std::move(frameData));
}

void GifEditorApp::registerInputWidget(InputAction *widget){
    unRegisterInputWidget(widget);
    inputWidgets.emplace_back(widget);
}

void GifEditorApp::unRegisterInputWidget(InputAction *widget){
    for(auto it = inputWidgets.begin() ; it != inputWidgets.end(); it++){
        if(*it == widget){
            inputWidgets.erase(it);
            break;
        }
    }//end for each
}

void GifEditorApp::handleInputAction(purple::InputEvent &e){
    if(catchedInputWidget != nullptr){
        catchedInputWidget->onTouchEvent(e);
        if(e.action == purple::EVENT_ACTION_END || e.action == purple::EVENT_ACTION_CANCEL){
            catchedInputWidget = nullptr;
            return;
        }
    }

    for(auto &widget : inputWidgets){
        if(widget->onTouchEvent(e)){
            catchedInputWidget = widget;
            break;
        }
    }//end for each;
}

long long GifEditorApp::getLastFrameDeltaTime(){
    if(timeMs < 0){
        return 0;
    }
    return purple::currentTimeMillis() - timeMs;
}

void GifEditorApp::onResize(int w , int h){
    onInit();
}

void GifEditorApp::onTick(){
    mMainView.render();
    timeMs = purple::currentTimeMillis();
}

void GifEditorApp::onDispose(){
    purple::InputManager::getInstance()->removeEventListener(GIF_EDITOR_INPUT);
    purple::Log::i("GifEditorApp","TestTextUi::onDispose");
}


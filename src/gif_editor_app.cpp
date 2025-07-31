#include "gif_editor_app.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

const char *FILE_PATH = "../assets/sharongzi.gif";
// const char *FILE_PATH = "../assets/yaoren.mp4";

const char *TAG = "gif_editor";

int decodeGifImage(const char *path){
    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;

    const AVCodec *decoder = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;

    int videoSteamIdx = -1;

    avformat_network_init();

    if(avformat_open_input(&formatContext, path, nullptr, nullptr) < 0){
        purple::Log::e(TAG, "open file %s failed", path);
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

    int frameCount = 0;
    while(av_read_frame(formatContext, packet) >= 0){
        if (packet->stream_index == videoSteamIdx) {
            int ret = avcodec_send_packet(codecContext, packet);
            if(ret < 0){
                purple::Log::e(TAG, "send packet failed");
                break;
            }
            
            while ((ret = avcodec_receive_frame(codecContext, frame)) >= 0) {
                auto ptsTime = av_q2d(stream->time_base) * frame->pts;

                purple::Log::w(TAG, "decode image size %d,%d #%d pts:%f dts:%d pictype:%c pixformat:%s", 
                    frame->width, frame->height,
                    frameCount,
                    ptsTime,
                    frame->pkt_dts,
                    av_get_picture_type_char(frame->pict_type),
                    av_get_pix_fmt_name((AVPixelFormat)frame->format));
                frameCount++;
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

void GifEditorApp::onInit(){
    purple::Log::i("GifEditorApp","onInit");
    decodeGifImage(FILE_PATH);

    
}

void GifEditorApp::onResize(int w , int h){
    onInit();
}

void GifEditorApp::onTick(){
    auto batch = purple::Engine::getRenderEngine()->getSpriteBatch();
    batch->begin();
    
    batch->end();
}

void GifEditorApp::onDispose(){
    purple::Log::i("GifEditorApp","TestTextUi::onDispose");
}


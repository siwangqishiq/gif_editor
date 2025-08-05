#include "decode.h"
#include "purple.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

const std::string TAG = "decode";

int DecodeGifFile(std::string path, 
    std::function<void(uint8_t *data, int w, int h , double pts)> onGetFrameImageFunc){
    const char *filepath = path.c_str();

    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;

    const AVCodec *decoder = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;

    int videoSteamIdx = -1;

    avformat_network_init();

    if(avformat_open_input(&formatContext, filepath, nullptr, nullptr) < 0){
        purple::Log::e(TAG, "open file %s failed", filepath);
        return DECODE_ERR;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        purple::Log::e(TAG, "find steam info error");
        return DECODE_ERR;
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
        return DECODE_ERR;
    }

    AVStream *stream = formatContext->streams[videoSteamIdx];
    AVCodecParameters *codecpar = stream->codecpar;
    
    decoder = avcodec_find_decoder(codecpar->codec_id);
    if (!decoder) {
        purple::Log::e(TAG, "Not found decoder");
        return DECODE_ERR;
    }

    codecContext = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(codecContext, codecpar);

    if (avcodec_open2(codecContext, decoder, nullptr) < 0) {
        purple::Log::e(TAG, "open decode error");
        return DECODE_ERR;
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

                //copy and flip vertial
                // for (uint32_t y = height; y > 0; y--) {
                //     memcpy(dst + (height - y) * width * colorSize, 
                //             frame->data[0] + y * src_stride, 
                //             width * colorSize);
                // }//end for y

                //flip vertial
                uint8_t* flipDst = new uint8_t[width * height * colorSize];
                for(uint32_t y = height; y > 0 ;y--){
                    memcpy(flipDst + (height - y) * width * colorSize, 
                            dst + (y - 1) * width * colorSize, 
                            width * colorSize); 
                }//end for y

                // onGetFrameImage(flipDst, frame->width, frame->height, ptsTime);
                if(onGetFrameImageFunc != nullptr){
                    onGetFrameImageFunc(flipDst, frame->width, frame->height, ptsTime);
                }

                delete[] flipDst;
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

    return DECODE_SUCCESS;
}



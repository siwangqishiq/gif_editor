#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

const char *FILE_PATH = "../assets/sharongzi.gif";
// const char *FILE_PATH = "../assets/yaoren.mp4";

int decodeGifImage(const char *path){
    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;

    const AVCodec *decoder = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;

    int videoSteamIdx = -1;
    int ret;

    avformat_network_init();

    if(avformat_open_input(&formatContext, path, nullptr, nullptr) < 0){
        std::cerr << "open file " << path << " failed\n"; 
        return -1;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        std::cerr << "find steam info error\n";
        return -1;
    }

    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        std::cout << "codec type " << formatContext->streams[i]->codecpar->codec_type << std::endl;
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoSteamIdx = i;
            break;
        }
    }//end for i

    if (videoSteamIdx == -1) {
        std::cerr << "Not fund steam \n";
        return -1;
    }

    AVStream *stream = formatContext->streams[videoSteamIdx];
    AVCodecParameters *codecpar = stream->codecpar;
    
    decoder = avcodec_find_decoder(codecpar->codec_id);
    if (!decoder) {
        std::cerr << "Not fund decoder \n";
        return -1;
    }

    codecContext = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(codecContext, codecpar);

    if (avcodec_open2(codecContext, decoder, nullptr) < 0) {
        std::cerr << "open decode error \n";
        return -1;
    }

    packet = av_packet_alloc();
    frame = av_frame_alloc();

    int frameCount = 0;
    while(av_read_frame(formatContext, packet) >= 0){
        if (packet->stream_index == videoSteamIdx) {
            int ret = avcodec_send_packet(codecContext, packet);
            // std::cout << "send packet " << packet->pos << std::endl;
            if(ret < 0){
                std::cout << "send packet failed \n";
                break;
            }
            
            while ((ret = avcodec_receive_frame(codecContext, frame)) >= 0) {
                auto ptsTime = av_q2d(stream->time_base) * frame->pts;
                std::cout 
                    << "decode image size " << frame->width << ", " << frame->height 
                    << "  #" << frameCount 
                    << " pts:" << ptsTime 
                    << " dts:" << frame->pkt_dts 
                    << " pictype:" << av_get_picture_type_char(frame->pict_type)
                    << " pixformat: " << av_get_pix_fmt_name((AVPixelFormat)frame->format) 
                    << " colorSpace: " << frame->colorspace
                    << std::endl; 
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

int main(){
    auto infoStr = av_version_info();
    std::cout << "ffmpeg version " << infoStr << std::endl;
    

    if(decodeGifImage(FILE_PATH) != 0){
        std::cerr << "decode gif file failed\n";
    }
    return 0;
}



#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

int main(){
    std::cout << "gif editor\n";
    auto infoStr = av_version_info();
    std::cout << "ffmpeg version " << infoStr << std::endl;
    
    return 0;
}



#include "time_util.h"
#include <iostream>

std::wstring TimeUtil::intToTimeStr(int value){
    if(value >= 0 && value < 10){
        return L"0" + std::to_wstring(value);
    }

    return std::to_wstring(value);
}

std::wstring TimeUtil::timeMillisToStr(long long timeMills){
    int minutes = 0;
    int seconds = 0;
    int microSec = 0;

    seconds = timeMills / 1000;
    microSec = ((timeMills - 1000 * seconds)/1000.0f) * 100;
    
    if(seconds >= 60){
        minutes = seconds / 60;
        seconds = seconds - 60 * minutes;
    }

    std::wstring resultStr; // std::wstringstream
    resultStr.reserve(16);
    if(minutes > 0){
        resultStr.append(intToTimeStr(minutes));
        resultStr.append(L":");
    }

    resultStr.append(intToTimeStr(seconds));
    resultStr.append(L".");
    resultStr.append(intToTimeStr(microSec));

    return resultStr;
}




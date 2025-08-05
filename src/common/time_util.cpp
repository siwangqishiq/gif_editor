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

    std::wstring resultStr= L"";
    if(minutes > 0){
        resultStr = resultStr + intToTimeStr(minutes);
        resultStr = resultStr + L":";
    }

    resultStr = resultStr + intToTimeStr(seconds);
    resultStr = resultStr + L".";
    resultStr = resultStr + intToTimeStr(microSec);

    // std::cout << "minute :" << minutes << " sec:" <<seconds << " micro :"
    //      << microSec << " "  << std::endl;
    // std::wcout << resultStr << std::endl;
    return resultStr;
}




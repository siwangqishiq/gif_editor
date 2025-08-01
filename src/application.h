#pragma once
#include <string>
#include <vector>

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <memory>
#include "purple.h"


class Application{
public:
    const std::string TAG = "GifEditor";

    const static int screenWidth = 600;
    const static int screenHeight = 800;
    // const static int screenWidth = 800;
    // const static int screenHeight = 1280;

    //init application
    void init();

    void onCreate();
    
    // main loop
    void runLoop();
    
    //on app close
    void dispose();

    //show debug info
    void showDebugInfo();

    void onResize(int w , int h);

    //扩展信息显示
    void showExtensionInfo();
    
    //run this app
    int execute(int argc, char **argv){
        parseParams(argc, argv);
        
        init();
        runLoop();
        dispose();
        return 0;
    }

    void tick();

    bool onInputEvent(purple::InputEvent &event);

    static int fps;

    std::vector<std::string> params;

    void parseParams(int argc,char **argv);
private:
    GLFWwindow* window = nullptr;
    
    std::vector<std::shared_ptr<purple::IApp>> appInstanceList;

    std::wstring fpsStr = L"0";
};


#ifndef MOILVIEW_H
#define MOILVIEW_H
#include <stdio.h>
#include <string.h>
#include "nlohmann/json.hpp"    // skc: must include json.hpp earlier
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>
#include <time.h>
#include <iostream>
#include <fstream>
#include <termios.h>
#include <unistd.h>

#include "moildev.h"

using namespace std;
using namespace cv;
using json = nlohmann::json; 
namespace Ui
{
    class MoilView;
}

class MoilView
{

public:
    MoilView();
    void Show();
    void initMat();
    char MainLoop(bool isProcessKey);
    ~MoilView();
    enum class AnypointControl
    {
        None,
        Up,
        Down,
        Left,
        Right,
        ZoomIn,
        ZoomOut,
        Reset,
        Pause,
        OpenCamera,
        CloseCamera,
        OpenIPCamera,
        OpenVideo        
    };
    void AnypointCtrl(AnypointControl ctrl);
    void AnypointGoto(int Alpha, int Beta);
private:
    Moildev *md;
    // rpi_220
    const std::string videoStreamAddress = "http://192.168.100.9:8000/stream.mjpg";  
    const std::string videoFileAddress = "../video/video.mp4";  
    bool isAnyPointUpdate = false;
    bool isDisplayUpdate = false;
    Mat image_input;
    Mat image_display;
    Mat mapX, mapY;
    double m_ratio;
    int x_base = 640;
    int y_base = 30;
    // rpi_220
    int fix_width = 2592;
    int fix_height = 1944;
    // T265
    //  int fix_width = 848;
    //  int fix_height = 800;
    int currCh = 0, prevCh = 0;
    int currPara = 0;
    int currAlpha = 0;
    int currBeta = 0;
    int currInc = 10;
    double currZoom = 4;
    double defaultZoom = 4;

    const double minZoom = 1;
    const double maxZoom = 12;
    int width_main = 1280;
    int height_main = 940;
    string Para[2];
    enum class MediaType
    {
        NONE,
        IMAGE_FILE,
        CAMERA,
        IPCAMERA,
        VIDEO_FILE
    };
    MediaType mediaType = MediaType::NONE;
    enum class PlayState { STOP, PLAY, PAUSE };
    PlayState playState;
    void Display();

    void doAnyPoint();
    void closeCamera();
    void openVideo(string videoStreamAddress);    
    void loadParameterJson(string str);
    VideoCapture cap0;
    void freeMemory();
};

#endif

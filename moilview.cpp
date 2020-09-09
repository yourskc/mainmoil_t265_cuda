
#include "moilview.h"
#include "moilcuda.h"
#define USE_PICAMERA false
#define FAST_ANYPOINT false
MoilView::MoilView()
{
    md = new Moildev();
}
void MoilView::Show()
{
    
    md->Config("rpi_220", 1.4, 1.4,
               1320.0, 1017.0, 1.048,
               2592, 1944, 3.4, // 4.05
               // 0, 0, 0, 0, -47.96, 222.86
               0, 0, 0, 10.11, -85.241, 282.21);
              
    /*
    md->Config("endoscope", 2, 2,
            1120.0, 520.0, 1,
            1920, 1080, 3,
            0, 0, 0, 0, 0, 130
                  );               
*/
    /*        
    md->Config("t265", 3, 3,
        427, 394, 1,
        848, 800, 1.68,
        0, 0, -24.964, 38.2, -16.956, 183.42
        );        
*/
    double calibrationWidth = md->getImageWidth();
    double iCy = md->getiCy();
    image_input = imread("../images/image.jpg", IMREAD_COLOR);
    // image_input = imread( "images/image2.jpg", IMREAD_COLOR);
    MediaType mediaType = MediaType::IMAGE_FILE;
    double fix_width = image_input.cols;
    double fix_height = image_input.rows;
    mapX = Mat(fix_height, fix_width, CV_32F);
    mapY = Mat(fix_height, fix_width, CV_32F);
    Mat image_result(fix_height, fix_width, CV_32F);
    m_ratio = fix_width / calibrationWidth;
    namedWindow("MOIL Anypoint", WINDOW_AUTOSIZE);
    moveWindow("MOIL Anypoint", x_base, y_base);
    doAnyPoint();
    Display();
}
void MoilView::Display()
{
    Mat image_result;
    if (image_input.empty())
        return;

    remap(image_input, image_result, mapX, mapY, INTER_CUBIC, BORDER_CONSTANT, Scalar(0, 0, 0));
    cv::resize(image_result, image_display, Size(width_main, height_main));
    if (mediaType == MediaType::CAMERA) 
        cvtColor(image_display, image_display, COLOR_BGR2RGB);
    imshow("MOIL Anypoint", image_display);
}

void MoilView::AnypointCtrl(AnypointControl ctrl)
{
    switch (ctrl)
    {
    case AnypointControl::Up:
        currAlpha = currAlpha + currInc;
        currAlpha = (currAlpha > 90) ? 90 : currAlpha;
        isAnyPointUpdate = true;
        break;
    case AnypointControl::Down:
        currAlpha = currAlpha - currInc;
        currAlpha = (currAlpha < -90) ? -90 : currAlpha;
        isAnyPointUpdate = true;
        break;
    case AnypointControl::Left:
        currBeta = currBeta - currInc;
        currBeta = (currBeta < -90) ? -90 : currBeta;
        isAnyPointUpdate = true;
        break;
    case AnypointControl::Right:
        currBeta = currBeta + currInc;
        currBeta = (currBeta > 90) ? 90 : currBeta;
        isAnyPointUpdate = true;
        break;
    case AnypointControl::ZoomIn:
        currZoom += 1;
        if (currZoom > maxZoom)
            currZoom = maxZoom;
        isAnyPointUpdate = true;
        break;
    case AnypointControl::ZoomOut:
        currZoom -= 1;
        if (currZoom < minZoom)
            currZoom = minZoom;
        isAnyPointUpdate = true;
        break;
    case AnypointControl::Reset:
        currAlpha = 0;
        currBeta = 0;
        currZoom = defaultZoom;
        isAnyPointUpdate = true;
        break;
    case AnypointControl::OpenCamera:
        cap0.open(0);
        cap0.set(CAP_PROP_FRAME_WIDTH, fix_width);
        cap0.set(CAP_PROP_FRAME_HEIGHT, fix_height);
        if (cap0.isOpened())
        {
            mediaType = MediaType::CAMERA;
        }
        break;
    case AnypointControl::OpenIPCamera:
        cap0.open(videoStreamAddress);
        if (cap0.isOpened())
        {
            mediaType = MediaType::IPCAMERA;
        }        
        break;        
    case AnypointControl::CloseCamera:    
        cap0.release();
        mediaType = MediaType::NONE;
        break;
    }
}

void MoilView::AnypointGoto(int Alpha, int Beta)
{
    currAlpha = Alpha;
    currBeta = Beta;
    currAlpha = (currAlpha < -90) ? -90 : currAlpha;
    currAlpha = (currAlpha > 90) ? 90 : currAlpha;
    currBeta = (currBeta < -90) ? -90 : currBeta;
    currBeta = (currBeta > 90) ? 90 : currBeta;
    isAnyPointUpdate = true;
}

char MoilView::MainLoop(bool isProcessKey)
{
    char c;
    // Mat frame;
    if (((mediaType == MediaType::CAMERA)||(mediaType == MediaType::IPCAMERA)) && cap0.isOpened())
    {
        cap0 >> image_input;
        if ((image_input.cols != fix_width) || (image_input.rows != fix_height))
            cv::resize(image_input, image_input, Size(fix_width, fix_height));
        isDisplayUpdate = true;
    }
    else
        mediaType == MediaType::NONE;

    if (image_input.empty())
    {
        return -1;
    }

    if (isAnyPointUpdate)
    {
        doAnyPoint();
        isAnyPointUpdate = false;
    }
    if (isDisplayUpdate)
    {
        Display();
        isDisplayUpdate = false;
    }
    c = waitKey(33);
    if (isProcessKey)
    {
        if ((int)c == 82) // up
        {
            AnypointCtrl(AnypointControl::Up);
        }
        else if ((int)c == 84) // Down
        {
            AnypointCtrl(AnypointControl::Down);
        }
        else if ((int)c == 81) // left
        {
            AnypointCtrl(AnypointControl::Left);
        }
        else if ((int)c == 83) // right
        {
            AnypointCtrl(AnypointControl::Right);
        }
        else if ((int)c == 43) // +
        {
            AnypointCtrl(AnypointControl::ZoomIn);
        }
        else if ((int)c == 45) // -
        {
            AnypointCtrl(AnypointControl::ZoomOut);
        }
        else if ((c == 'c') || (c == 'C')) // C : Camera
        {
            AnypointCtrl(AnypointControl::OpenCamera);
        }  
        else if ((c == 'i') || (c == 'I')) // C : IP Camera
        {
            AnypointCtrl(AnypointControl::OpenIPCamera);
        }                
        else if ((c == 'r') || (c == 'R')) // R : Reset
        {
            AnypointCtrl(AnypointControl::Reset);
        }
    }
    return c;
}

void MoilView::doAnyPoint()
{
    ConfigData_cuda *cfg = new ConfigData_cuda;

    cfg->iCx = md->cfg->iCx;
    cfg->iCy = md->cfg->iCy;
    cfg->imageWidth = md->cfg->imageWidth;
    cfg->imageHeight = md->cfg->imageHeight;
    cfg->cameraSensorWidth = md->cfg->cameraSensorWidth;
    cfg->cameraSensorHeight = md->cfg->cameraSensorHeight;
    cfg->ratio = md->cfg->ratio;
    cfg->para0 = md->cfg->para0;
    cfg->para1 = md->cfg->para1;
    cfg->para2 = md->cfg->para2;
    cfg->para3 = md->cfg->para3;
    cfg->para4 = md->cfg->para4;
    cfg->para5 = md->cfg->para5;
    cfg->calibrationRatio = md->cfg->calibrationRatio;

    if (FAST_ANYPOINT)
        md->fastAnyPointM2((float *)mapX.data, (float *)mapY.data, mapX.cols, mapX.rows, currAlpha, currBeta, currZoom, m_ratio); // front view
    else
        AnyPointM_cuda((float *)mapX.data, (float *)mapY.data, mapX.cols, mapX.rows, (float)currAlpha, (float)currBeta, currZoom, m_ratio, cfg, 1);
        // md->AnyPointM2((float *)mapX.data, (float *)mapY.data, mapX.cols, mapX.rows, currAlpha, currBeta, currZoom, m_ratio); // front view
    isDisplayUpdate = true;
}

void MoilView::freeMemory()
{
    cout << "free memory" << endl;
    image_input.release();
    image_display.release();
    mapX.release();
    mapY.release();
}
MoilView::~MoilView()
{
    freeMemory();
    delete md;
}

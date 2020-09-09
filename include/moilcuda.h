// moilcuda.h

#ifndef MOILCUDA_H
#define MOILCUDA_H
class ConfigData_cuda
{
public:
    float cameraSensorWidth;
    float cameraSensorHeight;
    float iCx;
    float iCy;
    float ratio;
    float imageWidth;
    float imageHeight;
    float para0;
    float para1;
    float para2;
    float para3;
    float para4;
    float para5;
    float calibrationRatio;
};

extern "C" void AnyPoint_cuda(Mat image_input, Mat &image_result, int alpha, int beta, ConfigData_cuda *cfg, int apMode);
extern "C" void AnyPointM_cuda(float *mapX, float *mapY, int w, int h, float alphaOffset, float betaOffset, float zoom, float magnification, ConfigData_cuda *cfg, int apMode);
extern "C" void AnyPointM_Remap_cuda(char *src, char *dest, int w, int h, float alphaOffset, float betaOffset, float zoom, float magnification, ConfigData_cuda *cfg, int apMode);


#endif // MOILCUDA_H

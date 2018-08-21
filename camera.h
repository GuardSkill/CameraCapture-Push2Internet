#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/opencv.hpp>
#include <encoder.h>
using namespace cv;
class Cameras
{
public:
    void Init();
    void GetNextFrame();
    void Destory();
public:
    VideoCapture cap;
    my_x264_encoder*  encoder;
    int n_nal;
    x264_picture_t pic_out;

    Mat img;
    unsigned char *RGB1;
};

#endif // CAMERA_H

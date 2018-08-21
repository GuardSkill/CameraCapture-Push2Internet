#ifndef H264ENCODER_H
#define H264ENCODER_H

#include <stdio.h>
#include <stdint.h>

#include <malloc.h>
#include "opencamera.h"
extern "C"
{
    #include <x264.h>
}


#define ENCODER_TUNE   "zerolatency"
#define ENCODER_PROFILE  "baseline"
#define ENCODER_PRESET "veryfast"
#define ENCODER_COLORSPACE X264_CSP_I420
#define CLEAR(x) (memset((&x),0,sizeof(x)))

typedef unsigned char  uint8_t;
typedef struct my_x264_encoder
{
    x264_param_t  * x264_parameter;
    char parameter_preset[20];
    char parameter_tune[20];
    char parameter_profile[20];
    x264_t  * x264_encoder;
    x264_picture_t * yuv420p_picture;
    long colorspace;
    unsigned char *yuv;
    x264_nal_t * nal;
} my_x264_encoder;


class H264Encoder
{
private:
    //yuv422平面格式转yuv420打包格式
    void yuyv2yuv(unsigned char *yuv, unsigned char *yuyv, int width, int height);
   FILE * f;
public:
    void Init(char *camerafile);
    void GetNextFrame();
    void Destory();
    void opfile();
    void savefile( );
public:
    OpenCamera camera;
    my_x264_encoder*  encoder;
    int n_nal;
    x264_picture_t pic_out;

    unsigned char * picture;
};

#endif // H264ENCODER_H

#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include <stdint.h>

extern "C"
{
    #include <x264.h>
}

typedef struct my_x264_encoder{
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

#endif // ENCODER_H

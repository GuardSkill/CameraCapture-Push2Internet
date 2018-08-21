/*
*  H264FramedLiveSource.cpp
*/

#include "h264framedlivesource.h"
//#include "camera.h"
#include "h264encoder.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
extern class H264Encoder Camera; //in mainRTSPServer.cpp

H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment& env, char const* fileName, unsigned preferredFrameSize=0, unsigned playTimePerFrame=0) : FramedSource(env)
{
    //fp = fopen(fileName, "rb");
}

H264FramedLiveSource* H264FramedLiveSource::createNew(UsageEnvironment& env, char const* fileName, unsigned preferredFrameSize /*= 0*/, unsigned playTimePerFrame /*= 0*/)
{
    H264FramedLiveSource* newSource = new H264FramedLiveSource(env, fileName, preferredFrameSize, playTimePerFrame);

    return newSource;
}

H264FramedLiveSource::~H264FramedLiveSource()
{
    //fclose(fp);
}


void H264FramedLiveSource::doGetNextFrame()
{

   // fDurationInMicroseconds =40000 ; //1000000/fFrameRate
   //gettimeofday(&fPresentationTime, NULL);
    fFrameSize = 0;
    //不知道为什么，多几帧一起发送效果会好一点点，也许是心理作怪     for(int i = 0; i < 2; i++){

        Camera.GetNextFrame();
        for (my_nal = Camera.encoder->nal; my_nal < Camera.encoder->nal + Camera.n_nal; ++my_nal)
        {
            //memmove((unsigned char*)fTo + fFrameSize, my_nal->p_payload, my_nal->i_payload);
            memcpy((unsigned char*)fTo + fFrameSize, my_nal->p_payload, my_nal->i_payload);
            fFrameSize += my_nal->i_payload;

        }
       // gettimeofday(&fPresentationTime, NULL);   //同一帧具有相同时间戳
   //}
    nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
        (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
    return;
}


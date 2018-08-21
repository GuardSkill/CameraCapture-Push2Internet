/*
*  H264LiveVideoServerMediaSubssion.cpp
*/

#include "h264livevideoservermediasubssion.h"
#include "h264framedlivesource.h"
#include <H264VideoStreamFramer.hh>
#include <string.h>


H264LiveVideoServerMediaSubssion* H264LiveVideoServerMediaSubssion::createNew (UsageEnvironment& env, char const* fileName, Boolean reuseFirstSource)
{
    return new H264LiveVideoServerMediaSubssion(env, fileName, reuseFirstSource);
}

H264LiveVideoServerMediaSubssion::H264LiveVideoServerMediaSubssion(UsageEnvironment& env, char const* fileName, Boolean reuseFirstSource)
: H264VideoFileServerMediaSubsession(env, fileName, reuseFirstSource)
{
    strcpy(fFileName, fileName);
}


H264LiveVideoServerMediaSubssion::~H264LiveVideoServerMediaSubssion()
{
}

FramedSource* H264LiveVideoServerMediaSubssion::createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate)
{
    estBitrate = 1000; // kbps

    H264FramedLiveSource* liveSource = H264FramedLiveSource::createNew(envir(), fFileName);
    if (liveSource == NULL)
    {
        return NULL;
    }
    return H264VideoStreamFramer::createNew(envir(), liveSource);
}

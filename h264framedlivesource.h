#ifndef _H264FRAMEDLIVESOURCE_H
#define _H264FRAMEDLIVESOURCE_H

#include <FramedSource.hh>
#include <UsageEnvironment.hh>
#include <x264.h>

class H264FramedLiveSource : public FramedSource
{
public:
    static H264FramedLiveSource* createNew(UsageEnvironment& env, char const* fileName, unsigned preferredFrameSize = 0, unsigned playTimePerFrame = 0);
    x264_nal_t * my_nal;

protected:
    H264FramedLiveSource(UsageEnvironment& env, char const* fileName, unsigned preferredFrameSize, unsigned playTimePerFrame); // called only by createNew()
    ~H264FramedLiveSource();
    virtual unsigned maxFrameSize() const        // 这个很重要, 如果不设置, 可能导致 getNextFrame() 出现 fMaxSize 小于实际编码帧的情况, 导致图像不完整
        {    return 100*1024; }

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    int TransportData(unsigned char* to, unsigned maxSize);
    //static int nalIndex;

protected:
    FILE *fp;

};

#endif

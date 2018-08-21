#ifndef _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_H
#define _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_H

#include <H264VideoFileServerMediaSubsession.hh>
#include <UsageEnvironment.hh>


class H264LiveVideoServerMediaSubssion : public H264VideoFileServerMediaSubsession {

public:
    static H264LiveVideoServerMediaSubssion*
        createNew(UsageEnvironment& env,
        char const* fileName,
        Boolean reuseFirstSource);

protected:
    H264LiveVideoServerMediaSubssion(UsageEnvironment& env, char const* fileName, Boolean reuseFirstSource);
    ~H264LiveVideoServerMediaSubssion();

protected:
    FramedSource* createNewStreamSource(unsigned clientSessionId,
        unsigned& estBitrate);
public:
    char fFileName[100];

};


#endif

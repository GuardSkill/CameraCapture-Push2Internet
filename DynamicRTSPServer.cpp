                                                                                                  #include "DynamicRTSPServer.hh"
#include "h264livevideoservermediasubssion.h"
#include <liveMedia.hh>
#include <string.h>



DynamicRTSPServer* DynamicRTSPServer::createNew(
                UsageEnvironment& env, Port ourPort,
                 UserAuthenticationDatabase* authDatabase,
                 unsigned reclamationTestSeconds)
{
  int ourSocket = setUpOurSocket(env, ourPort);
  if (ourSocket == -1) return NULL;

  return new DynamicRTSPServer(env, ourSocket, ourPort, authDatabase, reclamationTestSeconds);
}


DynamicRTSPServer::DynamicRTSPServer(UsageEnvironment& env, int ourSocket, Port ourPort,
                     UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds)
  : RTSPServerSupportingHTTPStreaming(env, ourSocket, ourPort, authDatabase, reclamationTestSeconds) {}
DynamicRTSPServer::~DynamicRTSPServer() {}             //构造与析构函数


static ServerMediaSession* createNewSMS(UsageEnvironment& env, char const* fileName/*, FILE* fid*/); // forward


ServerMediaSession* DynamicRTSPServer::lookupServerMediaSession(char const* streamName, Boolean isFirstLookupInSession)
{
  // Next, check whether we already have a "ServerMediaSession" for this file:
  ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(streamName); //
  Boolean smsExists = sms != NULL;

  // Handle the four possibilities for "fileExists" and "smsExists":
  if (smsExists && isFirstLookupInSession)
  {
      // Remove the existing "ServerMediaSession" and create a new one, in case the underlying  潜在的
      // file has changed in some way:
      removeServerMediaSession(sms);
      sms = NULL;
  }
  if (sms == NULL)
  {
      sms = createNewSMS(envir(), streamName/*, fid*/);
      addServerMediaSession(sms);
  }

  return sms;
}


static ServerMediaSession* createNewSMS(UsageEnvironment& env, char const* fileName/*, FILE* fid*/)
{
  // Use the file name extension to determine the type of "ServerMediaSession":
  char const* extension = strrchr(fileName, '.');  //查找字符在指定字符串中从后面开始的第一次出现的位置，如果成功，则返回从该位置到字符串结尾的所有字符，如果失败，则返回 false。
  if (extension == NULL) return NULL;

  ServerMediaSession* sms = NULL;
  Boolean const reuseSource = False;

  if (strcmp(extension, ".264") == 0) {
    // Assumed to be a H.264 Video Elementary Stream file:
    char const* descStr = "H.264 Video, streamed by the LIVE555 Media Server";
    sms = ServerMediaSession::createNew(env, fileName, fileName, descStr);
    OutPacketBuffer::maxSize = 100000; // allow for some possibly large H.264 frames  //100000
    sms->addSubsession(H264LiveVideoServerMediaSubssion::createNew(env, fileName, reuseSource));
  }

  return sms;
}

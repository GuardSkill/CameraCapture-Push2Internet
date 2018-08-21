#include <BasicUsageEnvironment.hh>
#include "DynamicRTSPServer.hh"
#include "h264framedlivesource.h"
#include "liveMedia.hh"
//#include "camera.h"
#include "h264encoder.h"
//#include <opencv/highgui.h>
#include <iostream>
//"version"
#ifndef _MEDIA_SERVER_VERSION_HH
#define _MEDIA_SERVER_VERSION_HH
#define MEDIA_SERVER_VERSION_STRING "0.85"
#endif


//Cameras Camera;
H264Encoder Camera;

int main(int argc, char** argv) {
  // Begin by setting up our usage environment:
   //  开始建立我们的使用环境：
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();   //创建具体调度类
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  // 实现客户端访问控制的RTSP服务器，做到以下几点：
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server.  Try first with the default port number (554),
  // and then with the alternative port number (8554):
  RTSPServer* rtspServer;
  portNumBits rtspServerPortNum = 554;
  Camera.Init("/dev/video0");
  //Camera.Init();
  rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  if (rtspServer == NULL) {
    rtspServerPortNum = 8554;
    rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  }
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  *env << "LIVE555 Media Server\n";
  *env << "\tversion " << MEDIA_SERVER_VERSION_STRING
       << " (LIVE555 Streaming Media library version "
       << LIVEMEDIA_LIBRARY_VERSION_STRING << ").\n";

  char* urlPrefix = rtspServer->rtspURLPrefix();
  *env << "Play streams from this server using the URL\n\t"
       << urlPrefix << "<filename>\nwhere <filename> is a file present in the current directory.\n";
  *env << "Each file's type is inferred from its name suffix:\n";
  *env << "\t\".264\" => a H.264 Video Elementary Stream file\n";

  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    *env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling, or for HTTP live streaming (for indexed Transport Stream files only).)\n";
  } else {
    *env << "(RTSP-over-HTTP tunneling is not available.)\n";
  }

  env->taskScheduler().doEventLoop(); // does not return
  Camera.Destory();
  return 0; // only to prevent compiler warning
}

// int main(int argc, char** argv)
// {
//       Camera.Init("/dev/video0");
//       Camera.opfile();
//       printf("start save in /home/habi/movies/my_encoder.264");
//       while(1)
//       {
//           Camera.GetNextFrame();
//       }

//       Camera.Destory();
// }


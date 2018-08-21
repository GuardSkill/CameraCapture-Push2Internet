#ifndef _DYNAMIC_RTSP_SERVER_HH
#define _DYNAMIC_RTSP_SERVER_HH

#include "GroupsockHelper.hh"
#include "FramedSource.hh"
#include "BasicUsageEnvironment.hh"

#ifndef _RTSP_SERVER_SUPPORTING_HTTP_STREAMING_HH
#include <RTSPServerSupportingHTTPStreaming.hh>
#endif

class DynamicRTSPServer: public RTSPServerSupportingHTTPStreaming {
public:
  static DynamicRTSPServer* createNew(UsageEnvironment& env, Port ourPort,
                      UserAuthenticationDatabase* authDatabase,
                      unsigned reclamationTestSeconds = 65);

protected:
  DynamicRTSPServer(UsageEnvironment& env, int ourSocket, Port ourPort,
            UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds);
  // called only by createNew();
  virtual ~DynamicRTSPServer();

protected: // redefined virtual functions
  virtual ServerMediaSession*
  lookupServerMediaSession(char const* streamName, Boolean isFirstLookupInSession);
};

#endif

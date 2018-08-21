TARGET=livePlayer
OBJ=main.o\
    h264framedlivesource.o\
    h264livevideoservermediasubssion.o\
    DynamicRTSPServer.o\
    opencamera.o\
    h264encoder.o
CXX=g++
#CXX=arm-none-linux-gnueabi-g++
CXXFLAGS=-g
#AR=arm-none-linux-gnueabi-ar
LIB =/usr/local/lib/libliveMedia.a\
/usr/local/lib/libBasicUsageEnvironment.a\
/usr/local/lib/libgroupsock.a\
/usr/local/lib/libUsageEnvironment.a\
 /usr/local/lib/libx264.a\
-lpthread -ldl
#LFLAGS= -Wl,-O1
INCPATH= -I../livePlayer -I. -isystem /usr/local/include\
-isystem /usr/local/include/liveMedia\
-isystem /usr/local/include/BasicUsageEnvironment\
-isystem /usr/local/include/groupsock\
-isystem /usr/local/include/UsageEnvironment
$(TARGET):$(OBJ)
	$(CXX) $(CXXFLAGS) $(INCPATH) -static -o $(TARGET) $(OBJ) $(LIB)
#$(CXX) $(CXXFLAGS) $(INCPATH)  -o $(TARGET) $(OBJ) $(LIB)
main.o:h264framedlivesource.h h264encoder.h DynamicRTSPServer.hh\
main.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c main.cpp
opencamera.o:opencamera.h opencamera.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c opencamera.cpp
h264livevideoservermediasubssion.o:h264livevideoservermediasubssion.h\
          h264framedlivesource.h
	$(CXX) $(CXXFLAGS) $(INCPATH) -c h264livevideoservermediasubssion.cpp
h264framedlivesource.o:h264framedlivesource.h h264encoder.h
	$(CXX) $(CXXFLAGS) $(INCPATH) -c h264framedlivesource.cpp
h264encoder.o:h264encoder.h
	$(CXX) $(CXXFLAGS) $(INCPATH) -c h264encoder.cpp
DynamicRTSPServer.o:DynamicRTSPServer.hh\
h264livevideoservermediasubssion.h
	$(CXX) $(CXXFLAGS) $(INCPATH) -c DynamicRTSPServer.cpp
.PHONY:clean
clean:
	rm -f livePlayer $(OBJ) 

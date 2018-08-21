TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    h264framedlivesource.cpp \
    h264livevideoservermediasubssion.cpp \
    DynamicRTSPServer.cpp \
    opencamera.cpp \
    h264encoder.cpp

INCLUDEPATH +=  /usr/local/include \
                               /usr/local/include/liveMedia \
                               /usr/local/include/BasicUsageEnvironment \
                               /usr/local/include/groupsock \
                               /usr/local/include/UsageEnvironment


LIBS += /usr/local/lib/libliveMedia.a \
              /usr/local/lib/libBasicUsageEnvironment.a \
              /usr/local/lib/libgroupsock.a \
              /usr/local/lib/libUsageEnvironment.a \
              /usr/local/lib/libx264.a \
#             /usr/local/lib/lib*.so.* \
               -lpthread -ldl



HEADERS += \
    h264framedlivesource.h \
    h264livevideoservermediasubssion.h \
    DynamicRTSPServer.hh \
    opencamera.h \
    h264encoder.h

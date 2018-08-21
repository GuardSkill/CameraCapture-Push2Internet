#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <memory.h>

#define WIDTH  320        //320
#define HEIGHT 240        //240

#define DENOMINATOR 1   //1
#define NUMERATOR 25//25

typedef struct VideoBuffer{
    __u8* start;
    size_t length;
}VIDEOBUF;



class OpenCamera
{
private:
    //capture device supported formats
    void queryFomat();
    fd_set rfds;
    struct timeval ts;  //该结构体用于描述一段时间长度
public:
    //Used to create a frame queue
    VIDEOBUF *vedio;
    //device id
    int vfd;
    /* enum video type */
    struct v4l2_fmtdesc fmtd;
    //frame format
    struct v4l2_frmsizeenum frmsize;   //Max framesize
    struct v4l2_format v4l2_fmt;
    /* mmap buffer*/
    struct v4l2_requestbuffers  v4l2_reqbuf;
    //a frame
    struct v4l2_buffer buffer;
    OpenCamera();
    ~OpenCamera();
    //open the device file
    bool openCam(char* device);
    //init the camera
    void camInit();
    //get the frame from video buffer
    int getFrame(__u8 *frame);
    //
    void saveImage(__u8* address, int length);
    //close device and stop vedio stream
    void closeCam();
};

//interface for "C"
#ifdef __cplusplus
extern "C"
{
#endif
    extern void *construct_camera();
    extern int camera_init(void *camera, char *devicename);
    extern int get_frame(void *camera, __u8 * frame);
    extern void destroy_camera(void *camera);
#ifdef __cplusplus
}
#endif


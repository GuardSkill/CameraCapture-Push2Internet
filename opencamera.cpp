#include "opencamera.h"

OpenCamera::OpenCamera(){}
OpenCamera::~OpenCamera(){}

bool OpenCamera::openCam(char* device)
{
    //open the device with rw
    vfd = open( device, O_RDWR);
    if(vfd < 0)
    {
        perror("open the video faiure.\n");
        return false;
    }

}
/*
void OpenCamera::queryFomat()
{
    struct v4l2_capability cap;
    int ret = ioctl(vfd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
    printf("VIDIOC_QUERYCAP failed (%d)\n", ret);
    }
    // Print capability infomations
    printf("Capability Informations:\n");
    printf(" driver: %s\n", cap.driver);
    printf(" card: %s\n", cap.card);
    printf(" bus_info: %s\n", cap.bus_info);
    printf(" version: %08X\n", cap.version);
    printf(" capabilities: %08X\n", cap.capabilities);

    memset(&fmtd, 0, sizeof(fmtd));
    fmtd.index=0;
    fmtd.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("Support format:\n");

    while(ioctl(vfd, VIDIOC_ENUM_FMT, &fmtd) != -1)      // 获取当前视频设备支持的视频格式 。
    {
        printf("\t%d.%s\n",fmtd.index+1,fmtd.description);
        printf("If been coded:%d\n", fmtd.flags);
        fmtd.index++;

    }
}
*/
void OpenCamera::queryFomat()
{

        memset(&fmtd, 0, sizeof(fmtd));
        //the format number
        fmtd.index = 0;

        fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        while (ioctl(vfd, VIDIOC_ENUM_FMT, &fmtd) == 0)
        {
            frmsize.pixel_format=fmtd.pixelformat;
            frmsize.index=0;
            fmtd.index++;
            printf("{ pixelformat = ''%c%c%c%c'', description = ''%s'' }\n",
                fmtd.pixelformat & 0xFF, (fmtd.pixelformat >> 8) & 0xFF,
                (fmtd.pixelformat >> 16) & 0xFF, (fmtd.pixelformat >> 24) & 0xFF,
                fmtd.description);
            printf("%d\n", fmtd.flags);
            while (ioctl(vfd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) {
                       if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                           printf("support PR%d:   %dx%d\n",frmsize.index,
                                             frmsize.discrete.width,
                                             frmsize.discrete.height);
                       } else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
                           printf("support PR%d:  %dx%d\n",frmsize.index,
                                             frmsize.stepwise.max_width,
                                             frmsize.stepwise.max_height);
                       }
                           frmsize.index++;
                       }                    //printf available  resolution
        }
}                                                               //查询支持的格式(大四的代码)
void OpenCamera::camInit()
{
    queryFomat();
    /*******set the format of frame*********/
    memset(&v4l2_fmt, 0, sizeof(v4l2_format));
    //format values
    v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_fmt.fmt.pix.width = WIDTH;
    v4l2_fmt.fmt.pix.height= HEIGHT;
    v4l2_fmt.fmt.pix.pixelformat =V4L2_PIX_FMT_YUYV;// V4L2_FIELD_INTERLACED;
//      v4l2_fmt.fmt.pix.pixelformat = fmtd.pixelformat;
    //YUYV需要隔行扫描  (Interlaced scanning)，否则花屏
//     v4l2_fmt.fmt.pix.field     = V4L2_FIELD_INTERLACED;
    v4l2_fmt.fmt.pix.field     = V4L2_FIELD_NONE ;

    // Verify frame format
    if(ioctl(vfd, VIDIOC_TRY_FMT, &v4l2_fmt) == -1)    //验证当前驱动的显示格式,检查是否支持某种帧格式:fmtd.pixelformat;
    {
        perror("verify frame format failure.\n");
        exit(1);
    }

    //v4l2_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //set the format
    if( ioctl(vfd, VIDIOC_S_FMT, &v4l2_fmt) == -1)     //设置当前驱动的频捕获格式
    {
        perror("set format failure.\n");
        exit(1);
    }


    struct v4l2_streamparm Stream_Parm;

    memset(&Stream_Parm, 0, sizeof(struct v4l2_streamparm));
    Stream_Parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    Stream_Parm.parm.capture.timeperframe.denominator =DENOMINATOR; //分母。 例：30
    Stream_Parm.parm.capture.timeperframe.numerator = NUMERATOR;  // 分子。例：1

    if(ioctl(vfd, VIDIOC_S_PARM, &Stream_Parm) < 0)      //设置数据流的参数，一般设置帧率等参数
    {
        exit(1);
    }

    /************aplly buffers***********/
    //buffer format
    memset(&v4l2_reqbuf, 0, sizeof(struct v4l2_requestbuffers));
    v4l2_reqbuf.count = 2;         //当memory=V4L2_MEMORY_MMAP时，此处才有效。表明要申请的buffer个数。
    v4l2_reqbuf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //map the kernel space to user space
    v4l2_reqbuf.memory= V4L2_MEMORY_MMAP;    //既然是Memory Mapping模式，则此处设置为这个
    //allocate a buffer memory
    /*注意：count是个输入输出函数。因为你所申请到的Buffer个数不一定就是你所输入的Number。所以在ioctl
     * 执行后，driver会将真实申请到的buffer个数填充到此field. 这个数目有可能大于你想要申请的，也可能小与，
     * 甚至可能是0个。应用程序可以再次调用ioctl--VIDIOC_REQBUFS 来修改buffer个数。但前提是必须先释放已
     * 经 mapped 的 buffer ，可以先 munmap ，然后设置参数 count 为 0 来释放所有的 buffer。*/
    if(ioctl(vfd, VIDIOC_REQBUFS, &v4l2_reqbuf))
    {
        perror("aplly buffer failure.\n");
        exit(1);
    }
   //请求V4L2驱动分配视频缓冲区(申请V4L2视频驱动分配内存)，V4L2是视频设备的驱动层，
    //位于内核空间，所以通过VIDIOC_REQBUFS控制命令字申请的内存位于内核空间，应
   //用程序不能直接访问，需要通过调用mmap内存映射函数把内核空间内存映射到用户空间后，
    //应用程序通过访问用户空间地址来访问内核空间。注意：VIDIOC_REQBUFS会修改tV4L2_reqbuf的count值，t
    //V4L2_reqbuf的count值返回实际申请成功的视频缓冲区数目;
    vedio = (VIDEOBUF*)calloc(v4l2_reqbuf.count,sizeof(VIDEOBUF));

    for(unsigned int i = 0; i < v4l2_reqbuf.count; i++)
    {
       memset(&buffer, 0, sizeof(buffer));
        //buffer values
       buffer.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
       // using mapping mode
       buffer.memory=  V4L2_MEMORY_MMAP;
       buffer.index = i;
        //read buffer
       if(ioctl(vfd, VIDIOC_QUERYBUF, &buffer)== -1)   //next read aim at VIDIOC_QUERYBUF
           exit(1);
       //功能： 查询已经分配的V4L2的视频缓冲区的相关信息，包括视频缓冲区的使用状态、在内核空间的偏移地址、缓冲区
       //长度等。在应用程序设计中通过调 VIDIOC_QUERYBUF来获取内核空间的视频缓冲区信息，然后调用函数mmap
       //把内核空间地址映射到用户空间，这样应用程序才能够访问位于内核空间的视频缓冲区。
       vedio[i].length=buffer.length;
       //address translation
       vedio[i].start =  (__u8 *) mmap(NULL,buffer.length,PROT_READ | PROT_WRITE,MAP_SHARED,
                            vfd, buffer.m.offset);
       //将内核空间映射进内存 （页内容可读且可写,与其它所有映射这个对象的进程共享映射空间。）
       //off_toffset：被映射对象内容的起点。 NULL:让内核帮我们自动寻找一个合适映射区的开始地址
       if(vedio[i].start == MAP_FAILED)  //MAP_FAILED ==	((void *) -1)失败时，mmap()返回MAP_FAILED[其值为(void *)-1
       {
              exit(1);
       }
       //put  the frame buffer into the queue
       ioctl(vfd, VIDIOC_QBUF, &buffer);
       //功能： 投放一个空的视频缓冲区到视频缓冲区输入队列中 ;

   }

   /*open video stream*/
  enum v4l2_buf_type v4l2_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if(ioctl(vfd, VIDIOC_STREAMON, &v4l2_type) == -1)
  {
      perror("open vedio stream failure.\n");
      exit(1);
  }
  //功能： 启动视频采集命令，应用程序调用VIDIOC_STREAMON启动视频采集命令后，
  //视频设备驱动程序开始采集视频数据，并把采集到的视频数据保存到视频驱动的视频缓冲区中。
  //select mechanism init
  FD_ZERO(&rfds);//将指定的文件描述符集清空，在对文件描述符集合进行设置前，
  //必须对其进行初始化，如果不清空，由于在系统分配内存空间后，通常并不作清空处理，所以结果是不可知的。
  printf("vfd = %d\n", vfd);
  FD_SET(vfd, &rfds);//用于在文件描述符集合中增加一个新的文件描述符。

}
//return frame data pointer，must be free outside
int OpenCamera::getFrame(__u8 *frame)       //将队列中收集到的帧复制到frame中
{
    if(frame==NULL) //查询是否为图片帧赋空间
        return -1;
    //usleep(25000);
     ts.tv_sec =2;   //seconds
     ts.tv_usec=0;   //microseconds
     //monitoring file descriptor
    int ret = select(vfd+1, &rfds, NULL, NULL, &ts); //关注readfd(rfds)所包含的(句柄)是否有数据可读
    if(ret < 0)
    {
        perror("select\n");
        return -1;
    }
    else if(0 == ret)
    {
        printf("timeout \n");
        return -1;
    }
    else
    {
       if(FD_ISSET(vfd,&rfds))    //if有数据可读
       {
           //get one frame from queue
            if(ioctl(vfd, VIDIOC_DQBUF, &buffer)< 0)
            {
                perror("ioctl-DQBUF");
                return -1;
            }
           //从视频缓冲区的输出队列中取得一个已经保存有一帧视频数据的视频缓冲区;
            //VEDIO* yuv;
            //yuv = (VEDIO*)malloc(sizeof(VEDIO));
            memcpy(frame,vedio[buffer.index].start,buffer.length);
           //将内存中图片的信息复制到帧中
            if(ioctl(vfd, VIDIOC_QBUF, &buffer)< 0)
            {
                perror("ioctl-QBUF");
                return -1;
            }

       }
    }
    return 0;
  }


void OpenCamera::saveImage(__u8* address, int length)
{
    FILE* fp;
    static int num = 0;
    char picture_name[20];
    sprintf(picture_name, "picture%d.yuv", num++);
    fp = fopen(picture_name, "w");
    if( fp == NULL )
    {
        printf("open picture failure.\n");
        exit(1);
    }

    fwrite(address, length, 1, fp);
    fclose(fp);
}


void OpenCamera::closeCam()
{
    enum v4l2_buf_type v4l2_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl( vfd, VIDIOC_STREAMOFF, &v4l2_type)==-1)
    {
        perror("close vedio stream failure.\n");
        exit(1);
    }
    free(vedio);
    close(vfd);
}


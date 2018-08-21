#include "h264encoder.h"


void H264Encoder::yuyv2yuv(unsigned char *yuyv, unsigned char *yuv, int width, int height)
{
        int ynum=width*height;
        int i,j,k=0;
        //得到Y分量
        for( i = 0; i < ynum; i++ )
        {
            yuv[ i ]=yuyv[ i << 1 ];
         }
         //得到U分量
         for( i = 0; i < height; i++ )
         {
            if( ( i % 2 ) != 0 ) continue;
            for( j = 0 ; j < ( width>>1 ); j++ )
            {
                  if( ( ( j<<2 ) + 1  )> ( width<<1 ) )  break;
                  yuv[ynum + (k<<1) * (width>>2) + j]=yuyv[ (i<<1)*width + (j<<2) + 1];
             }
                    k++;
          }
             k = 0;
            //得到V分量
             for( i = 0; i < height; i++ )
             {
                 if(( i%2 ) == 0 ) continue;
                 for( j = 0; j < ( width>>1 ); j++ )
                 {
                     if( ( (j<<2) + 3 ) > ( width<<1) ) break;
                     yuv[ ynum+ (ynum>>2)+ (k<<1) * (width>>2) + j]=yuyv[ (i<<1)*width + (j<<2) + 3 ];

                  }
                  k++;
               }

//    int ynum=width*height;
//           int i,j,k=0;
//        //得到Y分量
//           for(i=0;i<ynum;i++){
//               yuv[i]=yuyv[i*2];
//           }
//        //得到U分量
//           for(i=0;i<height;i++){
//               if((i%2)!=0)continue;
//               for(j=0;j<(width/2);j++){
//                   if((4*j+1)>(2*width))break;
//                   yuv[ynum+k*2*width/4+j]=yuyv[i*2*width+4*j+1];
//                           }
//                k++;
//           }
//           k=0;
//        //得到V分量
//           for(i=0;i<height;i++){
//               if((i%2)==0)continue;
//               for(j=0;j<(width/2);j++){
//                   if((4*j+3)>(2*width))break;
//                   yuv[ynum+ynum/4+k*2*width/4+j]=yuyv[i*2*width+4*j+3];

//               }
//                k++;
//           }

}

void H264Encoder::Init(char *camerafile)
{
    int ret;
    //初始化摄像头
    camera.openCam(camerafile);
    camera.camInit();

    encoder = (my_x264_encoder *)malloc(sizeof(my_x264_encoder));
    if (!encoder){
        printf("cannot malloc my_x264_encoder !\n");
        exit(EXIT_FAILURE);
    }
    CLEAR(*encoder);

    strcpy(encoder->parameter_preset, ENCODER_PRESET);             //速度最快
    strcpy(encoder->parameter_tune, ENCODER_TUNE);            //零延迟

    encoder->x264_parameter = (x264_param_t *)malloc(sizeof(x264_param_t));
    if (!encoder->x264_parameter){
        printf("malloc x264_parameter error!\n");
        exit(EXIT_FAILURE);
    }

    /*初始化编码器*/
    CLEAR(*(encoder->x264_parameter));
    x264_param_default(encoder->x264_parameter);

    if ((ret = x264_param_default_preset(encoder->x264_parameter, encoder->parameter_preset, encoder->parameter_tune))<0){
        printf("x264_param_default_preset error!\n");
        exit(EXIT_FAILURE);
    }

    /*cpuFlags 去空缓冲区继续使用不死锁保证*/
    encoder->x264_parameter->i_threads = X264_SYNC_LOOKAHEAD_AUTO;
    /*视频选项*/
    encoder->x264_parameter->i_width = WIDTH;//要编码的图像的宽度
    encoder->x264_parameter->i_height = HEIGHT;//要编码的图像的高度
    encoder->x264_parameter->i_frame_total = 0;//要编码的总帧数，不知道用0
    encoder->x264_parameter->i_keyint_max = 10 ; //I帧后最大数目的P帧
    /*流参数*/
    encoder->x264_parameter->i_bframe = 5;   /*两个参考帧之间的B帧数目   ==5*/
    encoder->x264_parameter->b_open_gop = 0;
    encoder->x264_parameter->i_bframe_pyramid = 0;
    encoder->x264_parameter->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;

    /*log参数，不需要打印编码信息时直接注释掉*/
//    encoder->x264_parameter->i_log_level = X264_LOG_DEBUG;

    encoder->x264_parameter->i_fps_num = NUMERATOR;//码率分子
    encoder->x264_parameter->i_fps_den = DENOMINATOR;//码率分母

    encoder->x264_parameter->b_intra_refresh = 1;
    encoder->x264_parameter->b_annexb = 1;
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    strcpy(encoder->parameter_profile, ENCODER_PROFILE);
    if ((ret = x264_param_apply_profile(encoder->x264_parameter, encoder->parameter_profile))<0){
        printf("x264_param_apply_profile error!\n");
        exit(EXIT_FAILURE);
    }
    /*打开编码器*/
    encoder->x264_encoder = x264_encoder_open(encoder->x264_parameter);
    encoder->colorspace = ENCODER_COLORSPACE;

    /*初始化pic*/
    encoder->yuv420p_picture = (x264_picture_t *)malloc(sizeof(x264_picture_t));
    if (!encoder->yuv420p_picture){
        printf("malloc encoder->yuv420p_picture error!\n");
        exit(EXIT_FAILURE);
    }
    if ((ret = x264_picture_alloc(encoder->yuv420p_picture, encoder->colorspace, WIDTH, HEIGHT))<0){
        printf("ret=%d\n", ret);
        printf("x264_picture_alloc error!\n");
        exit(EXIT_FAILURE);
    }

    encoder->yuv420p_picture->img.i_csp = encoder->colorspace;
    encoder->yuv420p_picture->img.i_plane = 3;
    encoder->yuv420p_picture->i_type = X264_TYPE_AUTO;//x264自动选择

    /*申请YUV buffer*/
    encoder->yuv = (uint8_t *)malloc(WIDTH*HEIGHT * 3/2);
    if (!encoder->yuv){
        printf("malloc yuv error!\n");
        exit(EXIT_FAILURE);
    }
    CLEAR(*(encoder->yuv));
    encoder->yuv420p_picture->img.plane[0] = encoder->yuv;
    encoder->yuv420p_picture->img.plane[1] = encoder->yuv + WIDTH*HEIGHT;
    encoder->yuv420p_picture->img.plane[2] = encoder->yuv + WIDTH*HEIGHT + WIDTH*HEIGHT / 4;
       //* 构建需要编码的源数据(YUV420色彩格式)


    n_nal = 0;
    encoder->nal = (x264_nal_t *)calloc(2, sizeof(x264_nal_t));
    if (!encoder->nal){
        printf("malloc x264_nal_t error!\n");
        exit(EXIT_FAILURE);
    }
    CLEAR(*(encoder->nal));

    picture = (unsigned char *)malloc(HEIGHT * WIDTH * 2);
    memset(picture, 0, HEIGHT * WIDTH * 2);

}

void H264Encoder::GetNextFrame()
{
    if(camera.getFrame(picture) < 0)
    {
        printf("fail to get frame!\n");
        exit(EXIT_FAILURE);
    }
    //yuyv平面格式转yuv420
    yuyv2yuv(picture, encoder->yuv, WIDTH, HEIGHT);
    //memcpy(encoder->yuv, picture, WIDTH*HEIGHT*2);
    encoder->yuv420p_picture->i_pts++;
   //printf("!!!!!\n"); xiamian jiang
    if (x264_encoder_encode(encoder->x264_encoder, &encoder->nal, &n_nal, encoder->yuv420p_picture, &pic_out) < 0){
        printf("x264_encoder_encode error!\n");
        exit(EXIT_FAILURE);
    }



      //savefile();
       //printf("@@@@@@\n");
       /* for ( my_nal = encoder->nal; my_nal < encoder->nal + n_nal; ++my_nal){
        write(fd_write, my_nal->p_payload, my_nal->i_payload);
  }*/
}
void H264Encoder::Destory()
{
    free(picture);
    camera.closeCam();
    free(encoder->yuv);
    free(encoder->yuv420p_picture);
    free(encoder->x264_parameter);
    x264_encoder_close(encoder->x264_encoder);
    free(encoder);
}
void H264Encoder::opfile( )
{
    if((f= fopen("/home/habi/movies/my_encoder.264","wb+"))==NULL)
    {   printf("Open error");
        exit(EXIT_FAILURE);
    }
}
void H264Encoder::savefile()
{
    int length=0;
    uint8_t * savebuff= (uint8_t *)malloc(sizeof(uint8_t) * WIDTH *  HEIGHT * 3); // 设置缓冲区
    uint8_t * p_buff=savebuff;

    for ( int i = 0; i <n_nal; i++ )
    {
        memcpy(p_buff ,encoder->nal[i].p_payload, encoder->nal[i].i_payload);
        p_buff+= encoder->nal[i].i_payload;
        length+=encoder->nal[i].i_payload;
    }
    //rewind(f);                               //rewrite for live
     if(length>0)    {if(!fwrite(savebuff,length,1, f))    {   printf("Save error");  exit(EXIT_FAILURE);}}

}

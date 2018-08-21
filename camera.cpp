#include "camera.h"

#define WIDTH 640
#define HEIGHT 480
#define widthStep 1920
#define ENCODER_TUNE   "zerolatency"
#define ENCODER_PROFILE  "baseline"
#define ENCODER_PRESET "veryfast"
#define ENCODER_COLORSPACE X264_CSP_I420
#define CLEAR(x) (memset((&x),0,sizeof(x)))

void Convert(unsigned char *RGB, unsigned char *YUV, unsigned int width, unsigned int height);
void Cameras::Init()
{
    int ret;
    //打开第一个摄像头
    cap.open(0);
    if (!cap.isOpened())
    {
        fprintf(stderr, "Can not open camera1.\n");
        exit(-1);
    }
    cap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);

    encoder = (my_x264_encoder *)malloc(sizeof(my_x264_encoder));
    if (!encoder){
        printf("cannot malloc my_x264_encoder !\n");
        exit(EXIT_FAILURE);
    }
    CLEAR(*encoder);

    strcpy(encoder->parameter_preset, ENCODER_PRESET);
    strcpy(encoder->parameter_tune, ENCODER_TUNE);

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
    encoder->x264_parameter->i_keyint_max = 25;
    /*流参数*/
    encoder->x264_parameter->i_bframe = 5;
    encoder->x264_parameter->b_open_gop = 0;
    encoder->x264_parameter->i_bframe_pyramid = 0;
    encoder->x264_parameter->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;

    /*log参数，不需要打印编码信息时直接注释掉*/
//    encoder->x264_parameter->i_log_level = X264_LOG_DEBUG;

    encoder->x264_parameter->i_fps_num = 25;//码率分子
    encoder->x264_parameter->i_fps_den = 1;//码率分母

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

    n_nal = 0;
    encoder->nal = (x264_nal_t *)calloc(2, sizeof(x264_nal_t));
    if (!encoder->nal){
        printf("malloc x264_nal_t error!\n");
        exit(EXIT_FAILURE);
    }
    CLEAR(*(encoder->nal));

    RGB1 = (unsigned char *)malloc(HEIGHT * WIDTH * 3);

}
void Cameras::GetNextFrame()
{
    cap >> img;
    if( img.empty() )
        return;

    for (int i = 0; i< HEIGHT; i++)
    {
        for (int j = 0; j< WIDTH; j++)
        {
            RGB1[(i*WIDTH + j) * 3] = img.data[i * widthStep + j * 3 + 2];;
            RGB1[(i*WIDTH + j) * 3 + 1] = img.data[i * widthStep + j * 3 + 1];
            RGB1[(i*WIDTH + j) * 3 + 2] = img.data[i * widthStep + j * 3];
        }
    }
    printf("there is!\n");
    Convert(RGB1, encoder->yuv, WIDTH, HEIGHT);
    encoder->yuv420p_picture->i_pts++;
//printf("!!!!!\n");
    if ( x264_encoder_encode(encoder->x264_encoder, &encoder->nal, &n_nal, encoder->yuv420p_picture, &pic_out) < 0){
        printf("x264_encoder_encode error!\n");
        exit(EXIT_FAILURE);
    }
//printf("@@@@@@\n");
    /*for (my_nal = encoder->nal; my_nal < encoder->nal + n_nal; ++my_nal){
        write(fd_write, my_nal->p_payload, my_nal->i_payload);
    }*/
}
void Cameras::Destory()
{
    free(RGB1);
    cap.release();
    free(encoder->yuv);
    free(encoder->yuv420p_picture);
    free(encoder->x264_parameter);
    x264_encoder_close(encoder->x264_encoder);
    free(encoder);
}


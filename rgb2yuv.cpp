#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
//转换矩阵
#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))
//#define MY(a,b,c) (( a*  0.257 + b*  0.504  + c*  0.098+16))
//#define MU(a,b,c) (( a*( -0.148) + b*(- 0.291) + c* 0.439 + 128))
//#define MV(a,b,c) (( a*  0.439  + b*(- 0.368) + c*( - 0.071) + 128))

//大小判断
#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))
#define CLIP(a) ((a) > 255 ? 255 : ((a) < 0 ? 0 : (a)))
//RGB to YUV
void Convert(unsigned char *RGB, unsigned char *YUV, unsigned int width, unsigned int height)
{
    //变量声明
    unsigned int i, x, y, j;
    unsigned char *Y = NULL;
    unsigned char *U = NULL;
    unsigned char *V = NULL;

    Y = YUV;
    U = YUV + width*height;
    V = U + ((width*height) >> 2);
    for (y = 0; y < height; y++)
    for (x = 0; x < width; x++)
    {
        j = y*width + x;
        i = j * 3;
        Y[j] = (unsigned char)(DY(RGB[i], RGB[i + 1], RGB[i + 2]));
        if (x % 2 == 1 && y % 2 == 1)
        {
            j = (width >> 1) * (y >> 1) + (x >> 1);
            //上面i仍有效
            U[j] = (unsigned char)
                ((DU(RGB[i], RGB[i + 1], RGB[i + 2]) +
                DU(RGB[i - 3], RGB[i - 2], RGB[i - 1]) +
                DU(RGB[i - width * 3], RGB[i + 1 - width * 3], RGB[i + 2 - width * 3]) +
                DU(RGB[i - 3 - width * 3], RGB[i - 2 - width * 3], RGB[i - 1 - width * 3])) / 4);
            V[j] = (unsigned char)
                ((DV(RGB[i], RGB[i + 1], RGB[i + 2]) +
                DV(RGB[i - 3], RGB[i - 2], RGB[i - 1]) +
                DV(RGB[i - width * 3], RGB[i + 1 - width * 3], RGB[i + 2 - width * 3]) +
                DV(RGB[i - 3 - width * 3], RGB[i - 2 - width * 3], RGB[i - 1 - width * 3])) / 4);
        }
    }
}

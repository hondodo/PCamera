#include "mathelper.h"

MatHelper *MatHelper::Init = new MatHelper();
MatHelper::MatHelper()
{

}

/*********************************************************************************************************************************************************
*函数描述：  brightnessException     计算并返回一幅图像的色偏度以及，色偏方向
*函数参数：  InputImg    需要计算的图片，BGR存放格式，彩色（3通道），灰度图无效
*           cast        计算出的偏差值，小于1表示比较正常，大于1表示存在亮度异常；当cast异常时，da大于0表示过亮，da小于0表示过暗
*函数返回值： 返回值通过cast、da两个引用返回，无显式返回值
* https://blog.csdn.net/kklots/article/details/12720359
**********************************************************************************************************************************************************/
void MatHelper::brightnessException (cv::Mat InputImg, float& cast, float& da)
{
    cv::Mat GRAYimg;
    cvtColor(InputImg, GRAYimg, CV_BGR2GRAY);
    float a = 0;
    int Hist[256];
    for(int i = 0; i < 256; i++)
    Hist[i] = 0;
    for(int i = 0; i < GRAYimg.rows; i++)
    {
        for(int j = 0; j < GRAYimg.cols; j++)
        {
            a += float(GRAYimg.at<uchar>(i, j) - 128);//在计算过程中，考虑128为亮度均值点
            int x = GRAYimg.at<uchar>(i, j);
            Hist[x]++;
        }
    }
    da = a / float(GRAYimg.rows * InputImg.cols);
    float D = abs(da);
    float Ma = 0;
    for(int i = 0; i < 256; i++)
    {
        Ma += abs( i - 128 - da) * Hist[i];
    }
    Ma /= float((GRAYimg.rows * GRAYimg.cols));
    float M = abs(Ma);
    float K = D / M;
    cast = K;
    return;
}

void MatHelper::brightnessException(cv::Mat InputImg, float &cast, float &da, cv::Rect rect)
{
    cv::Mat GRAYimg = InputImg(rect);
    brightnessException(GRAYimg, cast, da);
}

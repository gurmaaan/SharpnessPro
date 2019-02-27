#include "imgservice.h"

ImgService::ImgService()
{

}

QImage ImgService::genImgWithblackBorder(const QImage &baseImg)
{
    int newW = baseImg.width() + 2;
    int newH = baseImg.height() + 2;

    QImage  newImg(QSize(newW, newH), baseImg.format());

    for(int j = 0; j < newImg.height(); j++)
    {
        for(int i = 0; i < newImg.width(); i++)
        {
            newImg.setPixelColor(i, j, QColor(Qt::black));
        }
    }

    for(int j = 1; j < baseImg.height(); j++)
    {
        for(int i = 1; i < baseImg.width(); i++)
        {
            newImg.setPixel(i,j, baseImg.pixel(i-1, j-1));
        }
    }

    return newImg;
}

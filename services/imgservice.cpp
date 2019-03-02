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

QImage ImgService::applySobelMask(const QImage &borderImg, Qt::Orientation orient)
{
    int newW = borderImg.width() - 2;
    int newH = borderImg.height() - 2;
    QImage newImg(QSize(newW, newH), borderImg.format());
    QVector<QVector<int>> maskVector;
    QVector<int> v1, v2, v3;

    if(orient == Qt::Horizontal)
    {
        v1 << -1 << -2 << -1;
        v2 <<  0 <<  0 <<  0;
        v3 <<  1 <<  2 <<  1;
    }
    else if(orient == Qt::Vertical)
    {
        v1 << -1 << 0 << 1;
        v2 << -2 << 0 << 2;
        v3 << -1 << 0 << 1;
    }
    maskVector << v1 << v2 << v3;

    Mask mask(maskVector);

    for(int j = 1; j < borderImg.height() -1; j++)
    {
        for(int i = 1; i < borderImg.width() -1; i++)
        {
            int s1, s2, s3, s4, s5, s6, s7, s8, s9;
            s1 = qGray(borderImg.pixel(i-1, j-1)) * mask.at(0,0);
            s2 = qGray(borderImg.pixel(i  , j-1)) * mask.at(0,1);
            s3 = qGray(borderImg.pixel(i+1, j-1)) * mask.at(0,2);
            s4 = qGray(borderImg.pixel(i-1, j  )) * mask.at(1,0);
            s5 = qGray(borderImg.pixel(i  , j  )) * mask.at(1,1);
            s6 = qGray(borderImg.pixel(i+1, j  )) * mask.at(1,2);
            s7 = qGray(borderImg.pixel(i-1, j+1)) * mask.at(2,0);
            s8 = qGray(borderImg.pixel(i  , j+1)) * mask.at(2,1);
            s9 = qGray(borderImg.pixel(i+1, j+1)) * mask.at(2,2);
            int sum = s1+s2+s3+s4+s5+s6+s7+s8+s9;
            if(sum < 0)
                sum = sum * (-1);
            sum = validComponent(sum);
            newImg.setPixelColor(i-1,j-1, QColor(sum, sum, sum));
        }
    }

    return newImg;
}

QImage ImgService::evklid(const QImage &vertical, const QImage &horizontal)
{
    QImage newImg(vertical.size(), vertical.format());
    if(vertical.size() == horizontal.size())
    {
        for(int j = 0; j < newImg.height(); j++)
        {
            for(int i = 0; i < newImg.width(); i++)
            {
                int v = qGray(vertical.pixel(i,j));
                int h = qGray(horizontal.pixel(i,j));
                int n = validComponent(static_cast<int>(qSqrt( qPow(v,2) + qPow(h,2) )));
                newImg.setPixelColor(i,j,QColor(n,n,n));
            }
        }
    }
    return newImg;
}

QImage ImgService::manhattan(const QImage &vertical, const QImage &horizontal)
{
    QImage newImg(vertical.size(), vertical.format());
    if(vertical.size() == horizontal.size())
    {
        for(int j = 0; j < newImg.height(); j++)
        {
            for(int i = 0; i < newImg.width(); i++)
            {
                int v = qGray(vertical.pixel(i,j));
                int h = qGray(horizontal.pixel(i,j));
                int n = validComponent(qAbs(v) + qAbs(h));
                newImg.setPixelColor(i,j,QColor(n,n,n));
            }
        }
    }
    return newImg;
}

QImage ImgService::threshold(const QImage &sobelImg, int porog)
{
    QImage newImg(sobelImg.size(), sobelImg.format());
    for (int i = 0; i < sobelImg.width(); i++)
    {
        for(int j = 0; j < sobelImg.height(); j++)
        {
            if(qGray(sobelImg.pixel(i, j)) >= porog)
                newImg.setPixelColor(i,j, QColor(Qt::white));
            else
                newImg.setPixelColor(i,j,QColor(Qt::black));
        }
    }
    return newImg;
}

int ImgService::validComponent(int c)
{
    if(c > 255)
        return 255;
    else {
        return c;
    }
}

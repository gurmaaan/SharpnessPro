#ifndef IMGSERVICE_H
#define IMGSERVICE_H

#include <QObject>
#include <QImage>
#include <QDebug>
#include <QtMath>
//
#include <mask.h>

class ImgService
{
public:
    ImgService();
    //
    QImage genImgWithblackBorder(const QImage &baseImg);
    QImage applySobelMask(const QImage &borderImg, Qt::Orientation orient);
    QImage evklid(const QImage &vertical, const QImage &horizontal);
    QImage manhattan(const QImage &vertical, const QImage &horizontal);
    int validComponent(int c);
};

#endif // IMGSERVICE_H

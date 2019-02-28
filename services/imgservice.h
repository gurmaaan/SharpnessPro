#ifndef IMGSERVICE_H
#define IMGSERVICE_H

#include <QObject>
#include <QImage>
#include <QDebug>
//
#include <mask.h>

class ImgService
{
public:
    ImgService();
    //
    QImage genImgWithblackBorder(const QImage &baseImg);
    QImage applySobelMask(QImage borderImg, Qt::Orientation orient);
    int validComponent(int c);
};

#endif // IMGSERVICE_H

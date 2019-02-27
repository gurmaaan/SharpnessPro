#ifndef IMGSERVICE_H
#define IMGSERVICE_H

#include <QObject>
#include <QImage>

class ImgService
{
public:
    ImgService();
    //
    QImage genImgWithblackBorder(const QImage &baseImg);
};

#endif // IMGSERVICE_H

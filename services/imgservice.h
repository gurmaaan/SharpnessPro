#ifndef IMGSERVICE_H
#define IMGSERVICE_H

#include <QObject>
#include <QImage>
#include <QDebug>
#include <QtMath>
//
#include <services/mask.h>
#include <services/obj.h>

class ImgService
{
public:
    ImgService();
    //
    QImage genImgWithblackBorder(const QImage &baseImg);
    QImage applySobelMask(const QImage &borderImg, Qt::Orientation orient);
    QImage evklid(const QImage &vertical, const QImage &horizontal);
    QImage manhattan(const QImage &vertical, const QImage &horizontal);
    QImage threshold(const QImage &sobelImg, int porog);
    int validComponent(int c);
    //labeling
    QVector<Obj> labeling(QImage thresh);
    unsigned char* convertToMatrix(QImage thresh);
    QVector<QVector<int>> convertToVector(int *matrix, unsigned short W, unsigned short H);
    void labelImg(unsigned short W, unsigned short H,
                  unsigned char* input, int* output);
    void labelComponent(unsigned short W, unsigned short H,
                        unsigned char* input, int* output,
                        int labelN, unsigned short x, unsigned short y);
};

#endif // IMGSERVICE_H

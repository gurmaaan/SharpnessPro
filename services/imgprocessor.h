#ifndef IMGPROCESSOR_H
#define IMGPROCESSOR_H

#include <QObject>
#include <QImage>
//
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class ImgProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImgProcessor(QObject *parent = nullptr);
    QImage gaussianBlur(QImage img, int kernelSize);
    QImage sobel(QImage img);
    QImage erosion(QImage thresh, int kernelSize, int kernelType);
    QImage dilation(QImage thresh,int kernelSize, int kernelType);

signals:
    void imgUpdated(QImage img);

public slots:
};

#endif // IMGPROCESSOR_H

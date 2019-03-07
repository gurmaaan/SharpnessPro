#include "imgprocessor.h"

using namespace cv;

ImgProcessor::ImgProcessor(QObject *parent) : QObject(parent)
{

}

QImage ImgProcessor::gaussianBlur(QImage img, int kernelSize)
{
    QImage out;
    if(img.format() == QImage::Format_RGB32)
    {
        QImage conv = img.convertToFormat(QImage::Format_ARGB32);
        Mat srcImg(conv.height(), conv.width(), CV_8UC4, (void *)conv.constBits(), conv.bytesPerLine());
        GaussianBlur(srcImg, srcImg, cv::Size(kernelSize,kernelSize), 0, 0, BORDER_DEFAULT );
        out = QImage(static_cast<uchar*>(srcImg.data), srcImg.cols, srcImg.rows, srcImg.step[0], QImage::Format_ARGB32);
        emit imgUpdated(out);
    }
    return out;
}

QImage ImgProcessor::sobel(QImage img)
{
    QImage out;
    if(img.format() == QImage::Format_RGB32)
    {
        QImage conv = img.convertToFormat(QImage::Format_ARGB32);
        Mat src(conv.height(), conv.width(), CV_8UC4, (void *)conv.constBits(), conv.bytesPerLine());

        Mat gray;
        Mat gradX, gradY;
        Mat absGradX, absGradY;
        Mat grad;

        cvtColor(src, gray, COLOR_BGR2GRAY);

        Sobel(gray, gradX, CV_16S, 1, 0);
        convertScaleAbs(gradX, absGradX);
        Sobel(gray, gradY, CV_16S, 0, 1);
        convertScaleAbs(gradY, absGradY);
        addWeighted(absGradX, 0.5, absGradY, 0.5, 0l, grad);

        out = QImage(static_cast<uchar*>(grad.data), grad.cols, grad.rows, grad.step[0], QImage::Format_ARGB32);
        imshow("Sobel", grad);
        emit imgUpdated(out);
    }
    return out;
}

QImage ImgProcessor::erosion(QImage thresh, int kernelSize, int kernelType)
{
    QImage out;
    if(thresh.format() == QImage::Format_RGB32)
    {
        QImage conv = thresh.convertToFormat(QImage::Format_ARGB32);
        Mat srcImg(conv.height(), conv.width(), CV_8UC4, (void *)conv.constBits(), conv.bytesPerLine());

        Mat element = getStructuringElement(kernelType, Size( 2*kernelSize + 1, 2*kernelSize+1 ), Point( kernelSize, kernelSize ) );
        Mat outImg;
        erode( srcImg, outImg, element );
        out = QImage(static_cast<uchar*>(outImg.data), outImg.cols, outImg.rows, outImg.step[0], QImage::Format_ARGB32);
        emit imgUpdated(out);
    }
    return out;
}

QImage ImgProcessor::dilation(QImage thresh, int kernelSize, int kernelType)
{
    QImage out;
    if(thresh.format() == QImage::Format_RGB32)
    {
        QImage conv = thresh.convertToFormat(QImage::Format_ARGB32);
        Mat srcImg(conv.height(), conv.width(), CV_8UC4, (void *)conv.constBits(), conv.bytesPerLine());

        Mat element = getStructuringElement(kernelType, Size( 2*kernelSize + 1, 2*kernelSize+1 ), Point( kernelSize, kernelSize ) );
        Mat outImg;
        dilate(srcImg, outImg, element);
        out = QImage(static_cast<uchar*>(outImg.data), outImg.cols, outImg.rows, outImg.step[0], QImage::Format_ARGB32);
        emit imgUpdated(out);
    }
    return out;
}

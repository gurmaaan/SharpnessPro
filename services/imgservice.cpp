#include "imgservice.h"

ImgService::ImgService()
{

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
    if(c > 0)
    {
        if(c > 255)
            return 255;
        else {
            return c;
        }
    } else {
        return 0;
    }
}

QVector<Obj> ImgService::labeling(QImage thresh)
{   
    unsigned short W = static_cast<unsigned short>(thresh.width());
    unsigned short H = static_cast<unsigned short>(thresh.height());

    unsigned char* matrix = static_cast<unsigned char *>(malloc(W*H*sizeof(unsigned char)));
    int* output = static_cast<int*>(malloc(W*H*sizeof(int)));
    memset(output, 0, W*H*sizeof(int));
    matrix = convertToMatrix(thresh);
    labelImg(W,H, matrix, output);
    QVector<QVector<int>> labeledVector = convertToVector(output, W, H);

    QList<int> uniqLabelsNumbers;
    for(int y = 0; y < H; y++)
    {
        QVector<int> row = labeledVector.at(y);
        for(int x = 0; x < W; x++)
        {
            int l = row.at(x);
            if(l > 0)
                uniqLabelsNumbers << row.at(x);
        }
    }
    uniqLabelsNumbers = uniqLabelsNumbers.toSet().toList();
    std::sort(uniqLabelsNumbers.begin(), uniqLabelsNumbers.end());

    QVector<Obj> objVector;
    for(int num : uniqLabelsNumbers)
        objVector << Obj(num);

    for(int y = 0; y < H; y++)
    {
        QVector<int> row = labeledVector.at(y);
        for(int x = 0; x < W; x++)
        {
            int l = row.at(x);
            if(l > 0)
            {
                objVector[l - 1].appendPoint(QPoint(x,y));
            }
        }
    }

//    for(Obj ob : objVector)
//    {
//        qDebug() << ob.points().length();
//        qDebug() << ob.s();
//    }
    return objVector;
}

unsigned char* ImgService::convertToMatrix(QImage thresh)
{
    unsigned short W = static_cast<unsigned short>(thresh.width());
    unsigned short H = static_cast<unsigned short>(thresh.height());
    unsigned char* matrix = static_cast<unsigned char *>( malloc(W*H*sizeof(unsigned char)));
    int index = -1;
    for(unsigned short y = 0; y < H; y++)
    {
        for(unsigned short x = 0; x < W; x++)
        {
            index++;
            matrix[index] = (thresh.pixelColor(x,y) == QColor(Qt::white)) ? 1 : 0;
        }
    }
    return matrix;
}

QVector<QVector<int> > ImgService::convertToVector(int *matrix, unsigned short W, unsigned short H)
{
   QVector<QVector<int>> output;
   for(unsigned short y = 0; y < H; y++)
   {
       QVector<int> row;
       for(unsigned short x = 0; x < W; x++)
       {
           int index = x + W*y;
           row << matrix[index];
       }
       output << row;
   }
   return output;
}

void ImgService::labelImg(unsigned short W, unsigned short H, unsigned char *input, int *output)
{
    int labelN = 0;
    int index = -1;
    for (unsigned short y = 0; y < H; y++)
    {
        for (unsigned short x = 0; x < W; x++)
        {
            index++;

            if (input[index] == 0)
                continue;
            if (output[index] != 0)
                continue;

            labelN++;
            labelComponent(W, H, input, output, labelN, x, y);
        }
    }
}

void ImgService::labelComponent(unsigned short W, unsigned short H, unsigned char *input, int *output, int labelN, unsigned short x, unsigned short y)
{
    //BUG: тут почему-то вылетает если порог меньше 5
    int index = x + W*y;
    if (input[index] == 0)
        return;
    if (output[index] != 0)
        return;

    output[index] = labelN;

    if (x > 0)
        labelComponent(W, H, input, output, labelN, x-1, y  );
    if (x < W-1)
        labelComponent(W, H, input, output, labelN, x+1, y  );
    if (y > 0)
        labelComponent(W, H, input, output, labelN, x  , y-1);
    if (y < H-1)
        labelComponent(W, H, input, output, labelN, x  , y+1);
}

void ImgService::fillPixel(QImage *thresh, Obj obj, QColor clr)
{
    for(QPoint p : obj.points())
    {
        thresh->setPixelColor(p, clr);
    }
}

double ImgService::sharpnessK(QImage sobelImg, QImage thrImg)
{
    QVector<QPoint> whitePoints;
    for(int x = 0; x < thrImg.width(); x++)
    {
        for(int y = 0; y < thrImg.height(); y++)
        {
            if(thrImg.pixelColor(x, y) == QColor(Qt::white))
                whitePoints << QPoint(x, y);
        }
    }

    long int sum = 0;

    if(whitePoints.size() > 0)
    {
        for(QPoint p : whitePoints)
            sum += qGray(sobelImg.pixel(p));
    }
    else {
        //qDebug() << "Xui";
        return 0;
    }
    double k = static_cast<double>(sum) / static_cast<double>(whitePoints.count());
    return k;
}

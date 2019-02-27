#include "mask.h"

Mask::Mask(QSize size, MaskType mt) :
    _size(size), _maskType(mt)
{
    int w = size.width();
    int h = size.height();
    setMask(generator(w,h,mt));
}

Mask::Mask(QSize size, int mt)
{
    MaskType t = typeProvider(mt);
    int w = size.width();
    int h = size.height();
    setMask(generator(w,h,t));
}

Mask::Mask(QVector<QVector<int> > maskVector)
{
    setMask(maskVector);
}

QVector<QVector<int> > Mask::mask() const
{
    return _mask;
}

void Mask::setMask(const QVector<QVector<int> > &mask)
{
    _mask = mask;
    int h = mask.length();
    int w = mask.first().length();
    setSize(QSize(w, h));
}

QSize Mask::size() const
{
    return _size;
}

void Mask::print()
{
    qDebug() << "------ mask ------";
    for(int i = 0; i < _mask.length(); i++)
    {
        QString rowStr ="";
        QVector<int> row = _mask.at(i);
        for(int j = 0; j < row.length(); j++)
        {
            if(row.at(j) > 0)
                rowStr = rowStr + QString(" ") + QString::number(row.at(j));
            else
                rowStr = rowStr + QString::number(row.at(j));
        }
        qDebug().noquote() << rowStr;
    }
    qDebug() << "------------------";
}

int Mask::positiveSum()
{
    long int sum = 0;
    for(int i = 0; i < _mask.length(); i++)
    {
        QVector<int> row = _mask.at(i);
        for(int j = 0; j < row.length(); j++)
        {
            if(row.at(j) > 0)
                sum+=row.at(j);
        }
    }
    return sum;

}

int Mask::at(int x, int y)
{
    int output = 0;

    if(y >= 0 && y < size().height())
    {
        QVector<int> row = _mask.at(y);
        if(x >= 0 && x < size().width())
            output = row.at(x);
    }

    return output;
}

void Mask::setSize(const QSize &size)
{
    _size = size;
}

QVector<QVector<int> > Mask::generator(int w, int h, MaskType t)
{
    int halfW = w / 2;
    int halfH = h / 2;
    QVector<QVector<int> > mask;
    if(t == MaskType::VerticalMack)
    {
        for(int j = 0; j < h; j++)
        {
            QVector<int> row;
            for(int i = 0; i < w; i++)
            {
                if(i < halfW)
                    row << 1;
                else
                    row << -1;
            }
            mask << row;
            row.clear();
        }
    }
    else
    {
        for(int j = 0; j < h; j++)
        {
            QVector<int> row;
            for(int i = 0; i < w; i++)
            {
                if(j < halfH)
                    row << 1;
                else
                    row << -1;
            }
            mask << row;
            row.clear();
        }
    }
    return mask;
}

MaskType Mask::typeProvider(int t)
{
    MaskType type;
    type = (t == 0) ? MaskType::VerticalMack : MaskType::HorizontalMask;
    return type;
}

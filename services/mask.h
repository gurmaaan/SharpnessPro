#ifndef MASK_H
#define MASK_H

#include <QVector>
#include <QDebug>
#include <QSize>

enum class MaskType
{
    VerticalMack,
    HorizontalMask
};

class Mask
{
public:
    Mask(QSize size, MaskType mt);
    Mask(QSize size, int mt);
    Mask(QVector<QVector<int>> maskVector);
    QVector<QVector<int> > mask() const;
    QSize size() const;
    void setMask(const QVector<QVector<int> > &mask);
    //
    void print();
    int positiveSum();
    int at(int x, int y);

private:
    QSize _size;
    MaskType _maskType;
    QVector< QVector<int> > _mask;
    //
    void setSize(const QSize &size);
    QVector< QVector<int> > generator(int w, int h, MaskType t);
    MaskType typeProvider(int t);
};

#endif // MASK_H

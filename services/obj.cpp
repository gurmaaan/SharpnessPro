#include "obj.h"

Obj::Obj()
{
    Obj(0);
}

Obj::Obj(int id) :
    _id(id)
{
    qDebug() << _id;
}

Obj::Obj(const Obj &o) :
    _id(o._id), _points(o._points), _s(o._s)
{

}

int Obj::id() const
{
    return _id;
}

void Obj::setId(int id)
{
    _id = id;
}

QVector<QPoint> Obj::points() const
{
    return _points;
}

void Obj::setPoints(const QVector<QPoint> &points)
{
    _points = points;
}

void Obj::appendPoint(QPoint p)
{
    _points.append(p);
}

int Obj::s() const
{
    return _s;
}

void Obj::setS(int s)
{
    _s = s;
}

void Obj::calcS()
{
    setS(_points.length());
}

#ifndef OBJ_H
#define OBJ_H

#include <QDebug>
#include <QPoint>

class Obj
{
public:
    Obj();
    Obj(int id);
    Obj(const Obj &o);

    int id() const;
    void setId(int id);

    QVector<QPoint> points() const;
    void setPoints(const QVector<QPoint> &points);
    void appendPoint(QPoint p);

    int s() const;
    void setS(int s);

private:
    int _id;
    QVector<QPoint> _points;
    int _s;
    //
    void calcS();
};

#endif // OBJ_H

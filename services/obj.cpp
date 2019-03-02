#include "obj.h"

Obj::Obj(int num) :
    _num(num)
{
    qDebug() << _num;
}

Obj::Obj(const Obj &o) :
    _num(o._num)
{

}

#ifndef OBJ_H
#define OBJ_H

#include <QDebug>

class Obj
{
public:
    Obj() {}
    Obj(int num);
    Obj(const Obj &o);

private:
    int _num;
};

#endif // OBJ_H

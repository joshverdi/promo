#ifndef PTURTLE_H
#define PTURTLE_H

#include "viewport/pvector3d.h"

class PTurtle
{
public:
    PVector3D position;
    PVector3D vec_orientation;
public:
    PTurtle();
    PTurtle(PVector3D _position, PVector3D _vec_orientation);
};

#endif // PTURTLE_H

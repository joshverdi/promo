#include "pvector3d.h"

PVector3D::PVector3D()
{
    z=y=x=0;
}

PVector3D::PVector3D(float _x, float _y, float _z)
{
    x = _x;
    y = _y;
    z = _z;
}

PVector3D& PVector3D::operator=(const PVector3D& _vector3D)
{
    this->x = _vector3D.x;
    this->y = _vector3D.y;
    this->z = _vector3D.z;
    return *this;
}


bool PVector3D::operator==(const PVector3D& _vector3D) const
{
    return ((x == _vector3D.x) && (y == _vector3D.y) && (z == _vector3D.z));
}

bool PVector3D::operator!=(const PVector3D& _vector3D) const
{
    return !operator==(_vector3D);
}

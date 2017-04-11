#ifndef PVECTOR3D_H
#define PVECTOR3D_H

class PVector3D
{
public:
    float x, y, z;
public:
    PVector3D();
    PVector3D(float _x, float _y, float _z);
    PVector3D& operator=(const PVector3D& _vector3D);
    bool operator==(const PVector3D& _vector3D) const;
    bool operator!=(const PVector3D& _vector3D) const;
};

#endif // PVECTOR3D_H

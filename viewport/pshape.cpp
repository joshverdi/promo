#include "pshape.h"

GLUquadric* PShape::quad_obj = gluNewQuadric();

PShape::PShape()
{
    normal = GLU_SMOOTH;
    gluQuadricNormals(quad_obj, normal);
}

PShape::PShape(GLUquadric* _qobj)
{
    quad_obj = _qobj;
}

PShape::~PShape()
{
}

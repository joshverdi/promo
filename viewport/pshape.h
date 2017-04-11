#ifndef PSHAPE_H
#define PSHAPE_H

#include "GL/glu.h"
#include <vector>
#include "CGLA/Vec3f.h"
#include "viewport/pcglaprocessor.h"

using namespace CGLA;

class PShape
{
protected:
    static GLUquadric* quad_obj;
    GLenum normal;
public:
    PShape();
    PShape(GLUquadric* _qobj);
    ~PShape();
    virtual void draw() = 0;
    virtual void draw(PCglaProcessor& _cgla_proc) = 0;
};

#endif // PSHAPE_H

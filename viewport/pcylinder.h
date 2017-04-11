#ifndef PCYLINDER_H
#define PCYLINDER_H
#include "pshape.h"
#include "viewport/pcglaprocessor.h"

class PCylinder : public PShape
{
protected:
    GLdouble base, top, height;
    GLint slices, stacks;
public:
    PCylinder();
    PCylinder(GLUquadric* _qobj);
    PCylinder(GLdouble _height);
    PCylinder(GLdouble _height, GLdouble _base, GLdouble _top);
    PCylinder(const PCylinder& _cyl);
    void setBase(GLdouble _base){base=_base;}
    void setTop(GLdouble _top){top=_top;}
    ~PCylinder();
    float getBase() const {return ((float) base);}
    float getTop() const  {return ((float) top);}
    float getHeight() const {return ((float) height);}
    void draw();
    void draw(PCglaProcessor& _cgla_proc);
};

#endif // PCYLINDER_H

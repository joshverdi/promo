#ifndef PLINE_H
#define PLINE_H

#include "viewport/pshape.h"
#include "viewport/pcglaprocessor.h"

class PLine : public PShape
{
    GLfloat length;
    GLfloat width;
public:
    PLine();
    PLine(GLfloat _length);
    PLine(const PLine& _line);
    float getLength() const {return ((GLfloat) length);}
    void draw();
    void draw(PCglaProcessor& _cgla_proc);
};

#endif // PLINE_H

#ifndef PBOX_H
#define PBOX_H

#include "pshape.h"
#include "viewport/pcglaprocessor.h"

class PBox : public PShape
{
    GLdouble length, width, height;
    GLfloat vertices[8][3];
    GLfloat normals[6][3];
    GLint faces[6][4];
public:
    PBox();
    PBox(GLdouble _length,GLdouble _width, GLdouble _height);
    void init_vertices();
    void init_normals();
    void init_faces();
    void init();
    void draw();
    void draw(PCglaProcessor& _cgla_proc);
};

#endif // PBOX_H

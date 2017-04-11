#ifndef PSIMPLISTICLEAF_H
#define PSIMPLISTICLEAF_H

#include "pcylinder.h"

class PSimplisticLeaf : public PCylinder
{
    // Upper pyramid
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat leaf_base;
    GLfloat leaf_height;

    // Lower pyramid
    GLfloat u;
    GLfloat v;
    GLfloat w;
    
    // Next line is about the subdivision of the leaf
    GLfloat section;

public:
    PSimplisticLeaf();
    PSimplisticLeaf(GLdouble _height);
    ~PSimplisticLeaf();
    void draw();
    void draw(PCglaProcessor& _cgla_proc);
    void update_size();
};

#endif // PSIMPLISTICLEAF_H

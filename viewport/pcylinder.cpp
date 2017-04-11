#include "engine/procedural.h"
#include "viewport/pcylinder.h"
#include "CGLA/Vec3f.h"

PCylinder::PCylinder()
{
    top = base      = 0.1*PROCEDURAL_UNIT;
    height          = PROCEDURAL_UNIT;
    stacks = 1;
    slices = 16;
}

PCylinder::PCylinder(GLUquadric* _qobj):PShape(_qobj)
{
    top = base      = 0.1*PROCEDURAL_UNIT;
    height          = PROCEDURAL_UNIT;
    stacks = 1;
    slices = 16;
    quad_obj            = _qobj;
}

PCylinder::PCylinder(GLdouble _height)
{
    top = base      = 0.1*PROCEDURAL_UNIT;
    height          = _height*PROCEDURAL_UNIT;
    stacks = 1;
    slices = 16;
}

PCylinder::PCylinder(GLdouble _height, GLdouble _base, GLdouble _top)
{
    top             = _top*PROCEDURAL_UNIT;
    base            = _base*PROCEDURAL_UNIT;;
    height          = _height*PROCEDURAL_UNIT;
    stacks = 1;
    slices = 16;
}

PCylinder::PCylinder(const PCylinder& _cyl):PShape()
{
    top             = _cyl.getTop()*PROCEDURAL_UNIT;
    base            = _cyl.getBase()*PROCEDURAL_UNIT;;
    height          = _cyl.getHeight()*PROCEDURAL_UNIT;
    stacks = 1;
    slices = 16;
}

PCylinder::~PCylinder()
{
}

void PCylinder::draw()
{
    glRotatef(-90, 1.0, 0.0, 0.0);
    gluCylinder(quad_obj, base, top, height, slices, stacks);
    glRotatef(+90, 1.0, 0.0, 0.0);
    glTranslatef(0.0f, height*PROCEDURAL_UNIT, 0.0f);
}

void PCylinder::draw(PCglaProcessor& _cgla_proc)
{
    CGLA::Vec3f vec(1.0, 0.0, 0.0);
    GLfloat gl_mat[16];
    float angle=-90;
    _cgla_proc.cgla_rotate(angle, vec);
    _cgla_proc.to_gl_matrix(gl_mat);

    // Before every drawing process, OpenGL matrix must be saved and updated
    glPushMatrix();
    glMultMatrixf(gl_mat);
    gluCylinder(quad_obj, base, top, height, slices, stacks);
    glPopMatrix();

    // Rotate back and translate properly
    angle=90;
    _cgla_proc.cgla_rotate(angle, vec);
    vec=CGLA::Vec3f(0.0f, height*PROCEDURAL_UNIT, 0.0f);
    _cgla_proc.cgla_translate(vec);
}

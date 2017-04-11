#include "engine/procedural.h"
#include "viewport/pline.h"
#include "CGLA/Vec3f.h"

PLine::PLine()
{
    width = 1;
    length = PROCEDURAL_UNIT;
}

PLine::PLine(GLfloat _length)
{
    width = 1;
    length = _length;
}

PLine::PLine(const PLine& _line):PShape()
{
    width = 1;
    length = _line.getLength();
}

void PLine::draw()
{
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(0.0f, length, 0.0f);
    glEnd();
    glTranslatef(0.0f, length, 0.0f);
}

void PLine::draw(PCglaProcessor& _cgla_proc)
{
    CGLA::Vec3f vec(0, length, 0);
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex3f(0.0f,0.0f,0.0f);
    glVertex3f(0.0f, length, 0.0f);
    glEnd();
    _cgla_proc.cgla_translate(vec);
}

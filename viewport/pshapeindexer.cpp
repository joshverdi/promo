#include "pshapeindexer.h"
#include "viewport/pcylinder.h"
#include "viewport/pline.h"
#include "CGLA/Mat4x4f.h"

PShapeIndexer::PShapeIndexer():shape(NULL)
{
}

PShapeIndexer::PShapeIndexer(CGLA::Vec3f _supervertex):shape(NULL)
{

    supervertex = _supervertex;
}

PShapeIndexer::PShapeIndexer(unsigned int _name, CGLA::Vec3f _supervertex):shape(NULL)
{
    name = _name;
    supervertex = _supervertex;
}

PShapeIndexer::PShapeIndexer(unsigned int _name, CGLA::Vec3f _supervertex, CGLA::Vec3f _left, CGLA::Vec3f _head, CGLA::Vec3f _up)
:shape(NULL)
{
    name = _name;
    supervertex = _supervertex;
    left = _left;head=_head;up=_up;
}

PShapeIndexer::PShapeIndexer(unsigned int _name, CGLA::Vec3f _supervertex, CGLA::Vec3f _left, CGLA::Vec3f _head, CGLA::Vec3f _up, PShape* _shape)
{
    shape = _shape;
    name = _name;
    supervertex = _supervertex;
    left = _left;head=_head;up=_up;
    bind(shape);
}

PShapeIndexer::~PShapeIndexer()
{
    delete shape;
}

void PShapeIndexer::init()
{
    supervertex = left = head = up = CGLA::Vec3f(0,0,0);
    name = 0;
    vec_vertices.clear();
}

PShapeIndexer& PShapeIndexer::operator=(const PShapeIndexer& _si)
{
    vec_vertices=_si.getVertices();
    name = _si.getName();
    left= _si.getLeft();
    head = _si.getHead();
    up = _si.getUp();
    supervertex = _si.getSupervertex();
    return *this;
}

std::vector<CGLA::Vec3f> PShapeIndexer::bind(PShape* _shape)
{
    std::vector<CGLA::Vec3f> value;
    CGLA::Vec3f pt3d = Vec3f(0,0,0);
    PCylinder* cyl;
    PLine* line;
    cyl = dynamic_cast<PCylinder*>(_shape);
    line = dynamic_cast<PLine*>(_shape);
    if (cyl)
    {
        shape = new PCylinder(*cyl);
        // cylinder
        pt3d = supervertex; // basis cylinder in case of a cylinder
        value.push_back(pt3d);
        pt3d = supervertex + Vec3f(cyl->getBase()*(-left[0]), cyl->getBase()*(-left[1]), cyl->getBase()*(-left[2]));
        value.push_back(pt3d);
        for (int i=0; i<7; i++)
        {
            CGLA::Vec3f tmp=pt3d;
            pt3d=Procedural::local_rot(tmp, head, supervertex, 45);
            //pt3d=rotate(tmp, head, 45);
            value.push_back(pt3d);
        }
        // Top center:
        pt3d = supervertex + Vec3f(cyl->getHeight()*head[0],cyl->getHeight()*head[1],cyl->getHeight()*head[2]);
        value.push_back(pt3d);
        pt3d = pt3d + Vec3f(cyl->getTop()*(-left[0]), cyl->getTop()*(-left[1]), cyl->getTop()*(-left[2]));
        value.push_back(pt3d);
        for (int i=0; i<7; i++)
        {
            CGLA::Vec3f tmp=pt3d;
            pt3d=Procedural::local_rot( tmp,
                                        head,
                                        supervertex + Vec3f(cyl->getHeight()*head[0],cyl->getHeight()*head[1],cyl->getHeight()*head[2]),
                                        45);
            //pt3d=rotate(tmp, head, 45);
            value.push_back(pt3d);
        }
    }
    else if (line)
    {
        //shape = line;
        shape = new PLine(*line);
        pt3d = supervertex;
        value.push_back(pt3d);
        pt3d = supervertex + Vec3f(line->getLength()*(head[0]), line->getLength()*(head[1]), line->getLength()*(head[2]));
        value.push_back(pt3d);
    }
    vec_vertices = value;
    return value;
}

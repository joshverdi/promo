#ifndef PSHAPEINDEXER_H
#define PSHAPEINDEXER_H
#include "CGLA/Vec3f.h"
#include "CGLA/Mat3x3f.h"
#include "pshape.h"
#include <vector>

class PShapeIndexer
{
    std::vector<CGLA::Vec3f> vec_vertices;
    CGLA::Vec3f supervertex;               // this is a reference 3d point like barycentre, depending on the shape allure
    CGLA::Vec3f left, head, up;            // this is the turtle state at a given stage
    unsigned int name;
    PShape* shape;
public:
    PShapeIndexer();
    PShapeIndexer(CGLA::Vec3f _supervertex);
    PShapeIndexer(unsigned int _name, CGLA::Vec3f _supervertex);
    PShapeIndexer(unsigned int _name, CGLA::Vec3f _supervertex, CGLA::Vec3f _left, CGLA::Vec3f _head, CGLA::Vec3f _up);
    PShapeIndexer(unsigned int _name, CGLA::Vec3f _supervertex, CGLA::Vec3f _left, CGLA::Vec3f _head, CGLA::Vec3f _up, PShape* _shape);
    PShapeIndexer& operator=(const PShapeIndexer& _si);
    ~PShapeIndexer();
    void init();
    void setName(unsigned int _name){name=_name;}
    unsigned int getName() const {return name;}
    void setSupervertex(CGLA::Vec3f _supervertex){supervertex=_supervertex;}
    CGLA::Vec3f getSupervertex() const {return supervertex;}
    void setVertices(std::vector<CGLA::Vec3f> _vec_vertices){vec_vertices = _vec_vertices;}
    std::vector<CGLA::Vec3f> getVertices() const { return vec_vertices;}
    void setTurtle(CGLA::Vec3f _left, CGLA::Vec3f _head, CGLA::Vec3f _up){left=_left;head=_head;up=_up;}
    CGLA::Vec3f getLeft() const {return left;}
    CGLA::Vec3f getHead() const {return head;}
    CGLA::Vec3f getUp() const {return up;}
    PShape* getShape() const {return shape;}
public:
    std::vector<CGLA::Vec3f> bind(PShape* _shape);
};

#endif // PSHAPEINDEXER_H

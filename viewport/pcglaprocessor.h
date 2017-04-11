#ifndef PCGLAPROCESSOR_H
#define PCGLAPROCESSOR_H
#include <list>
#include "CGLA/Mat4x4f.h"
#include "CGLA/Vec3f.h"
#include "engine/procedural.h"
#include "GL/glu.h"

class PCglaProcessor
{
    std::list<CGLA::Mat4x4f> cgla_stack_mat4x4f;
    CGLA::Mat4x4f curr_mat4x4f;
    static CGLA::Vec3f left0(){return CGLA::Vec3f(-1.0,0.0,0.0);}
    static CGLA::Vec3f head0(){return CGLA::Vec3f(0.0,1.0,0.0);}
    static CGLA::Vec3f up0(){return CGLA::Vec3f(0.0,0.0,1.0);}
public:
    PCglaProcessor();
    PCglaProcessor(GLfloat* _ogl_mat);
    PCglaProcessor(CGLA::Mat4x4f _curr_mat4x4f);
    void init();
    unsigned int stack_size() const {return cgla_stack_mat4x4f.size();}
    void to_gl_matrix(GLfloat* _dest);
    void set_current_matrix(CGLA::Mat4x4f& _curr_mat4x4f){curr_mat4x4f=_curr_mat4x4f;}
    void get_current_matrix(CGLA::Mat4x4f& _dest){_dest=curr_mat4x4f;}
    inline void cgla_push_matrix(){cgla_stack_mat4x4f.push_back(curr_mat4x4f);}
    inline void cgla_push_matrix(CGLA::Mat4x4f _curr_mat4x4f){curr_mat4x4f = _curr_mat4x4f;cgla_stack_mat4x4f.push_back(curr_mat4x4f);}
    inline void cgla_pop_matrix(){curr_mat4x4f=cgla_stack_mat4x4f.back();cgla_stack_mat4x4f.pop_back();}
    inline void cgla_load_identity4x4f(){curr_mat4x4f=CGLA::identity_Mat4x4f();}
    inline void cgla_load_identity4x4f(CGLA::Mat4x4f& _mat){_mat=CGLA::identity_Mat4x4f();}
    void cgla_translate(CGLA::Vec3f& _vec3f, CGLA::Mat4x4f& _mat);
    void cgla_translate(CGLA::Vec3f& _vec3f);
    void cgla_rotate(float& _angle_deg, CGLA::Vec3f& _vec3f, CGLA::Mat4x4f& _mat);
    void cgla_rotate(float& _angle_deg, CGLA::Vec3f& _vec3f);
    void check_turtle_from(CGLA::Mat4x4f& _matrix,CGLA::Vec3f& _left, CGLA::Vec3f& _head, CGLA::Vec3f& _up);
    void rot_to_vert();
    void compute_basis(CGLA::Vec3f& _origin, CGLA::Vec3f& _left, CGLA::Vec3f& _head, CGLA::Vec3f& _up);
};

#endif // PCGLAPROCESSOR_H

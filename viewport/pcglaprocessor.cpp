#include "pcglaprocessor.h"
#include "engine/procedural.h"
#include "CGLA/Vec4f.h"

using namespace CGLA;

PCglaProcessor::PCglaProcessor()
{
}

PCglaProcessor::PCglaProcessor(GLfloat* _ogl_mat)
{
    curr_mat4x4f = Mat4x4f( Vec4f(_ogl_mat[0],  _ogl_mat[1],  _ogl_mat[2],  _ogl_mat[3]),
                            Vec4f(_ogl_mat[4],  _ogl_mat[5],  _ogl_mat[6],  _ogl_mat[7]),
                            Vec4f(_ogl_mat[8],  _ogl_mat[9],  _ogl_mat[10], _ogl_mat[11]),
                            Vec4f(_ogl_mat[12], _ogl_mat[13], _ogl_mat[14], _ogl_mat[15]));
}

PCglaProcessor::PCglaProcessor(CGLA::Mat4x4f _curr_mat4x4f)
{
    curr_mat4x4f = _curr_mat4x4f;
}

void PCglaProcessor::init()
{
    cgla_stack_mat4x4f.clear();
    cgla_load_identity4x4f();
}

void PCglaProcessor::to_gl_matrix(GLfloat* _dest)
{
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            _dest[4*i+j]=curr_mat4x4f[i][j];
        }
    }
}

void PCglaProcessor::cgla_translate(CGLA::Vec3f& _vec3f, CGLA::Mat4x4f& _mat)
{
    Mat4x4f translation_matrix = CGLA::identity_Mat4x4f();
    translation_matrix[3]=Vec4f(_vec3f,1.0);
    _mat = translation_matrix*_mat;
}

void PCglaProcessor::cgla_translate(CGLA::Vec3f& _vec3f)
{
    Mat4x4f translation_matrix = CGLA::identity_Mat4x4f();
    translation_matrix[3]=Vec4f(_vec3f,1.0);
    curr_mat4x4f = translation_matrix*curr_mat4x4f;
}

void PCglaProcessor::cgla_rotate(float& _angle_deg, CGLA::Vec3f& _vec3f, CGLA::Mat4x4f& _mat)
{
    Mat4x4f rot_matrix;
    float x,y,z;

    double angle_rad=_angle_deg*M_PI/180;
    float c=(float)cos(angle_rad);
    float s=(float)sin(angle_rad);

    x=_vec3f[0];
    y=_vec3f[1];
    z=_vec3f[2];

    rot_matrix = Mat4x4f   (Vec4f(x*x*(1-c)+c,   y*x*(1-c)+z*s, x*z*(1-c)-y*s, 0.0),
                            Vec4f(x*y*(1-c)-z*s, y*y*(1-c)+c,   y*z*(1-c)+x*s, 0.0),
                            Vec4f(x*z*(1-c)+y*s, y*z*(1-c)-x*s, z*z*(1-c)+c,   0.0),
                            Vec4f(0.0, 0.0, 0.0, 1.0));
    _mat=rot_matrix*_mat;
}

void PCglaProcessor::cgla_rotate(float& _angle_deg, CGLA::Vec3f& _vec3f)
{
    Mat4x4f rot_matrix;
    float x,y,z;
    double angle_rad=_angle_deg*M_PI/180;
    float c=(float)cos(angle_rad);
    float s=(float)sin(angle_rad);

    x=_vec3f[0];
    y=_vec3f[1];
    z=_vec3f[2];

    rot_matrix = Mat4x4f   (Vec4f(x*x*(1-c)+c,      y*x*(1-c)+z*s,  x*z*(1-c)-y*s, 0.0),
                            Vec4f(x*y*(1-c)-z*s,    y*y*(1-c)+c,    y*z*(1-c)+x*s, 0.0),
                            Vec4f(x*z*(1-c)+y*s,    y*z*(1-c)-x*s,  z*z*(1-c)+c, 0.0),
                            Vec4f(0.0, 0.0, 0.0, 1.0));
    curr_mat4x4f=rot_matrix*curr_mat4x4f;
}

void PCglaProcessor::check_turtle_from(CGLA::Mat4x4f& _matrix,CGLA::Vec3f& _left, CGLA::Vec3f& _head, CGLA::Vec3f& _up)
{
    _left = Vec3f((-1)*_matrix[0][0], (-1)*_matrix[0][1], (-1)*_matrix[0][2]);
    _head = Vec3f(_matrix[1][0], _matrix[1][1], _matrix[1][2]);
    _up   = Vec3f(_matrix[2][0], _matrix[2][1], _matrix[2][2]);
}

void PCglaProcessor::rot_to_vert()
{
    CGLA::Vec3f left_tmp, head_tmp, up_tmp;
    CGLA::Vec3f new_left, new_up;
    Vec3f v(0.0, 1.0, 0.0);
    check_turtle_from(curr_mat4x4f, left_tmp, head_tmp, up_tmp);

    new_left = CGLA::cross(v, head_tmp);
    new_left = CGLA::normalize(new_left);

    new_up = CGLA::cross(head_tmp, new_left);
    new_up = CGLA::normalize(new_up);

    curr_mat4x4f[0][0]=new_left[0];
    curr_mat4x4f[0][1]=new_left[1];
    curr_mat4x4f[0][2]=new_left[2];

    curr_mat4x4f[2][0]=new_up[0];
    curr_mat4x4f[2][1]=new_up[1];
    curr_mat4x4f[2][2]=new_up[2];
}

void PCglaProcessor::compute_basis(CGLA::Vec3f& _origin, CGLA::Vec3f& _left, CGLA::Vec3f& _head, CGLA::Vec3f& _up)
{
    _origin = CGLA::Vec3f(  curr_mat4x4f[3][0],         curr_mat4x4f[3][1],         curr_mat4x4f[3][2]);
    _left   = CGLA::Vec3f(  (-1)*curr_mat4x4f[0][0],    (-1)*curr_mat4x4f[0][1],    (-1)*curr_mat4x4f[0][2]);
    _head   = CGLA::Vec3f(  curr_mat4x4f[1][0],         curr_mat4x4f[1][1],         curr_mat4x4f[1][2]);
    _up     = CGLA::Vec3f(  curr_mat4x4f[2][0],         curr_mat4x4f[2][1],         curr_mat4x4f[2][2]);
}

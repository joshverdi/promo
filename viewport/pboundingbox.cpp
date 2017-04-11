#include "pboundingbox.h"
#include "engine/procedural.h"

PBoundingBox::PBoundingBox()
{
}

CGLA::Vec3f PBoundingBox::mean_point(std::vector<CGLA::Vec3f>& _vec_vertices)
{
    CGLA::Vec3f value;
    float x,y,z;
    unsigned int n;
    z=y=x=0;
    n = _vec_vertices.size();
    for (unsigned int i=0; i<n; i++)
    {
        x+=_vec_vertices[i][0];
        y+=_vec_vertices[i][1];
        z+=_vec_vertices[i][2];
    }
    x/=n;
    y/=n;
    z/=n;
    value[0]=x;
    value[1]=y;
    value[2]=z;
    return value;
}

CGLA::Mat3x3f PBoundingBox::covariance_matrix(std::vector<CGLA::Vec3f>& _vec_vertices, CGLA::Vec3f _mean)
{
    CGLA::Mat3x3f value;
    CGLA::Vec3f vec1, vec2, vec3;
    unsigned int n;
    float c11, c12, c13, c21, c22, c23, c31, c32, c33;

    c11=c12=c13=c21=c22=c23=c31=c32=c33=0;
    n = _vec_vertices.size();

    for (unsigned int i=0; i<n; i++)
    {
        c11 += pow(_vec_vertices[i][0]-_mean[0],2);
        c12 += (_vec_vertices[i][0]-_mean[0])*(_vec_vertices[i][1]-_mean[1]);
        c22 += pow(_vec_vertices[i][1]-_mean[1],2);
        c13 += (_vec_vertices[i][0]-_mean[0])*(_vec_vertices[i][2]-_mean[2]);
        c33 += pow(_vec_vertices[i][2]-_mean[2],2);
        c23 += (_vec_vertices[i][1]-_mean[1])*(_vec_vertices[i][2]-_mean[2]);
    }
    c11/=n;
    c12/=n;
    c21=c12;
    c22/=n;
    c13/=n;
    c31=c13;
    c33/=n;
    c23/=n;
    c32=c23;
    vec1 = CGLA::Vec3f(c11, c21, c31);
    vec2 = CGLA::Vec3f(c12, c22, c32);
    vec3 = CGLA::Vec3f(c13, c23, c33);
    value = CGLA::Mat3x3f(vec1, vec2, vec3);

    return value;
}

void PBoundingBox::compute_eigens(CGLA::Mat3x3f _input, CGLA::Mat3x3f& _eigenvec, CGLA::Vec3f& _eigenval)
{
    double mat_A[MATRIX_DIM][MATRIX_DIM];
    double mat_V[MATRIX_DIM][MATRIX_DIM];
    double d[MATRIX_DIM];
    for (int i=0; i<MATRIX_DIM; i++)
    {
        for (int j=0; j<MATRIX_DIM; j++)
        {
            mat_A[i][j]=_input[j][i];
        }
    }
    Procedural::eigen_decomposition(mat_A, mat_V, d);
    for (int i=0; i<MATRIX_DIM; i++)
    {
        _eigenval[i]=d[i];
        for (int j=0; j<MATRIX_DIM; j++)
        {
            _eigenvec[i][j]=mat_V[j][i];
        }
    }
}

CGLA::Vec3f PBoundingBox::normalize(CGLA::Vec3f& _vec3f)
{
    CGLA::Vec3f value;
    float length = sqrt(_vec3f[0]*_vec3f[0]+_vec3f[1]*_vec3f[1]+_vec3f[2]*_vec3f[2]);
    value[0] = _vec3f[0]/length;
    value[1] = _vec3f[1]/length;
    value[2] = _vec3f[2]/length;
    return value;
}

CGLA::Mat3x3f PBoundingBox::ortho_basis(CGLA::Mat3x3f _co_mat)
{
    CGLA::Mat3x3f value;
    CGLA::Vec3f u = _co_mat[0];
    CGLA::Vec3f v = _co_mat[1];
    CGLA::Vec3f w = _co_mat[2];
    value[0] = normalize(u);
    value[1] = normalize(v);
    value[2] = normalize(w);
    return value;
}

CGLA::Vec3f PBoundingBox::change_basis(CGLA::Vec3f& _point3d, CGLA::Vec3f& _translat, CGLA::Mat3x3f& _matrix, bool _trans_then_rotat)
{
    CGLA::Vec3f value=_point3d;
    if (_trans_then_rotat==true)
    {
        value=Procedural::translate(_point3d, _translat);
        value=_matrix*value;
    }
    else
    {
        value=_matrix*_point3d;
        value=Procedural::translate(value, _translat);
    }
    return value;
}

void PBoundingBox::extremum(const std::vector<CGLA::Vec3f>& vec_vertices,
              float& abs_min, float& abs_max,
              float& ord_min, float& ord_max,
              float& dep_min, float& dep_max)
{
    if (vec_vertices.empty())
    {
        abs_min=abs_max=ord_min=ord_max=dep_min=dep_max=0;
        return;
    }
    abs_min = abs_max = vec_vertices.front()[0];
    ord_min = ord_max = vec_vertices.front()[1];
    dep_min = dep_max = vec_vertices.front()[2];
    for (std::vector<CGLA::Vec3f>::const_iterator iter=vec_vertices.begin(); iter!=vec_vertices.end(); iter++)
    {
        abs_min = PROCEDURAL_MIN(abs_min, (*iter)[0]);
        abs_max = PROCEDURAL_MAX(abs_max, (*iter)[0]);
        ord_min = PROCEDURAL_MIN(ord_min, (*iter)[1]);
        ord_max = PROCEDURAL_MAX(ord_max, (*iter)[1]);
        dep_min = PROCEDURAL_MIN(dep_min, (*iter)[2]);
        dep_max = PROCEDURAL_MAX(dep_max, (*iter)[2]);
    }
}

std::vector<CGLA::Vec3f> PBoundingBox::box_vertices(CGLA::Vec3f& _vec3_min, CGLA::Vec3f& _vec3_max)
{
    std::vector<CGLA::Vec3f> value;
    CGLA::Vec3f tmp[8];
    tmp[0][0]=_vec3_min[0];tmp[0][1]=_vec3_min[1];tmp[0][2]=_vec3_max[2];
    tmp[1][0]=_vec3_max[0];tmp[1][1]=_vec3_min[1];tmp[1][2]=_vec3_max[2];
    tmp[2][0]=_vec3_max[0];tmp[2][1]=_vec3_max[1];tmp[2][2]=_vec3_max[2];
    tmp[3][0]=_vec3_min[0];tmp[3][1]=_vec3_max[1];tmp[3][2]=_vec3_max[2];
    tmp[4][0]=_vec3_min[0];tmp[4][1]=_vec3_min[1];tmp[4][2]=_vec3_min[2];
    tmp[5][0]=_vec3_max[0];tmp[5][1]=_vec3_min[1];tmp[5][2]=_vec3_min[2];
    tmp[6][0]=_vec3_max[0];tmp[6][1]=_vec3_max[1];tmp[6][2]=_vec3_min[2];
    tmp[7][0]=_vec3_min[0];tmp[7][1]=_vec3_max[1];tmp[7][2]=_vec3_min[2];
    for (int i=0; i<8; i++)
        value.push_back(tmp[i]);
    return value;
}

std::vector<CGLA::Vec3f> PBoundingBox::compute_corner(std::vector<CGLA::Vec3f>& _vec_vertices)
{
    std::vector<CGLA::Vec3f> value;
    std::vector<CGLA::Vec3f> vec_transformed_3dpt, vec_box3dpt;
    CGLA::Vec3f mean, trans_vec, eigen_val, vec3_tmp, vec3_min, vec3_max;
    CGLA::Mat3x3f co_mat, eigen_vec, /*normlzd_eigv,*/ mat_tmp;
    float absmin, absmax, ordmin, ordmax, depmin, depmax;

    mean = mean_point(_vec_vertices);
    co_mat = covariance_matrix(_vec_vertices, mean);
    compute_eigens(co_mat, eigen_vec, eigen_val);

//    normlzd_eigv = ortho_basis(eigen_vec);
    trans_vec = (-1)*mean;
    for (std::vector<CGLA::Vec3f>::iterator iter=_vec_vertices.begin(); iter!=_vec_vertices.end(); iter++)
    {
        vec3_tmp = change_basis(*iter, trans_vec, eigen_vec, true);
        vec_transformed_3dpt.push_back(vec3_tmp);
    }
    extremum(vec_transformed_3dpt, absmin, absmax, ordmin, ordmax, depmin, depmax);
    vec3_min = CGLA::Vec3f(absmin, ordmin, depmin);
    vec3_max = CGLA::Vec3f(absmax, ordmax, depmax);
    vec_box3dpt = box_vertices(vec3_min, vec3_max);

    trans_vec = mean;
    mat_tmp = CGLA::invert(eigen_vec);
    for (std::vector<CGLA::Vec3f>::iterator iter=vec_box3dpt.begin(); iter!=vec_box3dpt.end(); iter++)
    {
        vec3_tmp = change_basis(*iter, trans_vec, mat_tmp, false);
        value.push_back(vec3_tmp);
    }
    vec_corner=value;
    return value;
}

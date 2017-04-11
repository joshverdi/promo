#ifndef PBOUNDINGBOX_H
#define PBOUNDINGBOX_H

#include <vector>
#include "CGLA/Vec3f.h"
#include "CGLA/Mat3x3f.h"

class PBoundingBox
{
//    std::vector<CGLA::Vec3f> vec_vertices;
    std::vector<CGLA::Vec3f> vec_corner;
public:
    PBoundingBox();
    std::vector<CGLA::Vec3f> getVecCorner() const {return vec_corner;}
    void setVecCorner(std::vector<CGLA::Vec3f> _vec_corner){vec_corner=_vec_corner;}
    PBoundingBox& operator=(const PBoundingBox& _obb){vec_corner = _obb.getVecCorner(); return *this;}
//    void add_vertices(CGLA::Vec3f _vertex){vec_vertices.push_back(_vertex);}
//    void add_vertices(std::vector<CGLA::Vec3f> _vec_vertices){vec_vertices.insert(vec_vertices.end(),_vec_vertices.begin(), _vec_vertices.end());}
    CGLA::Vec3f mean_point(std::vector<CGLA::Vec3f>& _vec_vertices);
    CGLA::Mat3x3f covariance_matrix(std::vector<CGLA::Vec3f>& _vec_vertices, CGLA::Vec3f _mean);
    void compute_eigens(CGLA::Mat3x3f _input, CGLA::Mat3x3f& _eigenvec, CGLA::Vec3f& _eigenval);
    CGLA::Vec3f normalize(CGLA::Vec3f& _vec3f);
    CGLA::Mat3x3f ortho_basis(CGLA::Mat3x3f _co_mat);
    static CGLA::Vec3f change_basis(CGLA::Vec3f& _point3d, CGLA::Vec3f& _translat, CGLA::Mat3x3f& _matrix, bool _trans_then_rotat=true);
    void extremum(const std::vector<CGLA::Vec3f>& _vec_vertices,
                  float& abs_min, float& abs_max,
                  float& ord_min, float& ord_max,
                  float& dep_min, float& dep_max);
    std::vector<CGLA::Vec3f> box_vertices(CGLA::Vec3f& _vec3_min, CGLA::Vec3f& _vec3_max);
    std::vector<CGLA::Vec3f> compute_corner(std::vector<CGLA::Vec3f>& _vec_vertices);
public: // stats use
    float volume();
    CGLA::Vec3f center();
    CGLA::Vec3f u();
    CGLA::Vec3f v();
    CGLA::Vec3f w();
    float length();
    float width();
    float height();
};

#endif // PBOUNDINGBOX_H

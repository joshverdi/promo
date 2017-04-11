#ifndef PROCEDURAL_H
#define PROCEDURAL_H

#include <string>
#include <vector>

#include <QGLViewer/qglviewer.h>
#include <boost/bimap.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include "CGLA/Mat4x4f.h"
#include "gettimeofday.h"

#define PROCEDURAL_UNIT 1
#define LOCK_VIEW_ON_X 0
#define LOCK_VIEW_ON_Y 1
#define LOCK_VIEW_ON_Z 2
#define CONSTRAINTS_FREE_VIEW 3

#define MAX_RULE_LENGTH 1024
#define MAX_CONSTANT_NUM 16
#define MAX_CHILDREN_CONDITION 8
#define MAX_CHILDREN_TERM 32
#define MAX_SCALAR_STR_LENGTH 16
#define MAX_GROUP_NUM 9
#define MATRIX_DIM 3
#define NB_OUTPUT_IMG 27
#define GROUP_NAME_OFFSET 1000000

extern long DBG_NOW;

#ifdef PROCEDURAL_MAX
#undef PROCEDURAL_MAX
#endif

#ifdef PROCEDURAL_MIN
#undef PROCEDURAL_MIN
#endif

#define PROCEDURAL_MAX(a, b) ((a)>(b)?(a):(b))
#define PROCEDURAL_MIN(a, b) ((a)<(b)?(a):(b))

#define DECIMAL_SEPARATOR '.'
#define PI 3.1415926535

// Start point of every rule will be separated from the
// replacement string by dividing them from the arrow ->
typedef struct _STRUCTURED_RULE {
    std::string source;     // source in this structure contains the predecessor and the contexts
    std::string substitute;
} STRUCTURED_RULE;

enum {EQUAL, DIFF, INF, SUP, INF_OR_EQUAL, SUP_OR_EQUAL};
enum {NUM_VAL, ARITHM_OP, VAR};
enum {PWR, MULTN, DIVSN, ADDTN, SUBTN};
enum {NOT_OP, AND_OP, OR_OP};
enum {FROM_FILE_SHAPE, LINE_SHAPE, CYLINDER_SHAPE, CONIC_SHAPE, CONFIGURED_SHAPE, BOX_SHAPE};
enum {LENGTH_VAR, WIDTH_VAR, HEIGHT_VAR, RADIUS_VAR, DIAMETER_VAR};
enum {ALPHAB_MOD_CFG, VAR_MOD_CFG};
enum {BRANCHING_DEPTH_METHOD, SIBLING_METHOD};
enum {RES_600x400, RES_1024x768, RES_1200x780};

namespace Procedural
{
    std::string     file_basename(std::string _name_withorwithout_path);
    std::string     file_path(std::string _name_withorwithout_path);
    int             assign_op_type(std::string _operation_type);
    int             assign_op_type(char _operation_type);
    float           brute_format(float _x, unsigned int _decimal);
    bool            is_decimal_separator(char c);
    std::string     rgb_to_htmltag( float& _red, float& _green, float& _blue);
    std::string     rgb_to_htmltag_hex( float& _red, float& _green, float& _blue);
    int             eval_impact(    std::vector<unsigned int> _vec_start,
                                    std::vector<unsigned int> _vec_end,
                                    unsigned int _current_selection,
                                    unsigned int _rated_index);             // Assess in which part of the string _rated_index is
    float           impact_to_ratio(unsigned int _root_size, int _idx_rating_res);
    void            rgb_from_impact(float _impact_ratio, float& _red, float& _green, float& _blue);
    void            hsv_from_impact(float _impact_ratio, float& _hue, float& _saturation, float& _value);
    void            hsv_to_rgb(     float *r, float *g, float *b, float h, float s, float v);
    void            insert_tag(     std::string& _src,
                                    unsigned int _opener_idx,
                                    std::string tag_opener,
                                    unsigned int _closer_idx,
                                    std::string tag_closer);
    void            shift_vector(   std::vector<unsigned int>& _vec_src,
                                    unsigned int _start_idx,
                                    unsigned int _offset);
    float           compute_diff(   float _src, float _val, bool _is_percent=true);
    bool            comparator (const std::pair<unsigned int, float>& _left, const std::pair<unsigned int, float>& _right);
    template <typename T> std::vector< std::vector<T> > transpose(std::vector< std::vector<T> >&_src);
    //--------------------------------------------------------------------------------------------------
    void eigen_decomposition(double A[MATRIX_DIM][MATRIX_DIM], double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM]);
    //--------------------------------------------------------------------------------------------------
    template <class T> void load_matrix(std::vector< std::vector<T> >& _src, boost::numeric::ublas::matrix<T>& _dest)
    {
        try
        {
            _dest = boost::numeric::ublas::matrix<T>(_src.size(), _src.front().size());

            for (size_t i=0; i<_dest.size1(); i++)
            {
                for (size_t j=0; j<_dest.size2(); j++)
                {
                    _dest(i, j) = _src[i][j];
                }
            }
        }
        catch(...)
        {
            std::cerr << std::endl << "Unable to load boost matrix!";
        }
    }

    template <class T> void save_matrix(boost::numeric::ublas::matrix<T>& _src, std::vector< std::vector<T> >& _dest)
    {
        std::vector<T> tmp;
        _dest.clear();
        for (size_t i=0; i<_src.size1(); i++)
        {
            for (size_t j=0; j<_src.size2(); j++)
            {
                tmp.push_back(_src(i, j));
            }
            _dest.push_back(tmp);
            tmp.clear();
        }
    }

    template <typename T1, typename T2>
    inline void boost_dbg(boost::bimap<T1, T2> const& _bm)
    {
        std::cout << "Boost contents : " << _bm.size() << " Elements" << std::endl;
        for (typename boost::bimap< T1, T2 >::const_iterator iter=_bm.begin(); iter!=_bm.end(); iter++)
        {
            std::cout << iter->left << " <--> " << iter->right << std::endl;
        }
    }

    template <typename T1, typename T2>
    inline void boost_last_elt_dbg(boost::bimap<T1, T2> const& _bm)
    {
        if (!_bm.empty())
        {
            typename boost::bimap< T1, T2 >::const_iterator iter = _bm.end();
            --iter;
            std::cout << "Last element : " << iter->left << " <--> " << iter->right << std::endl;
        }
    }

    template <typename T>
    std::vector<size_t> order_by_idx(std::vector<T> const& _values)
    {
       using namespace boost::phoenix;
       using namespace boost::phoenix::arg_names;

       std::vector<size_t> vec_indices(_values.size());
       int i = 0;
       std::transform(_values.begin(), _values.end(), vec_indices.begin(), ref(i)++);
       std::sort(vec_indices.begin(), vec_indices.end(), boost::phoenix::ref(_values)[arg1] < boost::phoenix::ref(_values)[arg2]);
       return vec_indices;
    }

    void dbg_matrix (GLfloat* _src, std::string _name=std::string("no name"));
    void dbg_matrix (CGLA::Mat4x4f& _src, std::string _name=std::string("no name"));

    CGLA::Vec3f rotate(CGLA::Vec3f _point, CGLA::Vec3f _axe, float _angle_deg);
    CGLA::Vec3f translate(CGLA::Vec3f _point, CGLA::Vec3f _vec);
    CGLA::Vec3f local_rot(CGLA::Vec3f _point, CGLA::Vec3f _axis, CGLA::Vec3f _translat, float _angle_deg);
    int ppow (int x, int p);
    GLfloat euclidist(const std::vector<float>& _pt1, const std::vector<float>& _pt2);
}

#endif // PROCEDURAL_H



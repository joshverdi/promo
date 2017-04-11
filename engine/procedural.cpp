#include <stdio.h>
#include <sstream>

#include "procedural.h"

using namespace std;

namespace Procedural
{
    std::string file_basename(std::string _name_withorwithout_path)
    {
        std::string value(_name_withorwithout_path);
        size_t basename_start_idx, extension_idx;
        char path_separator;
        if (_name_withorwithout_path.find('\\')!=std::string::npos)
            path_separator = '\\';
        else if (_name_withorwithout_path.find('/')!=std::string::npos)
            path_separator = '/';
        basename_start_idx = _name_withorwithout_path.find_last_of(path_separator);
        if (basename_start_idx!=std::string::npos)
        {
            value = _name_withorwithout_path.substr(basename_start_idx+1);
        }
        extension_idx = value.find_last_of('.');
        value = value.substr(0, extension_idx);
        return value;
    }

    std::string file_path(std::string _name_withorwithout_path)
    {
        std::string value(_name_withorwithout_path);
        size_t path_idx;
        char path_separator;
        if (_name_withorwithout_path.find('\\')!=std::string::npos)
            path_separator = '\\';
        else if (_name_withorwithout_path.find('/')!=std::string::npos)
            path_separator = '/';
        path_idx = _name_withorwithout_path.find_last_of(path_separator);
        if (path_idx!=std::string::npos)
            value = value.substr(0, path_idx);
        return value;
    }

    int assign_op_type(std::string _operation_type)
    {
        if (!_operation_type.compare("^")) return PWR;
        if (!_operation_type.compare("*")) return MULTN;
        if (!_operation_type.compare("/")) return DIVSN;
        if (!_operation_type.compare("+")) return ADDTN;
        if (!_operation_type.compare("-")) return SUBTN;
        if (!_operation_type.compare("!")) return NOT_OP;
        if (!_operation_type.compare("&")) return AND_OP;
        if (!_operation_type.compare("|")) return OR_OP;
        return -1;
    }

    int assign_op_type(char _operation_type)
    {
        switch (_operation_type)
        {
        case '^': return PWR;
        case '*': return MULTN;
        case '/': return DIVSN;
        case '+': return ADDTN;
        case '-': return SUBTN;
        case '!': return NOT_OP;
        case '&': return AND_OP;
        case '|': return OR_OP;
        }
        return -1;
    }

    float brute_format(float _x, unsigned int _decimal)
    {
        float multiplicator = (float)pow(10, _decimal);
        float trash = _x*multiplicator - (long)(_x*multiplicator);
        trash/=multiplicator;
        return (_x-trash);
    }

    bool is_decimal_separator(char c)
    {
        return (c==DECIMAL_SEPARATOR)?true:false;
    }

    std::string rgb_to_htmltag(float& _red, float& _green, float& _blue)
    {
        std::string value = "<font color=\"rgb(";
        std::stringstream ss;
        int x;

        x=_red*255;
        ss << x;
        value+=ss.str();
        value+=",";
        ss.str(std::string());;

        x=_green*255;
        ss << x;
        value+=ss.str();
        value+=",";
        ss.str(std::string());;

        x=_blue*255;
        ss << x;
        value+=ss.str();
        value+=")\">";
        return value;
    }

    std::string rgb_to_htmltag_hex(float& _red, float& _green, float& _blue)
    {
        std::string value = "<font color=\"#";
        std::stringstream ss;
        int x;

        x=_red*255;
        if (x<16) value+="0";
        ss << std::hex << x;
        value+=ss.str();
        ss.str(std::string());

        x=_green*255;
        if (x<16) value+="0";
        ss << std::hex << x;
        value+=ss.str();
        ss.str(std::string());

        x=_blue*255;
        if (x<16) value+="0";
        ss << std::hex << x;
        value+=ss.str();
        value+="\">";
        return value;
    }

    int eval_impact(std::vector<unsigned int> _vec_start, std::vector<unsigned int> _vec_end, unsigned int _current_selection, unsigned int _rated_index)
    {
        int value=-1;
        assert(_vec_start.size()==_vec_end.size());
        if (_vec_start.empty()) return value;
        if ((_rated_index<_vec_start.back())||(_rated_index>_vec_end.back())) return value;
        if (_current_selection == _rated_index) {value=0;return value;}
        if (_rated_index < _current_selection)
        {
            value=1;
            for (unsigned int i=0; i<_vec_start.size(); i++)
            {
                if (_rated_index>_vec_start.at(i))
                    return value;
                value++;
            }
        }
        else
        {
            value=1;
            for (unsigned int i=0; i<_vec_end.size(); i++)
            {
                if (_rated_index<_vec_end.at(i))
                    return value;
                value++;
            }
        }
        return (-1);
    }

    void hsv_from_impact(float _impact_ratio, float& _hue, float& _saturation, float& _value)
    {
        _saturation = _value = 1;
        _hue = 240*(1-_impact_ratio);
    }

    void hsv_to_rgb( float *r, float *g, float *b, float h, float s, float v )
    {
        int i;
        float f, p, q, t;
        if( s == 0 ) {
            // achromatic (grey)
            *r = *g = *b = v;
            return;
        }
        h /= 60;			// sector 0 to 5
        i = floor( h );
        f = h - i;			// factorial part of h
        p = v * ( 1 - s );
        q = v * ( 1 - s * f );
        t = v * ( 1 - s * ( 1 - f ) );
        switch( i ) {
            case 0:
                *r = v;
                *g = t;
                *b = p;
                break;
            case 1:
                *r = q;
                *g = v;
                *b = p;
                break;
            case 2:
                *r = p;
                *g = v;
                *b = t;
                break;
            case 3:
                *r = p;
                *g = q;
                *b = v;
                break;
            case 4:
                *r = t;
                *g = p;
                *b = v;
                break;
            default:		// case 5:
                *r = v;
                *g = p;
                *b = q;
                break;
        }
    }

    void rgb_from_impact(float _impact_ratio, float& _red, float& _green, float& _blue)
    {
        float r, g, b;
        float h, s, v;
        hsv_from_impact(_impact_ratio, h, s, v);
        hsv_to_rgb(&r, &g, &b, h, s, v);
        _red=r; _green=g; _blue=b;
    }

    float impact_to_ratio(unsigned int _root_size, int _idx_rating_res)
    {
        if (_idx_rating_res == -1) return (-1);
        return (1-((float)_idx_rating_res/_root_size));
    }

    void insert_tag(     std::string& _src,
                         unsigned int _opener_idx,
                         std::string tag_opener,
                         unsigned int _closer_idx,
                         std::string tag_closer)
    {
        if (_src.empty()||tag_opener.empty()) return;
        assert(_opener_idx<_src.size());
        if (_closer_idx>_src.size())
            assert(_closer_idx<=_src.size());
        _closer_idx+=tag_opener.size();
        _src.insert(_opener_idx, tag_opener);
        _src.insert(_closer_idx, tag_closer);
    }

    void shift_vector(std::vector<unsigned int>& _vec_src, unsigned int _start_idx, unsigned int _offset)
    {
        unsigned int i;
        if (_vec_src.empty()) return;
        for (i=0; i<_vec_src.size(); i++)
        {
            if (_start_idx<=_vec_src[i])
            {
                _vec_src[i]+=_offset;
            }
        }
    }
    //---------------------------------------------------------------------------------------------------
    static double hypot2(double x, double y)
    {
        return sqrt(x*x+y*y);
    }

    // Symmetric Householder reduction to tridiagonal form.
    static void tred2(double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM], double e[MATRIX_DIM])
    {
    //  This is derived from the Algol procedures tred2 by
    //  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
    //  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
    //  Fortran subroutine in EISPACK.

        for (int j = 0; j < MATRIX_DIM; j++)
        {
            d[j] = V[MATRIX_DIM-1][j];
        }

        // Householder reduction to tridiagonal form.
        for (int i = MATRIX_DIM-1; i > 0; i--)
        {

            // Scale to avoid under/overflow.

            double scale = 0.0;
            double h = 0.0;
            for (int k = 0; k < i; k++)
            {
                scale = scale + fabs(d[k]);
            }
            if (scale == 0.0)
            {
                e[i] = d[i-1];
                for (int j = 0; j < i; j++)
                {
                    d[j] = V[i-1][j];
                    V[i][j] = 0.0;
                    V[j][i] = 0.0;
                }
            }
            else
            {
            // Generate Householder vector.
                for (int k = 0; k < i; k++)
                {
                    d[k] /= scale;
                    h += d[k] * d[k];
                }
                double f = d[i-1];
                double g = sqrt(h);
                if (f > 0)
                {
                    g = -g;
                }
                e[i] = scale * g;
                h = h - f * g;
                d[i-1] = f - g;
                for (int j = 0; j < i; j++)
                {
                    e[j] = 0.0;
                }

                // Apply similarity transformation to remaining columns.
                for (int j = 0; j < i; j++)
                {
                    f = d[j];
                    V[j][i] = f;
                    g = e[j] + V[j][j] * f;
                    for (int k = j+1; k <= i-1; k++)
                    {
                        g += V[k][j] * d[k];
                        e[k] += V[k][j] * f;
                    }
                    e[j] = g;
                }
                f = 0.0;
                for (int j = 0; j < i; j++)
                {
                    e[j] /= h;
                    f += e[j] * d[j];
                }
                double hh = f / (h + h);
                for (int j = 0; j < i; j++)
                {
                    e[j] -= hh * d[j];
                }
                for (int j = 0; j < i; j++)
                {
                    f = d[j];
                    g = e[j];
                    for (int k = j; k <= i-1; k++)
                    {
                      V[k][j] -= (f * e[k] + g * d[k]);
                    }
                    d[j] = V[i-1][j];
                    V[i][j] = 0.0;
                }
            }
            d[i] = h;
        }

        // Accumulate transformations.

        for (int i = 0; i < MATRIX_DIM-1; i++)
        {
            V[MATRIX_DIM-1][i] = V[i][i];
            V[i][i] = 1.0;
            double h = d[i+1];
            if (h != 0.0)
            {
                for (int k = 0; k <= i; k++)
                {
                    d[k] = V[k][i+1] / h;
                }
                for (int j = 0; j <= i; j++)
                {
                    double g = 0.0;
                    for (int k = 0; k <= i; k++)
                    {
                        g += V[k][i+1] * V[k][j];
                    }
                    for (int k = 0; k <= i; k++)
                    {
                        V[k][j] -= g * d[k];
                    }
                }
            }
            for (int k = 0; k <= i; k++)
            {
                V[k][i+1] = 0.0;
            }
        }
        for (int j = 0; j < MATRIX_DIM; j++)
        {
            d[j] = V[MATRIX_DIM-1][j];
            V[MATRIX_DIM-1][j] = 0.0;
        }
        V[MATRIX_DIM-1][MATRIX_DIM-1] = 1.0;
        e[0] = 0.0;
    }

    // Symmetric tridiagonal QL algorithm.
    static void tql2(double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM], double e[MATRIX_DIM])
    {
    //  This is derived from the Algol procedures tql2, by
    //  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
    //  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
    //  Fortran subroutine in EISPACK.

        for (int i = 1; i < MATRIX_DIM; i++)
        {
            e[i-1] = e[i];
        }
        e[MATRIX_DIM-1] = 0.0;

        double f = 0.0;
        double tst1 = 0.0;
        double eps = pow(2.0,-52.0);
        for (int l = 0; l < MATRIX_DIM; l++)
        {
            // Find small subdiagonal element
            tst1 = PROCEDURAL_MAX(tst1,fabs(d[l]) + fabs(e[l]));
            int m = l;
            while (m < MATRIX_DIM)
            {
                if (fabs(e[m]) <= eps*tst1)
                {
                    break;
                }
                m++;
            }

            // If m == l, d[l] is an eigenvalue,
            // otherwise, iterate.
            if (m > l)
            {
                int iter = 0;
                do
                {
                    iter = iter + 1;  // (Could check iteration count here.)
                    // Compute implicit shift

                    double g = d[l];
                    double p = (d[l+1] - g) / (2.0 * e[l]);
                    double r = hypot2(p,1.0);
                    if (p < 0)
                    {
                      r = -r;
                    }
                    d[l] = e[l] / (p + r);
                    d[l+1] = e[l] * (p + r);
                    double dl1 = d[l+1];
                    double h = g - d[l];
                    for (int i = l+2; i < MATRIX_DIM; i++)
                    {
                        d[i] -= h;
                    }
                    f = f + h;

                    // Implicit QL transformation.
                    p = d[m];
                    double c = 1.0;
                    double c2 = c;
                    double c3 = c;
                    double el1 = e[l+1];
                    double s = 0.0;
                    double s2 = 0.0;
                    for (int i = m-1; i >= l; i--)
                    {
                        c3 = c2;
                        c2 = c;
                        s2 = s;
                        g = c * e[i];
                        h = c * p;
                        r = hypot2(p,e[i]);
                        e[i+1] = s * r;
                        s = e[i] / r;
                        c = p / r;
                        p = c * d[i] - s * g;
                        d[i+1] = h + s * (c * g + s * d[i]);

                        // Accumulate transformation.

                        for (int k = 0; k < MATRIX_DIM; k++)
                        {
                            h = V[k][i+1];
                            V[k][i+1] = s * V[k][i] + c * h;
                            V[k][i] = c * V[k][i] - s * h;
                        }
                    }
                    p = -s * s2 * c3 * el1 * e[l] / dl1;
                    e[l] = s * p;
                    d[l] = c * p;

                // Check for convergence.
                } while (fabs(e[l]) > eps*tst1);
            }
            d[l] = d[l] + f;
            e[l] = 0.0;
        }

        // Sort eigenvalues and corresponding vectors.
        for (int i = 0; i < MATRIX_DIM-1; i++)
        {
            int k = i;
            double p = d[i];
            for (int j = i+1; j < MATRIX_DIM; j++)
            {
                if (d[j] < p)
                {
                    k = j;
                    p = d[j];
                }
            }
            if (k != i)
            {
                d[k] = d[i];
                d[i] = p;
                for (int j = 0; j < MATRIX_DIM; j++)
                {
                   p = V[j][i];
                   V[j][i] = V[j][k];
                   V[j][k] = p;
                }
            }
        }
    }
    //---------------------------------------------------------------------------------------------------
    /*  Symmetric matrix A => eigenvectors in columns of V, corresponding
        eigenvalues in d. */
    void eigen_decomposition(double A[MATRIX_DIM][MATRIX_DIM], double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM])
    {
        double e[MATRIX_DIM];
        for (int i = 0; i < MATRIX_DIM; i++)
        {
            for (int j = 0; j < MATRIX_DIM; j++)
            {
              V[i][j] = A[i][j];
            }
        }
        tred2(V, d, e);
        tql2(V, d, e);
    }
    //---------------------------------------------------------------------------------------------------

    void dbg_matrix (GLfloat* _src, std::string _name)
    {
        std::cout<<std::endl<<_name<<" :"<<std::endl;
        for(int i=0; i<4; i++)
        {
            for (int j=0; j<4; j++)
            {
                std::cout<<_src[4*j+i]<<"\t";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
    void dbg_matrix (CGLA::Mat4x4f& _src, std::string _name)
    {
        std::cout<<std::endl<<_name<<" :"<<std::endl;
        for(int i=0; i<4; i++)
        {
            for (int j=0; j<4; j++)
            {
                std::cout<<_src[j][i]<<"\t";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }

    CGLA::Vec3f rotate(CGLA::Vec3f _point, CGLA::Vec3f _axis, float _angle_deg)
    {
        CGLA::Vec3f result;
        double angle_rad=_angle_deg*M_PI/180;
        float c=(float)cos(angle_rad);
        float s=(float)sin(angle_rad);
        float x=_axis[0];
        float y=_axis[1];
        float z=_axis[2];
        float a11, a12, a13;
        float a21, a22, a23;
        float a31, a32, a33;

        a11=x*x*(1-c)+c;    a12=x*y*(1-c)-z*s;  a13=x*z*(1-c)+y*s;
        a21=y*x*(1-c)+z*s;  a22=y*y*(1-c)+c;    a23=y*z*(1-c)-x*s;
        a31=x*z*(1-c)-y*s;  a32=y*z*(1-c)+x*s;  a33=z*z*(1-c)+c;

        result[0]=a11*_point[0]+a12*_point[1]+a13*_point[2];
        result[1]=a21*_point[0]+a22*_point[1]+a23*_point[2];
        result[2]=a31*_point[0]+a32*_point[1]+a33*_point[2];
        return result;
    }

    CGLA::Vec3f translate(CGLA::Vec3f _point, CGLA::Vec3f _vec)
    {
        return CGLA::Vec3f(_point[0]+_vec[0], _point[1]+_vec[1], _point[2]+_vec[2]);
    }

    CGLA::Vec3f local_rot(CGLA::Vec3f _point, CGLA::Vec3f _axis, CGLA::Vec3f _translat, float _angle_deg)
    {
        CGLA::Vec3f value=_point;
        _translat*=(-1);
        value = translate(value,_translat);
        value = rotate(value, _axis, _angle_deg);
        _translat*=(-1);
        value = translate(value,_translat);
        return value;
    }

    float compute_diff(float _src, float _val, bool _is_percent)
    {
        try
        {
            return (_is_percent)?(100*(_val/_src-1)):(_val/_src-1);
        }
        catch(...)
        {
            return 0;
        }
    }

    bool comparator(const std::pair<unsigned int, float>& _left, const std::pair<unsigned int, float>& _right)
    {
        return _left.second < _right.second;
    }

    template <typename T> std::vector< std::vector<T> > transpose(std::vector< std::vector<T> >&_src)
    {
        try
        {
            std::vector< std::vector<T> > value(_src[0].size(), std::vector<T>(_src.size()));
            for (typename std::vector<T>::size_type i=0; i<_src[0].size(); i++)
                for (typename std::vector<T>::size_type j=0; j<_src.size(); j++)
                    value[i][j] = _src[j][i];
            return value;
        }
        catch(...)
        {
            std::cerr << "Invalid Access from Transpose function";
        }
    }

    int ppow (int x, int p)
    {
      if (p == 0) return 1;
      if (p == 1) return x;
      return x * ppow(x, p-1);
    }

    GLfloat euclidist(const std::vector<float>& _pt1, const std::vector<float>& _pt2)
    {
        GLfloat value=-1;
        if (_pt1.size()==_pt2.size())
        {
            value = 0;
            for (int i=0; i<_pt1.size(); i++)
            {
                value += pow((_pt1[i]-_pt2[i]), 2.0f);
            }
            value = sqrt(value);
        }
        else
        {
            std::cout << std::endl << "Warning: Attempt to compare unequal sized vectors!";
        }
        return value;
    }
}

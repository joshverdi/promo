#include <QFile>
#include <cassert>
#include "ppcaeval.h"
#include "procedural.h"
#include "math.h"
#include "piorecord.h"

using namespace boost::numeric;

PPcaEval::PPcaEval()
{
}

//PPcaEval::PPcaEval(std::vector<std::vector<float> >& _population, std::vector<std::vector<float> >& _eigenvec, std::vector<float>& _eigenval)
//{
//    std::vector<float> vec_mean = mean(_population);
//    std::vector<std::vector<float> > co_mat = covariance_matrix(_population, vec_mean);
//    compute_eigens(co_mat, _eigenvec, _eigenval);
//    eigen_vec = _eigenvec;
//    eigen_val = _eigenval;
//}

std::vector<float> PPcaEval::mean(std::vector<std::vector<float> >& _src)
{
    std::vector<float> value;

    if (_src.empty()) return value;
    for (std::vector<float>::iterator iter=_src.front().begin(); iter!=_src.front().end(); iter++)
    {
        value.push_back(0);
    }

    for (unsigned int i=0; i<_src.size(); i++)
    {
        for (unsigned int j=0; j<_src.at(i).size(); j++)
        {
            value.at(j)+=_src.at(i).at(j);
        }
    }

    for (std::vector<float>::iterator iter=value.begin(); iter!=value.end(); iter++)
    {
        (*iter)/=_src.size();
    }
    return value;
}

float PPcaEval::mean2(std::vector<std::vector<float> >& _src)
{
    float value=0;
    if (_src.empty()) return value;
    for (unsigned int i=0; i<_src.size(); i++)
    {
        for (unsigned int j=0; j<_src.at(i).size(); j++)
        {
            value+=_src.at(i).at(j);
        }
    }
    value/=(_src.front().size()*_src.size());
    return value;
}

float PPcaEval::compute_error(std::vector<std::vector<float> >& _src)
{
    float value=0;
    if (_src.empty()) return value;
    for (unsigned int i=0; i<_src.size(); i++)
    {
        for (unsigned int j=0; j<_src.at(i).size(); j++)
        {
            value+=pow((_src.at(i).at(j)),2);
        }
    }
    value/=(_src.front().size()*_src.size());
    value = sqrt(value);
    return value;
}

std::vector< std::vector<float> > PPcaEval::abs(std::vector<std::vector<float> >& _src)
{
    std::vector< std::vector<float> >  value;
    std::vector<float> vec_tmp;
    for (unsigned int i=0; i<_src.size(); i++)
    {
        for (unsigned int j=0; j<_src.at(i).size(); j++)
        {
            vec_tmp.push_back(fabs(_src.at(i).at(j)));
        }
        value.push_back(vec_tmp);
        vec_tmp.clear();
    }
    return value;
}

std::vector<std::vector<float> > PPcaEval::covariance_matrix(std::vector<std::vector<float> >& _src, std::vector<float>& _mean)
{
    std::vector<std::vector<float> > value;
    std::vector<float> tmp;
    size_t n=_src.front().size();

    for (unsigned int i=0; i<n; i++)
    {
        tmp.push_back(0);
    }
    for (unsigned int i=0; i<n; i++)
    {
        value.push_back(tmp);
    }

    for (unsigned int i=0; i<n; i++)
    {
        for (unsigned int j=0; j<n; j++)
        {
            for (std::vector<std::vector <float> >::iterator main_iter=_src.begin(); main_iter!=_src.end(); main_iter++)
            {
                value[i][j] += ((*main_iter)[i]-_mean[i])*((*main_iter)[j]-_mean[j]);
            }
            value[i][j] /= _src.size()-1;
        }
    }

//    for (unsigned int i=0; i<n; i++)
//    {
//        for (unsigned int j=0; j<n; j++)
//        {
//            value[i][j] /= n;
//        }
//    }

    return value;
}

void PPcaEval::compute_eigens(std::vector<std::vector<float> > _src, std::vector<std::vector<float> >& _eigenvec, std::vector<float>& _eigenval)
{
    size_t n = _src.size();
    if (n==0) return;
    if (n!=_src.front().size()) return;
    std::vector<float> vec_x;
    double **mat_A;
    double **mat_V;
    double *d;

    _eigenvec.clear();
    _eigenval.clear();
    d = new double[n];

    mat_A = new double*[n];
    mat_V = new double*[n];
    for (size_t i=0; i<n; i++)
    {
        mat_A[i] = new double[n];
        mat_V[i] = new double[n];
    }

    for (size_t i=0; i<n; i++)
    {
        for (size_t j=0; j<n; j++)
        {
            mat_A[i][j]=_src[i][j];
        }
    }
    eigen_decomposition(mat_A, mat_V, d, n);
    for (size_t i=0; i<n; i++)
    {
        _eigenval.push_back(d[i]);
    }
    this->eigen_val = _eigenval;

    for (size_t i=0; i<n; i++)
    {
        vec_x.clear();
        for (size_t j=0; j<n; j++)
        {
            vec_x.push_back(mat_V[j][i]);
        }
        _eigenvec.push_back(vec_x);
    }
    this->eigen_vec = _eigenvec;

    for (size_t i=0; i<n; i++)
    {
        delete [] mat_A[i];
        delete [] mat_V[i];
    }
    delete [] mat_A;
    delete [] mat_V;
    delete [] d;
}

void PPcaEval::tred2(double **V, double *d, double *e, const int dim)
{
//  This is derived from the Algol procedures tred2 by
//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
//  Fortran subroutine in EISPACK.

    for (int j = 0; j < dim; j++)
    {
        d[j] = V[dim-1][j];
    }

    // Householder reduction to tridiagonal form.
    for (int i = dim-1; i > 0; i--)
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

    for (int i = 0; i < dim-1; i++)
    {
        V[dim-1][i] = V[i][i];
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
    for (int j = 0; j < dim; j++)
    {
        d[j] = V[dim-1][j];
        V[dim-1][j] = 0.0;
    }
    V[dim-1][dim-1] = 1.0;
    e[0] = 0.0;
}

// Symmetric tridiagonal QL algorithm.
void PPcaEval::tql2(double **V, double *d, double *e, const int dim)
{
//  This is derived from the Algol procedures tql2, by
//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
//  Fortran subroutine in EISPACK.

    for (int i = 1; i < dim; i++)
    {
        e[i-1] = e[i];
    }
    e[dim-1] = 0.0;

    double f = 0.0;
    double tst1 = 0.0;
    double eps = pow(2.0,-52.0);
    for (int l = 0; l < dim; l++)
    {
        // Find small subdiagonal element
        tst1 = PROCEDURAL_MAX(tst1,fabs(d[l]) + fabs(e[l]));
        int m = l;
        while (m < dim)
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
                for (int i = l+2; i < dim; i++)
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

                    for (int k = 0; k < dim; k++)
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
    for (int i = 0; i < dim-1; i++)
    {
        int k = i;
        double p = d[i];
        for (int j = i+1; j < dim; j++)
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
            for (int j = 0; j < dim; j++)
            {
               p = V[j][i];
               V[j][i] = V[j][k];
               V[j][k] = p;
            }
        }
    }
}

/*  Symmetric matrix A => eigenvectors in columns of V, corresponding
    eigenvalues in d. */
void PPcaEval::eigen_decomposition(double **A, double **V, double *d, const int dim)
{
    double *e = new double[dim];
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
          V[i][j] = A[i][j];
        }
    }
    tred2(V, d, e, dim);
    tql2(V, d, e, dim);
    delete [] e;
}

double PPcaEval::hypot2(double x, double y)
{
    return sqrt(x*x+y*y);
}

std::vector<unsigned int> PPcaEval::order_by_idx(std::vector<float>& _src)
{
    unsigned int i=0;
    std::vector<unsigned int> value;
    std::pair<unsigned int, float> p;
    std::vector<std::pair<unsigned int, float> > vec_pair;

    for (std::vector<float>::iterator iter = _src.begin(); iter!=_src.end(); iter++, i++)
    {
        p = std::pair<unsigned int, float>(i, (*iter));
        vec_pair.push_back(p);
    }
    std::sort(vec_pair.begin(), vec_pair.end(), Procedural::comparator);
    for (std::vector<std::pair<unsigned int, float> >::iterator iter = vec_pair.begin(); iter!=vec_pair.end(); iter++)
    {
        value.push_back((*iter).first);
    }
    return value;
}

std::vector<std::vector<float> > PPcaEval::select_eigenvector(unsigned int _order)
{
    std::vector<std::vector<float> > value;
    std::vector<unsigned int> vec_ordered;

    vec_ordered = order_by_idx(eigen_val);
    for (std::vector<unsigned int>::reverse_iterator iter=vec_ordered.rbegin(); iter!=vec_ordered.rend(); iter++)
    {
        value.push_back(eigen_vec.at(*iter));
        if (value.size() == _order)
            break;
    }
    return value;
}

std::vector<std::vector<float> > PPcaEval::select_eigenvector(unsigned int _order, std::vector<std::vector<float> >& _eigenvector, std::vector<float>& _eigenval)
{
    std::vector<std::vector<float> > value;
    std::vector<unsigned int> vec_ordered;

    vec_ordered = order_by_idx(_eigenval);
    for (std::vector<unsigned int>::reverse_iterator iter=vec_ordered.rbegin(); iter!=vec_ordered.rend(); iter++)
    {
        value.push_back(_eigenvector.at(*iter));
        if (value.size() == _order)
            break;
    }
    return value;
}

std::vector< std::vector<float> > PPcaEval::compute_diff(std::vector< std::vector<float> >& _data1, std::vector< std::vector<float> >& _data2)
{
    assert(_data1.size()==_data2.size());
    try
    {
        std::vector< std::vector<float> > value(_data1.size(), std::vector<float>(_data1[0].size()));
        for (std::vector<float>::size_type i = 0; i<_data1.size(); i++)
            for (std::vector<float>::size_type j = 0; j<_data1.front().size(); j++)
            {
                value[i][j]=_data1[i][j]-_data2[i][j];
            }
        return value;
    }
    catch(...)
    {
        std::cerr << "Failed to allocate memory in compute_diff()";
    }
    return _data1; // this is a failure case
}

std::vector< std::vector<float> > PPcaEval::adjust_pop(std::vector<std::vector<float> >& _vec_vec_population)
{
    std::vector< std::vector<float> > value;
    std::vector<float> tmp;
    std::vector<float> vec_pop_mean = mean(_vec_vec_population);
    if (!_vec_vec_population.empty())
        assert(vec_pop_mean.size()==_vec_vec_population.front().size());
    for (std::vector<std::vector<float> >::iterator iter=_vec_vec_population.begin(); iter!=_vec_vec_population.end(); iter++)
    {
        for (size_t i=0; i<(*iter).size(); i++)
        {
            tmp.push_back((*iter)[i]-vec_pop_mean[i]);

        }
        value.push_back(tmp);
        tmp.clear();
    }
    return value;
}

std::vector<std::vector<float> > PPcaEval::compress_data(std::vector<std::vector<float> > _src, unsigned int _order)
{
    std::vector<std::vector<float> > new_eigvec, pop_adj, value;
    ublas::matrix<float> mx_feat_vec, mx_feat_vec_t, mx_pop_adjust, mx_pop_adjust_t;
    ublas::matrix<float> mx_final_pop, mx_final_pop_t;
    new_eigvec = select_eigenvector(_order);
    pop_adj = adjust_pop(_src);
    Procedural::load_matrix<float>(new_eigvec, mx_feat_vec);
    Procedural::load_matrix<float>(pop_adj, mx_pop_adjust);
    //mx_feat_vec_t   = ublas::trans(mx_feat_vec);
    mx_pop_adjust_t = ublas::trans(mx_pop_adjust);
    mx_final_pop    = ublas::prod(mx_feat_vec, mx_pop_adjust_t);
    mx_final_pop_t  = ublas::trans(mx_final_pop);
    Procedural::save_matrix<float>(mx_final_pop_t, value);
    return value;
}

std::vector<unsigned int> PPcaEval::wrapper_pt3d(std::vector< std::vector<float> >& _src)
{
    const char TARGETWRAP_FN[] = "targetwrap.tmp";
    const char SCATTERPLOT_FN[] = "scatterplot.tmp";
    QFile file_targetwrap(TARGETWRAP_FN);
    QFile file_scatterplot(SCATTERPLOT_FN);
    std::stringstream ss_targetwrap, ss_scatterplot;
    std::vector<unsigned int> value;
    std::vector< std::vector<float> > vec_targetbox;
    float xmin, ymin, zmin;
    float xmax, ymax, zmax;
    if (_src.empty())
        return value;
    xmin = xmax = _src.front()[0];
    ymin = ymax = _src.front()[1];
    zmin = zmax = _src.front()[2];
    for (std::vector< std::vector<float> >::iterator iter = _src.begin(); iter!=_src.end(); iter++)
    {
        if ((*iter).size()!=3)
            return value;
        xmin = PROCEDURAL_MIN(xmin, (*iter)[0]);
        ymin = PROCEDURAL_MIN(ymin, (*iter)[1]);
        zmin = PROCEDURAL_MIN(zmin, (*iter)[2]);
        xmax = PROCEDURAL_MAX(xmax, (*iter)[0]);
        ymax = PROCEDURAL_MAX(ymax, (*iter)[1]);
        zmax = PROCEDURAL_MAX(zmax, (*iter)[2]);
        ss_scatterplot << (*iter)[0] << "," << (*iter)[1] << "," << (*iter)[2] << std::endl;
    }
    vec_targetbox = extrem_wrap(xmin, ymin, zmin, xmax, ymax, zmax);
    for (std::vector< std::vector<float> >::iterator iter = vec_targetbox.begin(); iter!=vec_targetbox.end(); iter++)
    {
        value.push_back(nearest(*iter, _src));
    }
    ss_targetwrap << xmin << "," << ymin << "," << zmin << ","
                  << xmax << "," << ymax << "," << zmax;
    PIORecord::save(file_targetwrap, ss_targetwrap);
    PIORecord::save(file_scatterplot, ss_scatterplot);
    //remove_duplicates(value);
    return value;
}

unsigned int PPcaEval::nearest(float _target, std::vector<float>& _in)
{
    unsigned int value=0;
    float dist;

    if (_in.empty())
        return value;
    dist = fabs(_in[0]-_target);
    for (unsigned int i=0; i<_in.size(); i++)
    {
        if (fabs(_in[i]-_target)<dist)
        {
            value = i;
            dist = fabs(_in[i]-_target);
        }
    }
    return value;
}

unsigned int PPcaEval::nearest(std::vector<float> _target_pt, std::vector< std::vector<float> >& _vec_src)
{
    unsigned int value=0;
    float my_min;

    if (_vec_src.empty())
        return value;
    int i=0;
    std::vector<float> vec_tmp = _vec_src.at(i);
    bool is_already_slct=true;
    while(is_already_slct){
        vec_tmp = _vec_src.at(i);
        is_already_slct=false;
        std::vector<std::vector<float>>::iterator iter1 =vec_slct.begin() ;
        while ( !is_already_slct && iter1!=vec_slct.end())
        {
            std::vector<float> vect_slct_tmp =*iter1;
            if (vect_slct_tmp.at(0)==vec_tmp.at(0) && vect_slct_tmp.at(1)==vec_tmp.at(1) && vect_slct_tmp.at(2)==vec_tmp.at(2))
            {
                is_already_slct=true;
                i++;
            }
            iter1++;
        }
    }

    my_min = distance (_target_pt, _vec_src.at(i));
    value=i;
    i=0;

    for (std::vector<std::vector<float> >::iterator iter =_vec_src.begin(); iter!=_vec_src.end(); iter++)
    {
        is_already_slct=false;

        vec_tmp = *iter;
        for (std::vector<std::vector<float>>::iterator iter1 =vec_slct.begin(); iter1!=vec_slct.end(); iter1++)
        {
            std::vector<float> vect_slct_tmp =*iter1;
            if (vect_slct_tmp.at(0)==vec_tmp.at(0) && vect_slct_tmp.at(1)==vec_tmp.at(1) && vect_slct_tmp.at(2)==vec_tmp.at(2))
                is_already_slct=true;
        }
        if (distance(_target_pt, _vec_src[i])<my_min && !is_already_slct )
        {
            my_min = distance(_target_pt, _vec_src[i]);
            value = i;
        }
        i++;
    }

    vec_slct.push_back(_vec_src.at(value));
    return value;
}

void PPcaEval::remove_duplicates(std::vector< std::vector<float> >& _src)
{
    std::sort(_src.begin(), _src.end());
    _src.erase(std::unique(_src.begin(), _src.end()), _src.end());
}

std::vector< std::vector<float> > PPcaEval::extrem_wrap(float _xmin, float _ymin, float _zmin, float _xmax, float _ymax, float _zmax)
{
    std::vector< std::vector<float> > value;
    std::vector<float> vec_tmp;
    float _xmid = 0.5*(_xmin+_xmax);
    float _ymid = 0.5*(_ymin+_ymax);
    float _zmid = 0.5*(_zmin+_zmax);

    vec_tmp.push_back(_xmin);vec_tmp.push_back(_ymin);vec_tmp.push_back(_zmin);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmin);vec_tmp.push_back(_ymid);vec_tmp.push_back(_zmin);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmin);vec_tmp.push_back(_ymax);vec_tmp.push_back(_zmin);value.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(_xmin);vec_tmp.push_back(_ymin);vec_tmp.push_back(_zmid);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmin);vec_tmp.push_back(_ymid);vec_tmp.push_back(_zmid);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmin);vec_tmp.push_back(_ymax);vec_tmp.push_back(_zmid);value.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(_xmin);vec_tmp.push_back(_ymin);vec_tmp.push_back(_zmax);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmin);vec_tmp.push_back(_ymid);vec_tmp.push_back(_zmax);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmin);vec_tmp.push_back(_ymax);vec_tmp.push_back(_zmax);value.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(_xmid);vec_tmp.push_back(_ymin);vec_tmp.push_back(_zmin);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmid);vec_tmp.push_back(_ymid);vec_tmp.push_back(_zmin);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmid);vec_tmp.push_back(_ymax);vec_tmp.push_back(_zmin);value.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(_xmid);vec_tmp.push_back(_ymin);vec_tmp.push_back(_zmid);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmid);vec_tmp.push_back(_ymid);vec_tmp.push_back(_zmid);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmid);vec_tmp.push_back(_ymax);vec_tmp.push_back(_zmid);value.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(_xmid);vec_tmp.push_back(_ymin);vec_tmp.push_back(_zmax);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmid);vec_tmp.push_back(_ymid);vec_tmp.push_back(_zmax);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmid);vec_tmp.push_back(_ymax);vec_tmp.push_back(_zmax);value.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(_xmax);vec_tmp.push_back(_ymin);vec_tmp.push_back(_zmin);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmax);vec_tmp.push_back(_ymid);vec_tmp.push_back(_zmin);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmax);vec_tmp.push_back(_ymax);vec_tmp.push_back(_zmin);value.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(_xmax);vec_tmp.push_back(_ymin);vec_tmp.push_back(_zmid);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmax);vec_tmp.push_back(_ymid);vec_tmp.push_back(_zmid);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmax);vec_tmp.push_back(_ymax);vec_tmp.push_back(_zmid);value.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(_xmax);vec_tmp.push_back(_ymin);vec_tmp.push_back(_zmax);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmax);vec_tmp.push_back(_ymid);vec_tmp.push_back(_zmax);value.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(_xmax);vec_tmp.push_back(_ymax);vec_tmp.push_back(_zmax);value.push_back(vec_tmp);vec_tmp.clear();

    return value;
}

std::vector<unsigned int> PPcaEval::multisearch(std::vector< std::vector<float> >& _vec_data, std::vector< std::vector<float> >& _vec_queries)
{
    std::vector<unsigned int> value;
    for (std::vector< std::vector<float> >::iterator iter=_vec_queries.begin(); iter!=_vec_queries.end(); iter++)
    {
        for (unsigned int i=0; i<_vec_data.size(); i++)
        {
            if (_vec_data[i]==(*iter))
            {
                value.push_back(i);
                break;
            }
        }
    }
    return value;
}

float PPcaEval::distance(std::vector<float>& _vec1, std::vector<float>& _vec2)
{
    float value=0;
    assert(_vec1.size()==_vec2.size());
    for (unsigned int i=0; i<_vec1.size(); i++)
    {
        value+=((float)pow(_vec1[i]-_vec2[i], 2));
    }
    value = (float)sqrt(value);
    return value;
}


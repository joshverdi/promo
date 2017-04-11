#ifndef PPCAEVAL_H
#define PPCAEVAL_H
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "procedural.h"

class PPcaEval
{
    std::vector<std::vector<float> > eigen_vec;
    std::vector<float> eigen_val;
    std::vector<std::vector<float>> vec_slct;
public:
    PPcaEval();
    //PPcaEval(std::vector<std::vector<float> >& _population, std::vector<std::vector<float> >& _eigenvec, std::vector<float>& _eigenval);
    std::vector<std::vector<float> > getEigenVec() const {return eigen_vec;}
    std::vector<float> getEigenVal() const {return eigen_val;}
public:
    std::vector<float> mean(std::vector<std::vector<float> >& _src);
    float mean2(std::vector<std::vector<float> >& _src);
    std::vector<std::vector<float> > abs(std::vector<std::vector<float> >& _src);
    float compute_error(std::vector<std::vector<float> >& _src);
    std::vector<std::vector<float> > covariance_matrix(std::vector<std::vector<float> >& _src, std::vector<float>& _mean);
    void compute_eigens(std::vector<std::vector<float> > _src, std::vector<std::vector<float> >& _eigenvec, std::vector<float>& _eigenval);
    static void tred2(double **V, double *d, double *e, const int dim);
    static void tql2 (double **V, double *d, double *e, const int dim);
    static double hypot2(double x, double y);
    void eigen_decomposition(double **A, double **V, double *d, const int dim);
    std::vector<unsigned int> order_by_idx(std::vector<float>& _src);   // order ascending
    std::vector<std::vector<float> > select_eigenvector(unsigned int _order);
    std::vector<std::vector<float> > select_eigenvector(unsigned int _order, std::vector<std::vector<float> >& _eigenvector, std::vector<float>& _eigenval);
    std::vector< std::vector<float> > compute_diff(std::vector< std::vector<float> >& _data1, std::vector< std::vector<float> >& _data2);
    std::vector< std::vector<float> > adjust_pop(std::vector<std::vector<float> >& _vec_vec_population);
    std::vector<std::vector<float> > compress_data(std::vector<std::vector<float> > _src, unsigned int _order);
    std::vector<unsigned int> wrapper_pt3d(std::vector< std::vector<float> >& _src);
    unsigned int nearest(float _target, std::vector<float>& _in);
    unsigned int nearest(std::vector<float> _target_pt, std::vector< std::vector<float> >& _vec_src);
    void remove_duplicates(std::vector< std::vector<float> >& _src);
    std::vector< std::vector<float> > extrem_wrap(float _xmin, float _ymin, float _zmin, float _xmax, float _ymax, float _zmax);
    std::vector<unsigned int> multisearch(std::vector< std::vector<float> >& _vec_data, std::vector< std::vector<float> >& _vec_queries);
    float distance(std::vector<float>& _vec1, std::vector<float>& _vec2);
};

#endif // PPCAEVAL_H

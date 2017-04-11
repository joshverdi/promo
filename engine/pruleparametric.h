#ifndef PRULEPARAMETRIC_H
#define PRULEPARAMETRIC_H

#include <vector>
#include <string>
#include <boost/bimap.hpp>

#include "grammar/ppreproc.h"
#include "grammar/palphabet.h"
#include "grammar/psymbol.h"
#include "grammar/pvariable.h"
#include "grammar/pword.h"
#include "prule.h"
#include "pbooleanoperator.h"
#include "pcomparisonoperator.h"

class PRuleParametric : public PRule
{
    std::string predecessor;    // the predecessor is exclusively the alphabet : it is the source removed from their parameters and the conditions
    std::string condition;
    std::string successor;
    PBooleanOperator* pbo_condition;
    std::vector<PVariable*> vec_var;
    std::vector<PSymbol*> vec_successor;
    std::vector<unsigned int> vec_successor_idx;
    std::vector<PAlgebra*> vec_algebra;
    PPreproc* preproc;
public:
    std::string getSuccessor() const {return successor;}
    PPreproc* getPreproc() const {return preproc;}
public:
    PRuleParametric();
    PRuleParametric(std::string _text, PAlphabet* _palphabet, PPreproc* _preproc);
    ~PRuleParametric();
    PRuleParametric& operator=(const PRuleParametric& _rulepara);
    std::string pick_comparison_type(std::string& _src, unsigned int _index);
    std::string pick_predecessor();
    std::string pick_successor();
    unsigned int pick_variable();
    unsigned int pick_variable(std::string& _src, bool _reverse_reading=false);
    std::vector<float> pick_params(std::string& _src, unsigned int _idx);       // while _idx point to the alphabet (PWord*), this method pick the numeric value (vector<float>)
public:                                                                                // between brackets, recall that they are a string separated by commas
    bool pick_condition();
    bool is_candidate(std::string& _alphabet,
                      std::vector<float>& _vec_params,
                      std::string& _current_derivation,
                      unsigned int _index,
                      PIgnore* _ignore,
                      std::vector<std::string>& _vec_str_aux_params,
                      std::vector<float>& _vec_aux_params);
    bool is_condition_valid(std::string& _condition, std::vector<float> _vec_params);

public:
    bool is_binary_operation(std::string& _src);
    bool is_binary_operator(char _c);
    bool is_binary_operator(std::string& _src, unsigned int _index);
    bool is_decim_comp_operation(std::string& _src);
    bool is_variable(std::string& _src, unsigned int _index);
    bool is_constant(std::string& _src, unsigned int _index, unsigned int& _const_size);
    bool is_num_val(std::string& _src, unsigned int _index);
    bool is_arith_operator(std::string& _src, unsigned int _index);
    bool is_comparison_operator(std::string& _src, unsigned int _index);
    bool is_defined(std::string& _src, unsigned int _index, unsigned int& _char_size);
public:
    PBooleanOperator* parse_algebra(std::string& _src);
    PComparisonOperator* parse_comparison(std::string& _src);
    PBooleanOperator* parse_condition(std::string& _text);
    unsigned int parse_successor(std::string& _str_successor, PAlphabet* _palphabet);
    std::string compute(    float* _params, unsigned int _size, std::string* _pstr_aux_params, float* _aux_params, unsigned int _aux_size,
                            boost::bimap<unsigned int, unsigned int>& _bm_succidx_prodidx);
//    std::string compute(    float* _params, unsigned int _size, std::string* _pstr_aux_params, float* _aux_params, unsigned int _aux_size,
//                            boost::bimap<unsigned int, unsigned int>& _bm_succidx_prodidx,
//                            std::map<unsigned int, std::vector<float> _vec_params>& _map_params);
public:
    unsigned int pile_up();
    unsigned int pile_up(std::vector<PNumeric*>& _vec_numeric);
    unsigned int pile_up(std::vector<PAlgebra*>& _vec_algebra);
public:
    typedef boost::bimap<unsigned int, std::vector<PNumeric*> > bimap_formula;
    typedef boost::bimap<unsigned int, std::vector<PNumeric*> >::value_type formula_match;
    typedef boost::bimap<unsigned int, std::vector<PNumeric*> >::const_iterator iter_match;
    bimap_formula bm_formul;
    PNumeric* parse_numeric(std::string& _src);
    std::vector<PNumeric*> request_formula(unsigned int _idx);
    std::vector<std::string> var_to_arraystr();
};

#endif // PRULEPARAMETRIC_H

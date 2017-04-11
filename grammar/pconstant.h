#ifndef PCONSTANT_H
#define PCONSTANT_H
#include <string>
#include <vector>

#include "engine/pnumeric.h"
#include "engine/pnumericvalue.h"
#include "grammar/psymbol.h"

class PConstant : public PSymbol
{
    std::vector<PNumeric*> vec_numeric;
public:
    PConstant();
    PConstant(char c);
    PConstant(std::string _text);
    void add(PNumeric* _numeric);
    void add(float _val){vec_numeric.push_back(new PNumericValue(_val));}
    unsigned int size() {return getText().size();};
    std::vector<PNumeric*> getVecNumeric() const {return vec_numeric;}
public:
    bool is_constant(char c);
    bool is_constant(std::string _s);
public:
    unsigned int assign_params(float* _params, unsigned int _nb_params);
    unsigned int assign_params(std::vector<float>& _vec_params);
    std::string compute_str(std::string* _str_var, float* _params, unsigned int& _size);
};

#endif // PCONSTANT_H

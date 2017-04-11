#ifndef PNUMERICVALUE_H
#define PNUMERICVALUE_H

#include <string>

#include "pnumeric.h"

class PArithmetic;

class PNumericValue : public PNumeric
{
    float data;
    PArithmetic* root;
public:
    PNumericValue();
    PNumericValue(float _data);
    PNumericValue(float _data, unsigned int _priority);
    PNumericValue(std::string _s_data);
    PNumericValue(const PNumericValue& _pnv);
    ~PNumericValue();
    void setData(float _data){data = _data;}
    float getData() const {return data;}
    PArithmetic* getRoot(void) const {return root;}
    void setRoot(PArithmetic* _root){root = _root;}
    PNumericValue& operator=(const PNumericValue& _pnv);
public:
    float eval(std::string* _s_params, float* _value, unsigned int _nb_params);
    static bool is_numeric_value(std::string _s_input);
    float set_from(std::string _str_src);
    std::string to_stdstr();
    bool is_tied(std::string&){return false;}
};

#endif // PNUMERICVALUE_H

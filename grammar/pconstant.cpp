#include <string.h>

#include "grammar/pgrammar.h"
#include "grammar/pconstant.h"
#include "engine/procedural.h"
#include "engine/ptextparser.h"
#include "engine/pnumericvalue.h"
#include "pvariable.h"
#include "parithmetic.h"

using namespace std;

PConstant::PConstant()
{
}

PConstant::PConstant(char c):PSymbol(c)
{
}

PConstant::PConstant(string _text):PSymbol(_text)
{
}

void PConstant::add(PNumeric* _numeric)
{
    PVariable *pv, *my_pv;
    PArithmetic *pa, *my_pa;
    PNumericValue *pnv, *my_pnv;

    if (_numeric)
    {
        pv = NULL;
        pa = NULL;
        pnv = NULL;
        if ((pv=dynamic_cast<PVariable*>(_numeric)))
        {
            my_pv = new PVariable(*pv);
            vec_numeric.push_back(my_pv);
        }
        else if ((pnv=dynamic_cast<PNumericValue*>(_numeric)))
        {
            my_pnv = new PNumericValue(*pnv);
            vec_numeric.push_back(my_pnv);
        }
        else if ((pa=dynamic_cast<PArithmetic*>(_numeric)))
        {
            my_pa = new PArithmetic(*pa);
            vec_numeric.push_back(my_pa);
        }
    }
}

bool PConstant::is_constant(char c)
{
    return (PGrammar::getDefaultConstant().find(c)!=string::npos)?true:false;
}

bool PConstant::is_constant(string _s)
{
    return (PGrammar::getDefaultConstant().find(_s)!=string::npos)?true:false;
}

unsigned int PConstant::assign_params(float* _params, unsigned int _nb_params)
{
    for (unsigned int i=0; i<_nb_params; i++)
    {
        PNumericValue* num_val_tmp = new PNumericValue(_params[i]);
        vec_numeric.push_back(num_val_tmp);
    }
    return vec_numeric.size();
}

unsigned int PConstant::assign_params(vector<float>& _vec_params)
{
    for (unsigned int i=0; i<_vec_params.size(); i++)
    {
        PNumericValue* num_val_tmp = new PNumericValue(_vec_params.at(i));
        vec_numeric.push_back(num_val_tmp);
    }
    return vec_numeric.size();
}

string PConstant::compute_str(std::string* _str_var, float* _params, unsigned int& _size)
{
    string buffer;
    string value;
    PTextParser textparser;

    value = getText();
    if (!vec_numeric.empty()) value += "(";
    for (unsigned int i=0; i<vec_numeric.size(); i++)
    {
        float x_buffer = vec_numeric.at(i)->eval(_str_var, _params, _size);
        buffer = textparser.float_to_stdstr(x_buffer);
        value += buffer;
        //debug_str = value;
        if (i==vec_numeric.size()-1) value += ")";
        else value += ",";
    }
    return value;
}


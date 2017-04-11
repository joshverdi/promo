#include <string>
#include <sstream>

#include "pword.h"
#include "engine/procedural.h"
#include "engine/ptextparser.h"
#include "engine/pnumericvalue.h"
#include "grammar/pvariable.h"
#include "grammar/parithmetic.h"

using namespace std;

PWord::PWord()
{
}

PWord::PWord(char _c):PSymbol(_c)
{
}

PWord::PWord(string _text):PSymbol(_text)
{
}

PWord::PWord(const PWord& _word):PSymbol()
{
    PVariable *pv, *my_pv;
    PArithmetic *pa, *my_pa;
    PNumericValue *pnv, *my_pnv;
    std::vector<PNumeric*> vec_pn = _word.getVecNumeric();
    text = _word.getText();
    for (std::vector<PNumeric*>::iterator iter=vec_pn.begin(); iter!=vec_pn.end(); iter++)
    {
        pv = NULL;
        pa = NULL;
        pnv = NULL;
        if ((pv=dynamic_cast<PVariable*>(*iter)))
        {
            my_pv = new PVariable(*pv);
            vec_numeric.push_back(my_pv);
        }
        else if ((pnv=dynamic_cast<PNumericValue*>(*iter)))
        {
            my_pnv = new PNumericValue(*pnv);
            vec_numeric.push_back(my_pnv);
        }
        else if ((pa=dynamic_cast<PArithmetic*>(*iter)))
        {
            my_pa = new PArithmetic(*pa);
            vec_numeric.push_back(my_pa);
        }
    }
    //vec_numeric = _word.getVecNumeric();
}

PWord::~PWord()
{
    for (std::vector<PNumeric*>::iterator iter=vec_numeric.begin(); iter!=vec_numeric.end(); iter++)
    {
        delete (*iter);
    }
    vec_numeric.clear();
}

PWord& PWord::operator=(const PWord& _word)
{
    PVariable *pv, *my_pv;
    PArithmetic *pa, *my_pa;
    PNumericValue *pnv, *my_pnv;
    std::vector<PNumeric*> vec_pn = _word.getVecNumeric();
    text = _word.getText();
    for (std::vector<PNumeric*>::iterator iter=vec_pn.begin(); iter!=vec_pn.end(); iter++)
    {
        pv = NULL;
        pa = NULL;
        pnv = NULL;
        if ((pv=dynamic_cast<PVariable*>(*iter)))
        {
            my_pv = new PVariable(*pv);
            vec_numeric.push_back(my_pv);
        }
        else if ((pnv=dynamic_cast<PNumericValue*>(*iter)))
        {
            my_pnv = new PNumericValue(*pnv);
            vec_numeric.push_back(my_pnv);
        }
        else if ((pa=dynamic_cast<PArithmetic*>(*iter)))
        {
            my_pa = new PArithmetic(*pa);
            vec_numeric.push_back(my_pa);
        }
    }
    return *this;
}

void PWord::add(PNumeric* _numeric)
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
void PWord::add(float _val)
{

    vec_numeric.push_back(new PNumericValue(_val));
}

bool PWord::is_similar(string _text)
{
    return (getText().compare(_text)==0)?true:false;
}

bool PWord::is_similar(PWord* _pw)
{
    return is_similar(_pw->getText());
}

//unsigned int PWord::assign_params(float* _params, unsigned int _nb_params)
//{
//    vec_numeric.clear();
//    for (unsigned int i=0; i<_nb_params; i++)
//    {
//        PNumericValue* num_val_tmp = new PNumericValue(_params[i]);
//        vec_numeric.push_back(num_val_tmp);
//    }
//    return vec_numeric.size();
//}

//unsigned int PWord::assign_params(vector<float>& _vec_params)
//{
//    vec_numeric.clear();
//    for (unsigned int i=0; i<_vec_params.size(); i++)
//    {
//        PNumericValue* num_val_tmp = new PNumericValue(_vec_params.at(i));
//        vec_numeric.push_back(num_val_tmp);
//    }
//    return vec_numeric.size();
//}

string PWord::compute_str()
{
    string buffer;
    string value;
    PNumeric* pn;
    PNumericValue* pnv=NULL;

    value = getText();
    if (!vec_numeric.empty()) value += "(";
    for (unsigned int i=0; i<vec_numeric.size(); i++)
    {
        pn = vec_numeric.at(i);
        pnv = dynamic_cast<PNumericValue*>(pn);
        if (!pnv) return value;
        buffer = pnv->to_stdstr();
        value += buffer;
        if (i==vec_numeric.size()-1) value += ")";
        else value += ",";
    }
    return value;
}

string PWord::compute_str(std::string* _str_var, float* _params, unsigned int& _size)
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

bool PWord::contains(string _text)
{
    return (getText().find(_text)!=string::npos)?true:false;
}


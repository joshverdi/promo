#include <vector>
#include <iostream>
#include <boost/bimap.hpp>

#include "grammar/parithmetic.h"
#include "grammar/palgebra.h"
#include "grammar/pconstant.h"
#include "grammar/pvariable.h"
#include "pruleparametric.h"
#include "pbooleanoperator.h"
#include "plogicaloperator.h"
#include "pcomparisonoperator.h"
#include "pnumericvalue.h"
#include "prule.h"
#include "ptextparser.h"
#include "procedural.h"
#include "assert.h"

using namespace std;

PRuleParametric::PRuleParametric()
{
    preproc = 0;
}

PRuleParametric::PRuleParametric(string _text, PAlphabet* _palphabet, PPreproc* _preproc):PRule(_text, _palphabet)
{
    if (_preproc)
    {
        preproc = new PPreproc();
        *preproc = *_preproc;
    }
    else preproc = 0;

    pick_predecessor();
    pick_condition();
    successor = pick_successor();
    pick_variable();
    if ((!condition.compare("*"))||(!condition.compare("(*)")))
        condition= string("(0<1)");
    if (!condition.empty())
        pbo_condition = parse_algebra(condition);
    else pbo_condition = 0;
    parse_successor(successor, alphabet);
}

PRuleParametric::~PRuleParametric()
{
    delete preproc;
}

PRuleParametric& PRuleParametric::operator=(const PRuleParametric& _rulepara)
{
    PRule::operator =(_rulepara);
    if (_rulepara.getPreproc())
    {
        preproc = new PPreproc();
        *preproc = *_rulepara.getPreproc();
    }
    else preproc = 0;

    pick_predecessor();
    pick_condition();
    successor = pick_successor();
    pick_variable();
    if ((!condition.compare("*"))||(!condition.compare("(*)")))
        condition= string("(0<1)");
    if (!condition.empty())
        pbo_condition = parse_algebra(condition);
    else pbo_condition = 0;
    parse_successor(successor, alphabet);
    return *this;
}

string PRuleParametric::pick_comparison_type(string& _src, unsigned int _index)
{
    string value = string("");
    char c0 = _src[_index];
    char c1;

    if (_index<_src.size()-1) c1 = _src[_index+1];

    if ((c0=='<')||(c0=='>')||(c0=='=')||(c0=='!'))
    {
        value += _src.substr(_index, 1);
        if (c1=='=')
            value += _src.substr(_index+1, 1);
    }
    return value;
}

bool PRuleParametric::pick_condition()
{
    bool value = false;
    size_t separator_index;
    separator_index = source.find(':');
    if (separator_index != source.npos)
    {
        condition = source.substr(separator_index+1);

        value = true;
    }
    return value;
}

string PRuleParametric::pick_successor()
{
    return (successor = pick_substitute());
}

unsigned int PRuleParametric::pick_variable()
{
    PTextParser textparser;
    string buffer = source;
    string str_left_buffer, str_right_buffer;

    size_t idx_buf;
    if ((idx_buf=source.find(':'))!=string::npos)
    {
        buffer = source.substr(0, idx_buf);
    }
    idx_buf = buffer.find('<');
    if (idx_buf!=string::npos)
    {
        str_left_buffer = buffer.substr(0, idx_buf);
        buffer = buffer.substr(idx_buf+1);
        //pick_variable(str_left_buffer, true);
    }
    idx_buf = buffer.find('>');
    if (idx_buf!=string::npos)
    {
        str_right_buffer = buffer.substr(idx_buf+1);
        buffer = buffer.substr(0,idx_buf);
        //pick_variable(str_right_buffer);
    }
    idx_buf = buffer.find('(');
    if (idx_buf!=string::npos)
    {
        unsigned int idx_closer;
        string seq_buf;
        vector<string> vec_str_var;
        PVariable* pvar_tmp;
        idx_closer = textparser.find_bracket_closer(buffer, idx_buf);
        seq_buf = buffer.substr(idx_buf+1, idx_closer-idx_buf-1);
        vec_str_var = textparser.arrange_sequence(seq_buf);
        for (unsigned int i=0; i<vec_str_var.size(); i++)
        {
            pvar_tmp = new PVariable(vec_str_var.at(i));
            vec_var.push_back(pvar_tmp);
        }
    }
    return vec_var.size();
}

unsigned int PRuleParametric::pick_variable(std::string& _src, bool _reverse_reading)
{
    unsigned int value, i, idx_second;
    string buffer;
    vector<string> vec_str;
    PVariable* pvar_tmp;
    PTextParser textparser;

    value=0;

    if (!_reverse_reading)
    {
        i=0;
        while(i<_src.size())
        {
            if (_src[i]=='(')
            {
                idx_second = textparser.find_bracket_closer(_src, i);
                buffer = _src.substr(i+1, idx_second - i - 1);
                vec_str = textparser.arrange_sequence(buffer);
                for (unsigned int j=0; j<vec_str.size(); j++)
                {
                    pvar_tmp = new PVariable(vec_str.at(j));
                    vec_var.push_back(pvar_tmp);
                }
                i=idx_second;
            }
            i++;
        }
    }
    else
    {
        i=_src.size()-1;
        while (i<_src.size())
        {
            if (_src[i]==')')
            {
                idx_second = textparser.find_bracket_opener(_src, i);
                buffer = _src.substr(idx_second+1, i-idx_second-1);
                vec_str = textparser.arrange_sequence(buffer);
                for (unsigned int j=0; j<vec_str.size(); j++)
                {
                    pvar_tmp = new PVariable(vec_str.at(j));
                    vec_var.push_back(pvar_tmp);
                }
                i=idx_second;
            }
            i--;
        }
    }
    return vec_str.size();
}

vector<float> PRuleParametric::pick_params(std::string& _src, unsigned int _idx)
{
    vector<float> vec_value;
    vector<string> vec_str_buffer;
    string str_buffer;
    unsigned int idx_closer;
    PTextParser textparser;

    if (alphabet->is_alphabet(_src[_idx]))
    {
        if (_idx+1<_src.size())
        {
            if (_src[_idx+1]=='(')
            {
                idx_closer = textparser.find_bracket_closer(_src, _idx+1);
                str_buffer = _src.substr(_idx+2, idx_closer-_idx-2);
                vec_str_buffer = textparser.arrange_sequence(str_buffer);
                for (unsigned int i=0; i<vec_str_buffer.size(); i++)
                {
                    vec_value.push_back(textparser.stdstr_to_float(vec_str_buffer.at(i)));
                }
            }
        }
    }
    return vec_value;
}

string PRuleParametric::pick_predecessor()
{
    size_t i;
    string var_buffer;
    vector<string> vec_str_var;
    if ((i=source.find(":"))!=string::npos)
    {
        predecessor = source.substr(0,i);
    }
    else predecessor = source;

    if (predecessor.empty()) return predecessor;

    if (predecessor[0]=='.')
    {
        for (i=1; (isdigit(predecessor[i])&&(i<predecessor.size())); i++);
        i++;
        predecessor = predecessor.substr(i);
    }

    if ((i=predecessor.find('<'))!=string::npos)
    {
        left_context = predecessor.substr(0, i);
        predecessor = predecessor.substr(i+1);
    }

    if ((i=predecessor.find('>'))!=string::npos)
    {
        right_context = predecessor.substr(i+1);
        predecessor = predecessor.substr(0, i);
    }

    if ((i=predecessor.find('('))!=string::npos)
        predecessor.erase(i);
    return predecessor;
}

bool PRuleParametric::is_binary_operator(char _c)
{
    return ((_c=='!')||(_c=='&')||(_c=='|'));
}

bool PRuleParametric::is_binary_operator(std::string& _src, unsigned int _index)
{
    return is_binary_operator(_src[_index]);
}

bool PRuleParametric::is_binary_operation(string& _src)
{
    bool value = false;
    if (    (_src.find("&") != string::npos)
          ||(_src.find("|") != string::npos)
          ||(_src.find("!") != string::npos)
       )
        value = true;
    return value;
}

bool PRuleParametric::is_decim_comp_operation(string& _src)
{
    bool value = false;
    if (is_binary_operation(_src)) return false;
    if (    (_src.find("=") != string::npos)
          ||(_src.find("<") != string::npos)
          ||(_src.find(">") != string::npos)
       )
        value = true;
    return value;
}

bool PRuleParametric::is_variable(string& _src, unsigned int _index)
{
    PNumeric* pnum;
    PVariable* pvar;
    PWord* pw;
    bool value = false;
    string buffer = _src.substr(_index, 1);
    for (unsigned int i=0; i<vec_var.size(); i++)
    {
        if (vec_var.at(i)->is_similar(buffer)) return true;
    }
    if (context)
    {
        for (unsigned int i=0; i<context->getVecLeft().size(); i++)
        {
            pw = context->getVecLeft().at(i);
            for (unsigned int j=0; j<pw->getVecNumeric().size(); j++)
            {
                pnum = pw->getVecNumeric().at(j);
                pvar = dynamic_cast<PVariable*>(pnum);
                if (!pvar) return false;
                if (pvar->is_similar(buffer))
                    return true;
            }
        }
        for (unsigned int i=0; i<context->getVecRight().size(); i++)
        {
            pw = context->getVecRight().at(i);
            for (unsigned int j=0; j<pw->getVecNumeric().size(); j++)
            {
                pnum = pw->getVecNumeric().at(j);
                pvar = dynamic_cast<PVariable*>(pnum);
                if (!pvar) return false;
                if (pvar->is_similar(buffer))
                    return true;
            }
        }
    }
    return value;
}

bool PRuleParametric::is_constant(std::string& _src, unsigned int _index, unsigned int& _const_size)
{
    bool value = false;
    string s;
    if (!preproc) return value;
    for (unsigned int i=0; i<preproc->getVecDefine().size(); i++)
    {
        s = preproc->getVecDefine()[i]->getConstantName();
        try
        {
            if (!s.compare(_src.substr(_index, s.size())))
            {
                value = true;
                _const_size = s.size();
                return value;
            }
        }
        catch (...)
        {
            return value;
        }
    }
    return value;
}

bool PRuleParametric::is_num_val(std::string& _src, unsigned int _index)
{
    return (isdigit(_src[_index]))?true:false;
}

bool PRuleParametric::is_arith_operator(std::string& _src, unsigned int _index)
{
    char c = _src[_index];
    switch (c)
    {
    case '+':case '-': case '*': case '/': case '^': return true;
    }
    return false;
}

bool PRuleParametric::is_comparison_operator(std::string& _src, unsigned int _index)
{
    char c1, c2;

    c1 = _src[_index];
    switch (c1)
    {
    case '=':case '<':case '>': return true;
    case '!':
        if (_index+1<_src.size())
        {
            c2 = _src[_index+1];
            if (c2 == '=') return true;
        }

    }
    return false;
}

bool PRuleParametric::is_candidate(string& _alphabet,
                  vector<float>& _vec_params,
                  string& _current_derivation,
                  unsigned int _index,
                  PIgnore* _ignore,
                  vector<string>& _vec_str_aux_params,
                  vector<float>& _vec_aux_params)
{
    bool context_value     = true;
    bool condition_value   = true;
    PNumeric* pnum;
    PNumericValue* pnv;
    PVariable* pvar;
    PWord* pw;
    string* pstr_params;
    float* params;
    unsigned int i, j, k, jump;
    unsigned int params_size, auxiliary_size=0;
    PTextParser textparser;
    vector<PVariable*> vec_var_tmp;
    vector<PNumeric*> vec_num_tmp;
    vector<PWord*> vec_left_tmp, vec_right_tmp;
    vector<float> vec_params_tmp;
    vector<string> vec_str_tmp;

    if (predecessor.compare(_alphabet)) return false;

    // This block collects the contexts parameters if any
    if (context)
    {
        bool context_value_left = context->check_left(_current_derivation, _index, _ignore);
        bool context_value_right = context->check_right(_current_derivation, _index, _ignore);
        if (context->getVecRight().size()==0)
            context_value=context_value_left;
        else if(context->getVecLeft().size()==0)
            context_value=context_value_right;
        else context_value=(context_value_left && context_value_right);
        if (context_value==false) return false;
        // Note that the order here is strict, it means that left contexts is retrieved before
        // right ones, so that the numeric (float) correspondance can be done with the same way
        // The next lines load the numeric (float) parameters from the context which was
        // checked before

        // IMPORTANT: Please note that auxiliary parameters have been removed in the recent versions
        // Those parameters are related to the ones which are in the left and right contexts of the alphabet
        // Let the predecessor be: A(a)<B(b,x)>C(c), then a and c will be taken as auxiliary parameters
        // For the current version of the project, the contexts are not handled correctly, and
        // the auxiliary parameters are left uninitialized.

//        i=0;
//        k=_index-1;
//        while(i<context->getVecLeft().size())
//        {
//            jump = 1;
//            if (k<_current_derivation.size())
//            {
//                if (_current_derivation[k]==')')
//                {
//                    unsigned int idx_second = textparser.find_bracket_opener(_current_derivation, k);
//                    jump = k-idx_second+1;
//                }
//                else if (alphabet->is_alphabet(_current_derivation[k]))
//                {
//                    pw = new PWord(_current_derivation[k]);
//                    if (context->getVecLeft().at(i)->is_similar(pw))
//                    {
//                        vec_params_tmp = pick_params(_current_derivation, k);
//                        for (unsigned int l=0; l<vec_params_tmp.size(); l++)
//                        {
//                            pw->add(vec_params_tmp.at(l));
//                        }
//                        //vec_params_tmp.insert(vec_params_tmp.begin(), vec_word_params_tmp.begin(), vec_word_params_tmp.end());
//                        vec_left_tmp.push_back(pw);
//                        i++;
//                    }
//                }
//            }
//            k-=jump;
//        }
//        i=0;
//        k=_index+1;

//        while(i<context->getVecRight().size())
//        {
//            jump=1;
//            if (k<_current_derivation.size())
//            {
//                if (alphabet->is_alphabet(_current_derivation[k]))
//                {
//                    pw = new PWord(_current_derivation[k]);
//                    if (context->getVecRight().at(i)->is_similar(pw))
//                    {
//                        if (k+1<_current_derivation.size())
//                            if (_current_derivation[k+1]=='(')
//                            {
//                                unsigned int idx_second = textparser.find_bracket_closer(_current_derivation, k+1);
//                                jump = idx_second-k+1;
//                            }
//                        vec_params_tmp = pick_params(_current_derivation, k);
//                        for (unsigned int l=0; l<vec_params_tmp.size(); l++)
//                        {
//                            pw->add(vec_params_tmp.at(l));
//                        }
//                        vec_right_tmp.push_back(pw);
//                        i++;
//                    }
//                }
//                else if (_current_derivation[k]=='(')
//                {
//                    unsigned int idx_second = textparser.find_bracket_closer(_current_derivation, k);
//                    jump = idx_second-k+1;
//                }
//            }
//            k+=jump;
//        }
//        vec_params_tmp.clear();
//        vec_str_tmp.clear();
//        if (context->getVecLeft().size()!=vec_left_tmp.size()) return false;
//        for (i=0; i<vec_left_tmp.size(); i++)
//        {
//            if (context->getVecLeft().at(i)->getVecNumeric().size()!=vec_left_tmp.at(i)->getVecNumeric().size()) return false;
//            for (j=0; j<vec_left_tmp.at(i)->getVecNumeric().size(); j++)
//            {
//                pnum = context->getVecLeft().at(i)->getVecNumeric().at(j);
//                pvar = dynamic_cast<PVariable*>(pnum);

//                pnum = vec_left_tmp.at(i)->getVecNumeric().at(j);
//                pnv = dynamic_cast<PNumericValue*>(pnum);
//                if (!pvar&&pnv) return false;
//                vec_params_tmp.push_back(pnv->getData());
//                vec_str_tmp.push_back(pvar->getName());
//            }
//        }
//        if (context->getVecRight().size()!=vec_right_tmp.size()) return false;
//        for (i=0; i<vec_right_tmp.size(); i++)
//        {
//            if (context->getVecRight().at(i)->getVecNumeric().size()!=vec_right_tmp.at(i)->getVecNumeric().size()) return false;
//            for (j=0; j<vec_right_tmp.at(i)->getVecNumeric().size(); j++)
//            {
//                pnum = context->getVecRight().at(i)->getVecNumeric().at(j);
//                pvar = dynamic_cast<PVariable*>(pnum);

//                pnum = vec_right_tmp.at(i)->getVecNumeric().at(j);
//                pnv = dynamic_cast<PNumericValue*>(pnum);
//                if (!pvar&&pnv) return false;
//                vec_params_tmp.push_back(pnv->getData());
//                vec_str_tmp.push_back(pvar->getName());
//            }
//        }
//    }
    }
//    _vec_aux_params = vec_params_tmp;
//    _vec_str_aux_params = vec_str_tmp;
//    params_size = vec_var.size();
//    if (params_size!=_vec_params.size()) return false;
//    auxiliary_size = vec_params_tmp.size();
//    params_size+=auxiliary_size;
//    pstr_params = new string[params_size];
//    params = new float[params_size];
//    for (i=0; i<vec_var.size(); i++)
//    {
//        params[i] = _vec_params.at(i);
//        pstr_params[i] = vec_var.at(i)->getName();
//    }
//    for (i=vec_var.size(), j=0; i<params_size; i++, j++)
//    {
//        params[i] = vec_params_tmp.at(j);
//        pstr_params[i] = vec_str_tmp.at(j);
//    }

//    if (pbo_condition)
//        condition_value = pbo_condition->eval(pstr_params, params, params_size);
//    delete [] params;
//    delete [] pstr_params;

    if (!condition.empty())
    {
        string cond =condition;
        if(_vec_params.size()!=vec_var.size())
        {
            condition_value=false;
            std::cout<<"Not enougth var"<<std::endl;
        }
        else
        {
            condition_value=is_condition_valid(condition,_vec_params);
        }

    }

    return (condition_value&&context_value);
}

bool PRuleParametric::is_condition_valid(string& _condition,std::vector<float> _vec_params)
{
    QString _cond=QString::fromStdString(_condition);
    unsigned int ind_temp = 0;
    bool res=true;
    bool res1=true;
    bool res2 =true;
    if(_cond.startsWith('('))
    {
        ind_temp = PTextParser::find_bracket_closer(_condition.substr(0,_condition.npos),0);
        std::string cond1=_condition.substr(1,ind_temp-1);
        if(ind_temp!=_condition.size()-1)
        {
            string cond2=_condition.substr(ind_temp+3,_condition.npos);
            string op=_condition.substr(ind_temp+1,2);
            res1=is_condition_valid(cond1,_vec_params);
            res2=is_condition_valid(cond2,_vec_params);
            int ind_opo = op.find('|');
            int ind_opa = op.find('&');
            if(ind_opo>=0 )
            {
                res=res1||res2;
            }
            else if (ind_opa>=0)
            {
                res=res1&&res2;
            }
            else
            {
                std::cout<<"Problem with Condition"<<std::endl;
                res = false;
            }
        }
        else res= is_condition_valid(cond1,_vec_params);
    }
    else
    {
        int ind_opo = _condition.find('|');
        int ind_opa = _condition.find('&');

        std::string cond1;
        std::string cond2;
        if (ind_opo>0)
        {
            cond1=_condition.substr(0,ind_opo);
            cond2=_condition.substr(ind_opo+2,_condition.npos);
            res1=is_condition_valid(cond1,_vec_params);
            res2=is_condition_valid(cond2,_vec_params);
            res=res1||res2;
        }
        else if (ind_opa>0)
        {
            cond1=_condition.substr(0,ind_opo);
            cond2=_condition.substr(ind_opo+2,_condition.npos);
            res1=is_condition_valid(cond1,_vec_params);
            res2=is_condition_valid(cond2,_vec_params);
            res=res1&&res2;
        }
        else
        {
            std::vector<float>::iterator iter_param = _vec_params.begin();
            std::vector<PVariable*>::iterator iter_var = vec_var.begin();
            res=true;

            while(iter_param!=_vec_params.end() && iter_var!=vec_var.end())
            {
                PVariable* var =*iter_var;

                if (var->getName().compare(_condition.substr(0,1))==0)
                {
                    int ind_tmp = _condition.find('<');
                    int inde_tmp = _condition.find('=');
                    if(ind_tmp<0 && inde_tmp>0)
                    {
                        std::string tmp= _condition.substr(inde_tmp+1,_condition.npos);
                        float val_num = std::stof(tmp);
                        if(!(*iter_param==val_num))
                            res= false;
                    }

                    else if(ind_tmp>0)
                    {
                        if(inde_tmp>0)
                        {
                            std::string tmp= _condition.substr(inde_tmp+1,_condition.npos);
                            float val_num = std::stof(tmp);
                            if(!(*iter_param<=val_num))
                                res= false;
                        }
                        else
                        {
                            std::string tmp= _condition.substr(ind_tmp+1,_condition.npos);
                            float val_num = std::stof(tmp);
                            if(!(*iter_param<val_num))
                                res= false;
                        }
                    }
                    else {
                        ind_tmp = _condition.find('>');

                        if(inde_tmp>0)
                        {
                            std::string tmp= _condition.substr(inde_tmp+1,_condition.npos);
                            float val_num = std::stof(tmp);
                            if(!(*iter_param>=val_num))
                                res= false;
                        }
                        else
                        {
                            std::string tmp= _condition.substr(ind_tmp+1,_condition.npos);
                            float val_num = std::stof(tmp);
                            if(!(*iter_param>val_num))
                                res= false;
                        }
                    }
                }
                iter_param++;
                iter_var++;
            }
            return res;
        }
    }
    return res;

}
PComparisonOperator* PRuleParametric::parse_comparison(string& _src)
{
    PComparisonOperator* value=0;
    PNumeric* pn_left;
    PNumeric* pn_right;
    string str_left, str_right;
    string comp_type;
    unsigned int left_boundary, right_boundary;
    size_t comp_idx, comp_size;


    if ((comp_idx=_src.find("<="))!=string::npos)
        {comp_size = 2;comp_type="<=";}
    else if ((comp_idx=_src.find(">="))!=string::npos)
        {comp_size = 2;comp_type=">=";}
    else if ((comp_idx=_src.find("!="))!=string::npos)
        {comp_size = 2;comp_type="!=";}
    else if ((comp_idx=_src.find("="))!=string::npos)
        {comp_size = 1;comp_type="=";}
    else if ((comp_idx=_src.find("<"))!=string::npos)
        {comp_size = 1;comp_type="<";}
    else if ((comp_idx=_src.find(">"))!=string::npos)
        {comp_size = 1;comp_type=">";}

    if (comp_idx!=string::npos)
    {
        left_boundary = comp_idx;
        right_boundary = comp_idx + comp_size;
        str_left = _src.substr(0, left_boundary);
        str_right = _src.substr(right_boundary);
    }

    pn_left = parse_numeric(str_left);
    pn_right = parse_numeric(str_right);

    if (pn_left&&pn_right) value = new PComparisonOperator(comp_type, pn_left, pn_right);
    return value;
}

PBooleanOperator* PRuleParametric::parse_algebra(std::string& _src)
{
    unsigned int i, char_size;
    float x;
    PBooleanOperator* value=0;
    PLogicalOperator* plo;
    PComparisonOperator* pco;
    PVariable* pvar;
    PNumeric* pnum;
    PAlgebra* algebra;
    PTextParser textparser;

    vec_algebra.clear();
    i=0;
    while (i<_src.size())
    {
        char_size=1;
        if (is_variable(_src, i))
        {
            pvar = new PVariable(_src[i]);
            vec_algebra.push_back(pvar);
        }
        else if (is_num_val(_src, i))
        {
            x = textparser.extract_number(_src, i, char_size, DECIMAL_SEPARATOR);
            pnum = new PNumericValue(x);
            vec_algebra.push_back(pnum);
        }
        else if (is_arith_operator(_src, i))
        {
            pnum = new PArithmetic(_src[i]);
            vec_algebra.push_back(pnum);
        }
        else if (is_binary_operator(_src[i]))
        {
            plo = new PLogicalOperator(_src[i]);
            vec_algebra.push_back(plo);
        }
        else if (is_comparison_operator(_src, i))
        {
            string str_buf = pick_comparison_type(_src, i);
            pco = new PComparisonOperator(str_buf);
            char_size = str_buf.size();
            vec_algebra.push_back(pco);
        }
        else if (_src[i]==')')
        {
            pile_up();
        }
        i+=char_size;
    }
    algebra = vec_algebra.back();
    value = dynamic_cast<PBooleanOperator*>(algebra);
    return value;
}

PBooleanOperator* PRuleParametric::parse_condition(string& _text)
{
    unsigned int i;
    unsigned int i_closer;
    unsigned int depth;
    unsigned int jump;
    PBooleanOperator* value=pbo_condition;
    PBooleanOperator* pleft;
    PBooleanOperator* pright;
    PLogicalOperator* proot;
    PTextParser textparser;

    i=0;
    jump = 1;
    while (i<_text.size())
    {
        string str_buf;

        switch (_text[i])
        {
        case '(':
            depth++;
            i_closer = textparser.find_bracket_closer(_text, i);
            str_buf = _text.substr(i+1, i_closer-1-i);
            if (is_binary_operation(str_buf))
            {
                jump = 1;
            }
            else if (is_decim_comp_operation(str_buf))
            {
                jump = str_buf.size();
                PComparisonOperator* pco_tmp = parse_comparison(str_buf);
                if (!pleft)
                {
                    pleft = pco_tmp;
                }
                else
                {
                    proot->attach(pco_tmp);
                }
            }

            break;
        case ')':
            depth--;
            jump = 1;
            break;
        case '&':case '|':
            PLogicalOperator* plo_tmp = new PLogicalOperator(_text.substr(i, 1), depth);
            if ((!proot)&&(pleft))
            {
                proot = plo_tmp;
                pleft->setRoot(proot);
                proot->setLeft(pleft);
            }
            else
            {
                pright = plo_tmp;
                proot->attach(pright);
            }
            jump = 1;
            break;
        }
        i+=jump;

    }
    return value;
}

unsigned int PRuleParametric::parse_successor(string& _str_successor, PAlphabet* _alphabet)
{
    unsigned int i=0;
    unsigned int character_size, closer, alphabet_size;
    string buffer;
    PNumeric* pn;
    PConstant* pconst;
    PWord* pw;
    PSymbol* pc;
    PTextParser textparser;
    vector<string> vec_str;
    vector<PNumeric*> vec_num;

    while (i<_str_successor.size())
    {
        character_size = 1;
        alphabet_size = _alphabet->search(_str_successor, i);
        if (alphabet_size!=0)
        {
            character_size = alphabet_size;
            pw = new PWord(successor.substr(i, alphabet_size));
            if (i+alphabet_size<_str_successor.size())
            {
                if (_str_successor[i+alphabet_size]=='(')
                {
                    closer = textparser.find_bracket_closer(_str_successor, i+alphabet_size);
                    buffer = _str_successor.substr(i+alphabet_size+1, closer-i-alphabet_size-1);
                    //E.g.:foxalpha(10)bravo => 11-3-5-1
                    vec_num.clear();
                    vec_str.clear();
                    vec_str = textparser.arrange_sequence(buffer);
                    for (unsigned int j = 0; j<vec_str.size(); j++)
                    {
                        buffer = vec_str.at(j);
                        pn = parse_numeric(buffer);
                        if (pn)
                        {
                            pw->add(pn);
                            vec_num.push_back(pn);
                        }
                    }
                    character_size = closer - i + 1;
                    bm_formul.insert(formula_match(i, vec_num));
                }
            }
            vec_successor.push_back(pw);
            vec_successor_idx.push_back(i);
        }
        else if (_alphabet->is_constant(_str_successor[i]))
        {
            pconst = new PConstant(_str_successor[i]);
            if (i+1<_str_successor.size())
            {
                if (_str_successor[i+1]=='(')
                {
                    closer = textparser.find_bracket_closer(_str_successor, i+1);
                    buffer = _str_successor.substr(i+2, closer-2-i);
                    vec_num.clear();
                    vec_str.clear();
                    vec_str = textparser.arrange_sequence(buffer);
                    for (unsigned int j = 0; j<vec_str.size(); j++)
                    {
                        buffer = vec_str.at(j);
                        pn = parse_numeric(buffer);
                        if (pn)
                        {
                            pconst->add(pn);
                            vec_num.push_back(pn);
                        }
                    }
                    character_size = closer - i + 1;
                    bm_formul.insert(formula_match(i, vec_num));
                }
            }
            vec_successor.push_back(pconst);
            vec_successor_idx.push_back(i);
        }
        // Theoretically this function should never assess the ELSE instruction below, though square brackets do
        else
        {
            pc = new PSymbol(_str_successor.substr(i,1));
            vec_successor.push_back(pc);
            vec_successor_idx.push_back(i);
            character_size = 1;
        }
        i+=character_size;
    }
    return vec_successor.size();
}

/***********************************************************************************
_params seems like numeric parameters
_size their numbers
_pstr_aux_params sound like auxiliary parameters more like the define strings
_aux_size is the size are more precisely the numbers of auxiliary parameters
To make short: for F(0,1,2) with '#define x 7', '#define y 8', '#define z 9'
_params are 0, 1, 2
_size is 3
_pstr_aux_params is x, y, z
_aux_params is 7, 8, 9
_aux_size is 3
***********************************************************************************/
string PRuleParametric::compute(    float* _params, unsigned int _size, string* _pstr_aux_params, float* _aux_params, unsigned int _aux_size,
                                    boost::bimap<unsigned int, unsigned int>& _bm_succidx_prodidx)
{
    string value;
    string buffer;
    unsigned int full_params_size, preproc_const_size;
    string* pstr_params_full;
    float* full_params;

    value.clear();
    full_params_size = preproc_const_size = 0;
    if (_size != vec_var.size()) return value;
    if (preproc) preproc_const_size = preproc->getVecDefine().size();
    full_params_size = _size+_aux_size+preproc_const_size;
    full_params = new float[full_params_size];
    pstr_params_full= new string[full_params_size];
    // Fill the match between parameters name and parameters values:
    for (unsigned int i=0; i<_size; i++)
    {
        pstr_params_full[i] = vec_var.at(i)->getName();
        full_params[i] = _params[i];
    }
    // Fill the match between auxiliary parameters name and parameters values:
    for (unsigned int i=_size; i<full_params_size-preproc_const_size; i++)
    {
        pstr_params_full[i] = _pstr_aux_params[i-_size];
        full_params[i] = _aux_params[i-_size];
    }
    if (preproc)
    {
        for (unsigned int i=full_params_size-preproc_const_size;i<full_params_size;i++)
        {
            pstr_params_full[i] = preproc->getVecDefine()[i-_size-_aux_size]->getConstantName();
            full_params[i] = preproc->getVecDefine()[i-_size-_aux_size]->getConstantValue();
        }
        //--------DEBUGGING ISSUE------------
        vector<string> vec_str_debug;
        vector<float> vec_x_debug;
        for (unsigned int i=0; i<full_params_size; i++)
        {
            vec_str_debug.push_back(pstr_params_full[i]);
            vec_x_debug.push_back(full_params[i]);
        }
        vec_str_debug.clear();
        //------------------------------------
    }
    assert(vec_successor.size()==vec_successor_idx.size());
    _bm_succidx_prodidx.clear();
    for (unsigned int i=0; i<vec_successor.size(); i++)
    {
        buffer = vec_successor.at(i)->compute_str(pstr_params_full, full_params, full_params_size);
        _bm_succidx_prodidx.insert(boost::bimap<unsigned int, unsigned int>::value_type(vec_successor_idx.at(i), value.size()));
        /*
        //Debug:
        Procedural::boost_last_elt_dbg(_bm_succidx_prodidx);
        std::cout << "Current Computed Text : " << value << std::endl;
        std::cout << std::endl << "****========****" << std::endl;
        */
        value+=buffer;

    }
    incr_applycount();

    /*
    //Debug:
    std::cout << std::endl << "****========****" << std::endl;
    std::cout << "Rule Line : " << getText() << std::endl;
    std::cout << "Production Line : " << value << std::endl;
    std::cout << "Bimap generated : " <<  _bm_succidx_prodidx.size() << "Elements." << std::endl;
    Procedural::boost_dbg(_bm_succidx_prodidx);
    */


    delete[] full_params;
    delete[] pstr_params_full;
    return value;
}

unsigned int PRuleParametric::pile_up()
{
    unsigned int value = 0;
    PAlgebra *main_elt, *left_elt, *right_elt;
    PArithmetic* parith;
    PNumeric *pnum_left, *pnum_right;
    PBooleanOperator *pbo_left, *pbo_right;
    PLogicalOperator* plo;
    PComparisonOperator* pco;

    if (vec_algebra.size()>2)
    {
        right_elt = vec_algebra.back();
        vec_algebra.pop_back();

        std::swap(vec_algebra[vec_algebra.size()-1], vec_algebra[vec_algebra.size()-2]);

        left_elt = vec_algebra.back();
        vec_algebra.pop_back();

        main_elt = vec_algebra.back();
        value = 2;

        if (main_elt&&left_elt&&right_elt)
        {
            if ((pco=dynamic_cast<PComparisonOperator*>(main_elt))!=0)
            {
                pnum_left =dynamic_cast<PNumeric*>(left_elt);
                pnum_right=dynamic_cast<PNumeric*>(right_elt);
                if (pnum_left&&pnum_right)
                {
                    pco->setLeftMember(pnum_left);
                    pco->setRightMember(pnum_right);
                }
            }
            else if ((parith=dynamic_cast<PArithmetic*>(main_elt))!=0)
            {
                pnum_left =dynamic_cast<PNumeric*>(left_elt);
                pnum_right=dynamic_cast<PNumeric*>(right_elt);
                if (pnum_left&&pnum_right)
                {
                    parith->setLeft(pnum_left);
                    parith->setRight(pnum_right);
                    pnum_left->setRoot(parith);
                    pnum_right->setRoot(parith);
                }
            }
            else if ((plo=dynamic_cast<PLogicalOperator*>(main_elt))!=0)
            {
                pbo_left =dynamic_cast<PBooleanOperator*>(left_elt);
                pbo_right=dynamic_cast<PBooleanOperator*>(right_elt);
                if (pbo_left&&pbo_right)
                {
                    plo->setLeft(pbo_left);
                    plo->setRight(pbo_right);
                    pbo_left->setRoot(plo);
                    pbo_right->setRoot(plo);
                }
            }
        }
    }

    return value;

}

unsigned int PRuleParametric::pile_up(std::vector<PAlgebra*>& _vec_algebra)
{
    unsigned int value = 0;
    PAlgebra *main_elt, *left_elt, *right_elt;
    PArithmetic* parith;
    PNumeric *pnum_left, *pnum_right;
    PBooleanOperator *pbo_left, *pbo_right;
    PLogicalOperator* plo;
    PComparisonOperator* pco;

    if (_vec_algebra.size()>2)
    {
        right_elt = _vec_algebra.back();
        _vec_algebra.pop_back();

        std::swap(_vec_algebra[_vec_algebra.size()-1], _vec_algebra[_vec_algebra.size()-2]);

        left_elt = _vec_algebra.back();
        _vec_algebra.pop_back();

        main_elt = _vec_algebra.back();
        value = 2;

        if (main_elt&&left_elt&&right_elt)
        {
            if ((pco=dynamic_cast<PComparisonOperator*>(main_elt))!=0)
            {
                pnum_left =dynamic_cast<PNumeric*>(left_elt);
                pnum_right=dynamic_cast<PNumeric*>(right_elt);
                if (pnum_left&&pnum_right)
                {
                    pco->setLeftMember(pnum_left);
                    pco->setRightMember(pnum_right);
                }
            }
            else if ((parith=dynamic_cast<PArithmetic*>(main_elt))!=0)
            {
                pnum_left =dynamic_cast<PNumeric*>(left_elt);
                pnum_right=dynamic_cast<PNumeric*>(right_elt);
                if (pnum_left&&pnum_right)
                {
                    parith->setLeft(pnum_left);
                    parith->setRight(pnum_right);
                    pnum_left->setRoot(parith);
                    pnum_right->setRoot(parith);
                }
            }
            else if ((plo=dynamic_cast<PLogicalOperator*>(main_elt))!=0)
            {
                pbo_left =dynamic_cast<PBooleanOperator*>(left_elt);
                pbo_right=dynamic_cast<PBooleanOperator*>(right_elt);
                if (pbo_left&&pbo_right)
                {
                    plo->setLeft(pbo_left);
                    plo->setRight(pbo_right);
                    pbo_left->setRoot(plo);
                    pbo_right->setRoot(plo);
                }
            }
        }
    }

    return value;
}

unsigned int PRuleParametric::pile_up(std::vector<PNumeric*>& _vec_numeric)
{
    unsigned int value = 0;
    PNumeric *main_elt, *left_elt, *right_elt;
    PArithmetic* parith;

    if (_vec_numeric.size()>2)
    {
        right_elt = _vec_numeric.back();
        _vec_numeric.pop_back();

        std::swap(_vec_numeric[_vec_numeric.size()-1], _vec_numeric[_vec_numeric.size()-2]);

        left_elt = _vec_numeric.back();
        _vec_numeric.pop_back();

        main_elt = _vec_numeric.back();
        value = 2;

        if (main_elt)
        {
            if ((parith=dynamic_cast<PArithmetic*>(main_elt))!=0)
            {
                parith->setLeft(left_elt);
                parith->setRight(right_elt);
                left_elt->setRoot(parith);
                right_elt->setRoot(parith);
            }
        }
    }

    return value;
}

PNumeric* PRuleParametric::parse_numeric(std::string& _src)
{
    PNumeric* value = 0;
    PNumericValue* pnv;
    PArithmetic* parith;
    PVariable* pvar;
    PTextParser textparser;
    vector<PNumeric*> vec_num;
    unsigned int i, char_size;
    float x;

    i = 0;
    while (i<_src.size())
    {
        char_size = 1;
        if (is_defined(_src, i, char_size))
        {
            pvar = new PVariable(_src.substr(i, char_size));
            vec_num.push_back(pvar);
        }
        else if (is_constant(_src, i, char_size))
        {
            pvar = new PVariable(_src.substr(i, char_size));
            vec_num.push_back(pvar);
        }
        else if (is_variable(_src, i))
        {
            pvar = new PVariable(_src[i]);
            vec_num.push_back(pvar);
        }
        else if (is_num_val(_src, i))
        {
            x = textparser.extract_number(_src, i, char_size, DECIMAL_SEPARATOR);
            pnv = new PNumericValue(x);
            vec_num.push_back(pnv);
        }
        else if (is_arith_operator(_src, i))
        {
            parith = new PArithmetic(_src[i]);
            vec_num.push_back(parith);
        }
        else if (_src[i]==')')
        {
            pile_up(vec_num);
        }
        i += char_size;
        if(i>=_src.size() && _src.size()>1)
            pile_up(vec_num);
    }
    if (!vec_num.empty()) value = vec_num.back();
    return value;
}

std::vector<PNumeric*> PRuleParametric::request_formula(unsigned int _idx)
{
    std::vector<PNumeric*> value;
    try
    {
        value = bm_formul.left.at(_idx);
    }
    catch(...)
    {
        return value;
    }
    return value;
}

std::vector<std::string> PRuleParametric::var_to_arraystr()
{
    std::vector<std::string> value;
    std::vector<PVariable*>::iterator iter;
    PVariable* pvar;
    for (iter = vec_var.begin(); iter!=vec_var.end(); iter++)
    {
        pvar=*iter;
        value.push_back(pvar->getName());
    }
    return value;
}

bool PRuleParametric::is_defined(std::string& _src, unsigned int _index, unsigned int& _char_size)
{
    _char_size = 1;
    if (!preproc) return false;
    for(unsigned int i=0; i<preproc->getVecDefine().size(); i++)
    {
        if (_src.find(preproc->getVecDefine()[i]->getConstantName(), _index) == _index)
        {
            _char_size = preproc->getVecDefine()[i]->getConstantName().size();
            return true;
        }
    }
    return false;
}

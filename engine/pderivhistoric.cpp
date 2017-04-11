#include "engine/pnumeric.h"
#include "engine/pnumericvalue.h"
#include "grammar/pvariable.h"
#include "grammar/parithmetic.h"
#include "pderivhistoric.h"
#include "procedural.h"
#include "grammar/ppreproc.h"
#include "assert.h"

#include <string>

using namespace std;

PDerivHistoric::PDerivHistoric()
{
    idx_within_rule = idx_into_deriv = idx_into_gen = -1;
    rule_gen_num       = deriv_num      = -1;
    stochastic_param   = 1;
    parent = 0;
    preproc = 0;
    word = NULL;
}

PDerivHistoric::PDerivHistoric( PWord* _word, PPreproc* _preproc, std::vector<float> _vec_params,
                                int _idx_into_gen, int _deriv_num,  int _idx_into_deriv, int _rule_gen_num, int _idx_within_rule, double _stochastic_param)
{
    parent              = 0;
    idx_into_gen        = _idx_into_gen;
    deriv_num           = _deriv_num;
    idx_into_deriv      = _idx_into_deriv;
    rule_gen_num        = _rule_gen_num;
    idx_within_rule     = _idx_within_rule;
    stochastic_param    = _stochastic_param;
    if (_word)
        word = _word;
    else word = 0;
    if (_preproc)
        preproc = _preproc;
    else preproc = 0;
    match_idx_dn match_tmp;
    match_tmp.deriv_num         = deriv_num;
    match_tmp.idx_into_deriv    = idx_into_deriv;
    vec_match_idx_dn.push_back(match_tmp);
}

PDerivHistoric::PDerivHistoric(PDerivHistoric* _parent, PWord* _word, PPreproc* _preproc, std::vector<PNumeric*> _vec_numeric,  std::vector<float> _vec_params,
                               int _deriv_num, int _idx_into_deriv, int _rule_gen_num, int _idx_within_rule, double _stochastic_param)
{
    parent              = _parent;
    vec_numeric         = _vec_numeric;
    vec_params          = _vec_params;
    idx_into_gen        = _parent->getIdxIntoDeriv();
    deriv_num           = _deriv_num;
    idx_into_deriv      = _idx_into_deriv;
    rule_gen_num        = _rule_gen_num;
    idx_within_rule     = _idx_within_rule;
    stochastic_param    = _stochastic_param;
    if (_word)
        word = _word;
    else word = 0;
    if (_preproc)
        preproc = _preproc;
    else preproc = 0;
    parent->add_child(this);
    match_idx_dn match_tmp;
    match_tmp.deriv_num         = deriv_num;
    match_tmp.idx_into_deriv    = idx_into_deriv;
    vec_match_idx_dn.push_back(match_tmp);
}

PDerivHistoric::PDerivHistoric(PDerivHistoric* _parent, PWord* _word, PPreproc* _preproc, std::vector<PNumeric*> _vec_numeric,  std::vector<float> _vec_params,
                               int _deriv_num, int _idx_into_deriv, int _idx_into_gen, int _rule_gen_num, int _idx_within_rule, double _stochastic_param)
{
    parent              = _parent;
    vec_numeric         = _vec_numeric;
    vec_params          = _vec_params;
    idx_into_gen        = _idx_into_gen;
    deriv_num           = _deriv_num;
    idx_into_deriv      = _idx_into_deriv;
    rule_gen_num        = _rule_gen_num;
    idx_within_rule     = _idx_within_rule;
    stochastic_param    = _stochastic_param;
    if (_word)
        word = new PWord((*_word));
    else word = 0;
    if (_preproc)
        preproc = _preproc;
    else preproc = 0;
    if(parent != 0)
        parent->add_child(this);
    match_idx_dn match_tmp;
    match_tmp.deriv_num         = deriv_num;
    match_tmp.idx_into_deriv    = idx_into_deriv;
    vec_match_idx_dn.push_back(match_tmp);
}

PDerivHistoric::~PDerivHistoric()
{
    for (std::vector<PDerivHistoric*>::iterator iter=vec_children.begin(); iter!=vec_children.end(); iter++)
    {
        delete (*iter);
    }
    vec_children.clear();
    delete word;
}

void PDerivHistoric::append_word_xml(QDomDocument& _doc, QDomElement& _root_deriv_num)
{
    QDomElement dom_elt_word                = _doc.createElement("word");
    QDomElement dom_elt_word_txt            = _doc.createElement("text");
    QDomElement dom_elt_idx_into_gen        = _doc.createElement("idxIntoGen");
    QDomElement dom_elt_deriv_num           = _doc.createElement("derivNum");
    QDomElement dom_elt_idx_into_deriv      = _doc.createElement("idxIntoDeriv");
    QDomElement dom_elt_rule_gen_num        = _doc.createElement("ruleGenNum");
    QDomElement dom_elt_idx_within_rule     = _doc.createElement("idxWithinRule");
    QDomElement dom_elt_stochastic_param    = _doc.createElement("stochParam");
    QDomElement* dom_elt_word_params        = new QDomElement[word->getVecNumeric().size()];
    if (!word->getVecNumeric().empty())
    {
        for (unsigned int i=0; i<word->getVecNumeric().size(); i++)
            dom_elt_word_params[i] = _doc.createElement("params");
    }

    char pstr_idx_into_gen[MAX_SCALAR_STR_LENGTH];
    char pstr_deriv_num[MAX_SCALAR_STR_LENGTH];
    char pstr_idx_into_deriv[MAX_SCALAR_STR_LENGTH];
    char pstr_rule_gen_num[MAX_SCALAR_STR_LENGTH];
    char pstr_idx_within_rule[MAX_SCALAR_STR_LENGTH];
    char pstr_stochastic_param[MAX_SCALAR_STR_LENGTH];
    //char pstr_pstr_params[word->getVecNumeric().size()][MAX_SCALAR_STR_LENGTH];
    char** pstr_pstr_params = new char*[word->getVecNumeric().size()];
    for (unsigned int i=0; i<word->getVecNumeric().size(); i++)
    {
        pstr_pstr_params[i] = new char[MAX_SCALAR_STR_LENGTH];
    }

    sprintf(pstr_idx_into_gen,      "%d", idx_into_gen);
    sprintf(pstr_deriv_num,         "%d", deriv_num);
    sprintf(pstr_idx_into_deriv,    "%d", idx_into_deriv);
    sprintf(pstr_rule_gen_num,      "%d", rule_gen_num);
    sprintf(pstr_idx_within_rule,   "%d", idx_within_rule);
    sprintf(pstr_stochastic_param,  "%f", stochastic_param);
    for (unsigned int i=0; i<word->getVecNumeric().size(); i++)
    {
        PNumericValue* numval = dynamic_cast<PNumericValue*>(word->getVecNumeric().at(i));
        sprintf(pstr_pstr_params[i], "%f", numval->getData());
    }

    QDomText txt_word               = _doc.createTextNode(QString(word->getText().c_str()));
    QDomText txt_idx_into_gen       = _doc.createTextNode(pstr_idx_into_gen);
    QDomText txt_deriv_num          = _doc.createTextNode(pstr_deriv_num);
    QDomText txt_idx_into_deriv     = _doc.createTextNode(pstr_idx_into_deriv);
    QDomText txt_rule_gen_num       = _doc.createTextNode(pstr_rule_gen_num);
    QDomText txt_idx_within_rule    = _doc.createTextNode(pstr_idx_within_rule);
    QDomText txt_stochastic_param   = _doc.createTextNode(pstr_stochastic_param);
    QDomText* txt_params            = new QDomText[word->getVecNumeric().size()];
    if (!word->getVecNumeric().empty())
    {
        for (unsigned int i=0; i<word->getVecNumeric().size(); i++)
            txt_params[i] = _doc.createTextNode(pstr_pstr_params[i]);
    }

    _root_deriv_num.appendChild(dom_elt_word);
    dom_elt_word.appendChild(dom_elt_word_txt);
    dom_elt_word.appendChild(dom_elt_idx_into_gen);
    dom_elt_word.appendChild(dom_elt_deriv_num);
    dom_elt_word.appendChild(dom_elt_idx_into_deriv);
    dom_elt_word.appendChild(dom_elt_rule_gen_num);
    dom_elt_word.appendChild(dom_elt_idx_within_rule);
    dom_elt_word.appendChild(dom_elt_stochastic_param);
    if (!word->getVecNumeric().empty())
    {
        for (unsigned int i=0; i<word->getVecNumeric().size(); i++)
        {
            dom_elt_word.appendChild(dom_elt_word_params[i]);
        }
    }

    dom_elt_word_txt.appendChild(txt_word);
    dom_elt_idx_into_gen.appendChild(txt_idx_into_gen);
    dom_elt_deriv_num.appendChild(txt_deriv_num);
    dom_elt_idx_into_deriv.appendChild(txt_idx_into_deriv);
    dom_elt_rule_gen_num.appendChild(txt_rule_gen_num);
    dom_elt_idx_within_rule.appendChild(txt_idx_within_rule);
    dom_elt_stochastic_param.appendChild(txt_stochastic_param);
    if (!word->getVecNumeric().empty())
    {
        for (unsigned int i=0; i<word->getVecNumeric().size(); i++)
        {
            dom_elt_word_params[i].appendChild(txt_params[i]);
        }
    }
    delete[] dom_elt_word_params;
    delete[] txt_params;
    for (unsigned int i=0; i<word->getVecNumeric().size(); i++)
    {
        delete[] pstr_pstr_params[i];
    }
    delete[] pstr_pstr_params;
}

void PDerivHistoric::add_match_idx_dn(int _deriv_num, int _idx_into_deriv)
{
    match_idx_dn mid_tmp;
    mid_tmp.deriv_num       = _deriv_num;
    mid_tmp.idx_into_deriv  = _idx_into_deriv;
    vec_match_idx_dn.push_back(mid_tmp);

    deriv_num       = _deriv_num;
    idx_into_deriv  = _idx_into_deriv;
}

std::vector<PDerivHistoric*> PDerivHistoric::leaves()
{
    std::vector<PDerivHistoric*> children_leaves;
    std::vector<PDerivHistoric*> value;
    std::vector<PDerivHistoric*>::iterator iter;
    PDerivHistoric* tmp;

    if (vec_children.empty())
        value.push_back(this);
    else
    {
        for (iter=vec_children.begin(); iter!=vec_children.end(); iter++)
        {
            tmp = *iter;
            children_leaves = tmp->leaves();
            value.insert(value.end(), children_leaves.begin(), children_leaves.end());
        }
    }
    return value;
}

// Choose between elements of _vecdh, which ones of them are leaves
std::vector<PDerivHistoric*> PDerivHistoric::leaves(const std::vector<PDerivHistoric*> _vecdh)
{
    std::vector<PDerivHistoric*> children_leaves, vec_leaves_tmp;
    std::vector<PDerivHistoric*> value;
    std::vector<PDerivHistoric*>::iterator iter;
    PDerivHistoric* tmp;

    if (vec_children.empty())
        vec_leaves_tmp.push_back(this);
    else
    {
        for (iter=vec_children.begin(); iter!=vec_children.end(); iter++)
        {
            tmp = *iter;
            children_leaves = tmp->leaves();
            vec_leaves_tmp.insert(vec_leaves_tmp.end(), children_leaves.begin(), children_leaves.end());
        }
    }
    for (std::vector<PDerivHistoric*>::const_iterator jter =_vecdh.begin(); jter!=_vecdh.end(); jter++)
    {
        if (std::find(vec_leaves_tmp.begin(), vec_leaves_tmp.end(), *jter) != vec_leaves_tmp.end())
            value.push_back(*jter);
    }
    return value;
}

void PDerivHistoric::del_child(PDerivHistoric* _dh_child)
{
    std::vector<PDerivHistoric*>::iterator iter;

    for (iter=vec_children.begin(); iter!=vec_children.end(); iter++)
    {
        if (*iter == _dh_child)
        {
            _dh_child->setParent(0);
            vec_children.erase(iter);
            break;
        }
    }
}

void PDerivHistoric::del_child()
{
    for (std::vector<PDerivHistoric*>::iterator iter=vec_children.begin(); iter!=vec_children.end(); iter++)
        delete (*iter);
    vec_children.clear();
}

bool PDerivHistoric::is_tied(std::string& _var_name)
{
    for (std::vector<PNumeric*>::iterator iter=vec_numeric.begin(); iter!=vec_numeric.end(); iter++)
    {
        PNumeric* num=*iter;
        if (num->is_tied(_var_name))
            return true;
    }
    return false;
}

// Actually _new_value is useless but it is left as a part of the argument for the function consistency
// _const_name is the variable that needed to be checked if the instance is dependant
// This function needed some refactoring due to the fact that it has too much task for his name
// The method is called for computing the values inside PWord* word which is a property of PDerivHistoric
void PDerivHistoric::update_value(std::string& _const_name, float _new_value, std::vector<std::string>& _vec_var_name, PPreproc* _preproc)
{
    PNumericValue* pnv;
    PNumeric* pnum;
    std::string* s;
    float* f;
    unsigned int i;
    unsigned int full_params_size = _preproc->getVecDefine().size() + _vec_var_name.size();

    if (is_tied(_const_name))
    {
        s = new string[full_params_size];
        f = new float[full_params_size];

        assert(vec_params.size()==_vec_var_name.size());
        for (i=0; i<_vec_var_name.size(); i++)
        {
            s[i] = _vec_var_name[i];
            f[i] = vec_params[i];
        }
        for (i=0; i<_preproc->getVecDefine().size(); i++)
        {
            s[i+_vec_var_name.size()] = _preproc->getVecDefine()[i]->getConstantName();
            f[i+_vec_var_name.size()] = _preproc->getVecDefine()[i]->getConstantValue();
        }
        assert(vec_numeric.size()==word->getVecNumeric().size());
        i=0;
        for (std::vector<PNumeric*>::iterator iter=vec_numeric.begin(); iter!=vec_numeric.end(); iter++, i++)
        {
            PNumeric* tmp = *iter;
            pnum = word->getVecNumeric()[i];
            pnv = dynamic_cast<PNumericValue*>(pnum);
            pnv->setData(tmp->eval(s, f, full_params_size));
        }
        delete[] s;
        delete[] f;
    }
}

void PDerivHistoric::update_value(std::string& _const_name, float _new_value, std::vector<std::string>& _vec_var_name)
{
    PNumericValue* pnv;
    PNumeric* pnum;
    std::string* s;
    float* f;
    unsigned int i;
    unsigned int full_params_size = preproc->getVecDefine().size() + _vec_var_name.size();

    if (is_tied(_const_name))
    {
        s = new string[full_params_size];
        f = new float[full_params_size];

        assert(vec_params.size()==_vec_var_name.size());
        for (i=0; i<_vec_var_name.size(); i++)
        {
            s[i] = _vec_var_name[i];
            f[i] = vec_params[i];
        }
        for (i=0; i<preproc->getVecDefine().size(); i++)
        {
            s[i+_vec_var_name.size()] = preproc->getVecDefine()[i]->getConstantName();
            f[i+_vec_var_name.size()] = preproc->getVecDefine()[i]->getConstantValue();
        }
        assert(vec_numeric.size()==word->getVecNumeric().size());
        i=0;
        for (std::vector<PNumeric*>::iterator iter=vec_numeric.begin(); iter!=vec_numeric.end(); iter++, i++)
        {
            PNumeric* tmp = *iter;
            pnum = word->getVecNumeric()[i];
            pnv = dynamic_cast<PNumericValue*>(pnum);
            pnv->setData(tmp->eval(s, f, full_params_size));
        }
        delete[] s;
        delete[] f;
    }
}

float PDerivHistoric::request_const_value(std::string _name) const
{
    for (unsigned int i=0; i<preproc->getVecDefine().size(); i++)
    {
        if (!preproc->getVecDefine()[i]->getConstantName().compare(_name))
            return preproc->getVecDefine()[i]->getConstantValue();
    }
    return 0;
}

PDerivHistoric* PDerivHistoric::getRoot(unsigned int _stages)
{
    PDerivHistoric* value=this;
    for (unsigned int i=0; i<_stages; i++)
    {
        value = value->getParent();
        if (!value) break;
    }
    return value;
}

std::vector<PDerivHistoric*> PDerivHistoric::getRoots()
{
    std::vector<PDerivHistoric*> value;
    PDerivHistoric* root_tmp=this;
    while (1)
    {
        root_tmp = root_tmp->getParent();
        if (!root_tmp) break;
        value.push_back(root_tmp);
    }
    return value;
}

std::vector<PDerivHistoric*> PDerivHistoric::getRoots(unsigned int _stages)
{
    std::vector<PDerivHistoric*> value;
    PDerivHistoric* root_tmp=this;

    for (unsigned int i=0; i<_stages; i++)
    {
        root_tmp = root_tmp->getParent();
        if (!root_tmp) break;
        value.push_back(root_tmp);
    }
    return value;
}

bool PDerivHistoric::is_ramification(PDerivHistoric* _pdh)
{
    if (std::find(vec_children.begin(), vec_children.end(), _pdh) != vec_children.end())
        return true;
    else
        for (std::vector<PDerivHistoric*>::iterator iter = vec_children.begin(); iter != vec_children.end(); iter++)
        {
            if (is_ramification(*iter))
                return true;
        }
    return false;
}

bool PDerivHistoric::is_root(PDerivHistoric* _pdh)
{
    bool value = false;
    std::vector<PDerivHistoric*> vec_roots = getRoots();
    if (std::find(vec_roots.begin(), vec_roots.end(), _pdh) != vec_roots.end())
        value = true;
    return value;
}

void PDerivHistoric::update_leaves_const(std::string& _const_name, float _new_value, std::vector<PRuleParametric*>& _vec_rules)
{
    std::vector<std::string> vec_var_name_tmp;
    if (_vec_rules.empty()) return;
    if (getVecChildren().empty())
    {
        if ((rule_gen_num>=0)&&((unsigned)rule_gen_num<_vec_rules.size()))
            vec_var_name_tmp = _vec_rules[rule_gen_num]->var_to_arraystr();
        update_value(_const_name, _new_value, vec_var_name_tmp);
    }
    else
    {
        for (unsigned int i=0; i<getVecChildren().size(); i++)
        {
            int rule_generator = getVecChildren()[i]->getRuleGenNum();
            if ((rule_generator>=0)&&((unsigned)rule_generator<_vec_rules.size()))
                vec_var_name_tmp = _vec_rules[rule_generator]->var_to_arraystr();
            getVecChildren()[i]->getPreproc()->copy_const(preproc);
            getVecChildren()[i]->update_leaves_const(_const_name, _new_value, _vec_rules);
            //getVecChildren()[i]->update_value(_const_name, _new_value, vec_var_name_tmp);
        }
    }
}

PDerivHistoric* PDerivHistoric::clone(PDerivHistoric* _pdh)
{
    PDerivHistoric *value=0, *pdh_tmp;
    std::vector<PDerivHistoric*> vec_kids;
    if (_pdh)
    {
        deriv_num       = _pdh->getDerivNum();
        idx_into_deriv  = _pdh->getIdxIntoDeriv();
        idx_into_gen    = _pdh->getIdxIntoGen();
        idx_within_rule = _pdh->getIdxWithinRule();
        rule_gen_num    = _pdh->getRuleGenNum();
        stochastic_param= _pdh->getStochasticParam();
        if (_pdh->getWord())
            word = new PWord(*_pdh->getWord());
        vec_kids = _pdh->getVecChildren();
        for (auto kid:vec_kids)
        {
            pdh_tmp = clone(kid);
            pdh_tmp->setParent(this);
            add_child(pdh_tmp);
        }
    }
    return value;
}

unsigned int PDerivHistoric::dist_to_leaf(unsigned int _offset)
{
    unsigned int value=_offset;

    if (!vec_children.empty())
        ++value;
    for (std::vector<PDerivHistoric*>::iterator iter = vec_children.begin(); iter != vec_children.end(); iter++)
    {
        value = PROCEDURAL_MAX(value,(*iter)->dist_to_leaf(_offset+1));
    }
    return value;
}

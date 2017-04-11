#include "grammar/ppreproc.h"
#include "grammar/pword.h"
#include "plsystemparametric.h"
#include "pnumericvalue.h"
#include "pruleparametric.h"
#include "ptextparser.h"
#include "procedural.h"
#include "assert.h"
#include "gettimeofday.h"

#include <fstream>

using namespace std;

PLSystemParametric::PLSystemParametric():PLSystem()
{
    dbg_previous_text = text;
}

PLSystemParametric::PLSystemParametric(const PLSystemParametric& _lsys, bool _start_derive):PLSystem(_lsys.getText())
{
    string prods;
    axiom = new PAxiom(*_lsys.getAxiom());
    text = axiom->getText();
    angle = _lsys.getAngle();

    *alphabet = *_lsys.getAlphabet();
    *preproc = *_lsys.getPreproc();
    unsigned int i=0;
    while (i<_lsys.getVecRules().size())
    {
        prods+=_lsys.getVecRules().at(i)->getText();
        if (++i<_lsys.getVecRules().size()) prods+="\n";
    }
    load(prods);
    target_deriv_num = _lsys.getTargetDerivNum();

    if (_start_derive)
    {
        for (int i=0; i<target_deriv_num; i++)
        {
            derive();
        }
    }
}

PLSystemParametric::~PLSystemParametric()
{
    std::vector<PVariable*>         vec_var;
    std::vector<PRuleParametric*>   vec_rules;
    for (std::vector<PVariable*>::iterator iter = vec_var.begin(); iter!=vec_var.end(); iter++)
    {
        delete (*iter);
    }
    clear_rules();
}

int PLSystemParametric::load(string _productions)
{
    int value = 0;
    size_t start, end;
    string linebuffer;
    PRuleParametric* rulebuffer;
    PTextParser textparser(_productions);

    end = start = 0;
    textparser.clean_final_endl(_productions);

    clear_rules();
    while (end != _productions.size())
    {
        if (_productions[end] == '\n')
        {
            linebuffer = _productions.substr(start, end-start);
            if (linebuffer!="\n" && linebuffer!="")
            {
                rulebuffer = new PRuleParametric(linebuffer, alphabet, preproc);
                vec_rules.push_back(rulebuffer);
            }
            start = end + 1;
            value++;
        }
        else if (end == _productions.size()-1)
        {
            linebuffer = _productions.substr(start, end-start+1);

            if (linebuffer.empty()) break;

            rulebuffer = new PRuleParametric(linebuffer, alphabet, preproc);
            vec_rules.push_back(rulebuffer);
            value++;
        }
        end++;
    }

    return value;
}

void PLSystemParametric::init()
{
    for (std::vector<PVariable*>::iterator iter=vec_var.begin(); iter!=vec_var.end(); iter++)
    {
        delete (*iter);
    }
    vec_var.clear();
    for (std::vector<PRuleParametric*>::iterator iter=vec_rules.begin(); iter!=vec_rules.end(); iter++)
    {
        delete (*iter);
    }
    vec_rules.clear();
    for (std::vector< std::vector<PDerivHistoric*> >::iterator iter = vec_vec_dh_v2.begin(); iter!=vec_vec_dh_v2.end(); iter++)
    {
        for (std::vector<PDerivHistoric*>::iterator jter = (*iter).begin(); jter != (*iter).end(); jter++)
        {
            delete (*jter);
        }
    }
    vec_vec_dh_v2.clear();
    text.clear();
    raw.clear();
    delete axiom;
    if (preproc)
    {
        delete preproc;
        preproc = new PPreproc();
    }
    axiom = new PAxiom();
}

void PLSystemParametric::init_dh()
{
    PTextParser textparser;
    size_t alphabet_size;
    PDerivHistoric* dh_tmp;
    PWord* word_tmp;
    vector<PDerivHistoric*> vec_dh_tmp;
    vector<float> vec_params_tmp;
    //std::map<unsigned int, std::vector<float> > map_params_tmp;
    unsigned int i, char_size;
    string str_word;

    for (std::vector< std::vector<PDerivHistoric*> >::iterator iter = vec_vec_dh_v2.begin(); iter!=vec_vec_dh_v2.end(); iter++)
    {
        for (std::vector<PDerivHistoric*>::iterator jter = (*iter).begin(); jter != (*iter).end(); jter++)
        {
            delete (*jter);
        }
    }

    vec_vec_dh_v2.clear();
    map_params.clear();
    map_offset.clear();
    bm_dh.clear();
    i=0;
    while (i<text.size())
    {
        //k = i; // We will lost the value of i in the next line
        vec_params_tmp.clear();
        str_word = pick_char_at(i, char_size, vec_params_tmp);
        alphabet_size = alphabet->search(text, i);
        if ((alphabet_size!=0)||(alphabet->is_shifting(text[i]))||(alphabet->is_constant(text[i])))
        {
            if (!vec_params_tmp.empty())
            {
                map_params.insert(std::pair<unsigned int, std::vector<float> >(i, vec_params_tmp));
                map_offset.insert(std::pair<unsigned int, unsigned int>(i, char_size));
            }
            if (alphabet_size==0)
                alphabet_size=1;
            word_tmp = new PWord(text.substr(i, alphabet_size));
            for (unsigned j=0; j<vec_params_tmp.size(); j++)
                word_tmp->add(vec_params_tmp.at(j));
            dh_tmp = new PDerivHistoric(word_tmp, preproc, vec_params_tmp, 0, 0, i, 0, -1, 1);
            vec_dh_tmp.push_back(dh_tmp);
            bm_dh.insert(dh_match(i, dh_tmp));
        }
        else
        {
            char_size = 1;
            word_tmp = NULL;
            switch(text[i])
            {
            case '[':case ']':case '|':case '$':
            case '+':case '-':case '&':case '^':case '\\':case '/':
                if (!vec_params_tmp.empty())
                {
                    map_params.insert(std::pair<unsigned int, std::vector<float> >(i, vec_params_tmp));
                    map_offset.insert(std::pair<unsigned int, unsigned int>(i, char_size));
                }
                word_tmp = new PWord(getText()[i]);
                dh_tmp = new PDerivHistoric(word_tmp, preproc, vec_params_tmp, 0, 0, i, 0, 0, 1);
                vec_dh_tmp.push_back(dh_tmp);
                bm_dh.insert(dh_match(i, dh_tmp));
            }
        }
        i+=char_size;
    }
    vec_vec_dh_v2.push_back(vec_dh_tmp);
    map_iter=map_params.begin();
    map_offset_iter = map_offset.begin();
}

vector<float> PLSystemParametric::pick_params(unsigned int _index)
{
    vector<float> vec_value;
    PTextParser textparser;
    size_t alphabet_size;

    alphabet_size = alphabet->search(text, _index);
    if (_index<getText().size()-1)
    {
        unsigned int start, end;
        if (alphabet_size!=0)
        {
            _index += alphabet_size;
            start = _index;
        }
        else start = ++_index;
        end = start + 1;
        if (text[start]=='(')
        {
            vector<unsigned int> vec_idx;
            vec_idx.push_back(start);
            char c;
            do
            {
                c=getText()[end];
                if (c == ',')
                    vec_idx.push_back(end);
                ++end;
            }
            while (c!=')');
            vec_idx.push_back(--end);


            for (unsigned int i=1; i<vec_idx.size(); i++)
            {
                float x;
                string str_buffer;

                start = vec_idx.at(i-1);
                end = vec_idx.at(i);
                start++;
                end--;
                str_buffer = getText().substr(start, end-start+1);
                x = textparser.stdstr_to_float(str_buffer);
                vec_value.push_back(x);
            }
        }
    }
    return vec_value;
}

vector<float> PLSystemParametric::pick_params(std::string& _src)
{
    vector<float> vec_value;
    vector<string> vec_str;
    PTextParser textparser;

    vec_str = textparser.arrange_sequence(_src);
    for (unsigned int i=0; i<vec_str.size(); i++)
    {
        float buffer = textparser.stdstr_to_float(vec_str.at(i));
        vec_value.push_back(buffer);
    }
    return vec_value;
}

vector<float> PLSystemParametric::pick_params(std::string& _src, unsigned int _index)
{
    vector<float> vec_value;
    PTextParser textparser;
    size_t alphabet_size;

    alphabet_size = alphabet->search(text, _index);

    if (_index<_src.size()-1)
    {
        unsigned int start, end;
        if (alphabet_size!=0)
        {
            _index += alphabet_size;
            start = _index;
        }
        else start = ++_index;

        end = start + 1;
        if (_src[start]=='(')
        {
            vector<unsigned int> vec_idx;
            vec_idx.push_back(start);
            char c;
            do
            {
                c=_src[end];
                if (c == ',')
                    vec_idx.push_back(end);
                ++end;
            }
            while (c!=')');
            vec_idx.push_back(--end);


            for (unsigned int i=1; i<vec_idx.size(); i++)
            {
                float x;
                string str_buffer;

                start = vec_idx.at(i-1);
                end = vec_idx.at(i);
                start++;
                end--;
                str_buffer = _src.substr(start, end-start+1);
                x = textparser.stdstr_to_float(str_buffer);
                vec_value.push_back(x);
            }
        }
    }
    return vec_value;
}

vector<float> PLSystemParametric::pick_params(std::string& _src, unsigned int _index, unsigned int& _offset_out)
{
    vector<float> vec_value;
    PTextParser textparser;
    size_t alphabet_size;

    alphabet_size = alphabet->search(_src, _index);
    _offset_out = (alphabet_size==0)?1:alphabet_size;
    if (_index<_src.size()-1)
    {
        unsigned int start, end;
        //_index += _offset_out;
        start = _index+_offset_out;
        end = start + 1;
        if (_src[start]=='(')
        {
            vector<unsigned int> vec_idx;
            vec_idx.push_back(start);
            char c;
            do
            {
                c=_src[end];
                if (c == ',')
                    vec_idx.push_back(end);
                ++end;
            }
            while (c!=')');
            _offset_out = end - _index;
            vec_idx.push_back(--end);


            for (unsigned int i=1; i<vec_idx.size(); i++)
            {
                float x;
                string str_buffer;

                start = vec_idx.at(i-1);
                end = vec_idx.at(i);
                start++;
                end--;
                str_buffer = _src.substr(start, end-start+1);
                x = textparser.stdstr_to_float(str_buffer);
                vec_value.push_back(x);
            }
        }
    }
    return vec_value;
}

std::vector<float> PLSystemParametric::current_map_params()
{
    std::vector<float> value;
    if (map_iter!=map_params.end())
    {
        value = (*map_iter).second;
        map_iter++;
    }
    return value;
}

// The role of the next function is to find at the fastest
// possible way where the next character should be taken
// The magic in this function is that whenever you are asking for
// an invalid index, you should not overwrite the value of the iterator
// And everytime you are looking for an invalid index, we should proceed in
// a way that the iterator value should remain intact
// The next function is critical to the application
// The instructions should be kept as efficient as possible
unsigned int PLSystemParametric::current_offset(unsigned int _index_to_confirm)
{
    unsigned int value=1;
     std::map<unsigned int, unsigned int>::iterator map_offset_iter_tmp = map_offset_iter;
    if (map_offset_iter!=map_offset.end())
    {
        if (_index_to_confirm==(*map_offset_iter).first)
        {
            value = (*map_offset_iter).second;
            map_offset_iter++;
        }
        else
        {
            while ((*map_offset_iter).first!=_index_to_confirm)
            {
                if (map_offset_iter==map_offset.end())
                    break;
                map_offset_iter++;
            }
            //map_iter = map_params.find(_index_to_confirm);
            if (map_offset_iter!=map_offset.end())
            {
                value = (*map_offset_iter).second;
                map_offset_iter++;
            }
            else
            {
                // This is where it is all about
                // If you don't find occurence
                // You keep the iterator at its old place
                map_offset_iter = map_offset_iter_tmp;
            }
        }
    }

    return value;
}

// Stay assured that if you call this function, you have a valid index, which is absolutely inside the map
std::vector<float> PLSystemParametric::current_map_params(unsigned int _index_to_confirm)
{
    std::vector<float> value;
    if (map_iter!=map_params.end())
    {
        if (_index_to_confirm==(*map_iter).first)
        {
            value = (*map_iter).second;
            map_iter++;
        }
        else
        {
            while ((*map_iter).first!=_index_to_confirm)
            {
                if (map_iter==map_params.end())
                    break;
                map_iter++;
            }
            //map_iter = map_params.find(_index_to_confirm);
            if (map_iter!=map_params.end())
            {
                value = (*map_iter).second;
                map_iter++;
            }
        }
    }

    return value;
}

// Here just pick word WITHOUT the params strings
string PLSystemParametric::pick_char_at(unsigned int& _index, unsigned int& _char_size)
{
    string value("");
    _char_size = alphabet->search(text, _index);

    if (_char_size!=0)
    {
        value = text.substr(_index, _char_size);
    }
    else
    {
        _char_size = 1;
        value = text.substr(_index,_char_size);
    }
    if (_index+_char_size<text.size())
        if (text[_index+_char_size]=='(')
        {
            unsigned int idx_closer = PTextParser::find_bracket_closer(getText(), _index+_char_size);
            _char_size = idx_closer - _index + 1;
        }

    return value;
}

// Here just pick word WITHOUT the params strings
string PLSystemParametric::pick_char_at(unsigned int& _index, unsigned int& _char_size, vector<float>& _vec_params)
{
    string value;

    _char_size = alphabet->search(text, _index);
    if (_char_size!=0)
        value = text.substr(_index,_char_size);
    else value = text.substr(_index,1);

    _vec_params.clear();
    if (_index<text.length())
    {
        if (_char_size!=0)
        {
            if (_index+_char_size<text.size())
            {
                if (text[_index+_char_size]=='(')
                {
                    unsigned int i_closer = PTextParser::find_bracket_closer(getText(), _index+_char_size);
                    string buffer = text.substr(_index+_char_size+1, i_closer-_index-_char_size-1);
                    _vec_params = pick_params(buffer);
                    _char_size = i_closer - _index + 1;
                }
            }
        }
        else
        {
            _char_size = 1;
            if (_index+1<text.size())
            {
                if (text[_index+1]=='(')
                {
                    unsigned int i_closer = PTextParser::find_bracket_closer(getText(), _index+1);
                    string buffer = text.substr(_index+2, i_closer-_index-2);
                    _vec_params = pick_params(buffer);
                    _char_size = i_closer - _index + 1;
                }
            }
        }
    }
    return value;
}

// This is the function which apply the production along the current string
string PLSystemParametric::apply_prod(string& _src, unsigned int& _index, unsigned int& _rule_number, vector<float> _vec_params,
                                      boost::bimap<unsigned int, unsigned int>& _bm_succidx_prodidx)
{
    string value;
    unsigned int i;
    unsigned int params_size, aux_size;
    float* params;
    float* aux_params;
    string* pstr_aux_params;
    PTextParser textparser;
    vector<int> vec_indices;
    vector<double> vec_stoch_params;
    vector<float> vec_auxiliary_params;
    vector<string> vec_str_auxiliary_params;

    value = _src;

    // The loop below concerns the stochastic L-system
    // All the candidate rules are stocked in vec_indices
    // where the indices are the rules ones
    for (i=0; i<vec_rules.size(); i++)
    {
        if (    vec_rules.at(i)->is_candidate(  _src, _vec_params, text, _index, (preproc)?preproc->getIgnore():0,
                                                vec_str_auxiliary_params, vec_auxiliary_params)
            )
        {
            vec_indices.push_back(i);
            vec_stoch_params.push_back(vec_rules.at(i)->getStochasticParam());
        }
    }

    // First if is for at least one rule found
    if (!vec_indices.empty())
    {
        value.clear();
        params_size = _vec_params.size();
        params = new float[params_size];
        for (unsigned int i=0; i<_vec_params.size(); i++)
            params[i] = _vec_params.at(i);

        aux_size = vec_str_auxiliary_params.size();
        pstr_aux_params = new string[aux_size];
        aux_params = new float[aux_size];
        for (unsigned int i=0; i<aux_size; i++)
        {
            aux_params[i] = vec_auxiliary_params.at(i);
            pstr_aux_params[i] = vec_str_auxiliary_params.at(i);
        }
        _rule_number = vec_indices.at(random(vec_stoch_params));
        //The symbol '*' is not interpreted correctly
        // The string generated is computed here:
        value = vec_rules.at(_rule_number)->compute(params, params_size, pstr_aux_params, aux_params, aux_size, _bm_succidx_prodidx);
        delete [] aux_params;
        delete [] pstr_aux_params;
        delete [] params;
    }
    // If no rules apply
    else if (_index+_src.size()<text.size())
    {
        if (text[_index+_src.size()]=='(')
        {
            string buffer;
            unsigned int idx_closer = textparser.find_bracket_closer(text, _index+_src.size());
            buffer = text.substr(_index+_src.size(), idx_closer-_index-_src.size()+1);
            value+=buffer;
        }
    }
    return value;
}

std::string PLSystemParametric::derive(unsigned int _index)
{
    unsigned int char_size, rule_applied_idx;
    unsigned int offset_tmp, alphabet_size;
    string value;
    vector<float> vec_params, vec_params_prod;
    PTextParser textparser;
    vector<PDerivHistoric*> vec_leaves;
    PDerivHistoric *deriv_hist_root_tmp, *deriv_hist_tmp;
    PWord* word;
    boost::bimap<unsigned int, unsigned int> bm_succidx_prodidx;

    //dbg_string();
    //dbg_map_params();

    if (_index<text.length())
    {
        string alphabet_buffer, prod_buffer, alphabet_set_buffer;
        alphabet_buffer = pick_char_at(_index, char_size, vec_params);
        alphabet_buffer = textparser.clean_final_space(alphabet_buffer);
        prod_buffer     = apply_prod(alphabet_buffer, _index, rule_applied_idx, vec_params, bm_succidx_prodidx);
        alphabet_set_buffer = alphabet_buffer;
        if (_index+alphabet_buffer.size()<text.length())
        {
            if (text[_index+alphabet_buffer.size()]=='(')
            {
                unsigned int idx_closer = textparser.find_bracket_closer(text, _index+alphabet_buffer.size());
                alphabet_set_buffer += text.substr(_index+alphabet_buffer.size(), idx_closer-_index-alphabet_buffer.size()+1);
            }
        }

        // Belows are all about DERIVATION HISTORIC
        try
        {
            deriv_hist_root_tmp = bm_dh.left.at(_index);
        }
        catch (...)
        {
            cout << endl << "Derivation number: " << num_derivation << endl
                 << "Index i: " << _index << endl
                 << "Bimap size: " << bm_dh.size() << endl;
            dbg_bimap(bm_dh);
            return string("");
        }

        if (prod_buffer.compare(alphabet_set_buffer)!=0)
        {
            drop_map(_index);
            for (unsigned int j=0; j<prod_buffer.size(); j+=offset_tmp)
            {
                alphabet_size = alphabet->search(prod_buffer, j);
                word = (alphabet_size==0)?new PWord(prod_buffer[j]):new PWord(prod_buffer.substr(j, alphabet_size));
                unsigned int idx_into_deriv_buf = j+value.size();
                vec_params_prod.clear();
                vec_params_prod = pick_params(prod_buffer, j, offset_tmp);
                //map_params.insert(std::pair(i))
                if ((alphabet_size!=0)||(alphabet->is_constant(prod_buffer[j]))||(alphabet->is_shifting(prod_buffer[j])))
                {     
                    for (unsigned int k=0; k<vec_params_prod.size(); k++)
                    {
                        word->add(vec_params_prod.at(k));
                    }
                    //The next instruction need further explanation
                    //It sets an offset from every character on the produced chain
                    //Need a precise debugging session

                    deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                            num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                            vec_rules.at(rule_applied_idx)->getStochasticParam());

                    insert_map(_index+j, offset_tmp, vec_params_prod);
                }
                else
                {
                    switch(prod_buffer[j])
                    {
                    //case '[':case ']':case '|':case '$':
                    case '+':case '-':case '&':case '^':case '\\':case '/':case'!':
                        for (unsigned int k=0; k<vec_params_prod.size(); k++)
                        {
                            word->add(vec_params_prod.at(k));
                        }
                        deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                                num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                                vec_rules.at(rule_applied_idx)->getStochasticParam());
                        insert_map(_index+j, offset_tmp, vec_params_prod);
                        break;
                    case '[':case ']':case '|':case '$':
                        deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                                num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                                vec_rules.at(rule_applied_idx)->getStochasticParam()); 
                        insert_map(_index+j, offset_tmp, vec_params_prod);
                        break;
                    }
                }
             }
        }
    }
    vec_leaves = historic_leaves();
    vec_vec_dh_v2.push_back(vec_leaves);
    str_from_historic();
    update_map_offset();

    //dbg_string();
    //dbg_map_params();

    return value;
}

std::string PLSystemParametric::derive(unsigned int _start, unsigned int _end)
{
    unsigned int i;
    unsigned int char_size, rule_applied_idx;
    unsigned int offset_tmp, alphabet_size;
    string alphabet_buffer, alphabet_set_buffer, prod_buffer;
    string value;
    PTextParser textparser;
    PDerivHistoric* deriv_hist_tmp, *deriv_hist_root_tmp;
    PWord* word;
    vector<PDerivHistoric*> vec_leaves;
    vector<float> vec_params, vec_params_prod;
    boost::bimap<unsigned int, unsigned int> bm_succidx_prodidx;
    assert (_end<text.length());

    i = _start;
    while (i<=_end)
    {
        alphabet_buffer = pick_char_at(i, char_size, vec_params);
        alphabet_buffer = textparser.clean_final_space(alphabet_buffer);
        prod_buffer     = apply_prod(alphabet_buffer, i, rule_applied_idx, vec_params, bm_succidx_prodidx);
        alphabet_set_buffer = alphabet_buffer;

        if (i+alphabet_buffer.size()<_end)
        {
            if (text[i+alphabet_buffer.size()]=='(')
            {
                unsigned int idx_closer = textparser.find_bracket_closer(text, i+alphabet_buffer.size());
                alphabet_set_buffer += text.substr(i+alphabet_buffer.size(), idx_closer-i-alphabet_buffer.size()+1);
            }
        }

        // Belows are all about DERIVATION HISTORIC
        // Debug bm_dh
        //dbg_bimap(bm_dh);
        // Belows are all about DERIVATION HISTORIC
        try
        {
            deriv_hist_root_tmp = bm_dh.left.at(i);
        }
        catch (...)
        {
            cout << endl << "Derivation number: " << num_derivation << endl
                 << "Index i: " << i << endl
                 << "Bimap size: " << bm_dh.size() << endl;
            //dbg_bimap(bm_dh);
            //return string("");
        }

        // Next block involves any alphabet that wasn't found in the early historic
        // Mostly because it was an initial character found in the axiom
        if (prod_buffer.compare(alphabet_set_buffer)!=0)
        {
            for (unsigned int j=0; j<prod_buffer.size(); j+=offset_tmp)
            {
                unsigned int idx_into_deriv_buf = j+value.size();
                alphabet_size = alphabet->search(prod_buffer, j);
                word = (alphabet_size==0)?new PWord(prod_buffer[j]):new PWord(prod_buffer.substr(j, alphabet_size));
                vec_params_prod.clear();
                vec_params_prod = pick_params(prod_buffer, j, offset_tmp);
                if ((alphabet_size!=0)||(alphabet->is_constant(prod_buffer[j]))||(alphabet->is_shifting(prod_buffer[j])))
                {
                    for (unsigned int k=0; k<vec_params_prod.size(); k++)
                    {
                        word->add(vec_params_prod.at(k));
                    }
                    //The next instruction need further explanation
                    //It sets an offset from every character on the produced chain
                    //Need a precise debugging session
                    deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                            num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                            vec_rules.at(rule_applied_idx)->getStochasticParam());
                    insert_map(i+j, offset_tmp, vec_params_prod);
                }
                else
                {
                    /*
                    // Debug:
                    std::cout<<std::endl<<"****========****"<<std::endl;
                    std::cout<<"Actual Production : "<<prod_buffer<<std::endl;
                    std::cout<<"Rule applied : "<<vec_rules.at(rule_applied_idx)->getText()<<std::endl;
                    std::cout<<"Bimap Successor-Production : "<<std::endl;
                    Procedural::boost_dbg(bm_succidx_prodidx);
                    */

                    switch(prod_buffer[j])
                    {
                    case '+':case '-':case '&':case '^':case '\\':case '/':case'!':
                        for (unsigned int k=0; k<vec_params_prod.size(); k++)
                        {
                            word->add(vec_params_prod.at(k));
                        }
                        deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                                num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                                vec_rules.at(rule_applied_idx)->getStochasticParam());
                        insert_map(i+j, offset_tmp, vec_params_prod);
                        break;
                    case '[':case ']':case '|':case '$':
                        deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                                num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                                vec_rules.at(rule_applied_idx)->getStochasticParam());
                        insert_map(i+j, offset_tmp, vec_params_prod);
                        break;
                    }
                }
                deriv_hist_tmp = 0;
            }
        }
        value += prod_buffer;
        i+=char_size;
    }
    vec_leaves = historic_leaves();
    vec_vec_dh_v2.push_back(vec_leaves);
    str_from_historic();
    update_map_offset();
    return value;
}

std::string PLSystemParametric::copie(PLSystemParametric* lsystem_c){

    boost_bimap_dh bm_tmp = lsystem_c->getBmDh(), bm_tmp2;
    std::map<unsigned int, std::vector<float> > map_params_tmp = lsystem_c->getMapParams();
    std::map<unsigned int, unsigned int> map_offset_tmp = lsystem_c->map_offset;
    std::vector< std::vector<PDerivHistoric*> > vec_dh_v2_tmp = lsystem_c->getVecVecDhV2(), vec_vec_dh_v22;
    std::vector<PDerivHistoric*> vec_pdh_root, vec_pdh_rootc, vec_pdh_copie, vec_dh_v_tmp, vec_dh_v_tmp2, vec_kids, vec_kids2;
    PDerivHistoric* pdh_copie, dh_copie;
    //int deriv;

    map_params.clear();
    map_params.insert(map_params_tmp.begin(), map_params_tmp.end());
    map_iter=map_params.begin();
    map_offset.clear();
    map_offset.insert(map_offset_tmp.begin(), map_offset_tmp.end());
    map_offset_iter = map_offset.begin();

    vec_vec_dh_v2.clear();
    bm_dh.clear();

    vec_pdh_rootc = lsystem_c->getVecVecDhV2().front();
    vec_dh_v_tmp.clear();
    for(std::vector<PDerivHistoric*>::iterator jter = vec_pdh_rootc.begin(); jter !=vec_pdh_rootc.end();jter++){

        PDerivHistoric* _parent = (*jter)->getParent();

        vec_dh_v_tmp.push_back(new PDerivHistoric( _parent, (*jter)->getWord(), (*jter)->getPreproc(), (*jter)->getVecNumeric(), (*jter)->getVecParams(),
                                                   (*jter)->getDerivNum(), (*jter)->getIdxIntoDeriv(), (*jter)->getIdxIntoGen(), (*jter)->getRuleGenNum(), (*jter)->getIdxWithinRule(),
                                                   (*jter)->getStochasticParam()));
        bm_dh.insert(dh_match((*jter)->getIdxIntoDeriv(),vec_dh_v_tmp.back()));
        vec_kids = (*jter)->getVecChildren();
        createcopie(vec_dh_v_tmp.back(),vec_kids);

    }

    vec_vec_dh_v2.insert(vec_vec_dh_v2.begin(),vec_dh_v_tmp);

    std::cout << std::endl << "Bitmap size:" << bm_tmp.size() << " VS " << bm_dh.size() << std::endl;

    //bm_dh.insert(bm_tmp.begin(), bm_tmp.end());



    num_derivation = lsystem_c->getNumDerivation();
    text = string(lsystem_c->getText());
    this->dbg_previous_text = text;

    return text;
}

void PLSystemParametric::createcopie(PDerivHistoric* _parent, std::vector<PDerivHistoric*> _pdh)
{
    std::vector<PDerivHistoric*> vec_kids, _vec_dh_v_tmp;

    if(!_pdh.empty())
    {

        for (auto kid: _pdh)
        {
                _vec_dh_v_tmp.push_back(new PDerivHistoric(_parent, kid->getWord(), kid->getPreproc(), kid->getVecNumeric(), kid->getVecParams(),
                                       kid->getDerivNum(), kid->getIdxIntoDeriv(), kid->getIdxIntoGen(), kid->getRuleGenNum(), kid->getIdxWithinRule(),
                                       kid->getStochasticParam()));
                bm_dh.insert(dh_match(kid->getIdxIntoDeriv(),_vec_dh_v_tmp.back()));
                vec_kids = kid->getVecChildren();
                createcopie(_vec_dh_v_tmp.back(),vec_kids);
        }
        vec_vec_dh_v2.push_back(_vec_dh_v_tmp);
    }

}

std::string PLSystemParametric::derive()
{
    unsigned int i;
    unsigned int rule_applied_idx;
    unsigned int char_size, alphabet_size;
    string value;
    string prod_buffer;
    PDerivHistoric* deriv_hist_tmp, *deriv_hist_root_tmp;
    PTextParser textparser(text);
    vector<unsigned int> vec_params_idx;
    vector<float> vec_params, vec_params_prod;
    vector<PDerivHistoric*> vec_dh_v2_tmp;
    boost_bimap_dh bm_tmp;
    boost::bimap<unsigned int, unsigned int> bm_succidx_prodidx;
    std::map<unsigned int, unsigned int> map_offset_tmp;
    std::map<unsigned int, std::vector<float> > map_params_tmp;
    struct timeval dbg_before, dbg_after;
    gettimeofday(&dbg_before, NULL);

    std::cout << std::endl << "====**====**====";
    std::cout << std::endl << "Before the derivation... :\t";
    std::cout << 0.000001*(dbg_before.tv_usec + dbg_before.tv_sec*1000000 - DBG_NOW);

    i = 0;
    while (i<text.length())
    {
        string alphabet_buffer, alphabet_set_buffer;
        deriv_hist_root_tmp = 0;
        vec_params.clear();

        vec_params = map_params[i];

        alphabet_buffer = pick_char_at(i, char_size);
        alphabet_buffer = textparser.clean_final_space(alphabet_buffer);
        prod_buffer     = apply_prod(alphabet_buffer, i, rule_applied_idx, vec_params, bm_succidx_prodidx);

        alphabet_set_buffer = alphabet_buffer;
        if (i+alphabet_buffer.size()<text.length())
        {
            if (text[i+alphabet_buffer.size()]=='(')
            {
                unsigned int idx_closer = textparser.find_bracket_closer(text, i+alphabet_buffer.size());
                alphabet_set_buffer += text.substr(i+alphabet_buffer.size(), idx_closer-i-alphabet_buffer.size()+1);
            }
        }

        // Belows are all about DERIVATION HISTORIC
        try
        {
            deriv_hist_root_tmp = bm_dh.left.at(i);
        }
        catch (...)
        {
            cout << endl << "Derivation number: " << num_derivation << endl
                 << "Index i: " << i << endl
                 << "Bimap size: " << bm_dh.size() << endl;
            dbg_bimap(bm_dh);
            return string("");
        }

        //std::cout << "Debug index i : " << i << std::endl;
        if (prod_buffer.compare(alphabet_set_buffer)==0)
        {
            if ((alphabet->search(prod_buffer,0)!=0)||(alphabet->is_constant(prod_buffer[0]))||(alphabet->is_shifting(prod_buffer[0])))
            {
                deriv_hist_root_tmp->add_match_idx_dn(num_derivation+1, value.size());
                vec_dh_v2_tmp.push_back(deriv_hist_root_tmp);
                bm_tmp.insert(dh_match(value.size(), deriv_hist_root_tmp));
                map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >(value.size(), vec_params));
                map_offset_tmp.insert(std::pair<unsigned int, unsigned int>(value.size(), prod_buffer.size()));
            }
            else
            {
                switch(prod_buffer[0])
                {
                case '[':case ']':case '|':case '$':
                case '+':case '-':case '&':case '^':case '\\':case '/':case'!':
                    deriv_hist_root_tmp->add_match_idx_dn(num_derivation+1, value.size());
                    vec_dh_v2_tmp.push_back(deriv_hist_root_tmp);
                    bm_tmp.insert(dh_match(value.size(), deriv_hist_root_tmp));
                    map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >(value.size(), vec_params));
                    map_offset_tmp.insert(std::pair<unsigned int, unsigned int>(value.size(), prod_buffer.size()));
                    break;
                }
            }
        }
        // Next block involves any alphabet that wasn't found in the early historic
        // Mostly because it was an initial character found in the axiom
        else
        {
            unsigned int offset_tmp;
            for (unsigned int j=0; j<prod_buffer.size(); j+=offset_tmp)
            {
                unsigned int idx_into_deriv_buf = j+value.size();
                PWord* word;

                offset_tmp = 1;
                alphabet_size = alphabet->search(prod_buffer,j);
                vec_params_prod.clear();
                if (alphabet_size!=0)
                {
                    std::string alphabet_str = prod_buffer.substr(j,alphabet_size);
                    word = new PWord(alphabet_str);
                    vec_params_prod = pick_params(prod_buffer, j, offset_tmp);
                    for (unsigned int k=0; k<vec_params_prod.size(); k++)
                    {
                        word->add(vec_params_prod.at(k));
                    }
                    //The next instruction need further explanation
                    //It sets an offset from every character on the produced chain
                    //Need a precise debugging session
                    deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                            num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                            vec_rules.at(rule_applied_idx)->getStochasticParam());
                    vec_dh_v2_tmp.push_back(deriv_hist_tmp);
                    bm_tmp.insert(dh_match(idx_into_deriv_buf, deriv_hist_tmp));
                    map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >(j+value.size(), vec_params_prod));
                    map_offset_tmp.insert(std::pair<unsigned int, unsigned int>(j+value.size(), offset_tmp));
                }
                // Note that actually the shifting is one character wide
                else if (alphabet->is_shifting(prod_buffer[j]))
                {
                    word = new PWord(prod_buffer[j]);
                    vec_params_prod = pick_params(prod_buffer, j, offset_tmp);
                    for (unsigned int k=0; k<vec_params_prod.size(); k++)
                    {
                        word->add(vec_params_prod.at(k));
                    }
                    //The next instruction need further explanation
                    //It sets an offset from every character on the produced chain
                    //Need a precise debugging session
                    deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                            num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                            vec_rules.at(rule_applied_idx)->getStochasticParam());
                    vec_dh_v2_tmp.push_back(deriv_hist_tmp);
                    bm_tmp.insert(dh_match(idx_into_deriv_buf, deriv_hist_tmp));
                    map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >(j+value.size(), vec_params_prod));
                    map_offset_tmp.insert(std::pair<unsigned int, unsigned int>(j+value.size(), offset_tmp));
                }
                else
                {
                    /*
                    // Debug:
                    std::cout<<std::endl<<"****========****"<<std::endl;
                    std::cout<<"Actual Production : "<<prod_buffer<<std::endl;
                    std::cout<<"Rule applied : "<<vec_rules.at(rule_applied_idx)->getText()<<std::endl;
                    std::cout<<"Bimap Successor-Production : "<<std::endl;
                    Procedural::boost_dbg(bm_succidx_prodidx);
                    */
                    word=0;
                    switch(prod_buffer[j])
                    {
                    case '+':case '-':case '&':case '^':case '\\':case '/':case'!':
                        word = new PWord(prod_buffer[j]);
                        vec_params_prod = pick_params(prod_buffer, j, offset_tmp);
                        for (unsigned int k=0; k<vec_params_prod.size(); k++)
                        {
                            word->add(vec_params_prod.at(k));
                        }
                        deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                                num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                                vec_rules.at(rule_applied_idx)->getStochasticParam());
                        vec_dh_v2_tmp.push_back(deriv_hist_tmp);
                        bm_tmp.insert(dh_match(idx_into_deriv_buf, deriv_hist_tmp));
                        map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >(j+value.size(), vec_params_prod));
                        map_offset_tmp.insert(std::pair<unsigned int, unsigned int>(j+value.size(), offset_tmp));
                        break;
                    case '[':case ']':case '|':case '$':
                        word = new PWord(prod_buffer[j]);
                        deriv_hist_tmp = new PDerivHistoric(    deriv_hist_root_tmp, word, preproc, vec_rules.at(rule_applied_idx)->request_formula(bm_succidx_prodidx.right.at(j)), vec_params,
                                                                num_derivation+1,  idx_into_deriv_buf, rule_applied_idx, bm_succidx_prodidx.right.at(j),
                                                                vec_rules.at(rule_applied_idx)->getStochasticParam());
                        vec_dh_v2_tmp.push_back(deriv_hist_tmp);
                        bm_tmp.insert(dh_match(idx_into_deriv_buf, deriv_hist_tmp));
                        break;
                    }
                }
                deriv_hist_tmp = 0;
            }
        }
        value.append(prod_buffer);
        i+=char_size;
    }
    std::cout << std::endl << "After the derivation... :\t";
    gettimeofday(&dbg_after, NULL);
    std::cout << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - DBG_NOW);

    bm_dh.clear();
    bm_dh.insert(bm_tmp.begin(), bm_tmp.end());
    map_params.clear();
    map_params.insert(map_params_tmp.begin(), map_params_tmp.end());
    map_iter=map_params.begin();
    map_offset.clear();
    map_offset.insert(map_offset_tmp.begin(), map_offset_tmp.end());
    map_offset_iter = map_offset.begin();
    vec_vec_dh_v2.push_back(vec_dh_v2_tmp);
    num_derivation++;
    text = string(value);
    this->dbg_previous_text = text;

    return value;
}

std::string PLSystemParametric::derive(unsigned int _start, unsigned int _end, unsigned int _n_times)
{
    string src, modified_str;
    unsigned int i;

    i = 0;
    do
    {
        src = text;
        derive(_start, _end);
        modified_str = text;
        locate_str_diff(src, modified_str, _start, _end);
        i++;
    }
    while (i<_n_times);
    return text;

}

std::string PLSystemParametric::derive(QList<int> _qlist_idx)
{
    std::vector<PDerivHistoric*> vec_dh_tmp, vec_leaves;
    PDerivHistoric* dh_tmp;
    unsigned int idx_tmp;

    for (size_t i=0; i<_qlist_idx.size(); i++)
    {
        vec_dh_tmp.push_back( select_historic(_qlist_idx.at(i)));
    }
    for (std::vector<PDerivHistoric*>::iterator iter = vec_dh_tmp.begin(); iter!=vec_dh_tmp.end(); iter++)
    {
        vec_leaves = historic_leaves();
        dh_tmp = *iter;
        if (std::find(vec_leaves.begin(), vec_leaves.end(), dh_tmp)!=vec_leaves.end())
        {
            idx_tmp = dh_tmp->getIdxIntoDeriv();
            derive(idx_tmp);
        }
    }
    return text;
}

// Returns the start and end indices of where the current string is highlighted
void PLSystemParametric::select(    std::vector<unsigned int>& _vec_start,
                                    std::vector<unsigned int>& _vec_end,
                                    unsigned int _index
                               )
{
    PDerivHistoric *pdh;
    unsigned int idx_min, idx_max;
    try
    {
        pdh = 0;
        pdh = bm_dh.left.at(_index);
    }
    catch(...)
    {
        return;
    }

    _vec_start.clear();
    _vec_end.clear();
    if (!pdh) return;
    do
    {
        idx_min = pdh->left_boundary();
        idx_max = pdh->right_boundary();
        _vec_start.push_back(idx_min);
        _vec_end.push_back(idx_max); 
        pdh = pdh->getParent();
    }  while (pdh!=0);
}

std::string PLSystemParametric::str_from_historic()
{
    std::string value;
    std::vector<PDerivHistoric*>::iterator iter;
    PDerivHistoric* tmp;

    bm_dh.clear();
    for (iter=vec_vec_dh_v2.back().begin(); iter!=vec_vec_dh_v2.back().end();iter++)
    {
        tmp = *iter;
        //tmp->setIdxIntoGen(value.size());
        tmp->setIdxIntoDeriv(value.size());
        tmp->pop_match();
        tmp->add_match_idx_dn(vec_vec_dh_v2.size(),value.size());
        bm_dh.insert(dh_match(value.size(), tmp));
        value+=tmp->getWord()->compute_str();
    }
    text=value;
    return value;
}

PDerivHistoric* PLSystemParametric::historic_root(std::vector<PDerivHistoric*> _vec_dh)
{
    // Note that for performance issue, you can use directly reverse iterator without having to copy contents of vector
    // But for clarity of the code, vector contents has been copied instead
    PDerivHistoric *value, *pdh;
    std::vector<PDerivHistoric*> vec_path, vec_path_reverse;
    std::vector<PDerivHistoric*> vec_path_tmp, vec_path_reverse_tmp;
    if (!_vec_dh.empty())
    {
        pdh = _vec_dh.front();
        do
        {
            vec_path_reverse.push_back(pdh);
            pdh = pdh->getParent();
        }
        while (pdh != 0);
        for (std::vector<PDerivHistoric*>::reverse_iterator riter = vec_path_reverse.rbegin(); riter!=vec_path_reverse.rend(); riter++)
        {
            vec_path.push_back(*riter);
        }
        value = vec_path.back();
    }
    for (std::vector<PDerivHistoric*>::iterator iter=_vec_dh.begin()+1; iter!=_vec_dh.end(); iter++)
    {
        vec_path_reverse_tmp.clear();
        vec_path_tmp.clear();
        pdh = (*iter);
        do
        {
            vec_path_reverse_tmp.push_back(pdh);
            pdh = pdh->getParent();
        }
        while (pdh!=0);
        for (std::vector<PDerivHistoric*>::reverse_iterator riter = vec_path_reverse.rbegin(); riter!=vec_path_reverse.rend(); riter++)
        {
            vec_path_tmp.push_back(*riter);
        }
        if (vec_path_tmp.size()>vec_path.size())
        {
            vec_path_tmp.erase(vec_path_tmp.begin()+vec_path.size(), vec_path_tmp.end());
        }
        else if (vec_path_tmp.size()<vec_path.size())
        {
            vec_path.erase(vec_path.begin()+vec_path_tmp.size(), vec_path.end());
        }
        for (std::vector<PDerivHistoric*>::iterator jter=vec_path.begin(), kter=vec_path_tmp.begin(); (jter!=vec_path.end())&&(kter!=vec_path_tmp.end()); jter++, kter++)
        {
            if (*jter!=*kter)
            {
                vec_path.erase(jter, vec_path.end());
                break;
            }
        }
    }
    value = vec_path.back();
    return value;
}

void PLSystemParametric::drop(PDerivHistoric*& _root_output, unsigned int _idx, unsigned int _depth)
{
    std::vector<PDerivHistoric*> vec_leaves;
    std::vector<PDerivHistoric*>::iterator iter;
    unsigned int i=0;

    this->dbg_previous_text = text;
    assert(_idx<text.size());
    _root_output = select_historic(_idx);
    while ((i!=_depth)&&(_root_output!=0))
    {
        _root_output=_root_output->getParent();
        i++;
    }
    _root_output->del_child();
    vec_leaves = historic_leaves();
    vec_vec_dh_v2.push_back(vec_leaves);
    str_from_historic();
    update_map_offset();
}

void PLSystemParametric::drop(std::vector<PDerivHistoric*>& _vec_roots_out, QList<int> _list_idx, unsigned int _depth)
{
    std::vector<PDerivHistoric*> vec_dh_tmp, vec_leaves;
    std::vector<PDerivHistoric*>::iterator iter;
    QList<int>::iterator qiter;
    PDerivHistoric* dh_tmp;

    this->dbg_previous_text = text;

    for (size_t i=0; i<_list_idx.size(); i++)
    {
        vec_dh_tmp.push_back( select_historic(_list_idx.at(i)));
    }
    for (iter = vec_dh_tmp.begin(); iter!=vec_dh_tmp.end(); iter++)
    {
        vec_leaves = historic_leaves();
        dh_tmp = *iter;
        if (std::find(vec_leaves.begin(), vec_leaves.end(), dh_tmp)!=vec_leaves.end())
        {
            for (size_t i=0; (i!=_depth)&&(dh_tmp!=0); i++)
               dh_tmp=dh_tmp->getParent();
            dh_tmp->del_child();
            _vec_roots_out.push_back(dh_tmp);
        }
    }

    vec_leaves = historic_leaves();
    vec_vec_dh_v2.push_back(vec_leaves);
    str_from_historic();
    update_map_offset();
}

std::vector<PDerivHistoric*> PLSystemParametric::historic_leaves()
{
    
    PDerivHistoric* tmp;
    std::vector<PDerivHistoric*>::iterator iter;
    std::vector<PDerivHistoric*> value, vec_tmp;
    std::vector<PDerivHistoric*> vec_root = vec_vec_dh_v2.front();
    for(iter=vec_root.begin(); iter!=vec_root.end(); iter++)
    {
        tmp = *iter;
        vec_tmp = tmp->leaves();
        value.insert(value.end(), vec_tmp.begin(), vec_tmp.end());
    }
    return value;
}

// def_const() copy in _vec_params and _vec_values the default constants used by PLSystemParametric
void PLSystemParametric::def_const(std::vector<std::string>& _vec_params, std::vector<float>& _vec_values)
{
    std::string s;
    float val;
    _vec_params.clear();
    _vec_values.clear();
    if (!preproc) return;
    for (unsigned i=0; i<preproc->getVecDefine().size(); i++)
    {
        s = preproc->getVecDefine().at(i)->getConstantName();
        val = preproc->getVecDefine().at(i)->getConstantValue();
        _vec_params.push_back(s);
        _vec_values.push_back(val);
    }
}

void PLSystemParametric::reinit_preproc(PPreproc* _src)
{
    vector<string> vec_name_tmp;
    vector<float> vec_val_tmp;
    if (_src)
    {
        *preproc = *_src;
        return;
    }
    for (unsigned int i=0; i<preproc->getVecDefine().size(); i++)
    {
        vec_name_tmp.push_back(preproc->getVecDefine()[i]->getConstantName());
        vec_val_tmp.push_back(preproc->getVecDefine()[i]->getConstantValue());
    }
    reinit_global_const(vec_name_tmp, vec_val_tmp);
}

void PLSystemParametric::reinit_global_const(std::vector<string> _vec_const_name, std::vector<float> _vec_const_val)
{
    string const_name;
    float new_value;
    PDerivHistoric* pdh;
    assert(_vec_const_name.size() == _vec_const_val.size());
    for (unsigned int i=0; i<_vec_const_name.size(); i++)
    {
        //update_global_const(_vec_const_name[i], _vec_const_val[i]);
        const_name = _vec_const_name[i];
        new_value = _vec_const_val[i];
        for (std::vector<PDerivHistoric*>::iterator iter=vec_vec_dh_v2.back().begin(); iter!=vec_vec_dh_v2.back().end(); iter++)
        {
            int rule_generator;
            vector<string> vec_var_name;

            pdh=*iter;
            rule_generator = pdh->getRuleGenNum();
            if ((rule_generator>=0)&&((unsigned)rule_generator<vec_rules.size()))
            {
                vec_var_name = vec_rules[rule_generator]->var_to_arraystr();
                pdh->update_value(const_name, new_value, vec_var_name, preproc);
            }
        }
    }
    str_from_historic();
}

void PLSystemParametric::update_global_const(std::string _const_name, float _new_value)
{
    std::vector<PDerivHistoric*>::iterator iter;
    PDerivHistoric* pdh;
    bool state_changed = false;
    for (unsigned i=0; i<preproc->getVecDefine().size(); i++)
    {
        if (!preproc->getVecDefine().at(i)->getConstantName().compare(_const_name))
        {
            if (fabs(preproc->getVecDefine().at(i)->getConstantValue()-_new_value)>=0.001)
                state_changed = true;
            preproc->getVecDefine().at(i)->setConstantValue(_new_value);
        }
    }
    if (state_changed)
    {
        for (iter=vec_vec_dh_v2.back().begin(); iter!=vec_vec_dh_v2.back().end(); iter++)
        {
            int rule_generator;
            vector<string> vec_var_name;

            pdh=*iter;
            rule_generator = pdh->getRuleGenNum();
            if ((rule_generator>=0)&&((unsigned)rule_generator<vec_rules.size()))
            {
                vec_var_name = vec_rules[rule_generator]->var_to_arraystr();
                pdh->update_value(_const_name, _new_value, vec_var_name, preproc);
            }
        }
        str_from_historic();
    }
}

void PLSystemParametric::update_global_const(PPreproc* _preproc)
{
    assert(preproc->getVecDefine().size() == _preproc->getVecDefine().size());
    for(unsigned int i=0; i<_preproc->getVecDefine().size(); i++)
    {
        if (!_preproc->getVecDefine()[i]->getConstantName().compare(preproc->getVecDefine()[i]->getConstantName()))
            if (fabs(_preproc->getVecDefine()[i]->getConstantValue()-preproc->getVecDefine()[i]->getConstantValue())>=0.001)
        update_global_const(    _preproc->getVecDefine()[i]->getConstantName(),
                                _preproc->getVecDefine()[i]->getConstantValue());
    }
    preproc->copy_const(_preproc);
}

void PLSystemParametric::update_local_const(PDerivHistoric* _pdh, std::string _const_name, float _new_value)
{
    vector<string> vec_var_name;
    int rule_generator;
    bool state_changed = false;
    for (unsigned i=0; i<_pdh->getPreproc()->getVecDefine().size(); i++)
    {
        if (!_pdh->getPreproc()->getVecDefine().at(i)->getConstantName().compare(_const_name))
        {
            if (fabs(_pdh->getPreproc()->getVecDefine().at(i)->getConstantValue()-_new_value)>=0.001)
                state_changed = true;
            _pdh->getPreproc()->getVecDefine().at(i)->setConstantValue(_new_value);
        }
    }
    if (state_changed)
    {
        rule_generator = _pdh->getRuleGenNum();
        if ((rule_generator>=0)&&((unsigned)rule_generator<vec_rules.size()))
        {
            vec_var_name = vec_rules[rule_generator]->var_to_arraystr();
            _pdh->update_value(_const_name, _new_value, vec_var_name, _pdh->getPreproc());
        }
        str_from_historic();
    }
}

void PLSystemParametric::update_local_const(PPreproc* _preproc, PDerivHistoric* _pdh)
{
    if (!(_preproc&&_pdh)) return;
    for (unsigned i=0; i<_preproc->getVecDefine().size(); i++)
    {
        if (!_pdh->getPreproc()->getVecDefine().at(i)->getConstantName().compare(_preproc->getVecDefine().at(i)->getConstantName()))
        {
            if (fabs(_pdh->getPreproc()->getVecDefine().at(i)->getConstantValue()-_preproc->getVecDefine().at(i)->getConstantValue())>=0.001)
            {
                update_local_const(_pdh, _preproc->getVecDefine().at(i)->getConstantName(), _preproc->getVecDefine().at(i)->getConstantValue());
            }
        }
    }
    _pdh->getPreproc()->copy_const(_preproc);
}

void PLSystemParametric::update_child_const( PDerivHistoric* _pdh, std::string _const_name, float _new_value, unsigned int _stages)
{
    PDerivHistoric* tmp;
    tmp = _pdh->getRoot(_stages);
    if (!tmp) return;
    _pdh->update_leaves_const(_const_name, _new_value, vec_rules);
    str_from_historic();
}

void PLSystemParametric::update_child_const(PPreproc* _preproc, PDerivHistoric *_pdh, unsigned int _back_stages)
{
    PDerivHistoric* tmp;
    tmp = _pdh->getRoot(_back_stages);
    if (!tmp) return;
    //assert(_preproc->getVecDefine().size()==tmp->getPreproc()->getVecDefine().size());
    for (unsigned int i=0; i<_preproc->getVecDefine().size(); i++)
    {
        string const_name = _preproc->getVecDefine().at(i)->getConstantName();
        float const_val = _preproc->getVecDefine().at(i)->getConstantValue();
        if (!tmp->getPreproc()->getVecDefine().at(i)->getConstantName().compare(const_name))
        {
            if (fabs(tmp->getPreproc()->getVecDefine().at(i)->getConstantValue()-_preproc->getVecDefine().at(i)->getConstantValue())>=0.001)
            {
                tmp->getPreproc()->copy_const(_preproc);
                tmp->update_leaves_const(const_name, const_val, vec_rules);
            }
        }
    }
    str_from_historic();
    //tmp->getPreproc()->copy_const(_preproc);
}

// Insert tags inside the current string and returns the string
std::string PLSystemParametric::highlight_text( std::string& _src,
                                                std::vector<unsigned int> _vec_start,
                                                std::vector<unsigned int> _vec_end,
                                                unsigned int _index )
{
    const unsigned int size = _vec_start.size();
    size_t symbol_length=0;
    float* red, *green, *blue;
    string tag_closer = "</font>";
    string *tag_opener;
    string value = _src;
    PTextParser textparser;
    unsigned int start_tmp, end_tmp, closer_tmp;
    std::vector<unsigned int> vec_newend;

    assert(_vec_start.size()==_vec_end.size());
    if (_vec_start.empty()) return value;

    tag_opener = new string[size];
    red     = new float[size];
    green   = new float[size];
    blue    = new float[size];

    // Compute the color of the string
    for (unsigned int i=0; i<size; i++)
    {
        float ratio = (float)(i)/size;
        Procedural::rgb_from_impact(ratio, red[i], green[i], blue[i]);
        if (i==size-1) {red[i]=1; green[i]=0; blue[i]=0;}
        tag_opener[i] = Procedural::rgb_to_htmltag_hex(red[i], green[i], blue[i]);
    }

    // The exact value of vec_end in L-System points to the character
    // and not after the character. Although this should be after :
    for (unsigned int i=0; i<size; i++)
    {
        symbol_length = alphabet->search(_src, _vec_end[i]);
        end_tmp = _vec_end[i]+symbol_length;
        if (symbol_length!=0)
        {
            if (end_tmp <= value.size())
            {
                closer_tmp = PTextParser::find_bracket_closer(value, _vec_end[i]+symbol_length);
                if (closer_tmp!=0)
                    end_tmp=closer_tmp+1;
            }
        }
        vec_newend.push_back(end_tmp);
    }
    _vec_end = vec_newend;

    for(unsigned int i=0; i<size; i++)
    {
        start_tmp   = _vec_start[i];
        end_tmp     = _vec_end[i];
        Procedural::insert_tag(value, start_tmp, tag_opener[size-1-i], end_tmp, tag_closer);
        Procedural::shift_vector(_vec_end, end_tmp, tag_opener[size-1-i].size()+tag_closer.size());
    }

    delete [] tag_opener;
    delete [] red;
    delete [] green;
    delete [] blue;
    return value;
}

void PLSystemParametric::locate_str_diff(std::string& _src, std::string& _modified_str, unsigned int& _modif_start, unsigned int& _modif_end)
{
    _modif_start = 0;
    _modif_end = 0;
    if (_src.size()>=_modified_str.size()) return;
    for (unsigned int i=0; i<_src.size(); i++)
    {
        if (_src[i]!=_modified_str[i])
        {
            _modif_start = i;
            _modif_end = _modif_start + (_modified_str.size() - _src.size()) - 1;
            break;
        }
    }
    for (unsigned int i=_src.size()-1, j=_modified_str.size()-1; i!=0 && j!=0; i--, j--)
    {
         if (_src[i]!=_modified_str[j])
         {
             _modif_end = j;
             break;
         }
    }
    // If the differences are near the end, then we assume that there's no difference till the end
    if (_modif_end >= _modified_str.size()-4)
        _modif_end = _modified_str.size()-1;
}

void PLSystemParametric::regenerate(unsigned int _idx,
                                    unsigned int _back_stages,
                                    unsigned int _forward_stages,
                                    PDerivHistoric*& _dhroot_output)
{
    // TO DO: this function can be and should made simpler
    // When improved, please DO NOT remove this old version
    // since it will help understand the mechanics of historic tree regressing
    unsigned int idx_root;
    unsigned int bracket_closer;
    unsigned int alphabet_size=1;

    PTextParser textparser(text);
    drop(_dhroot_output, _idx, _back_stages);
    //idx_root = bm_dh.right.at(_dhroot_output);
    idx_root = _dhroot_output->getIdxIntoDeriv();

    alphabet_size = alphabet->search(text, idx_root);
    bracket_closer = textparser.find_bracket_closer(idx_root+alphabet_size);
    if (bracket_closer!=0)
    {
        derive(idx_root, bracket_closer, _forward_stages);
    }
    else
    {
        derive(idx_root, idx_root, _forward_stages);
    }
}

void  PLSystemParametric::regenerate(       QList<int> _list_idx,
                                            unsigned int _back_stages,
                                            unsigned int _forward_stages)
{
    std::vector<PDerivHistoric*> vec_dh_tmp, vec_leaves;
    PDerivHistoric* dh_tmp;
    unsigned int idx_tmp;

    drop(vec_dh_tmp, _list_idx, _back_stages);
    for (std::vector<PDerivHistoric*>::iterator iter = vec_dh_tmp.begin(); iter!=vec_dh_tmp.end(); iter++)
    {
        vec_leaves = historic_leaves();
        dh_tmp = *iter;
        if (std::find(vec_leaves.begin(), vec_leaves.end(), dh_tmp)!=vec_leaves.end())
        {
            idx_tmp = dh_tmp->getIdxIntoDeriv();
            generate(idx_tmp, _forward_stages);
        }
    }
}

void  PLSystemParametric::generate(         unsigned int _idx,
                                            unsigned int _forward_stages
                                    )
{
    unsigned int alphabet_size=1;
    unsigned int bracket_closer;
    PTextParser tp(text);
    alphabet_size = alphabet->search(text, _idx);
    bracket_closer = tp.find_bracket_closer(_idx+alphabet_size);
    derive(_idx, (bracket_closer!=0)?bracket_closer:_idx, _forward_stages);
}

void PLSystemParametric::generate(  QList<int> _qlist_idx,
                                    unsigned int _forward_stages
                                    )
{
    std::vector<PDerivHistoric*> vec_dh_tmp, vec_leaves;
    PDerivHistoric* dh_tmp;
    unsigned int idx_tmp;

    for (size_t i=0; i<_qlist_idx.size(); i++)
    {
        vec_dh_tmp.push_back( select_historic(_qlist_idx.at(i)));
    }
    for (std::vector<PDerivHistoric*>::iterator iter = vec_dh_tmp.begin(); iter!=vec_dh_tmp.end(); iter++)
    {
        vec_leaves = historic_leaves();
        dh_tmp = *iter;
        if (std::find(vec_leaves.begin(), vec_leaves.end(), dh_tmp)!=vec_leaves.end())
        {
            idx_tmp = dh_tmp->getIdxIntoDeriv();
            generate(idx_tmp, _forward_stages);
        }
    }
}

void  PLSystemParametric::inspect_selection(QList<int> _list_idx, unsigned int _back_stages)
{
    std::vector<PDerivHistoric*> vec_leaves, vec_slctn, vec_prts, vec_dhconfig_tmp;
    std::vector< std::vector<PDerivHistoric*> > vecs_dhconfig;
    std::vector<int> vec_newselection, vec_unwantedselection, vec_ignoredselection;
    std::fstream inspectorfile;
    std::stringstream ss;
    size_t elts_size;           // The number of leaves in the tree structure
    size_t selection_size;      // The number of selection provided as input
    size_t unwanted_selection;  // These are selected in the process but not by the user
    size_t ignored_selection;   // The selection supposedly ignored because is only a small proportion
    double selection_rate;      // The selection amount divided by the number of elements/leaves
    double unwantedslctn_rate;  // The number of unselected elements which were implicitly selected
    double ignored_rate;        // The number of ignored elements divided by selection number
    PDerivHistoric* pdh;

    ss << "Seeking conflicts under stage " << _back_stages << ".";
    vec_leaves = historic_leaves();
    for (size_t i=0; i<_list_idx.size(); i++)
        vec_slctn.push_back(select_historic(_list_idx.at(i)));

    // In this section we can already detect which instances share the exact same root
    for (std::vector<PDerivHistoric*>::iterator iter = vec_slctn.begin(); iter!=vec_slctn.end(); iter++)
    {
        std::vector<PDerivHistoric*>::iterator iterfound;
        pdh = *iter;
        for (size_t j=0; j<_back_stages && pdh->getParent(); j++)
            pdh = pdh->getParent();
        iterfound = std::find(vec_prts.begin(), vec_prts.end(), pdh);
        if (iterfound!=vec_prts.end())
            ss << std::endl << "Instance " << (*iter)->getIdxIntoDeriv() << " shares root with " << (*iterfound)->getIdxIntoDeriv() << '.';
        else
            vec_prts.push_back(pdh);
    }

    // In this section we can determine if any instance is in the same arm/ramification of any other in the selection
    for (std::vector<PDerivHistoric*>::iterator iter = vec_prts.begin(); iter!=vec_prts.end(); iter++)
    {
        std::vector<PDerivHistoric*>::iterator iterfound;
        std::vector<PDerivHistoric*> vec_kids_tmp;
        pdh = *iter;
        vec_kids_tmp = (*iter)->leaves(vec_slctn);
        iterfound = std::find(vec_kids_tmp.begin(), vec_kids_tmp.end(), *iter);
        if (iterfound!=vec_kids_tmp.end())
            vec_kids_tmp.erase(iterfound);
        if (!vec_kids_tmp.empty())
        {
            ss << std::endl << "Instance " << (*iter)->getIdxIntoDeriv() << " is conflicting with : ";
            for (std::vector<PDerivHistoric*>::iterator jter = vec_kids_tmp.begin(); jter!=vec_kids_tmp.end(); jter++)
                ss << '\t' << (*jter)->getIdxIntoDeriv();
        }
    }

    // Next step is about setting up configurations for the regeneration and computing selection rates for every
    // one of them. First of all, we must avoid returning nodes in the same hierarchies, and find the
    // best solution to avoid that. Here we adopt 2 methods consisting of eliminating roots elements at first,
    // and lastly we remove kids elements in our hierarchy. Every other method can lead very easily
    // to a combination that can grow exponentially in configurations.
    // We then delete roots part to generate the first configuration.
    vec_dhconfig_tmp = vec_prts;
    for (std::vector<PDerivHistoric*>::iterator iter = vec_dhconfig_tmp.begin(); iter!=vec_dhconfig_tmp.end(); iter++)
    {
        std::vector<PDerivHistoric*>::iterator iterfound;
        std::vector<PDerivHistoric*> vec_ramifs;
        vec_ramifs = (*iter)->getRoots();
        if (!vec_ramifs.empty())
        {
            for (std::vector<PDerivHistoric*>::iterator jter=vec_ramifs.begin(); jter!=vec_ramifs.end(); jter++)
            {
                do
                {
                    iterfound = std::find(vec_dhconfig_tmp.begin(), vec_dhconfig_tmp.end(), *jter);
                    if (iterfound != vec_dhconfig_tmp.end())
                        vec_dhconfig_tmp.erase(iterfound);
                }
                while (iterfound != vec_dhconfig_tmp.end());
            }
        }
    }
    vecs_dhconfig.push_back(vec_dhconfig_tmp);
    eval_selection(vec_dhconfig_tmp, _list_idx, vec_newselection, vec_unwantedselection, vec_ignoredselection);

    // The second configuration concerns only the roots and delete kids
    vec_dhconfig_tmp = vec_prts;
    for (std::vector<PDerivHistoric*>::iterator iter = vec_dhconfig_tmp.begin(); iter!=vec_dhconfig_tmp.end(); iter++)
    {
        for (std::vector<PDerivHistoric*>::iterator jter=vec_dhconfig_tmp.begin(); jter!=vec_dhconfig_tmp.end(); jter++)
        {
            if (iter!=jter)
            {
                if ((*iter)->is_ramification(*jter))
                {
                    vec_dhconfig_tmp.erase(jter);
                    jter--;
                }
            }
        }
    }
    vecs_dhconfig.push_back(vec_dhconfig_tmp);
    eval_selection(vec_dhconfig_tmp, _list_idx, vec_newselection, vec_unwantedselection, vec_ignoredselection);

    inspectorfile.open("selection.txt", std::ofstream::out | std::ofstream::trunc);
    inspectorfile << ss.rdbuf();
    inspectorfile.close();
}

void  PLSystemParametric::eval_selection(   const std::vector<PDerivHistoric*>& _inputvec_dhconfig,
                                            QList<int> _inputlist_selection,
                                            std::vector<int>& _outputvec_selection,
                                            std::vector<int>& _outputvec_unwantedslctn,
                                            std::vector<int>& _outputvec_ignoredslctn)
{
    std::vector<PDerivHistoric*> vec_leaves_tmp, vec_leaves_val;
    _outputvec_selection.clear();
    _outputvec_unwantedslctn.clear();
    _outputvec_ignoredslctn.clear();
    for (std::vector<PDerivHistoric*>::const_iterator iter = _inputvec_dhconfig.begin(); iter!=_inputvec_dhconfig.end(); iter++)
    {
        vec_leaves_tmp = (*iter)->leaves();
        for (std::vector<PDerivHistoric*>::iterator jter = vec_leaves_tmp.begin(); jter!=vec_leaves_tmp.end(); jter++)
        {
            if (std::find(vec_leaves_val.begin(), vec_leaves_val.end(), *jter) == vec_leaves_val.end())
                vec_leaves_val.push_back(*jter);
        }
    }

    for (std::vector<PDerivHistoric*>::iterator iter = vec_leaves_val.begin(); iter!=vec_leaves_val.end(); iter++)
        if (alphabet->is_alphabet((*iter)->getWord()))
            _outputvec_selection.push_back((*iter)->getIdxIntoDeriv());

    for (std::vector<int>::iterator iter = _outputvec_selection.begin(); iter!=_outputvec_selection.end(); iter++)
        if (std::find(_inputlist_selection.begin(), _inputlist_selection.end(), *iter) == _inputlist_selection.end())
            _outputvec_unwantedslctn.push_back(*iter);

    for (QList<int>::iterator iter = _inputlist_selection.begin(); iter!=_inputlist_selection.end(); iter++)
        if (std::find(_outputvec_selection.begin(), _outputvec_selection.end(), *iter) == _outputvec_selection.end())
            _outputvec_ignoredslctn.push_back(*iter);

    std::cout << std::endl << "Call to eval_selection().";
    std::cout << std::endl << _inputlist_selection.size()       << "\telement(s) was/were selected by the user.";
    std::cout << std::endl << _outputvec_selection.size()       << "\telement(s) is/are now selected in the process.";
    std::cout << std::endl << _outputvec_unwantedslctn.size()   << "\telement(s) was/were unwanted selection(s).";
    std::cout << std::endl << _outputvec_ignoredslctn.size()    << "\telement(s) was/were selected but will be ignored.";
}

unsigned int PLSystemParametric::count_alphabet(     const std::vector<PDerivHistoric*>& _vec_dhconfig)
{
    unsigned int value=0;
    for (std::vector<PDerivHistoric*>::const_iterator iter = _vec_dhconfig.begin(); iter!=_vec_dhconfig.end(); iter++)
        if (alphabet->is_alphabet((*iter)->getWord()))
            value++;
    return value;
}

std::vector<unsigned int> PLSystemParametric::sub_branches()
{
    std::vector<unsigned int> value;
    for (unsigned int i=0; i<=text.size(); i++)
    {
        if (alphabet->search(text,i)!=0)
            value.push_back(i);
    }
    return value;
}

// This method should be reassessed and fixed accordingly:
// Cette méthode doit être réévaluée et corrigée en fonction:
std::vector<unsigned int> PLSystemParametric::sub_branches(std::string _alphabet)
{
    std::vector<unsigned int> value;
    for (unsigned int i=0; i<=text.size(); i++)
    {

        if (alphabet->search(text,i)!=0)
        {
            stringstream ss;
            string s;
            char c = text.substr(i).at(0);
            ss << c;
            ss >> s;
            if(s.compare("A") == 0 || s.compare("B") == 0 )
                value.push_back(i);
        }

    }
    return value;
}


std::vector<unsigned int> PLSystemParametric::sub_branches(unsigned int _selection)
{
    std::vector<unsigned int> value;
    unsigned int start, end;
    int depth=0;

    start = end = _selection+1;
    value.push_back(_selection);
    if (start>text.size()-1) return value;

    while ((end<text.size())&&(depth>=0))
    {
        if (text[end]==']')
        {
            depth--;
        }
        else if (text[end]=='[')
        {
            depth++;
        }
        end++;
    }

    for (unsigned int i=start; i<=end; i++)
    {
        if (alphabet->is_alphabet(text[i]))
            value.push_back(i);
    }

    return value;
}

std::vector<unsigned int> PLSystemParametric::sub_branches(QList<int>_list_selection)
{
    std::vector<unsigned int> value;
    unsigned int start, end;

    for (size_t j=0; j<_list_selection.size(); j++)
    {
        int depth=0;
        start = end = _list_selection.at(j)+1;
        value.push_back(_list_selection.at(j));
        if (start>text.size()-1) return value;

        while ((end<text.size())&&(depth>=0))
        {
            if (text[end]==']')
            {
                depth--;
            }
            else if (text[end]=='[')
            {
                depth++;
            }
            end++;
        }

        for (unsigned int i=start; i<=end; i++)
        {
            if (alphabet->is_alphabet(text[i]))
                value.push_back(i);
        }
    }


    return value;
}

std::vector<PDerivHistoric*> PLSystemParametric::multiselect_historic(std::vector<unsigned int> _vec_selection)
{
    std::vector<PDerivHistoric*> value;
    for (std::vector<unsigned int>::iterator iter=_vec_selection.begin(); iter!=_vec_selection.end(); iter++)
        value.push_back(select_historic(*iter));
    return value;
}

std::vector<PDerivHistoric*> PLSystemParametric::multiselect_historic(QList<int> _list_selection)
{
    std::vector<PDerivHistoric*> value;
    for (QList<int>::iterator iter=_list_selection.begin(); iter!=_list_selection.end(); iter++)
        value.push_back(select_historic(*iter));
    return value;
}

void PLSystemParametric::update_sub_branches(unsigned int _selection, PPreproc* _preproc)
{
    vector<unsigned int> vec_branches = sub_branches(_selection);
    vector<PDerivHistoric*> vec_dh_tmp = multiselect_historic(vec_branches);
    for (vector<PDerivHistoric*>::iterator iter=vec_dh_tmp.begin(); iter!=vec_dh_tmp.end(); iter++)
    {
        update_local_const(_preproc, *iter);
    }
}

void PLSystemParametric::update_sub_branches(QList<int> _list_selection, PPreproc* _preproc)
{
    vector<unsigned int> vec_branches = sub_branches(_list_selection);
    vector<PDerivHistoric*> vec_dh_tmp = multiselect_historic(vec_branches);
    for (vector<PDerivHistoric*>::iterator iter=vec_dh_tmp.begin(); iter!=vec_dh_tmp.end(); iter++)
    {
        update_local_const(_preproc, *iter);
    }
}

std::vector<PPreproc> PLSystemParametric::generate_preproc()
{
    vector<PPreproc> value;
    vector<float> vec_rate1;
    vector<float> vec_rate2;

    vec_rate1.push_back(.10f);

    //vec_rate2.push_back(.15f);
    vec_rate2.push_back(.30f);
   // vec_rate2.push_back(.45f);

    int nb_rate = 3;

    unsigned int nb_param = preproc->getVecDefine().size();
    unsigned int nb_val = nb_rate * 2;

    //Calcul des différentes valeurs possibles que chaque parametre peut prendre et le stocke dans  param_values

    std::vector<std::vector <float>> param_values;


    for(int i = 0; i<nb_param; i++)
    {
        std::vector<float> vec_val;
        float new_val = preproc->getVecDefine().at(i)->getConstantValue();
        if (new_val<5)
        {
            for(std::vector<float>::iterator iter = vec_rate1.begin(); iter!=vec_rate1.end(); iter++)
            {
                vec_val.push_back(new_val*(1+*iter));
                vec_val.push_back(new_val*(1-*iter));
            }
        }
        else
        {
            for(std::vector<float>::iterator iter = vec_rate2.begin(); iter!=vec_rate2.end(); iter++)
            {
                vec_val.push_back(new_val*(1+*iter));
                vec_val.push_back(new_val*(1-*iter));
            }
        }
        vec_val.push_back(new_val);
        param_values.push_back(vec_val);
    }

    //Genere tous les preprocs possibles liés aux combinaisons possibles entre les valeurs que les parametres peuvent prendre
    //unsigned int nb_pos = static_cast<unsigned int>((double)pow((double)nb_val, (int)nb_param));
    PPreproc preproc_tmp=(*preproc);

    gen_preproc(nb_param-1,param_values,&value, preproc_tmp);

    return value;
}

void PLSystemParametric::gen_preproc(unsigned int nb_param, std::vector<std::vector<float>> param_values, std::vector<PPreproc> *preprocs, const PPreproc _preproc)
{
    std::vector<float> values = param_values.at(nb_param);
    if (nb_param==0)
    {
        for(std::vector<float>::iterator iter = values.begin(); iter!=values.end(); iter++)
        {
            //redefinie la derniere valeur et ajoute le preproc a value

            PPreproc preproc_tmp=_preproc;
            preproc_tmp.redefine_const(preproc_tmp.getVecDefine().at(nb_param)->getConstantName(), *iter);
            preprocs->push_back(preproc_tmp)
                    ;
        }
    }
    else
    {
        for(std::vector<float>::iterator iter = values.begin(); iter!=values.end(); iter++)
        {
            PPreproc preproc_tmp=_preproc;
            preproc_tmp.redefine_const(preproc_tmp.getVecDefine().at(nb_param)->getConstantName(), *iter);
            gen_preproc(nb_param-1, param_values, preprocs, preproc_tmp);
        }
    }
}

void PLSystemParametric::rewrite_rule(unsigned int _rule_nb, PPreproc* _new_preproc)
{
    string cst_name, str_cst_val, str_rule, str_prods;
    size_t pos;
    PTextParser tp;

    if (_rule_nb>=vec_rules.size()) return;
    for (unsigned int i=0; i<vec_rules.size(); i++)
    {
        str_rule=vec_rules.at(i)->getText();
        if (i==_rule_nb)
        {
            for (unsigned int j=0; j<_new_preproc->getVecDefine().size(); j++)
            {
                cst_name = _new_preproc->getVecDefine().at(j)->getConstantName();
                pos = str_rule.find(cst_name);
                if (pos!=string::npos)
                {
                    tp = PTextParser(_new_preproc->getVecDefine().at(j)->getConstantValue());
                    do
                    {
                        str_rule.replace(pos, cst_name.length(), tp.getText().c_str());
                        pos = str_rule.find(cst_name);
                    } while (pos!=string::npos);

                }
            }
            str_prods+=str_rule;
            if (i+1<vec_rules.size()) str_prods+="\n";
        }
        else
        {
            str_prods+=str_rule;
            if (i+1<vec_rules.size()) str_prods+="\n";
        }
    }
    vec_rules.clear();
    load(str_prods);
}

void PLSystemParametric::clear_rules()
{
    for (std::vector<PRuleParametric*>::iterator iter = vec_rules.begin(); iter!=vec_rules.end(); iter++)
        delete (*iter);
    vec_rules.clear();
}

void PLSystemParametric::dbg_bimap(const boost_bimap_dh _my_bimap)
{
    std::cout << std::endl << "Bimap uint<=>PDerivHistoric: " << _my_bimap.size() << " Elements" ;
    for(boost::bimap<unsigned int, PDerivHistoric*>::const_iterator iter=_my_bimap.begin(); iter!=_my_bimap.end(); iter++)
    {
        std::cout << std::endl << iter->left << "-->" << iter->right;
    }
}

void PLSystemParametric::dbg_map_params()
{
    std::cout << std::endl << "Map Params Content Debug: " << map_params.size() << " Elements" ;
    for (std::map<unsigned int, std::vector<float> >::iterator iter=map_params.begin(); iter != map_params.end(); iter++)
    {
        std::cout << std::endl << (*iter).first << " : ";
        for (unsigned int i=0; i < (*iter).second.size(); i++)
            std::cout << (*iter).second.at(i) << " ";
    }
}

void PLSystemParametric::dbg_map_offset()
{
    std::cout << std::endl << "Map Offset Content Debug: " << map_offset.size() << " Elements" ;
    for (std::map<unsigned int, unsigned int>::iterator iter=map_offset.begin(); iter != map_offset.end(); iter++)
    {
        std::cout << std::endl << (*iter).first << " <=> "<< (*iter).second;
    }
}

void PLSystemParametric::dbg_string()
{
    std::cout << std::endl << "Current string: " << text.size() << " Chars";
    std::cout << std::endl << text;
}

void PLSystemParametric::insert_map(unsigned _index, unsigned int _offset, std::vector<float> _vec_params)
{
    std::map<unsigned int, unsigned int> map_offset_tmp;
    std::map<unsigned int, std::vector<float> > map_params_tmp;
    std::map<unsigned int, unsigned int>::iterator map_offset_iter_tmp;
    std::map<unsigned int, std::vector<float> >::iterator map_iter_tmp;
    int final_offset = static_cast<int>(_offset);

    /*std::ofstream file("qlfr_timeline.txt", std::ofstream::out|std::ofstream::app);
        std::stringstream ss_buf;

        ss_buf << std::endl << "Comparaison de la longueur" << std::distance(map_offset.begin(),map_offset.end()) << " = " << std::distance(map_params.begin(),map_params.end());

        if (file.is_open())
        {
            file << ss_buf.str();
            file.close();
        }
        else
        {
            std::cout << std::endl << "WARNING: Could not open qlfr_timeline.txt...";
        }*/

    //unsigned int i=0;
    /*for (map_offset_iter_tmp = map_offset.begin(); map_offset_iter_tmp!=map_offset.end(); map_offset_iter_tmp++)
    {
        if ((*map_offset_iter_tmp).first == _index){
            map_offset_iter_tmp--;
            map_offset_tmp.insert(map_offset.begin(), map_offset_iter_tmp);
            map_offset_tmp.insert(std::pair<unsigned int, unsigned int>(_index, final_offset));
            map_offset_iter_tmp++;
            break;
        }
    }

    //map_offset_tmp.insert(map_offset.begin(), map_offset_iter_tmp);
    //map_offset_tmp.insert(std::pair<unsigned int, unsigned int>(_index, final_offset));
    /*if ((*map_offset_iter_tmp).first == _index){
        map_offset_iter_tmp++;
        final_offset = _offset - (*map_offset_iter_tmp).first;
    }*/
    /*while (map_offset_iter_tmp!=map_offset.end())
    {
        map_offset_tmp.insert(std::pair<unsigned int, unsigned int>((*map_offset_iter_tmp).first+final_offset, (*map_offset_iter_tmp).second));
        map_offset_iter_tmp++;
    }

    //map_offset.clear();
    //map_offset.insert(map_offset_tmp.begin(), map_offset_tmp.end());
    map_offset = map_offset_tmp;

    for (map_iter_tmp = map_params.begin(); map_iter_tmp!=map_params.end(); map_iter_tmp++)
    {
        if ((*map_iter_tmp).first == _index){
            map_iter_tmp--;
            map_params_tmp.insert(map_params.begin(), map_iter_tmp);
            map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >(_index, _vec_params));
            map_iter_tmp++;
            break;
        }
    }

    //map_params_tmp.insert(map_params.begin(), map_iter_tmp);
    //map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >(_index, _vec_params));
    /*if ((*map_iter_tmp).first == _index)
        map_iter_tmp++;*/
    /*while (map_iter_tmp!=map_params.end())
    {
        map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >((*map_iter_tmp).first+final_offset, (*map_iter_tmp).second));
        map_iter_tmp++;
    }
    //map_params.clear();
    //map_params.insert(map_params_tmp.begin(), map_params_tmp.end());
    map_params = map_params_tmp;*/

    for (map_offset_iter_tmp = map_offset.begin(), map_iter_tmp = map_params.begin(); map_offset_iter_tmp!=map_offset.end(); map_offset_iter_tmp++, map_iter_tmp++)
        {
            if ((*map_offset_iter_tmp).first == _index)
            {
                map_offset_iter_tmp--;
                map_offset_tmp.insert(map_offset.begin(), map_offset_iter_tmp);
                map_offset_tmp.insert(std::pair<unsigned int, unsigned int>(_index, final_offset));
                map_offset_iter_tmp++;

                map_iter_tmp--;
                map_params_tmp.insert(map_params.begin(), map_iter_tmp);
                map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >(_index, _vec_params));
                map_iter_tmp++;
                break;
            }
        }

    while (map_offset_iter_tmp!=map_offset.end())
    {
        map_offset_tmp.insert(std::pair<unsigned int, unsigned int>((*map_offset_iter_tmp).first+final_offset, (*map_offset_iter_tmp).second));
        map_offset_iter_tmp++;

        map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >((*map_iter_tmp).first+final_offset, (*map_iter_tmp).second));
        map_iter_tmp++;
    }

    map_offset = map_offset_tmp;
    map_params = map_params_tmp;

}


void PLSystemParametric::drop_map(unsigned _index)
{
    std::map<unsigned int, unsigned int> map_offset_tmp;
    std::map<unsigned int, std::vector<float> > map_params_tmp;
    std::map<unsigned int, unsigned int>::iterator map_offset_iter_tmp;
    std::map<unsigned int, std::vector<float> >::iterator map_iter_tmp;
    unsigned int offset_tmp;

    map_offset_iter_tmp = map_offset.find(_index);
    if (map_offset_iter_tmp!=map_offset.end())
    {
        offset_tmp = (*map_offset_iter_tmp).second;
        map_iter_tmp = map_params.find(_index);

        map_offset_tmp.insert(map_offset.begin(), map_offset_iter_tmp);
        map_params_tmp.insert(map_params.begin(), map_iter_tmp);

        // Increment two times to skip the current element
        map_offset_iter_tmp++;
        map_iter_tmp++;

        while ((map_iter_tmp!=map_params.end())&&(map_offset_iter_tmp!=map_offset.end()))
        {
            map_offset_tmp.insert(std::pair<unsigned int, unsigned int>((*map_offset_iter_tmp).first-offset_tmp, (*map_offset_iter_tmp).second));
            map_params_tmp.insert(std::pair<unsigned int, std::vector<float> >((*map_iter_tmp).first-offset_tmp, (*map_iter_tmp).second));
            map_offset_iter_tmp++;
            map_iter_tmp++;
        }
        map_offset.clear();
        map_offset.insert(map_offset_tmp.begin(), map_offset_tmp.end());
        map_params.clear();
        map_params.insert(map_params_tmp.begin(), map_params_tmp.end());
    }
}

void PLSystemParametric::to_std_stream(std::stringstream& _ss)
{
    std::string tmp;
    _ss.str("");
    if (preproc)
    {
        if (preproc->getSeed()>0)
        {
            time_t _seed = preproc->getSeed();
            _ss << "#seed " << _seed << std::endl;
        }
        for (size_t i=0; i<preproc->getVecWord().size(); i++)
        {
            if (preproc->getVecWord().at(i)->getText().size()>1)
                _ss << "#word " << preproc->getVecWord().at(i)->getText() << std::endl;
        }
        if (!preproc->getIgnore()->getText().empty())
            _ss << "#ignore " << preproc->getIgnore()->getText() << std::endl;
        if (preproc->getVecInclude().size()!=0)
        {
            for (int i=0; i<preproc->getVecInclude().size(); i++)
                _ss << "#include " << preproc->getVecInclude().at(i)->getText() << std::endl;
        }
        if (preproc->getVecDefine().size()!=0)
        {
            for (int i=0; i<preproc->getVecDefine().size(); i++)
                _ss << "#define " << preproc->getVecDefine().at(i)->getConstantName() << " " << preproc->getVecDefine().at(i)->getConstantValue() << std::endl;
        }
        _ss << "n:" << target_deriv_num << std::endl;
        _ss << "w:" << axiom->getText() << std::endl;
        _ss << "p:" << std::endl;
        for (unsigned int i=0; i<vec_rules.size(); i++)
        {
            _ss << vec_rules.at(i)->getText() << std::endl;
        }
    }
}

void PLSystemParametric::quick_load(std::string& _text)
{
    int n_deriv, n_angle;
    PTextParser textparser;
    string str_preproc, str_axiom, str_rules, str_buffer;
    std::string delimiter("\n");
    std::vector<std::string> vec_str_line = PTextParser::split_string(_text, delimiter);

    while (vec_str_line.back()=="")
        vec_str_line.pop_back();
    for (unsigned int i=0; i<vec_str_line.size(); i++)
    {
        str_buffer = vec_str_line.at(i);
        if (str_buffer.find('#')==0)
        {
            if (!str_preproc.empty())
                str_preproc+="\n";
            str_preproc+=str_buffer;
        }
        else if (str_buffer.find("w:")==0)
        {
            if (str_buffer.size()>2)
                str_buffer = str_buffer.substr(2);
            str_axiom = str_buffer;
        }
        else if (str_buffer.find("n:")==0)
        {
            if (str_buffer.size()>2)
                str_buffer = str_buffer.substr(2);
            n_deriv = (int)textparser.stdstr_to_float(str_buffer);
        }
        else if (str_buffer.find("d:")==0)
        {
            if (str_buffer.size()>2)
                str_buffer = str_buffer.substr(2);
            n_angle = textparser.stdstr_to_float(str_buffer);
        }
        else if (str_buffer.find("->")!=string::npos)
        {
            if(!str_buffer.empty())
                str_buffer+="\n";
            str_rules+=str_buffer;
        }
    }
    num_derivation = 0;
    target_deriv_num = n_deriv;
    update_params(str_preproc, str_axiom, n_angle, n_deriv);
    load(str_rules);
    init_dh();
    srand(getPreproc()->getSeed());
    for (int i=0; i<target_deriv_num; i++)
        derive();
}

void PLSystemParametric::update_map_offset()
{
    unsigned int offset_tmp, alphabet_size, char_size;
    vector<float> vec_params_tmp;

    map_params.clear();
    map_offset.clear();
    for (unsigned int i = 0;i<text.length();i+=offset_tmp)
    {
        vec_params_tmp.clear();
        pick_char_at(i, char_size, vec_params_tmp);
        alphabet_size = alphabet->search(text, i);
        offset_tmp=1;
        if ((alphabet_size!=0)||(alphabet->is_shifting(text[i]))||(alphabet->is_constant(text[i])))
        {
            if (!vec_params_tmp.empty())
            {
                map_params.insert(std::pair<unsigned int, std::vector<float> >(i, vec_params_tmp));
                map_offset.insert(std::pair<unsigned int, unsigned int>(i, char_size));
            }
        }
    }
    map_iter=map_params.begin();
    map_offset_iter = map_offset.begin();
}

void PLSystemParametric::update_leaves()
{
    vec_vec_dh_v2.push_back(historic_leaves());
    str_from_historic();
    update_map_offset();
}

#include <math.h>
#include <QGLWidget>

#define _PLSYSTEM_GLOBAL

#include "grammar/pcontext.h"
#include "plsystem.h"
#include "prule.h"
#include "ptextparser.h"

#include "procedural.h"

using namespace std;

PLSystem::PLSystem()
{
    num_derivation = 0;
    angle = 90;

    preproc     = new PPreproc();
    alphabet    = new PAlphabet();
    axiom       = new PAxiom();
    constant    = new PConstant();
}

PLSystem::PLSystem(char* _text)
{
    num_derivation = 0;
    angle = 90;

    text = string(_text);

    preproc     = new PPreproc();
    alphabet    = new PAlphabet();
    axiom       = new PAxiom();
    constant    = new PConstant();
}

PLSystem::PLSystem(std::string _text)
{
    num_derivation = 0;
    angle = 90;
    text = _text;

    preproc     = new PPreproc();
    alphabet    = new PAlphabet();
    axiom       = new PAxiom();
    constant    = new PConstant();
}

PLSystem::~PLSystem()
{
    delete axiom;
    delete constant;
    delete preproc;
}

void PLSystem::init()
{
    vec_rules.clear();
    text.clear();
    raw.clear();
    delete axiom;
    axiom = new PAxiom();
    if (preproc)
    {
        delete preproc;
        preproc = 0;
    }
}

int PLSystem::is_item(unsigned int _n, vector<unsigned int>& _vec_input)
{
    signed int value = -1;
    for (unsigned int k = 0; k<_vec_input.size(); k++)
        if ( _vec_input.at(k) == _n )
        {
            value = (signed) k;
            break;
        }
    return value;
}

int PLSystem::is_item(string _s, vector<string>& _vec_input)
{
    signed int value = -1;
    for (unsigned int k = 0; k<_vec_input.size(); k++)
        if (_s.compare(_vec_input.at(k)) == 0)
        {
            value = (signed) k;
            break;
        }
    return value;
}

char PLSystem::at(unsigned int _index)
{
    return (_index<=text.length())?text[_index]:0;
}

/*
string PLSystem::pick_deriv_num(unsigned int _num_derivation)
{
    string value = string("");
    if ((_num_derivation >= 0)&&(_num_derivation<vec_str_hist.size())) value = vec_str_hist.at(_num_derivation);
    return value;
}
*/

string PLSystem::pick_char_at(unsigned int& _index, unsigned int& _char_size)
{
    string value("");
    _char_size = alphabet->search(text, _index);
    if (_char_size!=0)
        value = text.substr(_index,_char_size);
    else if (constant->is_constant(text[_index]))
    {
        _char_size = 1;
        value = text.substr(_index,1);
    }

    return value;
}

int PLSystem::load(string _productions)
{
    int value = 0;
    size_t start, end;
    string linebuffer;
    PRule* rulebuffer;
    PTextParser textparser(_productions);

    end = start = 0;
    textparser.clean_final_endl(_productions);

    while (end != _productions.size())
    {
        if (_productions[end] == '\n')
        {            
            linebuffer = _productions.substr(start, end-start);
            if (!linebuffer.compare("\n"))
            {
                rulebuffer = new PRule(linebuffer, alphabet);
                vec_rules.push_back(rulebuffer);
            }
            start = end + 1;
            value++;
        }
        else if (end == _productions.size()-1)
        {
            linebuffer = _productions.substr(start, end-start+1);
            if (linebuffer.empty()) break;

            rulebuffer = new PRule(linebuffer, alphabet);
            vec_rules.push_back(rulebuffer);
            value++;
        }
        end++;
    }

    return value;
}

void PLSystem::update_params(string _s_preproc, string _s_axiom, float _angle, unsigned int _target_deriv_num)
{
    if (!_s_preproc.empty())
    {
        preproc = new PPreproc();
        preproc->read(_s_preproc);
    }
    for (unsigned int i=0; i<preproc->getVecWord().size(); i++)
        alphabet->add(preproc->getVecWord()[i]->getText());
    this->target_deriv_num = _target_deriv_num;
    this->angle = _angle;
    /*
    if (preproc)
        _s_axiom = preproc->apply_define(_s_axiom);
    */
    std::remove(_s_axiom.begin(), _s_axiom.end(), ' ');
    setText(_s_axiom);

    axiom->setText(_s_axiom);
    if (!vec_rules.empty()) vec_rules.clear();
}

vector<string> PLSystem::parse_spaced_words(string _input)
{
    size_t start, end;
    string word_buffer;
    vector<string> value;

    end = start = 0;
    for (unsigned int i=0; i<_input.size(); i++)
    {
        if (_input[i] == '\n')
        {
            end = i-1;
            word_buffer = _input.substr(start, end-start);
            value.push_back(word_buffer);
            start=end+2;
            end=start;
        }
        else if (i == _input.size()-1)
        {
            end = _input.size()-1;
            word_buffer = _input.substr(start, end-start);
            value.push_back(word_buffer);
        }
    }
    return value;
}

string PLSystem::derive()
{
    unsigned int i;
    unsigned int rule_applied_idx;
    unsigned int char_size;
    string value;
    string prod_buffer;
    PTextParser textparser;
    /*
    vector<unsigned int> vec_dh_mapping_tmp;

    p_vec_dh = new vector<PDerivHistoric*>;
    */
    i = 0;
    while (i<text.length())
    {
        string alphabet_buffer;
        alphabet_buffer = pick_char_at(i, char_size);
        alphabet_buffer = textparser.clean_final_space(alphabet_buffer);
        prod_buffer     = apply_prod(alphabet_buffer, i, rule_applied_idx);

        value += prod_buffer;
        i+=char_size;
    }
    /*
    pvec_vec_deriv_hist.push_back(p_vec_dh);
    vec_dh_mapping = vec_dh_mapping_tmp;
    */
    num_derivation++;
    text = string(value);
    //vec_str_hist.push_back(text);
    return value;
}

unsigned int PLSystem::random(vector<double> _vec_probas)
{
    unsigned int value = 0, i;
    double sum = 0, random_tmp;
    vector <double> vec_normalized_probas;
    vector <double> vec_range;

    for (i=0; i<_vec_probas.size(); i++)
    {
        sum += _vec_probas.at(i);
    }

    for (i=0; i<_vec_probas.size(); i++)
    {
        vec_normalized_probas.push_back(_vec_probas.at(i)/sum);
    }

    for (i=0, sum=0; i<vec_normalized_probas.size(); i++)
    {
        sum += vec_normalized_probas.at(i);
        vec_range.push_back(sum);
    }
    random_tmp = (double) (rand() % 1000);
    random_tmp = random_tmp / 1000;
    for (i = 0; i<vec_range.size(); i++)
    {
        if (random_tmp>vec_range.at(i))
        {
            value++;
        }
        else break;
    }

    return value;
}

string PLSystem::apply_prod(string& _src, unsigned int& _index, unsigned int& _rule_number)
{
    string value = _src;
    unsigned int i;

    vector<int> vec_indices;
    vector<double> vec_stoch_params;

    for (i=0; i<vec_rules.size(); i++)
    {
        string rule_src_buffer = vec_rules.at(i)->getSource();
        if (_src.compare(rule_src_buffer) == 0)
        {
            if (!vec_rules.at(i)->getContext())
            {
                vec_indices.push_back(i);
                vec_stoch_params.push_back(vec_rules.at(i)->getStochasticParam());

            }
            // All the following lines are about L-Systems with contexts
            // with the next codes, the context was encapsulated within a class for more robustness
            // and adaptability of the app
            // The five lines below were added on 23th October version
            // For more debugging info see the old fully functional implementation in older version
            else
            {
                if (vec_rules.at(i)->getContext()->check_right(text, _index, preproc->getIgnore()) || vec_rules.at(i)->getContext()->check_left(text, _index, preproc->getIgnore()))
                {
                    vec_indices.push_back(i);
                    vec_stoch_params.push_back(vec_rules.at(i)->getStochasticParam());
                }
            }
        }
    }

    if (!vec_indices.empty())
    {
        _rule_number = vec_indices.at(random(vec_stoch_params));
        value = vec_rules.at(_rule_number)->getSubstitute();
    }

    return value;
}

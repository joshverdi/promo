#ifndef PLSYSTEM_H
#define PLSYSTEM_H

#include <string>
#include <vector>
#include <sstream>
#include <boost/bimap.hpp>

#include "engine/procedural.h"
#include "engine/pderivhistoric.h"
#include "engine/prule.h"
#include "grammar/pgrammar.h"
#include "grammar/palphabet.h"
#include "grammar/paxiom.h"
#include "grammar/pignore.h"
#include "grammar/pconstant.h"
#include "grammar/ppreproc.h"

class PLSystem
{
protected:
    int                 target_deriv_num;
    int                 num_derivation;
    float               angle;
    std::string         text;
    std::stringstream   raw;

    PAlphabet*          alphabet;
    PAxiom*             axiom;
    PConstant*          constant;
    PPreproc*           preproc;
    //std::vector<std::string>    vec_str_hist;
    std::vector<PRule*>         vec_rules;

    typedef boost::bimap<unsigned int, PDerivHistoric*> boost_bimap_dh;
    typedef boost::bimap<unsigned int, PDerivHistoric*>::value_type dh_match;
    typedef boost::bimap<unsigned int, PDerivHistoric*>::const_iterator iter_match;

    boost_bimap_dh bm_dh;

public:
    PLSystem();
    PLSystem(std::string _text);
    PLSystem(char* _text);
    ~PLSystem(); 
    virtual void init();
    int getNumDerivation() const {return num_derivation;}
    int getTargetDerivNum() const {return target_deriv_num;}
    void setTargetDerivNum(int _target_deriv_num) {target_deriv_num = _target_deriv_num;}

public:
    char at(unsigned int _index);
    float       getAngle() const {return angle;}
    PAlphabet*  getAlphabet() const {return alphabet;}
    PAxiom*     getAxiom() const {return axiom;}
    PPreproc*   getPreproc() const {return preproc;}
    void setPreproc(PPreproc* _preproc){preproc=_preproc;}
    std::string getText() const {return text;}
    void setText(std::string _text){text = std::string(_text);};
    void setAngle(float _angle){angle = _angle;};

public:
    std::string                                 pick_char_at(unsigned int& _index, unsigned int& _char_size);
    //std::string                                 pick_deriv_num(unsigned int _num_derivation);
    int                                         is_item(unsigned int _n, std::vector<unsigned int>& _vec_input);
    int                                         is_item(std::string _s, std::vector<std::string>& _vec_input);
    unsigned int                                random(std::vector<double> _vec_probas);
    std::vector<std::string>                    parse_spaced_words(std::string _input);
    bool                                        is_valid(unsigned int _idx){return (_idx<text.size())?true:false;}

public:
    void        update_params(std::string _s_preproc, std::string _s_axiom, float _angle, unsigned int _target_deriv_num);
    int         load(std::string _productions);          // stores all the rules in vec_rules
    std::string derive();                        // derive the whole string stores in std::string text property
    std::string apply_prod(std::string& _src, unsigned int& _index, unsigned int& _rule_number);
    // apply_prod a word by checking all rules whose predecessor is _src and return the successor if exists, _src otherwise
    // the _index parameter will be used for checking the contexts
};

#endif // PLSYSTEM_H

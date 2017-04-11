#ifndef PRULE_H
#define PRULE_H

#include <vector>

#include "grammar/pcontext.h"
#include "grammar/palphabet.h"
#include "procedural.h"

class PRule
{
    std::string text;
protected:
    std::string source;      // predecessor
    std::string substitute;  // successor
    double stochastic_param;
    PContext* context;
    std::string left_context, right_context;
    PAlphabet* alphabet;
    unsigned int apply_count;

public:
    PRule();
    PRule(char* _text, PAlphabet* _alphabet);
    PRule(std::string _text,  PAlphabet * _alphabet);
    ~PRule();
public:
    void setText(char* _text){text = std::string(_text);}
    void setText(std::string _text){text = _text;}
    std::string getText(void) const {return text;}
    void incr_applycount(){apply_count++;}
    unsigned int getapplycount() const {return apply_count;}
public:
    STRUCTURED_RULE load();
    PAlphabet* getAlphabet() const {return alphabet;};
    std::string getSource() const {return source;};
    std::string getSubstitute() const {return substitute;};
    double getStochasticParam() const {return stochastic_param;};
    PContext* getContext() const {return context;};
    void setSource(std::string _s){source = _s;};
    void setSubstitute(std::string _s){substitute = _s;};
    void setStochasticParam(double _x) {stochastic_param = _x;};
    void setContext(PContext* _context){context = _context;};
    std::string pick_source();  // source will contain not only the alphabet source but the contexts with the '<'('>') sign
    std::string pick_substitute();
    PContext* pick_context();
    double pick_stochastic();
    PRule& operator=(const PRule& _rule);
};

#endif // PRULE_H

#ifndef PBOOLEANOPERATOR_H
#define PBOOLEANOPERATOR_H

#include <string>
#include "pnumeric.h"
#include "grammar/palgebra.h"
#include "grammar/palphabet.h"

class PBooleanOperator : public PAlgebra
{
protected:
    std::string type;
    std::string text;
    unsigned int priority;
    PBooleanOperator* root;
public:
    PBooleanOperator();
    PBooleanOperator(std::string _text);
    PBooleanOperator(std::string _text, std::string _type);
    PBooleanOperator(std::string _text, unsigned int _priority);
    std::string getText(void) const {return text;};
    void setText(std::string _text){text = _text;};
    std::string getType(void) const {return type;};
    void setType(std::string _type){type = _type;};
    unsigned int getPriority(void) const {return priority;};
    void setPriority(unsigned int _priority){priority = _priority;};
    PBooleanOperator* getRoot(void) const {return root;};
    void setRoot(PBooleanOperator* _root){root = _root;};
public:
    virtual bool eval(std::string* _s_params, float* _value, unsigned int _nb_params)=0;
public:
    int is_higher_than(PBooleanOperator* _src);
};

#endif // PBOOLEANOPERATOR_H

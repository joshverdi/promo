#ifndef PNUMERIC_H
#define PNUMERIC_H

#include <string>

#include "grammar/palgebra.h"

class PNumeric : public PAlgebra
{
    PNumeric *root;
protected:
    std::string type;
    std::string text;
    unsigned int priority;
public:
    std::string getType(void) const {return type;};
    void setType(std::string _type){type = _type;};
    std::string getText(void) const {return text;};
    void setText(std::string _text){text = _text;};
    unsigned int getPriority(void) const {return priority;};
    void setPriority(unsigned int _priority){priority = _priority;};
    PNumeric* getRoot(void) const {return root;};
    void setRoot(PNumeric* _root){root = _root;};
public:
    PNumeric();
    PNumeric(char _c);
    PNumeric(unsigned int _priority);
    PNumeric(std::string _text, std::string _type);
    PNumeric(std::string _text, std::string _type, unsigned int _priority);
    ~PNumeric();
public:
    int check_type(std::string _src);
    int is_higher_than(PNumeric* _src); // returns +1 if current object is prior than (PNumeric*)_src, 0 if equal, and -1 if lower
    PNumeric* absolute_root();
    PNumeric* absolute_root(PNumeric* _pn);
    virtual bool is_tied(std::string& _var_name) = 0;
    virtual PNumeric& operator=(const PNumeric& _pn);
    virtual float eval(std::string* _s_params, float* _value, unsigned int _nb_params)=0;
};

#endif // PNUMERIC_H

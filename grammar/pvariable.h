#ifndef PVARIABLE_H
#define PVARIABLE_H

#include <string>

#include "engine/pnumeric.h"

class PArithmetic;

class PVariable : public PNumeric
{
    std::string name;
public:
    PVariable();
    PVariable(char _c);
    PVariable(std::string _name);
    PVariable(std::string _name, unsigned int _priority);
    PVariable(const PVariable& _pv);
    void setName(std::string _name){name = _name;};
    std::string getName() const {return name;};
    PVariable& operator=(const PVariable& _pv);
public:
    bool is_variable(std::string _s_input);
public:
    float eval(std::string* _s_params, float* _value, unsigned int _nb_params);
    bool is_similar(std::string _s_var);
    bool is_similar(PVariable& _var);
    bool is_tied(std::string& _var_name);
};

#endif // PVARIABLE_H

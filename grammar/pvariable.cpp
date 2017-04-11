#include <sstream>
#include "pvariable.h"

using namespace std;

PVariable::PVariable()
{
    name.clear();
}

PVariable::PVariable(char _c):PNumeric(_c)
{
    std::ostringstream buffer_stream;
    buffer_stream<<_c;
    name = buffer_stream.str();
}

PVariable::PVariable(string _name):PNumeric(_name, _name)
{
    name.assign(_name);
}

PVariable::PVariable(string _name, unsigned int _priority):PNumeric(_name, _name, _priority)
{
    name = _name;
}

PVariable::PVariable(const PVariable& _pv):name(_pv.getName())
{
}

PVariable& PVariable::operator=(const PVariable& _pv)
{
    type = _pv.getType();
    text = _pv.getText();
    priority = _pv.getPriority();
    name = _pv.getName();
    return *this;
}

bool PVariable::is_variable(string _s_input)
{
    bool value = true;
    for (unsigned int i = 0; i<_s_input.size(); i++)
    {
        if (!isalpha(_s_input[i]))
        {
            value = false;
            break;
        }
    }
    return value;
}

float PVariable::eval(string* _s_params, float* _value, unsigned int _nb_params)
{
    float value = 0;

    for (unsigned int i=0; i<_nb_params; i++)
    {
        if (!name.compare(_s_params[i]))
        {
            value = _value[i];
            break;
        }
    }
    return value;
}

bool PVariable::is_similar(string _s_var)
{
    if (!name.compare(_s_var)) return true;
    return false;
}

bool PVariable::is_similar(PVariable& _var)
{
    if (!name.compare(_var.getName())) return true;
    return false;
}

bool PVariable::is_tied(std::string& _var_name)
{
    if (getName().compare(_var_name)==0)
        return true;
    return false;
}

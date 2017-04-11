#include <string>
#include <math.h>

#include "engine/ptextparser.h"
#include "pdefine.h"

using namespace std;

PDefine::PDefine()
{
}

PDefine::PDefine(std::string _constant_name, float _constant_value)
{
    constant_name = _constant_name;
    constant_value = _constant_value;
}

string PDefine::apply(string& _src)
{
    size_t idx;
    PTextParser textparser;
    string value = _src;
    string replacing_str;

    replacing_str=textparser.float_to_stdstr(constant_value);

    while ((idx=value.find(constant_name))!=string::npos)
    {
        value.replace(idx, constant_name.length(), replacing_str);
    }
    return value;
}

PDefine& PDefine::operator=(const PDefine& _define)
{
    constant_name = _define.getConstantName();
    constant_value = _define.getConstantValue();
    return *this;
}

bool PDefine::operator==(const PDefine& _define)
{
    if (    (constant_name.compare(_define.getConstantName())==0)
            &&
            (fabs(constant_value-_define.getConstantValue())<0.001)
       )
        return true;
    return false;
}

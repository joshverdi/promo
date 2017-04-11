#include "pcomparisonoperator.h"

#include "procedural.h"
#include "pnumericvalue.h"
#include <math.h>

using namespace std;

PComparisonOperator::PComparisonOperator()
{
    right_member = left_member = 0;
}

PComparisonOperator::PComparisonOperator(std::string _op):PBooleanOperator(_op, _op)
{
}

PComparisonOperator::PComparisonOperator(std::string _text, unsigned int _priority):PBooleanOperator(_text, _priority)
{
}

PComparisonOperator::PComparisonOperator(string _type, PNumeric* _left_member, PNumeric* _right_member)
    :PBooleanOperator(_type)
{
    left_member = _left_member;
    right_member = _right_member;
    setType(_type);
}

PComparisonOperator::~PComparisonOperator()
{
    if (left_member) delete left_member;
    if (right_member) delete right_member;
}

bool PComparisonOperator::is_comparison_operator(string _src)
{
    bool value = false;
    if (    !_src.compare("=")  ||
            !_src.compare("==") ||
            !_src.compare("!=") ||
            !_src.compare("<")  ||
            !_src.compare(">")  ||
            !_src.compare("<=") ||
            !_src.compare(">=")
        )
        value = true;
    return value;
}

bool PComparisonOperator::is_comparison_operation(string _src)
{
    bool value = false;
    if ( (_src.find("=")  != _src.npos) ||
         (_src.find("==") != _src.npos) ||
         (_src.find("!=") != _src.npos) ||
         (_src.find("<")  != _src.npos) ||
         (_src.find(">")  != _src.npos) ||
         (_src.find("<=") != _src.npos) ||
         (_src.find(">=") != _src.npos)
       )
        value = true;
    return value;
}

int PComparisonOperator::operation_type(string _type)
{
    int value=0;
    if (!_type.compare("="))        value = EQUAL;
    else if (!_type.compare("=="))  value = EQUAL;
    else if (!_type.compare("!="))  value = DIFF;
    else if (!_type.compare("<"))   value = INF;
    else if (!_type.compare(">"))   value = SUP;
    else if (!_type.compare("<="))  value = INF_OR_EQUAL;
    else if (!_type.compare(">="))  value = SUP_OR_EQUAL;
    return value;
}

bool PComparisonOperator::assess(string _type, float _left,float _right)
{
    bool value = false;
    float delta = _left - _right;
    int op_type = operation_type(_type);

    switch(op_type)
    {
    case EQUAL:
        if (fabs(delta)<0.000001) value = true;
        break;
    case DIFF:
        if (fabs(delta)>0.000001) value = true;
        break;
    case INF:
        if (delta<0) value = true;
        break;
    case SUP:
        if (delta>0) value = true;
        break;
    case INF_OR_EQUAL:
        if (delta<=0) value = true;
        break;
    case SUP_OR_EQUAL:
        if (delta>=0) value = true;
        break;
    }

    return value;
}

bool PComparisonOperator::eval(string* _s_params, float* _value, unsigned int _nb_params)
{
    float left_value  = left_member->eval(_s_params, _value, _nb_params);
    float right_value = right_member->eval(_s_params, _value, _nb_params);
    return assess(getType(), left_value, right_value);
}

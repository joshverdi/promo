#include "pbooleanoperator.h"
#include "plogicaloperator.h"
#include "pcomparisonoperator.h"
#include "grammar/parithmetic.h"
#include "grammar/palphabet.h"
#include "procedural.h"

using namespace std;

PBooleanOperator::PBooleanOperator()
{
    priority = 0;
    root = 0;
}

PBooleanOperator::PBooleanOperator(string _text)
{
    priority = 0;
    root = 0;
    text = _text;
    if (PLogicalOperator::is_logical_operator(_text)) type = _text;
    else if (PLogicalOperator::is_logical_operation(_text))
    {
        if (_text.find("&")) type = "&";
        else if (_text.find("|")) type = "|";
        if (_text[0] == '(') text = _text.substr(1, _text.size()-1);
    }
    else if (PComparisonOperator::is_comparison_operator(_text)) type = _text;
    else if (PComparisonOperator::is_comparison_operation(_text))
    {
        if      (_text.find("==") != _text.npos) type = "==";
        else if (_text.find("!=") != _text.npos) type = "!=";
        else if (_text.find("<=") != _text.npos) type = "<=";
        else if (_text.find(">=") != _text.npos) type = ">=";
        else if (_text.find(">")  != _text.npos) type = ">" ;
        else if (_text.find("<")  != _text.npos) type = "<" ;
        else if (_text.find("=")  != _text.npos) type = "=" ;

        if (_text[0] == '(') text = _text.substr(1, _text.size()-1);
    }
}

PBooleanOperator::PBooleanOperator(std::string _text, std::string _type)
{
    priority = 0;
    root = 0;
    text = _text;
    type = _type;
}

PBooleanOperator::PBooleanOperator(string _text, unsigned int _priority)
{
    priority = _priority;
    root = 0;
    text = _text;
    if (PLogicalOperator::is_logical_operator(_text)) type = _text;
    else if (PLogicalOperator::is_logical_operation(_text))
    {
        if (_text.find("&")) type = "&";
        else if (_text.find("|")) type = "|";
        if (_text[0] == '(') text = _text.substr(1, _text.size()-1);
    }
    else if (PComparisonOperator::is_comparison_operator(_text)) type = _text;
    else if (PComparisonOperator::is_comparison_operation(_text))
    {
        if      (_text.find("==") != _text.npos) type = "==";
        else if (_text.find("!=") != _text.npos) type = "!=";
        else if (_text.find("<=") != _text.npos) type = "<=";
        else if (_text.find(">=") != _text.npos) type = ">=";
        else if (_text.find(">")  != _text.npos) type = ">" ;
        else if (_text.find("<")  != _text.npos) type = "<" ;
        else if (_text.find("=")  != _text.npos) type = "=" ;

        if (_text[0] == '(') text = _text.substr(1, _text.size()-1);
    }
}

int PBooleanOperator::is_higher_than(PBooleanOperator* _src)
{
    int value = -1;
    if (priority>_src->getPriority()) value = -1;
    else if (priority<_src->getPriority()) value = 1;
    else if (priority==_src->getPriority())
    {
        int this_type_buffer = Procedural::assign_op_type(type);
        int src_type_buffer  = Procedural::assign_op_type(_src->getType());
        if (this_type_buffer == src_type_buffer) value = 0;
        else if (this_type_buffer<src_type_buffer) value = +1;
        else if (this_type_buffer>src_type_buffer) value = -1;
    }
    return value;
}

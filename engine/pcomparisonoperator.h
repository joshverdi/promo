#ifndef PCOMPARISONOPERATOR_H
#define PCOMPARISONOPERATOR_H

#include <string>

#include "pbooleanoperator.h"
#include "pnumeric.h"

class PLogicalOperator;

class PComparisonOperator : public PBooleanOperator
{
    PLogicalOperator *root;
    PNumeric *left_member, *right_member;
public:
    PComparisonOperator();
    PComparisonOperator(std::string _op);
    PComparisonOperator(std::string _text, unsigned int _priority);
    PComparisonOperator(std::string _type, PNumeric* _left_member, PNumeric* _right_member);
    ~PComparisonOperator();
public:
    void setLeftMember(PNumeric* _left_member){left_member = _left_member;};
    void setRightMember(PNumeric* _right_member){right_member = _right_member;};
public:
    static bool is_comparison_operator(std::string _src);
    static bool is_comparison_operation(std::string _src);
    int operation_type(std::string _type);
    bool assess(std::string _type, float _left,float _right);
    bool eval(std::string* _s_params, float* _value, unsigned int _nb_params);
};

#endif // PCOMPARISONOPERATOR_H

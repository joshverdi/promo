#ifndef PLOGICALOPERATOR_H
#define PLOGICALOPERATOR_H

#include <string>
#include "pbooleanoperator.h"

class PComparisonOperator;

class PLogicalOperator : public PBooleanOperator
{
    PLogicalOperator *root;
    PBooleanOperator *left, *right;
public:
    PLogicalOperator();
    PLogicalOperator(char _op);
    PLogicalOperator(std::string _type, unsigned int _priority);
    PBooleanOperator* getLeft(void) const {return left;};
    void setLeft(PBooleanOperator* _left){left = _left;};
    PBooleanOperator* getRight(void) const {return right;};
    void setRight(PBooleanOperator* _right){right = _right;};
    PLogicalOperator* getRoot(void) const {return root;};
    void setRoot(PLogicalOperator* _root){root = _root;};
public:
    static bool is_logical_operator(std::string _src, bool _include_negation = true);
    static bool is_logical_operation(std::string _src);
    PLogicalOperator* attach(PBooleanOperator* _b_op);
    PLogicalOperator* attach(PLogicalOperator* _operator_root, PBooleanOperator* _operand_node);
    bool eval(std::string* _s_params, float* _value, unsigned int _nb_params);
};

#endif // PLOGICALOPERATOR_H

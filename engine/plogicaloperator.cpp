#include <sstream>
#include "plogicaloperator.h"

using namespace std;

PLogicalOperator::PLogicalOperator()
{
}

PLogicalOperator::PLogicalOperator(char _op)
{
    std::ostringstream buffer_stream;
    buffer_stream<<_op;
    string buffer_str = buffer_stream.str();
    setText(buffer_str);
    setType(buffer_str);
}

PLogicalOperator::PLogicalOperator(std::string _type, unsigned int _priority)
    :PBooleanOperator(_type, _priority)
{
}

bool PLogicalOperator::is_logical_operator(string _src, bool _include_negation)
{
    bool value = false;
    if ( (!_src.compare("&")) || (!_src.compare("|")) ) value = true;
    if ( (_include_negation)  || (!_src.compare("!")) ) value = true;
    return value;
}

bool PLogicalOperator::is_logical_operation(string _src)
{
    bool value = false;
    if ((_src.find("&") != _src.npos)||(_src.find("|") != _src.npos)) value = true;
    return value;
}

// Return here for the next steps
bool PLogicalOperator::eval(string* _s_params, float* _value, unsigned int _nb_params)
{
    bool value = false;
    if (left&&right)
    {
        bool left_value  = left->eval(_s_params, _value, _nb_params);
        bool right_value = right->eval(_s_params, _value, _nb_params);
        if (getType().compare("&")==0) value = left_value&right_value;
        else if (getType().compare("|")==0) value = left_value|right_value;
        return value;
    }
    return false;
}

PLogicalOperator* PLogicalOperator::attach(PBooleanOperator* _bo)
{
    PLogicalOperator* lo_ptr;
    PLogicalOperator *value;
    value = this;

    // if current priority is higher, the aim is to locate the PBooleanOperator where the current operator (this instance)
    // should be placed.
    if (is_higher_than(_bo)<0)
    {
        lo_ptr = getRoot();
        while((lo_ptr!=0)&&(lo_ptr->is_higher_than(_bo)<0)&&(lo_ptr->getRoot()!=0))
        {
            lo_ptr = lo_ptr->getRoot();
        }
        _bo->setRoot(lo_ptr);
        PLogicalOperator* lo_buf = dynamic_cast<PLogicalOperator*>(_bo);
        if (lo_buf!=0)
        {
            lo_buf->setLeft(lo_ptr->getRight());
            lo_ptr->setRight(_bo);
            value = lo_buf->getRoot();
        }
    }
    else if (is_higher_than(_bo)==0)
    {
        PBooleanOperator* bo_buf = getRight();
        lo_ptr = dynamic_cast<PLogicalOperator*>(bo_buf);
        while ((lo_ptr!=0)&&(lo_ptr->is_higher_than(_bo)==0)&&(lo_ptr->getRight()!=0))
        {
            bo_buf = lo_ptr->getRight();
            lo_ptr = dynamic_cast<PLogicalOperator*>(bo_buf);
        }
        _bo->setRoot(lo_ptr->getRoot());
        PLogicalOperator* lo_buf = dynamic_cast<PLogicalOperator*>(_bo);
        if (lo_buf!=0)
        {
            lo_buf->setLeft(lo_ptr);
            lo_ptr->setRight(lo_buf);
            value = lo_buf->getRoot();
        }
    }
    else if (is_higher_than(_bo)>0)
    {
        PBooleanOperator* bo_buf = getRight();
        lo_ptr = dynamic_cast<PLogicalOperator*>(bo_buf);
        while ((lo_ptr!=0)&&(lo_ptr->is_higher_than(_bo)==0)&&(lo_ptr->getRight()!=0))
        {
            bo_buf = lo_ptr->getRight();
            lo_ptr = dynamic_cast<PLogicalOperator*>(bo_buf);
        }
        _bo->setRoot(lo_ptr->getRoot());
        PLogicalOperator* lo_buf = dynamic_cast<PLogicalOperator*>(_bo);
        if (lo_buf!=0)
        {
            lo_buf->setLeft(lo_ptr);
            lo_ptr->setRight(lo_buf);
            value = lo_buf->getRoot();
        }
    }
    return value;
}

PLogicalOperator* PLogicalOperator::attach(PLogicalOperator* _operator_root, PBooleanOperator* _operand_node)
{
    PLogicalOperator* lo_ptr;
    PLogicalOperator *value;
    value = this;

    // if current priority is higher, the aim is to locate the PBooleanOperator where the current operator (this instance)
    // should be placed.
    if (is_higher_than(_operator_root)<0)
    {
        lo_ptr = getRoot();
        while((lo_ptr!=0)&&(lo_ptr->is_higher_than(_operator_root)<0)&&(lo_ptr->getRoot()!=0))
        {
            lo_ptr = lo_ptr->getRoot();
        }
        _operator_root->setRoot(lo_ptr);
        PLogicalOperator* lo_buf = dynamic_cast<PLogicalOperator*>(_operator_root);
        if (lo_buf!=0)
        {
            lo_buf->setLeft(lo_ptr->getRight());
            lo_ptr->setRight(_operator_root);

            _operator_root->setRight(_operand_node);
            _operand_node->setRoot(_operator_root);
            value = lo_buf->getRoot();
        }
    }
    else if (is_higher_than(_operator_root)==0)
    {
        PBooleanOperator* bo_buf = getRight();
        lo_ptr = dynamic_cast<PLogicalOperator*>(bo_buf);
        while ((lo_ptr!=0)&&(lo_ptr->is_higher_than(_operator_root)==0)&&(lo_ptr->getRight()!=0))
        {
            bo_buf = lo_ptr->getRight();
            lo_ptr = dynamic_cast<PLogicalOperator*>(bo_buf);
        }
        _operator_root->setRoot(lo_ptr->getRoot());
        PLogicalOperator* lo_buf = dynamic_cast<PLogicalOperator*>(_operator_root);
        if (lo_buf!=0)
        {
            lo_buf->setLeft(lo_ptr);
            lo_ptr->setRight(lo_buf);

            _operator_root->setRight(_operand_node);
            _operand_node->setRoot(_operator_root);
            value = lo_buf->getRoot();
        }
    }
    else if (is_higher_than(_operator_root)>0)
    {
        PBooleanOperator* bo_buf = getRight();
        lo_ptr = dynamic_cast<PLogicalOperator*>(bo_buf);
        while ((lo_ptr!=0)&&(lo_ptr->is_higher_than(_operator_root)==0)&&(lo_ptr->getRight()!=0))
        {
            bo_buf = lo_ptr->getRight();
            lo_ptr = dynamic_cast<PLogicalOperator*>(bo_buf);
        }
        _operator_root->setRoot(lo_ptr->getRoot());
        PLogicalOperator* lo_buf = dynamic_cast<PLogicalOperator*>(_operator_root);
        if (lo_buf!=0)
        {
            lo_buf->setLeft(lo_ptr);
            lo_ptr->setRight(lo_buf);

            _operator_root->setRight(_operand_node);
            _operand_node->setRoot(_operator_root);
            value = lo_buf->getRoot();
        }
    }
    return value;
}

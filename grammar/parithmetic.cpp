#include <sstream>

#include "engine/procedural.h"
#include "engine/pnumericvalue.h"
#include "parithmetic.h"
#include "pvariable.h"

using namespace std;

PArithmetic::PArithmetic()
{
    root = NULL;
    left = right = NULL;
}

PArithmetic::PArithmetic(char _op)
{
    root = NULL;
    left = right = NULL;
    std::ostringstream buffer_stream;
    buffer_stream<<_op;

    string op_buffer = buffer_stream.str();
    setText(op_buffer);
    setType(op_buffer);
}

PArithmetic::PArithmetic(std::string _op):PNumeric(_op, _op)
{
    root = NULL;
    left = right = NULL;
}

PArithmetic::PArithmetic(std::string _op, unsigned int _priority):PNumeric(_op, _op, _priority)
{
    root = NULL;
    left = right = NULL;
}

PArithmetic::PArithmetic(const PArithmetic& _parith):PNumeric(_parith.getText(), _parith.getType(), _parith.getPriority())
{
    PVariable* pv=NULL;
    PNumericValue* pnv=NULL;
    PArithmetic* pa=NULL;

    if (_parith.getRoot())
    {
        root = new PArithmetic();
        *root = *_parith.getRoot();
    }
    if (_parith.getLeft())
    {
        if ((pv=dynamic_cast<PVariable*>(_parith.getLeft())))
        {
            left = new PVariable(*pv);
            *left = *pv;
        }
        else if ((pnv=dynamic_cast<PNumericValue*>(_parith.getLeft())))
        {
            left = new PNumericValue(*pnv);
            *left = *pnv;
        }
        else if ((pa=dynamic_cast<PArithmetic*>(_parith.getLeft())))
        {
            left = new PArithmetic(*pa);
            *left = *pa;
        }
    }
    pv=NULL;
    pnv=NULL;
    pa=NULL;
    if (_parith.getRight())
    {
        if ((pv=dynamic_cast<PVariable*>(_parith.getRight())))
        {
            right = new PVariable(*pv);
            *right = *pv;
        }
        else if ((pnv=dynamic_cast<PNumericValue*>(_parith.getRight())))
        {
            right = new PNumericValue(*pnv);
            *right = *pnv;
        }
        else if ((pa=dynamic_cast<PArithmetic*>(_parith.getRight())))
        {
            right = new PArithmetic(*pa);
            *right = *pa;
        }
    }
}

PArithmetic::~PArithmetic()
{
    delete root;
    delete left;
    delete right;
}

PArithmetic& PArithmetic::operator=(const PArithmetic& _parith)
{
    PVariable* pv=NULL;
    PNumericValue* pnv=NULL;
    PArithmetic* pa=NULL;

    if (_parith.getRoot())
    {
        root = new PArithmetic();
        *root = *_parith.getRoot();
    }
    if (_parith.getLeft())
    {
        if ((pv=dynamic_cast<PVariable*>(_parith.getLeft())))
        {
            left = new PVariable(*pv);
            *left = *pv;
        }
        else if ((pnv=dynamic_cast<PNumericValue*>(_parith.getLeft())))
        {
            left = new PNumericValue(*pnv);
            *left = *pnv;
        }
        else if ((pa=dynamic_cast<PArithmetic*>(_parith.getLeft())))
        {
            left = new PArithmetic(*pa);
            *left = *pa;
        }
    }
    pv=NULL;
    pnv=NULL;
    pa=NULL;
    if (_parith.getRight())
    {
        if ((pv=dynamic_cast<PVariable*>(_parith.getRight())))
        {
            right = new PVariable(*pv);
            *right = *pv;
        }
        else if ((pnv=dynamic_cast<PNumericValue*>(_parith.getRight())))
        {
            right = new PNumericValue(*pnv);
            *right = *pnv;
        }
        else if ((pa=dynamic_cast<PArithmetic*>(_parith.getRight())))
        {
            right = new PArithmetic(*pa);
            *right = *pa;
        }
    }
    return *this;
}

float PArithmetic::eval(string* _s_params, float* _value, unsigned int _nb_params)
{
    int op_type_buffer = Procedural::assign_op_type(type);
    float left_buffer = left->eval(_s_params, _value, _nb_params);
    float right_buffer = right->eval(_s_params, _value, _nb_params);
    float value = left_buffer;
    switch(op_type_buffer)
    {
    case PWR:
        while (right_buffer-1 > 0)
        {
            value *= left_buffer;
            right_buffer--;
        }
        break;
    case MULTN:
        value = left_buffer*right_buffer;
        break;
    case DIVSN:
        value = left_buffer/right_buffer;
        break;
    case ADDTN:
        value = left_buffer+right_buffer;
        break;
    case SUBTN:
        value = left_buffer-right_buffer;
        break;
    }
    return value;
}

// coding notes : Controls over 0 nodes are incomplete and needs to be detailed
// validity of while loop is uncertain in some null condition
// this function must return the parent root
PArithmetic* PArithmetic::attach(PNumeric* _numeric)
{
    PArithmetic* arith_ptr;
    PArithmetic* value;
    value = this;
    if (is_higher_than(_numeric)<0)
    {
        arith_ptr = getRoot();
        while((arith_ptr!=0)&&(arith_ptr->is_higher_than(_numeric)<0)&&(arith_ptr->getRoot()!=0))
        {
            arith_ptr = arith_ptr->getRoot();
        }
        // _numeric is now higher in the tree
        _numeric->setRoot(arith_ptr);
        PArithmetic* arith_buf = dynamic_cast<PArithmetic*>(_numeric);
        if (arith_buf!=0)
        {
            arith_buf->setLeft(arith_ptr->getRight());
            arith_ptr->setRight(_numeric);
            value = arith_buf->getRoot();
        }
    }
    else if (is_higher_than(_numeric)==0)
    {
        PNumeric* num_buf = getRight();
        arith_ptr = dynamic_cast<PArithmetic*>(num_buf);
        while ((arith_ptr!=0)&&(arith_ptr->is_higher_than(_numeric)==0)&&(arith_ptr->getRight()!=0))
        {
            num_buf = arith_ptr->getRight();
            arith_ptr = dynamic_cast<PArithmetic*>(num_buf);
        }
        _numeric->setRoot(arith_ptr->getRoot());
        PArithmetic* arith_buf = dynamic_cast<PArithmetic*>(_numeric);
        if (arith_buf!=0)
        {
            arith_buf->setLeft(arith_ptr);
            arith_ptr->setRoot(arith_buf);
            value = arith_buf->getRoot();
        }
    }
    else if (is_higher_than(_numeric)>0)
    {
        PNumeric* num_buf = getRight();
        arith_ptr = dynamic_cast<PArithmetic*>(num_buf);
        while ((arith_ptr!=0)&&(arith_ptr->is_higher_than(_numeric)==0)&&(arith_ptr->getRight()!=0))
        {
            num_buf = arith_ptr->getRight();
            arith_ptr = dynamic_cast<PArithmetic*>(num_buf);
        }
        _numeric->setRoot(arith_ptr->getRoot());
        PArithmetic* arith_buf = dynamic_cast<PArithmetic*>(_numeric);
        if (arith_buf!=0)
        {
            arith_buf->setLeft(arith_ptr);
            arith_ptr->setRoot(arith_buf);
            value = arith_buf->getRoot();
        }
    }

    return value;
}

PArithmetic* PArithmetic::attach(PArithmetic* _operator_root, PNumeric* _operand_node)
{
    PArithmetic* arith_ptr;
    PArithmetic* value;
    value = this;
    if (is_higher_than(_operator_root)<0)
    {
        arith_ptr = getRoot();
        while((arith_ptr!=0)&&(arith_ptr->is_higher_than(_operator_root)<0)&&(arith_ptr->getRoot()!=0))
        {
            arith_ptr = arith_ptr->getRoot();
        }
        // _numeric is now higher in the tree
        _operator_root->setRoot(arith_ptr);
        PArithmetic* arith_buf = dynamic_cast<PArithmetic*>(_operator_root);
        if (arith_buf!=0)
        {
            arith_buf->setLeft(arith_ptr->getRight());
            arith_ptr->setRight(_operator_root);

            _operator_root->setRight(_operand_node);
            _operand_node->setRoot(_operator_root);
            value = arith_buf->getRoot();
        }
    }
    else if (is_higher_than(_operator_root)==0)
    {
        PNumeric* num_buf = getRight();
        arith_ptr = dynamic_cast<PArithmetic*>(num_buf);
        while ((arith_ptr!=0)&&(arith_ptr->is_higher_than(_operator_root)==0)&&(arith_ptr->getRight()!=0))
        {
            num_buf = arith_ptr->getRight();
            arith_ptr = dynamic_cast<PArithmetic*>(num_buf);
        }
        _operator_root->setRoot(arith_ptr->getRoot());
        PArithmetic* arith_buf = dynamic_cast<PArithmetic*>(_operator_root);
        if (arith_buf!=0)
        {
            arith_buf->setLeft(arith_ptr);
            arith_ptr->setRoot(arith_buf);

            _operator_root->setRight(_operand_node);
            _operand_node->setRoot(_operator_root);
            value = arith_buf->getRoot();
        }
    }
    else if (is_higher_than(_operator_root)>0)
    {
        PNumeric* num_buf = getRight();
        arith_ptr = dynamic_cast<PArithmetic*>(num_buf);
        while ((arith_ptr!=0)&&(arith_ptr->is_higher_than(_operator_root)==0)&&(arith_ptr->getRight()!=0))
        {
            num_buf = arith_ptr->getRight();
            arith_ptr = dynamic_cast<PArithmetic*>(num_buf);
        }
        _operator_root->setRoot(arith_ptr->getRoot());
        PArithmetic* arith_buf = dynamic_cast<PArithmetic*>(_operator_root);
        if (arith_buf!=0)
        {
            arith_buf->setLeft(arith_ptr);
            arith_ptr->setRoot(arith_buf);

            _operator_root->setRight(_operand_node);
            _operand_node->setRoot(_operator_root);
            value = arith_buf->getRoot();
        }
    }

    return value;
}

bool PArithmetic::is_tied(std::string& _var_name)
{
    return ( (left->is_tied(_var_name)) || (right->is_tied(_var_name)) );
}


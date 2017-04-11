#ifndef PARITHMETIC_H
#define PARITHMETIC_H

#include "engine/pnumeric.h"

class PArithmetic : public PNumeric
{
    PArithmetic* root;
    PNumeric *left, *right;
public:
    PArithmetic();
    PArithmetic(char _op);
    PArithmetic(std::string _op);
    PArithmetic(std::string _op, unsigned int _priority);
    PArithmetic(const PArithmetic& _pa);
    ~PArithmetic();
    PNumeric* getLeft(void) const {return left;};
    void setLeft(PNumeric* _left){left = _left;};
    PNumeric* getRight(void) const {return right;};
    void setRight(PNumeric* _right){right = _right;};
    PArithmetic* getRoot(void) const {return root;};
    void setRoot(PArithmetic* _root){root = _root;};
public:
    PArithmetic* attach(PNumeric* _numeric);
    PArithmetic* attach(PArithmetic* _operator_root, PNumeric* _operand_node);
    float eval(std::string* _s_params, float* _value, unsigned int _nb_params);
    bool is_tied(std::string& _var_name);
    PArithmetic& operator=(const PArithmetic& _parith);
};

#endif // PARITHMETIC_H

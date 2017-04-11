#ifndef PWORD_H
#define PWORD_H

#include <string>
#include <vector>

#include "grammar/psymbol.h"
#include "engine/pnumeric.h"
#include "engine/pnumericvalue.h"

class PWord : public PSymbol
{
    std::vector<PNumeric*> vec_numeric;
public:
    PWord();
    PWord(char _c);
    PWord(std::string _text);
    PWord(const PWord& _word);
    ~PWord();
    void add(PNumeric* _numeric);
    void add(float _val);
    unsigned int size() {return getText().size();}
    std::vector<PNumeric*> getVecNumeric() const {return vec_numeric;}
    PWord& operator=(const PWord& _word);

public:
    bool is_similar(std::string _text);
    bool is_similar(PWord* _pw);
    bool contains(std::string _text);
//    unsigned int assign_params(float* _params, unsigned int _nb_params);
//    unsigned int assign_params(std::vector<float>& _vec_params);
    std::string compute_str();
    std::string compute_str(std::string* _str_var, float* _params, unsigned int& _size);
};

#endif // PWORD_H

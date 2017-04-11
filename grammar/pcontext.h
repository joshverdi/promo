#ifndef PCONTEXT_H
#define PCONTEXT_H

#include <string>
#include <vector>
#include "pword.h"
#include "pignore.h"
#include "palphabet.h"

class PContext
{
    std::vector<PWord*> vec_left;
    std::vector<PWord*> vec_right;
    std::string text; // the whole string containing contexts and predecessor, separated by symbols < and >
    PAlphabet* alphabet;
public:
    PContext(PAlphabet* _alphabet);
    PContext(char* _text, PAlphabet* _alphabet);
    PContext(std::string _text, PAlphabet* _alphabet);
    ~PContext();
    unsigned int count_left(){return vec_left.size();};
    unsigned int count_right(){return vec_right.size();};
    std::vector<PWord*> getVecLeft() const {return vec_left;};
    std::vector<PWord*> getVecRight() const {return vec_right;};

public:
    std::vector<PWord*> regular_parsing(std::string& _src); // read string from left to right
    std::vector<PWord*> reverse_parsing(std::string& _src); // read string from right to left
    unsigned int load_from(std::string& _src);
    unsigned int load_from(std::string& _str_left, std::string& _str_right);
    bool check_right(std::string _current_derivation, unsigned int _index, PIgnore* _ignore);
    bool check_left(std::string _current_derivation, unsigned int _index, PIgnore* _ignore);
};

#endif // PCONTEXT_H

#ifndef PGRAMMAR_H
#define PGRAMMAR_H

#include <iostream>
#include <string>
#include <string.h>

class PGrammar
{
    std::string text;
    static const std::string DEFAULT_CONSTANT;
public:
    PGrammar();
    void setText(std::string _text){text = _text;}
    std::string getText(void) const {return text;}
    static std::string getDefaultConstant() {return DEFAULT_CONSTANT;}
public:
    static size_t c_str_length(char*);
};

#endif // PGRAMMAR_H

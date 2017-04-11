#ifndef PSYMBOL_H
#define PSYMBOL_H

#include <string>

class PSymbol
{
protected:
    std::string text;
    int degree;

public:
    unsigned long _hash_;
    PSymbol();
    PSymbol(char _c);
    PSymbol(std::string _text);
    std::string getText() const {return text;}
    void setText(std::string _text){text = _text;_hash_ = hash(text);}
    void setDegree(unsigned int _degree){degree = _degree;}
    unsigned int getDegree(){return degree;}
    virtual std::string compute_str(std::string*, float*, unsigned int&){return text;}
    PSymbol& operator=(const PSymbol& _pc);
    unsigned long hash(std::string str);
};

#endif // PSYMBOL_H

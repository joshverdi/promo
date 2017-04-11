#ifndef PAXIOM_H
#define PAXIOM_H

#include <string.h>
#include <string>

#include "pgrammar.h"

class PAxiom : public PGrammar
{
    std::string text;
public:
    PAxiom();
    PAxiom(std::string _text);
    PAxiom(const PAxiom& _a);
public:
    void setText(std::string _text){text = _text;}
    std::string getText(void) const {return text;}
};

#endif // PAXIOM_H

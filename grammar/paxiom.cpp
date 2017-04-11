#include "paxiom.h"

PAxiom::PAxiom(){}

PAxiom::PAxiom(std::string _text):text(_text)
{
}

PAxiom::PAxiom(const PAxiom& _a):text(_a.getText())
{
}

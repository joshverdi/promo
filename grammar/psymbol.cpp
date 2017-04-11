#include "psymbol.h"
#include <sstream>
#include <iostream>

PSymbol::PSymbol()
{
    degree=0;
}

PSymbol::PSymbol(char _c)
{
    std::ostringstream buffer_stream;
    buffer_stream<<_c;
    text = buffer_stream.str();
    _hash_ = hash(text);
    degree=0;
}

PSymbol::PSymbol(std::string _text)
{
    text = _text;
    _hash_ = hash(text);
    degree=0;
}

PSymbol& PSymbol::operator=(const PSymbol& _pc)
{
    text = _pc.getText();
    _hash_ = hash(text);
    return *this;
}

unsigned long PSymbol::hash(std::string str)
{
    unsigned long hash = 5381;
    int c;

    for (c=0; c < str.length();c++)
        hash = ((hash << 5) + hash) + str[c]; /* hash * 33 + c */

    return hash;
}

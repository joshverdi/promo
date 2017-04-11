#include "pinclude.h"

PInclude::PInclude()
{
}

PInclude::PInclude(std::string _text)
{
    text = _text;
}

PInclude& PInclude::operator=(const PInclude& _include)
{
    text = _include.getText();
    return *this;
}

#ifndef PINCLUDE_H
#define PINCLUDE_H
#include <string>

class PInclude
{
    std::string text;
public:
    PInclude();
    PInclude(std::string _text);
    PInclude& operator=(const PInclude& _include);
    std::string getText() const {return text;}
};

#endif // PINCLUDE_H

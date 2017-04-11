#ifndef PIGNORE_H
#define PIGNORE_H

#include <string>
#include <vector>
#include "pgrammar.h"

class PIgnore : public PGrammar
{
    std::vector<std::string> vec_signore;
    std::string DEFAULT;
public:
    PIgnore();
    //PIgnore(char* _text);
    PIgnore(std::vector<std::string> _vec_signore);
    ~PIgnore();
public:
    void setIgnored(std::vector<std::string> _ignored){vec_signore = _ignored;}
    std::vector<std::string> getIgnored(void) const;
public:
    bool is_ignored(std::string _s);
    bool is_ignored(char _c);
    void add(std::string _s);
    PIgnore& operator=(const PIgnore& _ignore);
};

#endif // PIGNORE_H

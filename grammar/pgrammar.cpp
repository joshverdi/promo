#include <string>
#include "pgrammar.h"

using namespace std;

const string PGrammar::DEFAULT_CONSTANT("+-&^\\/|!");

PGrammar::PGrammar()
{
}

size_t PGrammar::c_str_length(char* _src)
{
    size_t n;
    for (n=0; _src[n]!='\0'; n++);
    return n;
}




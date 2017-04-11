#include "prule.h"
#include <string>
#include <ctype.h>

using namespace std;

PRule::PRule()
{
    alphabet=0;
    stochastic_param = 1;
    context=0;
    apply_count=0;
}

PRule::PRule(char* _text, PAlphabet * _alphabet)
{
    context=0;
    alphabet=_alphabet;
    text = string(_text);
    stochastic_param = pick_stochastic();
    load();
    apply_count=0;
}

PRule::PRule(string _text,  PAlphabet * _alphabet)
{
    context=0;
    alphabet=_alphabet;
    text = _text;
    stochastic_param = pick_stochastic();
    load();
    apply_count=0;
}

PRule::~PRule()
{
    if (context) delete context;
}

STRUCTURED_RULE PRule::load()
{
    STRUCTURED_RULE result;

    result.source     = pick_source();
    result.substitute = pick_substitute();

    setSource(result.source);
    setSubstitute(result.substitute);
    pick_context();

    return result;
}

string PRule::pick_source()
{
    unsigned int i = 0;
    string::size_type index;
    string str_buffer("");
    string result("");

    str_buffer += text;

    index = str_buffer.find("->");
    if ( index > 0) result = str_buffer.substr(0, index);

    if (result[0] == '.')
    {
        for (i=1; isdigit(result[i]) != 0; i++);
        result = result.erase(0, i);
    }

    return result;
}

string PRule::pick_substitute()
{
    size_t my_index;
    string str_buffer("");
    string result("");

    str_buffer += text;

    my_index = str_buffer.find("->");
    if ( my_index!= string::npos ) result = str_buffer.substr(my_index + 2);

    return result;
}

double PRule::pick_stochastic()
{
    unsigned int i = 0;
    char c = 0;
    double value=1, temp=0;

    if (text[0] == '.')
    {
        value = 0;
        for (i=1; isdigit(text[i]) != 0; i++)
        {
            c = text[i];
            temp = (double)atoi(&c);
            value += temp*pow((double)10, (double) (-1)*i);
        }
    }
    stochastic_param = value;
    return value;
}

PContext* PRule::pick_context()
{
    size_t idx;
    string buffer;
    //if ((idx=source.find("->"))!= string::npos)
    buffer = source;//.substr(0, idx);
    int tmp = buffer.find(':');
    int tmp1=buffer.find_first_of('<');
    int tmp2=buffer.find_first_of('>');
    if(tmp!=buffer.npos)
    {
        if ((tmp>=tmp1 && tmp1!=string::npos)||(tmp>=tmp2&& tmp2!=string::npos))
            context = new PContext(buffer, alphabet);
    }
    else if ((buffer.find('<')!=string::npos)||((buffer.find('>')!=string::npos)))
        context = new PContext(buffer, alphabet);
    return context;
}

PRule& PRule::operator=(const PRule& _rule)
{
    if(_rule.getContext()!=0)
        context= _rule.getContext();
    alphabet= _rule.getAlphabet();
    text = _rule.getText();
    stochastic_param = pick_stochastic();
    load();
    return *this;
}





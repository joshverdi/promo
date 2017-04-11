#include "pignore.h"
#include "engine/procedural.h"

using namespace std;

PIgnore::PIgnore()
{

}

//PIgnore::PIgnore(char* _text)
//{
//}

PIgnore::PIgnore(std::vector<string> _vec_signore)
{
    for(std::vector<string>::iterator iter=_vec_signore.begin(); iter!=_vec_signore.end(); iter++)
    {
        vec_signore.push_back(*iter);
    }
}

PIgnore::~PIgnore()
{
}

std::vector<std::string> PIgnore::getIgnored(void) const
{
    return vec_signore;
}

void PIgnore::add(string _s)
{
    unsigned int i=0;
    std::string str = _s;
    while(i<_s.size())
    {
        std::string tmp="";
        int jump=str.find_first_of(',');
        if(jump!=str.npos)
        {
            tmp = str.substr(0,jump);
            str=str.substr(jump+1,str.length());
        }
        else
        {
            jump=str.length();
            tmp=str;
        }
        i+=jump+1;

        vec_signore.push_back(tmp);

    }

}

bool PIgnore::is_ignored(string _s)
{
    for(std::vector<string>::iterator iter=vec_signore.begin(); iter!=vec_signore.end(); iter++){
        if(*iter==_s)
            return true;
    }
    return false;
}
bool PIgnore::is_ignored(char _c)
{
    string tmp(1,_c);
    for(std::vector<string>::iterator iter=vec_signore.begin(); iter!=vec_signore.end(); iter++){
        if(*iter==tmp)
            return true;
    }
    return false;
}
PIgnore& PIgnore::operator=(const PIgnore& _ignore)
{
    vec_signore=_ignore.getIgnored();
    return *this;
}

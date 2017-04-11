#include "palphabet.h"
#include "pword.h"
#include "pgrammar.h"
#include "engine/procedural.h"

#include <string>
#include <algorithm>
#include <QFile>

using namespace std;

size_t PAlphabet::max_size = 1;

PAlphabet::PAlphabet()
{
    text     = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const char TEXT[]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char SHIFT[] = "abcdefghijklmnopqrstuvwxyz";
    const char VAR[]   = "abcdefghijklmnopqrstuvwxyz";
    for (int i=0; TEXT[i]!='\0'; i++)
        vec_alphabet.push_back(std::string(1, TEXT[i]));
    for (int i=0; SHIFT[i]!='\0'; i++)
        vec_shift.push_back(std::string(1, SHIFT[i]));
    for (int i=0; VAR[i]!='\0'; i++)
        vec_var.push_back(std::string(1, VAR[i]));
}

bool PAlphabet::is_alphabet(char _c)
{
    return (std::find(vec_alphabet.begin(), vec_alphabet.end(), std::string(1,_c))!=vec_alphabet.end())?true:false;
}

bool PAlphabet::is_alphabet(string _s)
{
    return (std::find(vec_alphabet.begin(), vec_alphabet.end(), _s)!=vec_alphabet.end())?true:false;
}

bool PAlphabet::is_alphabet(PWord* _word)
{
    return (std::find(vec_alphabet.begin(), vec_alphabet.end(), _word->getText())!=vec_alphabet.end())?true:false;
}

bool PAlphabet::is_shifting(char _c)
{
    return (std::find(vec_shift.begin(), vec_shift.end(), std::string(1,_c))!=vec_shift.end())?true:false;
}

bool PAlphabet::is_variable(char _c)
{
    return (std::find(vec_var.begin(), vec_var.end(), std::string(1,_c))!=vec_var.end())?true:false;
}

bool PAlphabet::is_constant(char c)
{
    return (PGrammar::getDefaultConstant().find(c)!=string::npos)?true:false;
}

bool PAlphabet::is_constant(string _s)
{
    return (PGrammar::getDefaultConstant().find(_s)!=string::npos)?true:false;
}

string PAlphabet::pick_first_alphabet_after(unsigned int _index, string _txt)
{
    string value=0;
    if (is_alphabet(_txt[_index]))
    {
        value+=_txt[_index];
        return value;
    }
    do
    {
        _index++;
        if (_index<_txt.size())
        {
            if (is_alphabet(_txt[_index]))
            {
                value+=_txt[_index];
                return value;
            }
        }
    } while (_index<_txt.size());
    return value;
}

string PAlphabet::pick_first_alphabet_before(unsigned int _index, string _txt)
{
    string value=0;
    if (is_alphabet(_txt[_index]))
    {
        value+=_txt[_index];
        return value;
    }
    do
    {
        _index--;
        if (_index<_txt.size())
        {
            if (is_alphabet(_txt[_index]))
            {
                value+=_txt[_index];
                return value;
            }
        }
    } while (_index!=0);
    return value;
}

void PAlphabet::add(PWord* _w)
{
    max_size = PROCEDURAL_MAX(max_size, _w->getText().size());
    vec_alphabet.push_back(_w->getText());
}
void PAlphabet::add(std::string _new_alphabet)
{
    max_size = PROCEDURAL_MAX(max_size, _new_alphabet.size());
    vec_alphabet.push_back(_new_alphabet);
}

// This function returns 0 if it does not find any alphabet starting from _idx
// otherwise it will return the size of the alphabet found
size_t PAlphabet::search(const std::string& _src, unsigned int _idx)
{
    std::string s = _src.substr(_idx, max_size);
    std::string longest_str;
    std::vector<std::string> vec_found;
    std::vector<std::string>::iterator iter_found;
    size_t value=0;
    for (std::vector<std::string>::iterator iter = vec_alphabet.begin(); iter!=vec_alphabet.end(); iter++)
    {
        if (s.find_first_of(*iter)==0)
            vec_found.push_back(*iter);
    }

    for (std::vector<std::string>::iterator iter = vec_found.begin(); iter!=vec_found.end(); iter++)
    {
        if ((*iter).size()>value)
            value = (*iter).size();
    }
    return value;
}

// This function returns 0 if it does not find any alphabet ending to _idx
// otherwise it will return the size of the alphabet found
size_t PAlphabet::search_reverse(const std::string& _src)
{
    std::string longest_str;
    std::vector<std::string> vec_found;
    std::vector<std::string>::iterator iter_found;
    size_t value=0;
    for (std::vector<std::string>::iterator iter = vec_alphabet.begin(); iter!=vec_alphabet.end(); iter++)
    {
        string tmp=*iter;
        if(tmp=="bravo"){
            int i = 2;
        }
        int dbg =_src.find_last_of(tmp);
        if (dbg ==_src.length()-1)
            vec_found.push_back(tmp);
    }

    for (std::vector<std::string>::iterator iter = vec_found.begin(); iter!=vec_found.end(); iter++)
    {
        if ((*iter).size()>value)
            value = (*iter).size();
    }
    return value;
}

PAlphabet& PAlphabet::operator=(const PAlphabet& _pa)
{
    vec_alphabet = _pa.getVecAlphabet();
    vec_shift = _pa.getVecShift();
    vec_var = _pa.getVecVar();
    text = _pa.getText();
}

bool PAlphabet::operator==(const PAlphabet& _alphabet)
{
    return ((vec_alphabet==_alphabet.vec_alphabet)&&(vec_var==_alphabet.getVecVar())&&(vec_shift==_alphabet.getVecShift()));
}

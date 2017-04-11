#include <string>

#include "pcontext.h"
#include "palphabet.h"
#include "pvariable.h"

#include "engine/procedural.h"
#include "engine/ptextparser.h"

using namespace std;

PContext::PContext(PAlphabet *_alphabet)
{
    alphabet = _alphabet;
}

PContext::PContext(char* _text, PAlphabet* _alphabet)
{
    alphabet = _alphabet;
    text = string("");
    text += _text;
    load_from(text);
}

PContext::PContext(string _text, PAlphabet *_alphabet)
{
    alphabet = _alphabet;
    text = _text;
    load_from(_text);
}

PContext::~PContext()
{
    vec_left.clear();
    vec_right.clear();
    delete alphabet;
}

vector<PWord*> PContext::regular_parsing(string& _src)
{
    std::vector<PWord*> value;
    std::vector<string> vec_str_buffer;
    PWord* pw_tmp;
    PVariable* pvar;
    unsigned int i=0;
    unsigned int jump;
    unsigned int closing_idx=0;
    string str_buffer;
    PTextParser textparser;
    int degree=0;

    while (i<_src.size())
    {
        jump=1;
        if(_src[i]=='[')
            degree++;
        if(_src[i]== ']')
            degree--;
        int wrd_size = alphabet->search(_src.substr(i, _src.npos),0);
        PWord* word;
        if(wrd_size<=1)
        {
            word= new PWord(_src[i]);
            jump=1;
        }
        else {
            word = new PWord(_src.substr(i, wrd_size));
            jump=wrd_size;
        }

        if (alphabet->is_alphabet(word))
        {
            word->setDegree(degree);
            if (i+jump<_src.size())
            {
                if (_src[i+jump]=='(')
                {
                    closing_idx = textparser.find_bracket_closer(_src, i+jump);
                    str_buffer = _src.substr(i+jump+1, closing_idx-i-jump-1);
                    vec_str_buffer = textparser.arrange_sequence(str_buffer);
                    for (unsigned int j=0; j<vec_str_buffer.size(); j++)
                    {
                        pvar = new PVariable(vec_str_buffer.at(j));
                        word->add(pvar);
                    }
                }
            }
            value.push_back(word);
            if(closing_idx!=0)
                jump = closing_idx-i+1;
        }
        i+=jump;
    }
    return value;
}

vector<PWord*> PContext::reverse_parsing(std::string& _src)
{
    std::vector<PWord*> value;
    std::vector<string> vec_str_buffer;
    PWord* pw_tmp;
    PVariable* pvar;
    unsigned int i=0;
    unsigned int jump;
    unsigned int opening_idx;
    string str_buffer;
    PTextParser textparser;
    int degree=0;

    i = _src.size()-1;
    while (i<_src.size()) // since i is unsigned, we should be aware of the final value
    {
         jump = 1;

         if(_src[i]==']')
             degree++;
         if(_src[i]=='[')
             degree--;

         if (_src[i]==')')
         {
             opening_idx = textparser.find_bracket_opener(_src, i);
             if (opening_idx>0)
             {
                 int wrd_size = alphabet->search_reverse(_src.substr(0,opening_idx-1));
                 PWord* word;
                 if(wrd_size<2)
                 {
                     word= new PWord(_src[opening_idx-1]);
                     jump=1;
                 }
                 else {
                     word = new PWord(_src.substr(opening_idx-wrd_size, wrd_size));
                     jump=wrd_size;
                 }
                 jump+=i-opening_idx;
                 word->setDegree(degree);
                 value.push_back(word);

                 if (alphabet->is_alphabet(word))
                 {
                     str_buffer = _src.substr(opening_idx+1, i-opening_idx-1);
                     vec_str_buffer = textparser.arrange_sequence(str_buffer);
                     for (unsigned int j=0; j<vec_str_buffer.size(); j++)
                     {
                         pvar = new PVariable(vec_str_buffer.at(j));
                         word->add(pvar);
                     }
                     value.push_back(pw_tmp);
                 }
             }
         }
         else
         {
             int wrd_size = alphabet->search_reverse(_src.substr(0,i));
             PWord* word;
             if(wrd_size<2)
             {
                 word= new PWord(_src[i]);
                 jump=1;
             }
             else {
                 word = new PWord(_src.substr(i-wrd_size+1, wrd_size));
                 jump=wrd_size;
             }

             word->setDegree(degree);
             value.push_back(word);
         }
         i-=jump;
    }
    return value;
}

unsigned int PContext::load_from(std::string& _src)
{
    string str_left, str_right;
    string conditless_src = _src;
    size_t separ_idx;
    if ((separ_idx=_src.find(":"))!=string::npos)
    {
        conditless_src = _src.substr(0, separ_idx);
    }
    size_t i;
    if ((i=conditless_src.find('<'))!=string::npos)
        str_left = conditless_src.substr(0, i);
    if ((i=conditless_src.find('>'))!=string::npos)
        str_right = conditless_src.substr(i+1);
    return load_from(str_left, str_right);
}

unsigned int PContext::load_from(std::string& _str_left, std::string& _str_right)
{
    if (!_str_left.empty()) vec_left = reverse_parsing(_str_left);
    if (!_str_right.empty()) vec_right = regular_parsing(_str_right);
    return (vec_left.size()+vec_right.size());
}

bool PContext::check_left(string _current_derivation, unsigned int _index, PIgnore* _ignore)
{
    bool value = true;
    unsigned int i=0;
    int j1=0;
    PTextParser textparser;
    int degree=0; //compteur de bracket
    i=0;

    int pw_length = alphabet->search_reverse(_current_derivation.substr(0,_index));
    if(pw_length <2){
        j1=_index-1;
    }
    else j1=_index-pw_length;

    if (j1==-1)
        return false;
    while (i<vec_left.size())
    {
        int jump = 1;
        int current_degree=vec_left.at(i)->getDegree();

        if(j1<0)
            return false;
        if (degree<0)
            degree=0;
        else if(_current_derivation[j1]==']')
        {
            degree++;
            j1--;
        }
        else if (_current_derivation[j1]=='[')
        {
            degree--;
            j1--;
        }

        else if (_current_derivation[j1]==')')
        {
            j1=textparser.find_bracket_opener(_current_derivation, j1);
            j1--;
        }

        else if(degree<current_degree)
        {
            return false;
        }
        else if (degree==current_degree)
        {
            int wrd_size = alphabet->search_reverse(_current_derivation.substr(0,j1));
            PWord* word;
            if(wrd_size<2)
            {
                word= new PWord(_current_derivation[j1]);
                jump=1;
            }
            else {
                word = new PWord(_current_derivation.substr(j1-wrd_size+1, wrd_size));
                jump=wrd_size;
            }

            if (_ignore->is_ignored(word->getText()))
            {
                j1--;
            }
            else if (alphabet->is_alphabet(word))
            {
                if (!vec_left.at(i)->is_similar(word) || current_degree!=degree)
                {
                    return false;
                }
                i++;
                j1-=jump;
            }
            else {
                return false;
            }
            delete word;
        }
        else j1--;

    }

    return value;
}

bool PContext::check_right(string _current_derivation, unsigned int _index, PIgnore* _ignore)
{
    bool value = true;
    unsigned int i=0, j2;
    PTextParser textparser;
    int degree = 0;

    i=0;

    int pw_length = alphabet->search(_current_derivation.substr(_index, _current_derivation.npos), 0);
    if(pw_length <2){
        j2=_index+1;
    }
    else j2=_index+pw_length;

    if (j2==_current_derivation.size() || j2==0)
        return false;
    while ((i<vec_right.size()))//&&(j2<=_current_derivation.size()-1))
    {
        int current_degree=vec_right.at(i)->getDegree();
        int jump = 1;

        if(j2>_current_derivation.size())
            return false;

        if (_current_derivation[j2]=='[')
        {
            degree++;
            j2++;
        }
        else if (_current_derivation[j2]==']')
        {
            degree--;
            j2++;
        }
        else if (_current_derivation[j2]=='(')
        {
            j2=textparser.find_bracket_closer(_current_derivation, j2);
            j2++;
        }

        else if(degree<current_degree)
        {
            return false;
        }
        else if (degree==current_degree)
        {
            int wrd_size = alphabet->search(_current_derivation.substr(j2,_current_derivation.npos),0);
            PWord* word;
            if(wrd_size<2)
            {
                word= new PWord(_current_derivation[j2]);
                jump=1;
            }
            else {
                word = new PWord(_current_derivation.substr(j2, wrd_size));
                jump=wrd_size;
            }
            if (_ignore->is_ignored(word->getText()))
            {
                j2+=jump;
            }
            else if (alphabet->is_alphabet(word))
            {
                if (!vec_right.at(i)->is_similar(word) || current_degree!=degree)
                {
                    return false;
                }
                i++;
                j2+=jump;
            }
            else
                return false;
            delete word;
        }
        else j2++;
    }
    return value;
}

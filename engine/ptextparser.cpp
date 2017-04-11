#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include "procedural.h"
#include "ptextparser.h"
#include "grammar/palphabet.h"

using namespace std;

PTextParser::PTextParser()
{
}

PTextParser::PTextParser(std::string _text)
{
    text = _text;
}

PTextParser::PTextParser(float _x)
{
    float x = _x;
    text = float_to_stdstr(x);
}

std::string PTextParser::format_value(float _x, unsigned int _precision)
{
    std::ostringstream buffer_stream;
    buffer_stream.precision(_precision);
    if ((_x<0.001)&&(_x>-0.001))
        buffer_stream << 0;
    else
        buffer_stream << _x;
    return buffer_stream.str();
}

string& PTextParser::clean_final_endl(string& _src)
{
    size_t length = _src.length();
    if (_src[length-1] == '\n')
    {
        _src = _src.substr(0, length-1);
        clean_final_endl(_src);
    }
    return _src;
}

string& PTextParser::clean_final_space(string& _src)
{
    size_t length = _src.length();
    if (_src[length-1] == ' ')
    {
        _src = _src.substr(0, length-1);
        clean_final_space(_src);
    }
    return _src;
}

string& PTextParser::clean_all_spaces(string& _src)
{
    size_t n=0;
    do
    {
        if ((n = _src.find(' ')) != _src.npos) _src.erase(_src.begin()+n);
    }
    while (n != _src.npos);
    return _src;
}

string& PTextParser::clean_multispaces(string& _src)
{
    size_t n=0;
    do
    {
        n = _src.find(' ');
        if ((n != _src.npos)&&((n+1)<_src.length()))
            if (_src[n+1] == ' ')
                _src.erase(_src.begin()+n);
    }
    while (n != _src.npos);
    return _src;
}

float PTextParser::stdstr_to_float(std::string& _src)
{
//    float value = 0;
//    std::istringstream buffer_stream(_src);
//    buffer_stream>>value;
//    return value;
    float value;
    try
    {
        value = boost::lexical_cast<float>(_src);
    }
    catch(boost::bad_lexical_cast const&)
    {
        std::cout<<std::endl<<"Boost Lexical Cast Exception";
        return 0;
    }
    return value;
}

string PTextParser::float_to_stdstr(float& _val)
{
    std::ostringstream buffer_stream;
    buffer_stream<<_val;
    return buffer_stream.str();
}

float PTextParser::extract_number(std::string& _src, unsigned int _index, unsigned int& _output_size, char _decimal_separator)
{
    float value = 0;
    char c;
    unsigned int i = _index;
    string buffer;

    do
    {
        i++;
        c=_src[i];
    }
    while ((isdigit(c)||Procedural::is_decimal_separator(c))&&(i<_src.size()));

    buffer = _src.substr(_index, i-_index);
    _output_size = buffer.size();
    std::istringstream buffer_stream(buffer);
    buffer_stream>>value;
    return value;
}

vector<string> PTextParser::arrange_sequence(string& _src)
{
    vector<string> value;
    unsigned int start, end;
    string buffer;
    start = 0;
    end = start + 1;
    while(end<_src.size())
    {
        if (_src[end]==',')
        {
            buffer = _src.substr(start, end-start);
            value.push_back(buffer);
            start = end+1;
        }
        end++;
    }
    buffer = _src.substr(start, end-start);
    value.push_back(buffer);
    return value;
}

vector<string> PTextParser::arrange_sequence(string& _src, char _c_separator)
{
    vector<string> value;
    unsigned int start, end;
    string buffer;
    start = 0;
    end = start + 1;
    while(end<_src.size())
    {
        if (_src[end]==_c_separator)
        {
            buffer = _src.substr(start, end-start);
            value.push_back(buffer);
            start = end+1;
        }
        end++;
    }
    buffer = _src.substr(start, end-start);
    value.push_back(buffer);
    return value;
}

unsigned int PTextParser::find_bracket_closer(string _s, unsigned int _index)
{
    unsigned int depth, value;

    depth = 0;
    value = 0;
    if (_index<_s.size())
    {
        if (_s[_index] == '(')
        {
            ++depth;
            ++_index;

            while ((depth != 0)&&(_index<_s.size()))
            {
                switch(_s[_index])
                {
                case '(':
                    depth++;
                    _index++;
                    break;
                case ')':
                    depth--;
                    if (depth == 0) return value = _index;
                    _index++;
                    break;
                default:
                    _index++;
                    break;
                }
            }
        }
    }
    return value;
}

unsigned int PTextParser::find_bracket_opener(std::string _s, unsigned int _index)
{
    unsigned int depth, value;
    value = 0;
    if ((_index<_s.size())&&(_index>0))
    {
        if (_s[_index] == ')')
        {
            depth = 1;
            --_index;
            while (depth != 0)
            {
                switch(_s[_index])
                {
                case '(':
                    depth--;
                    if (depth == 0) return (value = _index);
                    _index--;
                    break;
                case ')':
                    depth++;
                    _index--;
                    break;
                default:
                    _index--;
                    break;
                }
            }
        }
    }
    return value;
}

unsigned int PTextParser::find_bracket_closer(unsigned int _index)
{
    return find_bracket_closer(text, _index);
}

unsigned int PTextParser::find_bracket_opener(unsigned int _index)
{
    return find_bracket_opener(text, _index);
}

string PTextParser::make_readable(std::string _src)
{
    string value;
    PAlphabet alphabet;
    unsigned int offset = 1;
    unsigned int closer;
    unsigned int i=0;
    while (i<_src.size())
    {
        offset = 1 ;
        if (alphabet.is_alphabet(_src[i]))
        {
            value+=_src.substr(i,1);
            if (i+1<_src.size())
            {
                if (_src[i+1]=='(')
                {
                    closer = find_bracket_closer(_src, i+1);
                    value+=_src.substr(i+1,closer-i);
                    offset = closer-i+1;
                }
            }
        }
        else if ((_src[i]=='[')||(_src[i]==']'))
        {
            value+=_src.substr(i,1);
        }

        i+=offset;
    }
    return value;
}

unsigned int PTextParser::find_sqbracket_closer(string _s, unsigned int _index)
{
    unsigned int depth, value;

    depth = 0;
    value = 0;
    if (_index<_s.size())
    {
        if (_s[_index] == '[')
        {
            ++depth;
            ++_index;

            while ((depth != 0)&&(_index<_s.size()))
            {
                switch(_s[_index])
                {
                case '[':
                    depth++;
                    _index++;
                    break;
                case ']':
                    depth--;
                    if (depth == 0) return value = _index;
                    _index++;
                    break;
                default:
                    _index++;
                    break;
                }
            }
        }
    }
    return value;
}

unsigned int PTextParser::find_sqbracket_opener(std::string _s, unsigned int _index)
{
    unsigned int depth, value;
    value = 0;
    if ((_index<_s.size())&&(_index>0))
    {
        if (_s[_index] == ']')
        {
            depth = 1;
            --_index;
            while (depth != 0)
            {
                switch(_s[_index])
                {
                case '[':
                    depth--;
                    if (depth == 0) return (value = _index);
                    _index--;
                    break;
                case ']':
                    depth++;
                    _index--;
                    break;
                default:
                    _index--;
                    break;
                }
            }
        }
    }
    return value;
}

unsigned int PTextParser::find_sqbracket_closer(unsigned int _index)
{
    return find_sqbracket_closer(text, _index);
}

unsigned int PTextParser::find_sqbracket_opener(unsigned int _index)
{
    return find_sqbracket_opener(text, _index);
}

vector<float> PTextParser::pick_params(unsigned int _index)
{
    vector<float> vec_value;
    PTextParser textparser;

    if (_index<getText().size()-1)
    {
        unsigned int start, end;
        start = ++_index;
        end = start + 1;
        if (getText()[start]=='(')
        {
            vector<unsigned int> vec_idx;
            vec_idx.push_back(start);
            char c;
            do
            {
                c=getText()[end];
                if (c == ',')
                    vec_idx.push_back(end);
                ++end;
            }
            while (c!=')');
            vec_idx.push_back(--end);


            for (unsigned int i=1; i<vec_idx.size(); i++)
            {
                float x;
                string str_buffer;

                start = vec_idx.at(i-1);
                end = vec_idx.at(i);
                start++;
                end--;
                str_buffer = getText().substr(start, end-start+1);
                x = textparser.stdstr_to_float(str_buffer);
                vec_value.push_back(x);
            }
        }
    }
    return vec_value;
}

vector<float> PTextParser::pick_params(std::string& _src, unsigned int _index)
{
    vector<float> vec_value;
    PTextParser textparser;

    if (_index<_src.size()-1)
    {
        unsigned int start, end;
        start = ++_index;
        end = start + 1;
        if (_src[start]=='(')
        {
            vector<unsigned int> vec_idx;
            vec_idx.push_back(start);
            char c;
            do
            {
                c=_src[end];
                if (c == ',')
                    vec_idx.push_back(end);
                ++end;
            }
            while (c!=')');
            vec_idx.push_back(--end);


            for (unsigned int i=1; i<vec_idx.size(); i++)
            {
                float x;
                string str_buffer;

                start = vec_idx.at(i-1);
                end = vec_idx.at(i);
                start++;
                end--;
                str_buffer = _src.substr(start, end-start+1);
                x = textparser.stdstr_to_float(str_buffer);
                vec_value.push_back(x);
            }
        }
    }
    return vec_value;
}

std::vector<float> PTextParser::pick_params(std::string& _src, unsigned int _index, std::vector<unsigned int>& _vec_idx_out)
{
    vector<float> vec_value;
    PTextParser textparser;

    _vec_idx_out.clear();
    if (_index<_src.size()-1)
    {
        unsigned int start, end;
        start = ++_index;
        end = start + 1;
        if (_src[start]=='(')
        {
            vector<unsigned int> vec_idx;
            vec_idx.push_back(start);
            _vec_idx_out.push_back(start);
            char c;
            do
            {
                c=_src[end];
                if (c == ',')
                {
                    vec_idx.push_back(end);
                    _vec_idx_out.push_back(end+1);
                }
                ++end;
            }
            while (c!=')');
            vec_idx.push_back(--end);


            for (unsigned int i=1; i<vec_idx.size(); i++)
            {
                float x;
                string str_buffer;

                start = vec_idx.at(i-1);
                end = vec_idx.at(i);
                start++;
                end--;
                str_buffer = _src.substr(start, end-start+1);
                x = textparser.stdstr_to_float(str_buffer);
                vec_value.push_back(x);
            }
        }
    }
    return vec_value;
}

std::vector<std::string> PTextParser::csvline_to_vector(std::string& _line)
{
    std::vector<std::string> value;
    boost::split(value, _line, boost::is_any_of(","));
    return value;
}

std::vector<std::string> PTextParser::split_string(const std::string& str,
                                      const std::string& delimiter)
{
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}

std::vector<std::string> PTextParser::add_digit_suffix(unsigned int _n)
{
    std::vector<std::string> value;
    for (int i=1; i<_n+1; i++)
    {
        value.push_back(text+std::to_string(i));
    }
    return value;
}

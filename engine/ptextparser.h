#ifndef PTEXTPARSER_H
#define PTEXTPARSER_H

#include <vector>
#include <string>

class PTextParser
{
    std::string text;
public:
    PTextParser();
    PTextParser(std::string _text);
    PTextParser(float _x);
    void setText(std::string _text){text=_text;}
    std::string getText() const {return text;}
    std::string format_value(float _x, unsigned int _precision);
public:
    bool is_valid_index(unsigned int _idx) const {return (_idx<text.size());}
    char get_char(unsigned int _idx) const {return is_valid_index(_idx)?text[_idx]:0;}
public:
    std::string& clean_final_endl(std::string& _src); // cleans only the lasts endlines
    std::string& clean_final_space(std::string& _src);// cleans only the lasts spaces
    std::string& clean_all_spaces(std::string& _src);
    static std::string& clean_multispaces(std::string& _src);
public:
    std::vector<std::string> arrange_sequence(std::string& _src);
    std::vector<std::string> arrange_sequence(std::string& _src, char _c_separator);
public:
    static float                       stdstr_to_float(std::string& _src);
    static std::string                 float_to_stdstr(float& _val);
    float                       extract_number(std::string& _src, unsigned int _index, unsigned int& _output_size, char _decimal_separator);
    std::vector<float>          pick_params(unsigned int _index);
    std::vector<float>          pick_params(std::string& _src, unsigned int _index);
    std::vector<float>          pick_params(std::string& _src, unsigned int _index, std::vector<unsigned int>& _vec_idx_out);
public:
    static unsigned int find_bracket_closer(std::string _s, unsigned int _index);
    static unsigned int find_bracket_opener(std::string _s, unsigned int _index);
    unsigned int find_bracket_closer(unsigned int _index);
    unsigned int find_bracket_opener(unsigned int _index);
    unsigned int find_sqbracket_closer(std::string _s, unsigned int _index);
    unsigned int find_sqbracket_opener(std::string _s, unsigned int _index);
    unsigned int find_sqbracket_closer(unsigned int _index);
    unsigned int find_sqbracket_opener(unsigned int _index);
public:
    std::string make_readable(std::string _src);
    static std::vector<std::string> csvline_to_vector(std::string& _line);
    static std::vector<std::string> split_string(const std::string& _str,
                                          const std::string& _delimiter);
    std::vector<std::string> add_digit_suffix(unsigned int _n);
};

#endif // PTEXTPARSER_H

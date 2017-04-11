#ifndef PALPHABET_H
#define PALPHABET_H

#include <vector>
#include <string>

#include "pgrammar.h"
#include "pword.h"

class PAlphabet : public PGrammar
{
    std::string text;
    std::vector<std::string> vec_alphabet;
    std::vector<std::string> vec_shift;
    std::vector<std::string> vec_var;
public:
    static size_t max_size;
public:
    PAlphabet();
    PAlphabet& operator=(const PAlphabet& _pa);
    bool operator==(const PAlphabet& _alphabet);
    bool is_alphabet(char _c);
    bool is_alphabet(std::string _s);
    bool is_alphabet(PWord* _word);
    bool is_shifting(char _c);
    bool is_variable(char _c);
    bool is_constant(char c);
    bool is_constant(std::string _s);
    void add(PWord* _w);
    void add(std::string _new_alphabet);
    size_t search(const std::string& _src, unsigned int _idx);
    size_t search_reverse(const std::string& _src);
public:
    void setText(char* _text){text += _text;}
    void setText(std::string _text){text = _text;}
    std::string getText(void) const {return std::string(text);}
    std::vector<std::string> getVecAlphabet() const {return vec_alphabet;}
    std::vector<std::string> getVecShift() const {return vec_shift;}
    std::vector<std::string> getVecVar() const {return vec_var;}
    std::string pick_first_alphabet_before(unsigned int _index, std::string _txt);
    std::string pick_first_alphabet_after(unsigned int _index, std::string _txt);
    void clear_alphabet() {vec_alphabet.clear();}
};

#endif // PALPHABET_H

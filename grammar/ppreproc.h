#ifndef PPREPROC_H
#define PPREPROC_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "grammar/pinclude.h"
#include "grammar/pignore.h"
#include "grammar/palphabet.h"

#include "pdefine.h"

class PPreproc
{
    std::stringstream raw;
    std::string text;
    std::vector<PDefine*> vec_define;
    std::vector<PInclude*> vec_include;
    std::vector<PWord*> vec_word;
    time_t seed;
    PIgnore* ignore;
public:
    PPreproc();
    PPreproc(const PPreproc& _preproc);
    PPreproc(std::string _text);
    ~PPreproc();
    PIgnore* getIgnore() const{return ignore;}
    std::string getText() const {return text;}
    void setText(std::string _text){text = _text;}
    time_t getSeed() const {return seed;}
    void setSeed(time_t _seed){seed = _seed;}
    std::vector<PDefine*> getVecDefine() const {return vec_define;}
    std::vector<PInclude*> getVecInclude() const {return vec_include;}
    std::vector<PWord*> getVecWord() const {return vec_word;}
    void push_define(PDefine* _define){vec_define.push_back(_define);}
    PPreproc& operator=(const PPreproc& _preproc);
    void del_define();
    void del_include();
    void to_csv_stream(std::stringstream& _cs);
    void from_csv_stream(std::stringstream& _cs);
public:
    void read(std::string& _text);
    std::string apply_define(std::string& _src);
    void copy_const(PPreproc* _preproc);
    bool redefine_const(std::string _name, float _val);
    float check_const(std::string _name);  
};

std::ostream& operator<< (std::ostream& os, PPreproc& _pp);

#endif // PPREPROC_H

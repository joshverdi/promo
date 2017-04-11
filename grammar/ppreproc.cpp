#include <string>
#include <vector>

#include "ppreproc.h"
#include "engine/procedural.h"
#include "engine/ptextparser.h"

using namespace std;

PPreproc::PPreproc()
{
    raw.str(string());
    ignore = new PIgnore();
    seed=0;
}

PPreproc::PPreproc(const PPreproc& _preproc)
{
    ignore = new PIgnore(_preproc.getIgnore()->getIgnored());
    vec_define.clear();
    vec_include.clear();
    vec_word.clear();
    for (unsigned int i=0; i<_preproc.getVecDefine().size(); i++)
    {
        vec_define.push_back(new PDefine(   _preproc.getVecDefine()[i]->getConstantName(),
                                            _preproc.getVecDefine()[i]->getConstantValue()  )
                             );
    }
    for (unsigned int i=0; i<_preproc.getVecInclude().size(); i++)
    {
        vec_include.push_back(new PInclude(_preproc.getVecInclude()[i]->getText()));
    }
    for (unsigned int i=0; i<_preproc.getVecWord().size(); i++)
    {
        vec_word.push_back(new PWord(_preproc.getVecWord()[i]->getText()));
    }
    seed=_preproc.getSeed();
}

PPreproc::PPreproc(std::string _text)
{
    ignore = new PIgnore();
    raw<<_text;
    text = _text;
    seed=0;
}

PPreproc::~PPreproc()
{
    delete ignore;
    for (std::vector<PDefine*>::iterator iter = vec_define.begin(); iter!=vec_define.end(); iter++)
    {
        delete (*iter);
    }
    vec_define.clear();
    for (std::vector<PInclude*>::iterator iter = vec_include.begin(); iter!=vec_include.end(); iter++)
    {
        delete (*iter);
    }
    vec_include.clear();
}

void PPreproc::read(std::string& _text)
{
    const string SHARP_DEFINE  = string("#define");
    const string SHARP_INCLUDE = string("#include");
    const string SHARP_IGNORE  = string("#ignore");
    const string SHARP_WORD    = string("#word");
    const string SHARP_SEED    = string("#seed");
    size_t start, end;
    string instruction_str;
    string &raw_str = _text;
    PTextParser textparser;
    vector<string> vec_str_buffer;

    delete ignore;
    vec_define.clear();
    vec_include.clear();
    ignore = new PIgnore();
    del_define();
    del_include();

    end = start = 0;
    raw.str(string());

    textparser.clean_final_endl(raw_str);

    while (end<=raw_str.size())
    {
        if ((raw_str[end]=='\n')||(end==raw_str.size()-1))
        {
            if (end==raw_str.size()-1) end++;
            instruction_str = raw_str.substr(start, end-start);
            if (instruction_str.empty()) continue;
            if (instruction_str.find(SHARP_DEFINE) == 0)
            {
                instruction_str = instruction_str.substr(SHARP_DEFINE.size()+1);
                vec_str_buffer = textparser.arrange_sequence(instruction_str, ' ');
                if (vec_str_buffer.size()>=2)
                {
                    vec_define.push_back(new PDefine(vec_str_buffer.at(0), textparser.stdstr_to_float(vec_str_buffer.at(1))));
                }
            }
            else if (instruction_str.find(SHARP_INCLUDE) == 0)
            {
                instruction_str = instruction_str.substr(SHARP_INCLUDE.size()+1);
                vec_include.push_back(new PInclude(instruction_str));
            }
            else if (instruction_str.find(SHARP_IGNORE) == 0)
            {
                instruction_str = instruction_str.substr(SHARP_INCLUDE.size());
                ignore->add(instruction_str);
            }
            else if (instruction_str.find(SHARP_WORD) == 0)
            {
                instruction_str = instruction_str.substr(SHARP_WORD.size()+1);
                vec_word.push_back((new PWord(instruction_str)));
            }
            else if (instruction_str.find(SHARP_SEED) == 0)
            {
                instruction_str = instruction_str.substr(SHARP_SEED.size()+1);
                seed=std::stoi(instruction_str);
            }
            start=end+1;
        }
        end++;
    }
}

string PPreproc::apply_define(string& _src)
{
    string value = _src;
    for (unsigned int i=0; i<vec_define.size(); i++)
    {
        value = vec_define.at(i)->apply(value);
    }
    return value;
}

PPreproc& PPreproc::operator=(const PPreproc& _preproc)
{
    if (_preproc.getIgnore()==0)
        cout<<endl<<"PPreproc or PIgnore failure";
    ignore = new PIgnore();
    *ignore = *_preproc.getIgnore();
    vec_define.clear();
    vec_include.clear();
    vec_word.clear();
    for (unsigned int i=0; i<_preproc.getVecDefine().size(); i++)
    {
        vec_define.push_back(new PDefine(   _preproc.getVecDefine()[i]->getConstantName(),
                                            _preproc.getVecDefine()[i]->getConstantValue()  )
                             );
    }
    for (unsigned int i=0; i<_preproc.getVecInclude().size(); i++)
    {
        vec_include.push_back(new PInclude(_preproc.getVecInclude()[i]->getText()));
    }
    for (unsigned int i=0; i<_preproc.getVecWord().size(); i++)
    {
        vec_word.push_back(new PWord(_preproc.getVecWord()[i]->getText()));
    }
    seed=_preproc.getSeed();
    return *this;
}

void PPreproc::copy_const(PPreproc* _preproc)
{
    if (!_preproc) return;
    vec_define.clear();
    for (unsigned int i=0; i<_preproc->getVecDefine().size(); i++)
    {
        vec_define.push_back(new PDefine(   _preproc->getVecDefine()[i]->getConstantName(),
                                            _preproc->getVecDefine()[i]->getConstantValue()
                                        ));
    }
}

bool PPreproc::redefine_const(string _name, float _val)
{
    for (unsigned int i=0; i<vec_define.size(); i++)
    {
        if (!vec_define[i]->getConstantName().compare(_name))
        {
            vec_define[i]->setConstantValue(_val);
            return true;
        }
    }
    return false;
}

float PPreproc::check_const(std::string _name)
{
    for (unsigned int i=0; i<vec_define.size(); i++)
    {
        if (!vec_define[i]->getConstantName().compare(_name))
            return vec_define[i]->getConstantValue();
    }
    return 0;
}

ostream& operator<<(ostream& os, PPreproc& _pp)
{
    os << std::endl << "Preproc info:";
    os << std::endl << _pp.getVecInclude().size() << "include(s).";
    if (!_pp.getVecDefine().empty())
    {
        for (unsigned int i=0; i<_pp.getVecDefine().size(); i++)
        {
            os << std::endl << _pp.getVecDefine().at(i)->getConstantName() << " = " << _pp.getVecDefine().at(i)->getConstantValue();
        }
    }
    else
    {
        os << std::endl << "No constants!";
    }
    return os;
}

void PPreproc::del_define()
{
    for (std::vector<PDefine*>::iterator iter = vec_define.begin(); iter!=vec_define.end(); iter++)
    {
        delete (*iter);
    }
    vec_define.clear();
}

void PPreproc::del_include()
{
    for (std::vector<PInclude*>::iterator iter = vec_include.begin(); iter!=vec_include.end(); iter++)
    {
        delete (*iter);
    }
    vec_include.clear();
}

void PPreproc::to_csv_stream(std::stringstream& _cs)
{
    PTextParser parser;
    _cs.str("");
    if (seed>0)
        _cs << "seed" << "," << seed << std::endl;

    if (!ignore->getText().empty())
        _cs << "ignore" << "," << ignore->getText() << std::endl;
    if (!vec_include.empty())
        for (std::vector<PInclude*>::iterator iter = vec_include.begin(); iter!=vec_include.end(); iter++)
        {
            _cs << "include" << "," << (*iter)->getText() << std::endl;
        }
    if (!vec_define.empty())
        for (std::vector<PDefine*>::iterator iter = vec_define.begin(); iter!=vec_define.end(); iter++)
        {
            _cs << "define" << "," <<  (*iter)->getConstantName() << "," << (*iter)->getConstantValue() << std::endl;
        }
}

void PPreproc::from_csv_stream(std::stringstream& _cs)
{
    std::vector<std::string> vec_line;
    std::string my_name;
    float my_value;
    std::string my_line;
    std::string input_str;

    input_str = _cs.str();
    vec_define.clear();
    vec_include.clear();

    while(std::getline(_cs, my_line))
    {
        vec_line = PTextParser::csvline_to_vector(my_line);
        if (vec_line.size()==3)
        {
            if (vec_line[0].compare(std::string("define")))
            {
                my_name = vec_line[1];
                my_value = PTextParser::stdstr_to_float(vec_line[2]);
                vec_define.push_back(new PDefine(my_name, my_value));
            }

        }
        else if (vec_line.size()==2)
        {
            if (vec_line[0].compare(std::string("include")))
            {
                vec_include.push_back(new PInclude(vec_line[1]));
            }
            else if (vec_line[0].compare(std::string("ignore")))
            {
                ignore->setText(vec_line[1]);
            }
            else if (vec_line[0].compare(std::string("seed")))
                seed=std::stoi(vec_line[1]);
        }
    }
}

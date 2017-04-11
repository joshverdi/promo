#include "pmodelingconfig.h"
#include "engine/piorecord.h"
#include "engine/ptextparser.h"

#include <boost/algorithm/string/replace.hpp>

PModelingConfig::PModelingConfig()
{
    init_color();
    init_shape();
}

PModelingConfig::PModelingConfig(std::string _color_fn)
{
    load_color(_color_fn);
}

PModelingConfig::PModelingConfig(PAlphabet _alphabet)
{
    alphabet = _alphabet;
    init_color();
    init_shape();
}

PModelingConfig::PModelingConfig(PAlphabet _alphabet, std::vector<std::vector<float> > _vec_color)
{
    alphabet = _alphabet;
    vec_color = _vec_color;
}

PModelingConfig::PModelingConfig(const PModelingConfig& _mod_config)
{
    alphabet = _mod_config.getAlphabet();
    vec_color = _mod_config.getVecColor();
    vec_shape_str = _mod_config.getVecShapeStr();
}

void PModelingConfig::init()
{
    init_color();
    init_shape();
}

//void PModelingConfig::load(std::string _color_fn)
//{
//    load_color(_color_fn);
//}

void PModelingConfig::init_color()
{
    std::vector<float> dflt_color {0.5f, 0.35f, 0.05f, 1.0f};
    vec_color.clear();
    for (size_t i=0; i<alphabet.getVecAlphabet().size(); i++)
    {
        vec_color.push_back(dflt_color);
    }
}

void PModelingConfig::init_shape()
{
    std::string dflt_shape("cylinder");
    vec_shape_str.clear();
    for (size_t i=0; i<alphabet.getVecAlphabet().size(); i++)
    {
        vec_shape_str.push_back(dflt_shape);
    }
}

void PModelingConfig::save_color(std::string _filename)
{
    // It is imperative that every vec_color has 4 values for R, G, B, and alpha
    QFile my_file(_filename.c_str());
    std::stringstream my_ss;
    if (alphabet.getVecAlphabet().size()==vec_color.size())
    {
        for (size_t i=0; i<alphabet.getVecAlphabet().size(); i++)
        {
            my_ss << alphabet.getVecAlphabet().at(i);
            for (size_t j=0; j<vec_color.at(i).size(); j++)
                my_ss << "," << vec_color.at(i).at(j);
            my_ss << "\n";
        }
        PIORecord::save(my_file, my_ss);
    }
}

void PModelingConfig::save_shape(std::string _filename)
{
    QFile my_file(_filename.c_str());
    std::stringstream my_ss;
    if (alphabet.getVecAlphabet().size()==vec_shape_str.size())
    {
        for (size_t i=0; i<vec_shape_str.size(); i++)
        {
            my_ss << alphabet.getVecAlphabet().at(i);
            my_ss << ",";
            my_ss << vec_shape_str.at(i);
            my_ss << "\n";
        }
        PIORecord::save(my_file, my_ss);
    }
}

void PModelingConfig::load_shape(std::string _filename)
{
    std::string my_line;
    std::stringstream my_ss;
    std::vector<std::string> vec_line;
    QFile my_file(_filename.c_str());
    PIORecord::load(my_file, my_ss);
    alphabet.clear_alphabet();
    vec_shape_str.clear();
    while(std::getline(my_ss, my_line))
    {
        vec_line = PTextParser::csvline_to_vector(my_line);
        if (!vec_line.empty())
        {
            alphabet.add(vec_line.at(0));
            if (vec_line.size()>=2)
                vec_shape_str.push_back(vec_line.at(1));
            else vec_shape_str.push_back(std::string("cylinder"));
        }
    }
}

void PModelingConfig::load_color(std::string _filename)
{
    std::string my_line;
    std::stringstream my_ss;
    std::vector<std::string> vec_line;
    std::vector<float> vec_color_tmp;
    std::vector<float> dflt_color {0.5f, 0.35f, 0.05f, 1.0f};
    QFile my_file(_filename.c_str());
    PIORecord::load(my_file, my_ss);

    alphabet.clear_alphabet();
    vec_color.clear();
    while(std::getline(my_ss, my_line))
    {
        vec_line = PTextParser::csvline_to_vector(my_line);
        if (!vec_line.empty())
        {
            vec_color_tmp.clear();
            alphabet.add(vec_line.at(0));
            if (vec_line.size()>=5)
            {
                for (size_t i=1; i<vec_line.size(); i++)
                    vec_color_tmp.push_back(PTextParser::stdstr_to_float(vec_line.at(i)));
            }
            else vec_color_tmp = dflt_color;
            vec_color.push_back(vec_color_tmp);
        }
    }
}
void PModelingConfig::load_color(const std::stringstream& _color_stream)
{
    std::string my_line;
    std::stringstream my_ss(_color_stream.str());
    std::vector<std::string> vec_line;
    std::vector<float> vec_color_tmp;
    std::vector<float> dflt_color {0.5f, 0.35f, 0.05f, 1.0f};

    alphabet.clear_alphabet();
    vec_color.clear();
    while(std::getline(my_ss, my_line))
    {
        vec_line = PTextParser::csvline_to_vector(my_line);
        if (!vec_line.empty())
        {
            vec_color_tmp.clear();
            alphabet.add(vec_line.at(0));
            if (vec_line.size()>=5)
            {
                for (size_t i=1; i<vec_line.size(); i++)
                    vec_color_tmp.push_back(PTextParser::stdstr_to_float(vec_line.at(i)));
            }
            else vec_color_tmp = dflt_color;
            vec_color.push_back(vec_color_tmp);
        }
    }
}

// Returns the name of the modeling config
std::string PModelingConfig::smart_load_color(std::string _lsystem_filename)
{
    std::string value;
    std::string lsys_path;
    std::string path_separator;
    lsys_path = Procedural::file_path(_lsystem_filename);
    value = Procedural::file_basename(_lsystem_filename);
    value += "_color_config.csv";
    if (!((lsys_path.find("/")==std::string::npos)&&(lsys_path.find("\\")==std::string::npos)))
    {
        path_separator = (lsys_path.find("/")!=std::string::npos)?"/":"\\";
        value = lsys_path + path_separator + value;
    }

    QFile my_file(value.c_str());
    if (my_file.exists())
    {
        load_color(value);
    }
    else value.clear();
    return value;
}

std::string PModelingConfig::smart_load_shape(std::string _lsystem_filename)
{
    std::string value;
    std::string lsys_path;
    std::string path_separator;
    lsys_path = Procedural::file_path(_lsystem_filename);
    value = Procedural::file_basename(_lsystem_filename);
    value += "_shape_config.csv";
    if (!((lsys_path.find("/")==std::string::npos)&&(lsys_path.find("\\")==std::string::npos)))
    {
        path_separator = (lsys_path.find("/")!=std::string::npos)?"/":"\\";
        value = lsys_path + path_separator + value;
    }

    QFile my_file(value.c_str());
    if (my_file.exists())
    {
        load_shape(value);
    }
    else value.clear();
    return value;
}



// Returns the name of the modeling config
std::string PModelingConfig::smart_save_color(std::string _lsystem_filename)
{
    std::string value;
    std::string lsys_path;
    std::string path_separator;
    lsys_path = Procedural::file_path(_lsystem_filename);
    value = Procedural::file_basename(_lsystem_filename);
    value += "_color_config.csv";
    if (!((lsys_path.find("/")==std::string::npos)&&(lsys_path.find("\\")==std::string::npos)))
    {
        path_separator = (lsys_path.find("/")!=std::string::npos)?"/":"\\";
        value = lsys_path + path_separator + value;
    }
    save_color(value);
    return value;
}

std::string PModelingConfig::smart_save_shape(std::string _lsystem_filename)
{
    std::string value;
    std::string lsys_path;
    std::string path_separator;
    lsys_path = Procedural::file_path(_lsystem_filename);
    value = Procedural::file_basename(_lsystem_filename);
    value += "_shape_config.csv";
    if (!((lsys_path.find("/")==std::string::npos)&&(lsys_path.find("\\")==std::string::npos)))
    {
        path_separator = (lsys_path.find("/")!=std::string::npos)?"/":"\\";
        value = lsys_path + path_separator + value;
    }
    save_shape(value);
    return value;
}

std::vector<float> PModelingConfig::check_color(std::string _alphabet)
{
    size_t i;
    std::vector<float> value{0.5f, 0.35f, 0.05f, 1.0f};
    for (i = 0; i<alphabet.getVecAlphabet().size(); i++)
    {
        if (_alphabet.compare(alphabet.getVecAlphabet().at(i))==0)
            break;
    }
    if (i<vec_color.size())
        value = vec_color.at(i);
    return value;
}

std::vector<float> PModelingConfig::check_color(unsigned int _index)
{
    std::vector<float> value{0.5f, 0.35f, 0.05f, 1.0f};
    if (_index<vec_color.size())
    {
        value = vec_color.at(_index);
    }
    return value;
}

std::string PModelingConfig::check_shape(std::string _alphabet)
{
    std::string value("cylinder");
    size_t i;
    for (i = 0; i<alphabet.getVecAlphabet().size(); i++)
    {
        if (_alphabet.compare(alphabet.getVecAlphabet().at(i))==0)
            break;
    }
    if (i<vec_shape_str.size())
        value = vec_shape_str.at(i);
    return value;
}

std::string PModelingConfig::check_shape(unsigned int _index)
{
    std::string value("cylinder");
    if (_index<vec_shape_str.size())
        value = vec_shape_str.at(_index);
    return value;
}

void PModelingConfig::change_color(std::string _alphabet, std::vector<float> _vec_color)
{
    std::vector<std::vector<float> > vec_color_tmp;
    if ((vec_color.size() == alphabet.getVecAlphabet().size())&&(_vec_color.size()>=3)&&(_vec_color.size()<=4))
    {
        if (_vec_color.size() == 3)
            _vec_color.push_back(1.0f);
        for (size_t i=0; i<alphabet.getVecAlphabet().size(); i++)
        {
            if (_alphabet.compare(alphabet.getVecAlphabet().at(i))==0)
                vec_color_tmp.push_back(_vec_color);
            else vec_color_tmp.push_back(vec_color.at(i));
        }
        vec_color = vec_color_tmp;
    }
}

void PModelingConfig::change_color(unsigned int _index, std::vector<float> _vec_color)
{
    std::vector<std::vector<float> > vec_color_tmp;
    if ((_index < vec_color.size())&&(vec_color.size()==alphabet.getVecAlphabet().size()))
    {
        if (_vec_color.size() == 3)
            _vec_color.push_back(1.0f);
        for (size_t i=0; i<vec_color.size(); i++)
        {
            if (i==_index)
                vec_color_tmp.push_back(_vec_color);
            else vec_color_tmp.push_back(vec_color.at(i));
        }
        vec_color = vec_color_tmp;
    }
}

void PModelingConfig::change_shape(std::string _alphabet, std::string _shape_str)
{
    std::vector<std::string> vec_shape_str_tmp;
    if (vec_shape_str.size() == alphabet.getVecAlphabet().size())
    {
        for (size_t i=0; i<alphabet.getVecAlphabet().size(); i++)
        {
            if (_alphabet.compare(alphabet.getVecAlphabet().at(i))==0)
                vec_shape_str_tmp.push_back(_shape_str);
            else vec_shape_str_tmp.push_back(vec_shape_str.at(i));
        }
        vec_shape_str = vec_shape_str_tmp;
    }
}

void PModelingConfig::change_shape(unsigned int _index, std::string _shape_str)
{
    std::vector<std::string> vec_shape_str_tmp;
    if (vec_shape_str.size() == alphabet.getVecAlphabet().size())
    {
        for (size_t i=0; i<alphabet.getVecAlphabet().size(); i++)
        {
            if (_index==i)
                vec_shape_str_tmp.push_back(_shape_str);
            else vec_shape_str_tmp.push_back(vec_shape_str.at(i));
        }
        vec_shape_str = vec_shape_str_tmp;
    }
}

void PModelingConfig::color_stream(std::stringstream& _ss)
{
    _ss.clear();
    if (alphabet.getVecAlphabet().size()==vec_color.size())
    {
        for (size_t i=0; i<alphabet.getVecAlphabet().size(); i++)
        {
            _ss << alphabet.getVecAlphabet().at(i);
            for (size_t j=0; j<vec_color.at(i).size(); j++)
                _ss << "," << vec_color.at(i).at(j);
            _ss << "\n";
        }
    }
}

void PModelingConfig::shape_stream(std::stringstream& _ss)
{
    _ss.clear();
    if (alphabet.getVecAlphabet().size()==vec_shape_str.size())
    {
        for (size_t i=0; i<alphabet.getVecAlphabet().size(); i++)
        {
            _ss << alphabet.getVecAlphabet().at(i);
            _ss << ",";
            _ss << vec_shape_str.at(i);
            _ss << "\n";
        }
    }
}

std::string PModelingConfig::load_shape_from_colorfn(std::string _color_filename)
{
    std::string value;
    if (_color_filename.find("_color_config.csv")!=std::string::npos)
    {
        value = _color_filename;
        boost::replace_all(value, "_color_config.csv", "_shape_config.csv");
        QFile file_shape(value.c_str());
        if (file_shape.exists())
        {
            load_shape(value);
        }
    }
    return value;
}

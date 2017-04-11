#ifndef PMODELINGCONFIG_H
#define PMODELINGCONFIG_H

#include <string>
#include <vector>
#include <sstream>

#include "grammar/palphabet.h"

class PModelingConfig
{
    PAlphabet alphabet;
    std::vector<std::vector<float> > vec_color; // red, green, blue, alpha
    std::vector<std::string> vec_shape_str;
public:
    PModelingConfig();
    PModelingConfig(PAlphabet _alphabet);
    PModelingConfig(PAlphabet _alphabet, std::vector<std::vector<float> > _vec_color);
    PModelingConfig(std::string _color_fn);
    PModelingConfig(const PModelingConfig& _mod_config);
    void init();
    std::string smart_load_color(std::string _lsystem_filename);    // lsystem filename is also what we call by basename
    std::string smart_save_color(std::string _lsystem_filename);    // removed from its extension '.txt'
    std::string smart_load_shape(std::string _lsystem_filename);
    std::string smart_save_shape(std::string _lsystem_filename);
    std::string load_shape_from_colorfn(std::string _color_filename);
    //void load(std::string _color_fn);
    void setAlphabet(PAlphabet _alphabet){alphabet=_alphabet;}
    PAlphabet getAlphabet() const {return alphabet;}
    void setVecShapeStr(std::vector<std::string> _vec_shape_str){vec_shape_str = _vec_shape_str;}
    std::vector<std::string> getVecShapeStr() const {return vec_shape_str;}
    std::vector<std::vector<float> > getVecColor() const {return vec_color;}

    void init_color();
    void init_shape();
    void save_color(std::string _filename);
    void load_color(std::string _filename);
    void load_color(const std::stringstream& _color_stream);
    void save_shape(std::string _filename);
    void load_shape(std::string _filename);
    std::vector<float> check_color(std::string _alphabet);
    std::vector<float> check_color(unsigned int _index);
    std::string check_shape(std::string _alphabet);
    std::string check_shape(unsigned int _index);
    void change_color(std::string _alphabet, std::vector<float> _vec_color);
    void change_color(unsigned int _index, std::vector<float> _vec_color);
    void change_shape(std::string _alphabet, std::string _shape_str);
    void change_shape(unsigned int _index, std::string _shape_str);
    void color_stream(std::stringstream& _ss);
    void shape_stream(std::stringstream& _ss);
    // The default name for color config will be 'filebasename_color_config.csv'
    // The default name for the shape and others is 'filebasename_shape_config.csv'
};

#endif // PMODELINGCONFIG_H

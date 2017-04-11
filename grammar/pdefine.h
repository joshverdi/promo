#ifndef PDEFINE_H
#define PDEFINE_H

#include <string>

class PDefine
{
    std::string constant_name;
    float constant_value;
public:
    PDefine();
    PDefine(std::string _constant_name, float _constant_value);
    std::string getConstantName() const {return constant_name;}
    float getConstantValue() const {return constant_value;}
    void setConstantValue(float _constant_value){constant_value = _constant_value;}
    std::string apply(std::string& _src);
    PDefine& operator=(const PDefine& _define);
    bool operator==(const PDefine& _define);
};

#endif // PDEFINE_H

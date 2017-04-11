#include <string>
#include <sstream>

#include "procedural.h"
#include "pnumericvalue.h"
#include "ptextparser.h"
#include "grammar/parithmetic.h"

using namespace std;

PNumericValue::PNumericValue()
{
    data = 0;
    root = NULL;
}

PNumericValue::PNumericValue(float _data, unsigned int _priority):PNumeric(_priority)
{
    data = _data;
    root = NULL;
}

PNumericValue::PNumericValue(float _data)
{
    std::ostringstream buffer_stream;
    data = _data;
    buffer_stream<<_data;
    setText(buffer_stream.str());
    setType(buffer_stream.str());
    root = NULL;
}

PNumericValue::PNumericValue(std::string _s_data)
{
    std::istringstream buffer_stream(_s_data);
    buffer_stream>>data;
    root = NULL;
}

PNumericValue::PNumericValue(const PNumericValue& _pnv):PNumeric(_pnv.getType(), _pnv.getText(), _pnv.getPriority()),data(_pnv.getData())
{

    PArithmetic* pa = _pnv.getRoot();
    if (pa)
    {
        root = new PArithmetic();
        *root = *pa;
    }
}

PNumericValue& PNumericValue::operator=(const PNumericValue& _pnv)
{
    data = _pnv.getData();
    if (_pnv.getRoot())
    {
        root = new PArithmetic(*_pnv.getRoot());
    }
    return *this;
}

PNumericValue::~PNumericValue()
{
}

float PNumericValue::eval(string* _s_params, float* _value, unsigned int _nb_params)
{
    return data;
}

bool PNumericValue::is_numeric_value(std::string _s_input)
{
    bool value = true;
    for (unsigned int i=0; i<_s_input.size(); i++)
    {
        if ((!isdigit(_s_input[i]))&&(_s_input[i]!=DECIMAL_SEPARATOR))
        {
            value = false;
            break;
        }
    }
    return value;
}

float PNumericValue::set_from(std::string _str_src)
{
    std::istringstream buffer_stream(_str_src);
    buffer_stream>>data;
    return data;
}

string PNumericValue::to_stdstr()
{
    PTextParser tp;
    return tp.float_to_stdstr(data);
}

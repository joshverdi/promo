#include <sstream>

#include "grammar/parithmetic.h"
#include "grammar/pvariable.h"
#include "procedural.h"
#include "pnumeric.h"
#include "pnumericvalue.h"


using namespace std;

PNumeric::PNumeric()
{
    root = 0;
    priority = 0;
}

PNumeric::PNumeric(char _c)
{
    std::ostringstream buffer_stream;
    buffer_stream<<_c;
    text = type = buffer_stream.str();
    root = 0;
}

PNumeric::PNumeric(unsigned int _priority)
{
    root = 0;
    priority = _priority;
}

PNumeric::PNumeric(std::string _text, std::string _type)
{
    root = 0;
    text = _text;
    type = _type;
}

PNumeric::PNumeric(std::string _text, std::string _type, unsigned int _priority)
{
    root = 0;
    text = _text;
    type = _type;
    priority = _priority;
}

PNumeric& PNumeric::operator=(const PNumeric& _pn)
{
    type = _pn.getType();
    text = _pn.getText();
    priority = _pn.getPriority();
    root = _pn.getRoot();
    return *this;
}

PNumeric::~PNumeric()
{
    delete root;
}

int PNumeric::check_type(std::string _src)
{
    if (PNumericValue::is_numeric_value(_src)) return NUM_VAL;
    if (    (_src.find('+') != string::npos) ||
            (_src.find('-') != string::npos) ||
            (_src.find('*') != string::npos) ||
            (_src.find('/') != string::npos) ||
            (_src.find('^') != string::npos)    )
        return ARITHM_OP;

    for (unsigned int i=0; i<_src.size(); i++)
    {
        if (!isalpha(_src[i])) return -1;
    }
    return VAR;
}

// return +1 if _src is higher priority, -1 if lower, and 0 if equal
// higher priority notably refers to an emplacement in the tree :
// tree leaves are highest priority, and root is the lowest one
int PNumeric::is_higher_than(PNumeric* _src)
{
   int value = -1;
   if (priority>_src->getPriority()) value = -1;
   else if (priority<_src->getPriority()) value = 1;
   else if (priority==_src->getPriority())
   {
       int this_type_buffer     = Procedural::assign_op_type(type);
       int src_type_buffer = Procedural::assign_op_type(_src->getType());
       if (this_type_buffer == src_type_buffer) value = 0;
       else if (this_type_buffer<src_type_buffer) value = +1;
       else if (this_type_buffer>src_type_buffer) value = -1;
   }
   return value;
}

// Returns the root of absolutely all leaves
PNumeric* PNumeric::absolute_root()
{
    PNumeric* value = this;
    while (value->getRoot())
    {
        value = value->getRoot();
    }
    return value;
}

PNumeric* PNumeric::absolute_root(PNumeric* _pn)
{
    PNumeric* value = _pn;
    while (value->getRoot())
    {
        value = _pn->getRoot();
    }
    return value;
}



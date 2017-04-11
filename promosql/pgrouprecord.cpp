#include "pgrouprecord.h"

PGroupRecord::PGroupRecord()
{
    img_id = -1;
    group = 0;
    is_main = false;
}

PGroupRecord::PGroupRecord(int _img_id, uint _group, bool _is_main)
{
    img_id = _img_id;
    group = _group;
    is_main = _is_main;
}

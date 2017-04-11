#ifndef PGROUPRECORD_H
#define PGROUPRECORD_H

#include "putils.h"

class PGroupRecord
{
    int img_id;
    uint group;
    bool is_main;
public:
    PGroupRecord();
    PGroupRecord(int _img_id, uint _group, bool _is_main=false);
    void set_is_main(bool _is_main){is_main = _is_main;}
    bool get_is_main() const {return is_main;}
    int get_img_id() const {return img_id;}
    uint get_group() const {return group;}
};

#endif // PGROUPRECORD_H

#include "Data.h"
#include <iostream>

Data::Data(int _id):id(_id)
{
    //ctor
    x = 0;
    y = DELTA_Y;
    depth = 0;
}

Data::Data(int _id, float _x, float _y):id(_id), x(_x), y(_y), depth(0)
{
}

Data::Data(int _id, int _depth, float _x, float _y):id(_id), x(_x), y(_y), depth(_depth)
{
}

void Data::print() const
{
    std::cout << std::endl << "\nData Info:" ;
    std::cout << std::endl << "id:\t"       << id;
    std::cout << std::endl << "depth:\t"    << depth;
    std::cout << std::endl << "x:\t"        << x;
    std::cout << std::endl << "y:\t"        << y;
}

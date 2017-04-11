#ifndef DATA_H
#define DATA_H

namespace DataNS
{
    const int WINDOW_WIDTH  = 160;
}

class Data
{
    int id;
    float x, y;
    int depth;
    public:
        Data(int _id);
        Data(int _id, float _x, float _y);
        Data(int _id, int _depth, float _x, float _y);
        inline void setx(float _x){x=_x;}
        inline void sety(float _y){y=_y;}
        inline void setcoords(float _x, float _y){x=_x;y=_y;}
        inline void setdepth(int _depth){depth=_depth;}
        inline int getid() const {return id;}
        inline float getx()  const {return x;}
        inline float gety()  const {return y;}
        inline int getdepth() const {return depth;}
        void print() const;
    public:
        static const int DELTA_Y = 16;

};

#endif // DATA_H

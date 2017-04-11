#include "engine/procedural.h"
#include "viewport/pturtle.h"

PTurtle::PTurtle()
{
    position = PVector3D();
}

PTurtle::PTurtle(PVector3D _position, PVector3D _vec_orientation)
{
    position = _position;
    vec_orientation = _vec_orientation;
}

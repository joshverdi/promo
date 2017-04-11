#include "ptextmap.h"
#include "procedural.h"
#include "viewport/pshapeinitializer.h"

PTextMap::PTextMap()
{
    index = 0;
    depth = 0;
    mindepth = maxdepth = 0;
    parent = 0;
}

PTextMap::PTextMap(unsigned int _index):index(_index)
{
    depth = 0;
    mindepth = maxdepth = 0;
    parent = 0;
}

PTextMap::PTextMap(unsigned int _depth, unsigned int _mindepth, unsigned int _maxdepth):depth(_depth), mindepth(_mindepth), maxdepth(_maxdepth)
{
    parent = 0;
}

PTextMap::PTextMap(PTextMap* _parent, unsigned int _index, unsigned int _depth, unsigned int _mindepth, unsigned int _maxdepth):parent(_parent), index(_index), depth(_depth), mindepth(_mindepth), maxdepth(_maxdepth)
{
    _parent->add_children(this);
}

PTextMap::PTextMap(PTextMap* _parent, unsigned int _index)
{
    index = _index;
    if (_parent)
    {
        parent = _parent;
        depth = _parent->getDepth()+1;
        maxdepth = mindepth = depth;
    }

    _parent->add_children(this);
}

void PTextMap::update_max()
{
    std::vector<unsigned int> vec_nodes;

    vec_nodes = nodes();
    if (!vec_nodes.empty())
    {
        maxdepth = vec_nodes.front();
        for (std::vector<unsigned int>::iterator iter = vec_nodes.begin(); iter!=vec_nodes.end(); iter++)
        {
            maxdepth = PROCEDURAL_MAX(maxdepth, (*iter));
        }
    }
    else maxdepth = depth;
}

void PTextMap::update_min()
{
    std::vector<unsigned int> vec_nodes;

    vec_nodes = nodes();
    if (!vec_nodes.empty())
    {
        mindepth = vec_nodes.front();
        for (std::vector<unsigned int>::iterator iter = vec_nodes.begin(); iter!=vec_nodes.end(); iter++)
        {
            mindepth = PROCEDURAL_MIN(mindepth, (*iter));
        }
    }
    else mindepth = depth;
}

void PTextMap::apply_conic(float _basis_ray, std::map<unsigned int, std::pair<float, float> >& _map_ray)
{
    vec_dimension.clear();
    vec_dimension.push_back(_basis_ray);
    float my_top = _basis_ray*(1-(PShapeInitializer::rate/(1+maxdepth-depth+maxdepth-mindepth)));
    vec_dimension.push_back(my_top);
    _map_ray.insert(std::make_pair(index, std::make_pair(_basis_ray, my_top)));
    for (std::vector<PTextMap*>::iterator iter=vec_children.begin(); iter!=vec_children.end(); iter++)
    {
        (*iter)->apply_conic(my_top, _map_ray);
    }
}

std::vector<unsigned int> PTextMap::nodes()
{
    std::vector<unsigned int> value;
    if (!vec_children.empty())
    {
        for (std::vector<PTextMap*>::iterator iter=vec_children.begin(); iter!=vec_children.end(); iter++)
        {
            std::vector<unsigned int> tmp = (*iter)->nodes();
            value.insert(value.end(), tmp.begin(), tmp.end());
        }
    }
    else value.push_back(depth);
    return value;
}

void PTextMap::update_childmax()
{
    update_max();
    if (!vec_children.empty())
    {
        for (std::vector<PTextMap*>::iterator iter=vec_children.begin(); iter!=vec_children.end(); iter++)
        {
            (*iter)->update_childmax();
            (*iter)->update_max();
        }
    }
    else maxdepth = depth;
}

void PTextMap::update_childmin()
{
    update_min();
    if (!vec_children.empty())
    {
        for (std::vector<PTextMap*>::iterator iter=vec_children.begin(); iter!=vec_children.end(); iter++)
        {
            (*iter)->update_childmin();
            (*iter)->update_min();
        }
    }
    else mindepth = depth;
}

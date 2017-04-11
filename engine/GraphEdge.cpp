
#include <iostream>
#include "GraphEdge.h"

GraphEdge::GraphEdge()
{
    //ctor
    first = nullptr;
    second = nullptr;
}

GraphEdge::GraphEdge(std::shared_ptr<GraphNode> _first, std::shared_ptr<GraphNode> _second)
{
    first  = _first;
    second = _second;
}

GraphEdge::GraphEdge(std::shared_ptr<GraphNode> _first, std::shared_ptr<GraphNode> _second, float _capacity)
{
    first    = _first;
    second   = _second;
    capacity = _capacity;
}

bool GraphEdge::is_valid()
{
    return (first||second)?true:false;
}

std::shared_ptr<GraphEdge> GraphEdge::uniquepredecessor(unsigned int _level)
{
    std::shared_ptr<GraphEdge> edgetmp = shared_from_this();
    std::shared_ptr<GraphEdge> value = nullptr;

    unsigned int i=0;
    while (i<_level)
    {
        if (edgetmp->has_uniqpred())
            edgetmp = edgetmp->getdqpredecessor().front();
        else
            break;
        i++;
    }
    if (i==_level)
        value = edgetmp;
    else
        value.reset();
    return value;
}

// Note that coded this way (as list duplicated in a recursive way in the stack)
// this method is costly in terms of both memory and execution time
void GraphEdge::transfercity(std::list<float> _list_capacity, int _level)
{
    std::shared_ptr<GraphEdge> edgetmp = shared_from_this();

    while (edgetmp&&!_list_capacity.empty())
    {
        edgetmp->add_capacity(_list_capacity.front());
        _list_capacity.pop_front();
        edgetmp = edgetmp->uniquepredecessor(_level);
    }
}

void GraphEdge::resetcity()
{
    if (capacity!=GraphNs::INFINITE_LINK)
        capacity = 0;
    for (auto pred: dqpredecessor)
    {
        pred->resetcity();
    }
}

void GraphEdge::resetcity(int _jump_level)
{

    int i=0;
    //static int dbg=0;
    //std::cout << std::endl << "call for resetcity() number:" << ++dbg;
    std::shared_ptr<GraphEdge> edgetmp = uniquepredecessor(_jump_level);

    while (edgetmp)
    {
        ++i;
        edgetmp->add_capacity(pow(2,i));
        edgetmp = edgetmp->uniquepredecessor(_jump_level);
    }
}

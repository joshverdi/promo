#include "Node.h"
#include<algorithm>
#include<iostream>

using namespace std;
extern const int WINDOW_WIDTH;

Node::Node(int _n)
{
    ndata = std::unique_ptr<Data>(new Data(_n));
}

void Node::remove_child()
{
    children.clear();
}
void Node::remove_child(shared_ptr<Node> _somechild)
{
    auto res = find(children.begin(), children.end(), _somechild);
    if (res!=children.end())
        children.erase(res);
}

int Node::sibling_count()
{
    int value = 1;
    shared_ptr<Node> root_tmp(root.lock());
    if (root_tmp)
        value = root_tmp->getchildren().size();
    return value;
}

int Node::childrank(std::shared_ptr<Node> _somechild)
{
    int value=-1;
    auto it = find(children.begin(), children.end(), _somechild);
    if (it!=children.end())
        value = it - children.begin();
    return value;
}


shared_ptr<Node> Node::drop(int _backsteps)
{
    int i=0;
    shared_ptr<Node> value = root.lock();

    while (i<_backsteps && !value)
    {
        value = value->getroot();
        ++i;
    }
    value->remove_child();
    return value;
}

void Node::data_autofill()
{
    int firstchildx, lastchildx;
    int nonunique_siblings, node_siblings;
    int root_width, node_width, node_rank;
    int node_x;
    shared_ptr<Node> nonunique_node, nonunique_root;
    shared_ptr<Node> roottmp(root.lock());

    ndata->setdepth(roottmp?roottmp->getndata().getdepth()+1:0);
    ndata->sety(roottmp?roottmp->getndata().gety()+Data::DELTA_Y:Data::DELTA_Y);
    if (roottmp)
    {
        if (roottmp->getchildren().size()==1)
            ndata->setx(root.lock()->getndata().getx());
        else
        {
            shared_ptr<Node>nonunique_node(roottmp);
            node_siblings  = roottmp->getchildren().size();
            node_rank      = roottmp->childrank(shared_from_this());

            while (nonunique_node->sibling_count()==1 && nonunique_node->getroot())
                nonunique_node = nonunique_node->getroot();
            nonunique_root = nonunique_node->getroot();

            if (nonunique_root)
            {
                firstchildx         = nonunique_root->getchildren().front()->getndata().getx();
                lastchildx          = nonunique_root->getchildren().back()->getndata().getx();
                nonunique_siblings  = nonunique_root->getchildren().size(); // Includes nonunique_node
                root_width          = (lastchildx-firstchildx)/(nonunique_siblings-1);
                node_width          = root_width/node_siblings;
                node_x              = (nonunique_node->getndata().getx() - (root_width/2)) + (node_width/2) + node_width*node_rank;
                ndata->setx(node_x);
            }
            else
            {
                // nonunique_node is naturally unique here
                // because it has no root
                ndata->setx((DataNS::WINDOW_WIDTH/(2*node_siblings))+((node_rank*DataNS::WINDOW_WIDTH)/node_siblings)-(DataNS::WINDOW_WIDTH/2));
            }
        }
    }
    else
    {
        ndata->setx(0);
    }
}

void Node::print_info()
{
    std::shared_ptr<Node> roottmp(root.lock());
    std::cout << std::endl  << "Node Dbg: this->"    << shared_from_this() << "(id=" << ndata->getid() << ")"
                            << " & root->"           << root.lock();
    if (roottmp)
        std::cout << "(id=" << roottmp->getndata().getid() << ")";
}

#ifndef NODE_H
#define NODE_H
#include <memory>
#include <deque>
#include <iostream>
#include <algorithm>

#include "Data.h"

class Node : public std::enable_shared_from_this<Node>
{
    std::weak_ptr<Node> root;
    std::deque<std::shared_ptr<Node>> children;
    std::unique_ptr<Data> ndata;
    public:
        Node(int _n);
        inline void setroot(std::shared_ptr<Node> _root){root = _root;}
        inline std::shared_ptr<Node> getroot()const {return root.lock();}
        inline void setndata(std::unique_ptr<Data> _ndata){ndata = std::move(_ndata);}
        inline std::deque<std::shared_ptr<Node>> getchildren() const {return children;}
        inline const Data& getndata() const {return *ndata;}
        inline void add_child(std::shared_ptr<Node> _child){if (std::find(children.begin(), children.end(), _child)==children.end()) children.push_back(_child);}
        void remove_child();
        void remove_child(std::shared_ptr<Node> _somechild);
        int sibling_count();
        int childrank(std::shared_ptr<Node> _somechild);
        bool is_leaf(){return children.size()==0?true:false;}
        std::shared_ptr<Node> drop(int _backsteps);
        void data_autofill();
        void print_info();
};

#endif // NODE_H

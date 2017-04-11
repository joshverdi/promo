#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include <memory>
#include <deque>
#include <list>
#include "GraphNode.h"

namespace GraphNs
{
    const float INFINITE_LINK = 10000000;
    const int NODE_MAX_CHILDREN = 4;
}

class GraphEdge : public std::enable_shared_from_this<GraphEdge>
{
    public:
        GraphEdge();
        GraphEdge(std::shared_ptr<GraphNode> _first, std::shared_ptr<GraphNode> _second);
        GraphEdge(std::shared_ptr<GraphNode> _first, std::shared_ptr<GraphNode> _second, float _capacity);
        bool is_valid();
        void setcapacity(float _capacity){capacity=_capacity;}
        float getcapacity() const {return capacity;}
        std::shared_ptr<GraphNode> getfirst()  const {return first;}
        std::shared_ptr<GraphNode> getsecond() const {return second;}
        std::deque<std::shared_ptr<GraphEdge>> getdqpredecessor() const {return dqpredecessor;}
        std::deque<std::shared_ptr<GraphEdge>> getdqsuccessor() const {return dqsuccessor;}
        inline bool has_predecessor(){return dqpredecessor.empty()?false:true;}
        inline bool has_successor(){return dqsuccessor.empty()?false:true;}
        inline void reset_first(){first.reset();}
        inline void reset_second(){second.reset();}
        inline void open(){capacity=GraphNs::INFINITE_LINK;}
        inline bool is_open() const {return capacity==GraphNs::INFINITE_LINK?true:false;}
        inline void add_predecessor(std::shared_ptr<GraphEdge> _pred){if (std::find(dqpredecessor.begin(), dqpredecessor.end(), _pred)==dqpredecessor.end()) dqpredecessor.push_back(_pred);}
        inline void add_successor  (std::shared_ptr<GraphEdge> _succ){if (std::find(dqsuccessor.begin(), dqsuccessor.end(), _succ)==dqsuccessor.end()) dqsuccessor.push_back(_succ);}
    public:
        inline bool has_uniqpred() const {return dqpredecessor.size()==1?true:false;}
        inline bool has_uniqsucc() const {return dqsuccessor.size()==1?true:false;}
        std::shared_ptr<GraphEdge> uniquepredecessor(unsigned int _level);
        inline void add_capacity(float _flow){capacity+=_flow;}
        void transfercity(std::list<float> _list_capacity, int _level=1); // Transfer capacity. Level is the next upper level where we want the capacity to be transfered
        void resetcity(int _jump_level);
        void resetcity();
    protected:
    private:
        std::shared_ptr<GraphNode> first;
        std::shared_ptr<GraphNode> second;
        std::deque<std::shared_ptr<GraphEdge>> dqpredecessor, dqsuccessor;
        float capacity;
};

#endif // GRAPHEDGE_H

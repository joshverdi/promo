#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <deque>
#include <memory>
#include <algorithm>

class GraphNode
{
    public:
        GraphNode(int _id);
        int getid() const {return id;}
        void setid(int _id){id=_id;}
        float getenergy() const{return energy;}
        void setenergy(float _energy){energy=_energy;}
        char getcategory() const{return category;}
        void setcategory(char _category){category=_category;}
        inline void resetcategory(){category='c';}

        /*
        inline void add_predecessor(std::shared_ptr<GraphNode> _pred){if (std::find(dqpredecessor.begin(), dqpredecessor.end(), _pred)!=dqpredecessor.end()) dqpredecessor.push_back(_pred);}
        inline void add_successor  (std::shared_ptr<GraphNode> _succ){if (std::find(dqsuccessor.begin(), dqsuccessor.end(), _succ)!=dqsuccessor.end()) dqsuccessor.push_back(_succ);}
        */
    protected:
    private:
        int id;
        float energy;
        char category;
        /*
        std::deque<std::shared_ptr<GraphNode>> dqpredecessor;
        std::deque<std::shared_ptr<GraphNode>> dqsuccessor;
        */
};

#endif // GRAPHNODE_H

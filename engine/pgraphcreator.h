#ifndef PGRAPHCREATOR_H
#define PGRAPHCREATOR_H

#include <boost/bimap.hpp>
#include <boost/algorithm/string.hpp>

#include "GraphEdge.h"
#include "Node.h"
#include "graph.h"
#include "pderivhistoric.h"


typedef Graph<int,int,int> GraphType;
typedef boost::bimap<std::shared_ptr<Node>, std::shared_ptr<GraphEdge>> bimaptreegraph;
typedef boost::bimap<std::shared_ptr<Node>, std::shared_ptr<GraphEdge>>::value_type bimaptg_match;
typedef boost::bimap<std::shared_ptr<Node>, std::shared_ptr<GraphEdge>>::const_iterator bimaptg_iter;

typedef boost::bimap<PDerivHistoric*,std::shared_ptr<Node>> bimapdhnary;
typedef boost::bimap<PDerivHistoric*,std::shared_ptr<Node>>::value_type bimapdn_match;
typedef boost::bimap<PDerivHistoric*,std::shared_ptr<Node>>::const_iterator bimapdn_iter;


class PGraphCreator
{
    unsigned int treenode_nb;
    bimaptreegraph maptg;
    bimapdhnary mapdn;
    std::list<std::shared_ptr<GraphEdge>> list_sourced;
    std::list<std::shared_ptr<GraphEdge>> list_drained;

    std::list<std::shared_ptr<GraphEdge>> list_edges;
    std::list<std::shared_ptr<GraphNode>> list_nodes;
    std::shared_ptr<GraphNode> gnodesource;
    std::shared_ptr<GraphNode> gnodedrain;

    std::shared_ptr<Node> treenoderoot;
    std::shared_ptr<GraphEdge> gedgeroot;

    float selection_weight;

public:
    PGraphCreator();
    void setselectionweight(float _selection_weight){selection_weight = _selection_weight;}
    void setselectionweight(int _selection_weight){if (_selection_weight<0||_selection_weight>100) selection_weight = 0; else selection_weight = 0.01f*_selection_weight;};
    inline void reset_treenodenb(){treenode_nb=0;mapdn.clear();}
    std::shared_ptr<Node> gettreenoderoot() const {return treenoderoot;}
    void settreenoderoot(std::shared_ptr<Node> _treenoderoot){treenoderoot=_treenoderoot;}
    std::shared_ptr<GraphEdge> getgedgeroot() const {return gedgeroot;}
    void setgedgeroot(std::shared_ptr<GraphEdge> _gedgeroot){gedgeroot=_gedgeroot;}
    int  treenodecount(std::shared_ptr<Node> _treeroot);
    int  graphedgecount(std::shared_ptr<GraphEdge> _graphedgeroot);
    int  graphnodecount(std::shared_ptr<GraphEdge> _graphedgeroot);
    void connect_to_source(std::shared_ptr<GraphEdge> _edge);
    void connect_to_drain (std::shared_ptr<GraphEdge> _edge);
    void connect_edges(std::shared_ptr<GraphEdge> _edge1, std::shared_ptr<GraphEdge> _edge2);
    void connect_treenodes(std::shared_ptr<Node> _treenodeparent, std::shared_ptr<Node> _child);
    void attach(std::shared_ptr<Node> _treeroot, std::shared_ptr<GraphEdge> _edgesrc);
    std::shared_ptr<GraphEdge> creategraph(std::shared_ptr<Node> _treeroot);
    void initcapacity();
    void createdisplay(std::shared_ptr<GraphEdge> _edgesrc, std::stringstream& _ss);
    void createdisplay(std::shared_ptr<GraphEdge> _edgesrc, std::deque<std::shared_ptr<Node>> _selected_nodes, std::stringstream& _ss);
    std::string createnodedisplay(int _id, std::string _label, std::string _bgcolor, bool _dotpenstyle=false, bool _ellipsenode=false);
    std::string createedgedisplay(int _source, int _target, std::string _label, int _edgeid);
    void graphcut();
    std::deque<int> read_selection(std::string _filename);
    std::shared_ptr<Node> treenode_from_id(int _id, std::shared_ptr<Node> _rootnode);
    std::shared_ptr<GraphEdge> edge_from_treenode(std::shared_ptr<Node> _node);
    std::shared_ptr<Node> treenode_from_edge(std::shared_ptr<GraphEdge> _gedge);
    std::shared_ptr<Node> treenode_from_dh(PDerivHistoric* _pdh);
    PDerivHistoric* dh_from_treenode(std::shared_ptr<Node> _treenode);
    static std::list<float> captyprogression();
    static std::list<float> captyprogression(float _selection_weight);
    static std::list<float> captyprogression(int _selection_weight);
    std::shared_ptr<Node> createntree(PDerivHistoric* _pdh, PAlphabet* _alphabet); // Please call reset_treenodenb() before calling this function the first time
    std::deque<std::shared_ptr<GraphEdge>> selected_edges();
    std::deque<std::shared_ptr<Node>> selected_nodes();
    std::vector<PDerivHistoric*> selected_derivhistoric();
};

#endif // PGRAPHCREATOR_H

#include "pgraphcreator.h"

#include <fstream>
#include <sstream>

PGraphCreator::PGraphCreator()
{
    treenode_nb = 0;
    selection_weight = 0.5;
}

int PGraphCreator::treenodecount(std::shared_ptr<Node> _treeroot)
{
    int value = 0;
    if (_treeroot)
    {
        value++;
        for (auto kid: _treeroot->getchildren())
            value+=treenodecount(kid);
    }
    return value;
}


// WARNING: beware of infinite loop, returning edges are not implemented
int  PGraphCreator::graphedgecount(std::shared_ptr<GraphEdge> _graphedgeroot)
{
    int value = 0;
    if (_graphedgeroot)
    {
        value++;
        for (auto kid: _graphedgeroot->getdqsuccessor())
            value+=graphedgecount(kid);
    }
    return value;
}

// WARNING: beware of infinite loop, returning edges are not implemented
int  PGraphCreator::graphnodecount(std::shared_ptr<GraphEdge> _graphedgeroot)
{
    return graphedgecount(_graphedgeroot)+1;
}

void PGraphCreator::connect_to_source(std::shared_ptr<GraphEdge> _edge)
{
    _edge->reset_first();
    _edge->open();
    list_sourced.push_back(_edge);
}

void PGraphCreator::connect_to_drain(std::shared_ptr<GraphEdge> _edge)
{
    _edge->reset_second();
    _edge->open();
    list_drained.push_back(_edge);
}

void PGraphCreator::connect_edges(std::shared_ptr<GraphEdge> _edge1, std::shared_ptr<GraphEdge> _edge2)
{
    _edge1->add_successor  (_edge2);
    _edge2->add_predecessor(_edge1);
}

void PGraphCreator::connect_treenodes(std::shared_ptr<Node> _treenodeparent, std::shared_ptr<Node> _child)
{
    _treenodeparent->add_child(_child);
    _child->setroot(_treenodeparent);
}

void PGraphCreator::attach(std::shared_ptr<Node> _treeroot, std::shared_ptr<GraphEdge> _edgesrc)
{
    std::shared_ptr<GraphNode> gnode1, gnode2, gnodekid;
    std::shared_ptr<GraphEdge> gedgetmp, gedgekid;
    std::deque<std::shared_ptr<Node>> childrentmp;

    if (_treeroot&&_edgesrc)
    {
        if (_edgesrc->getsecond())
            gnode1 = _edgesrc->getsecond();
        if (!gnode1)
            gnode1 = std::make_shared<GraphNode>(2*_treeroot->getndata().getid());
        gnode2 = std::make_shared<GraphNode>(2*_treeroot->getndata().getid()+1);
        gedgetmp = std::shared_ptr<GraphEdge>(new GraphEdge(gnode1, gnode2, 1));
        maptg.insert(bimaptg_match(_treeroot, gedgetmp));
        connect_edges(_edgesrc, gedgetmp);

        list_nodes.push_back(gnode1);
        list_nodes.push_back(gnode2);
        list_edges.push_back(gedgetmp);

        if (_treeroot->is_leaf()==false)
        {
            childrentmp = _treeroot->getchildren();
            for (auto kid : childrentmp)
            {
                gnodekid.reset();
                gedgekid.reset();
                gnodekid = std::make_shared<GraphNode>(2*kid->getndata().getid());
                gedgekid = std::shared_ptr<GraphEdge>(new GraphEdge(gnode2, gnodekid, GraphNs::INFINITE_LINK));
                gedgekid->open();
                connect_edges(gedgetmp, gedgekid);
                attach(kid, gedgekid);
                list_edges.push_back(gedgekid);
            }
        }
        else
        {
            list_drained.push_back(gedgetmp);
        }
    }
}

std::shared_ptr<GraphEdge> PGraphCreator::creategraph(std::shared_ptr<Node> _treeroot)
{
    std::shared_ptr<GraphEdge> gedgedrain,  gedgesource, gedgetmp;
    std::shared_ptr<GraphNode> gnodetmp;

    list_sourced.clear();
    list_drained.clear();
    list_nodes.clear();
    list_edges.clear();
    maptg.clear();
    gnodesource.reset();
    gnodedrain.reset();
    gnodesource = std::shared_ptr<GraphNode>(new GraphNode(2*treenodecount(_treeroot)));
    gnodedrain  = std::shared_ptr<GraphNode>(new GraphNode(2*treenodecount(_treeroot)+1));

    list_nodes.push_back(gnodesource);
    list_nodes.push_back(gnodedrain);

    // Create the first edge from source/root
    gnodetmp    = std::make_shared<GraphNode>(2*_treeroot->getndata().getid());
    gedgesource = std::shared_ptr<GraphEdge>(new GraphEdge(gnodesource, gnodetmp, GraphNs::INFINITE_LINK));
    list_sourced.push_back(gedgesource);
    list_edges.push_back(gedgesource);

    attach(_treeroot, gedgesource);

    for (auto edge: list_drained)
    {
        gedgetmp = std::shared_ptr<GraphEdge>(new GraphEdge(edge->getsecond(), gnodedrain, GraphNs::INFINITE_LINK));
        connect_edges(edge, gedgetmp);
        list_edges.push_back(gedgetmp);
    }
    return gedgesource;
}

void PGraphCreator::initcapacity()
{
    //std::cout << std::endl << list_drained.size() << " edge(s) are now connected to the drain.";
    for (auto edge: list_drained)
        edge->resetcity();      // Set all non infinite capacity edges to zero

    for (auto edge: list_drained)
        edge->setcapacity(1);   // Set drain related edges to 1

    for (auto edge: list_drained)
        edge->resetcity(2);     // Add the capacity of every two subsequent edges to the double of the current capacity
}


void PGraphCreator::createdisplay(std::shared_ptr<GraphEdge> _edgesrc, std::stringstream& _ss)
{
    const std::string OPENING_GRAPHTAG("<graph>"), CLOSING_GRAPHTAG("</graph>");
    int edgeidtmp = 2000000; // Offset for edgeid - used only when displaying

    _ss << OPENING_GRAPHTAG;

    //value << std::endl << createnodedisplay(gnodesource->getid(), std::string("S"), std::string("BLUE"), true);
    //value << std::endl << createnodedisplay(gnodedrain->getid(), std::string("T"), std::string("BLUE"), true);

    for (auto node: list_nodes)
    {
        if (node==gnodesource)
            _ss << std::endl << createnodedisplay(node->getid(), "S", std::string("BLUECLAIR"), true, true);
        else if (node==gnodedrain)
            _ss << std::endl << createnodedisplay(node->getid(), "T", std::string("ROSECLAIR"), true, true);
        else
        {
            std::string bgcolor;
            if (node->getcategory()=='s')
                bgcolor = "BLUECLAIR";
            else if (node->getcategory()=='t')
                bgcolor = "ROSECLAIR";
            _ss << std::endl << createnodedisplay(node->getid(), std::to_string(node->getid()), bgcolor, false);
        }

    }

    for (auto edge: list_edges)
    {
        if (edge->getcapacity()<1000000)
            _ss << std::endl << createedgedisplay(edge->getfirst()->getid(), edge->getsecond()->getid(), std::to_string((int)edge->getcapacity()), ++edgeidtmp);
        else
            _ss << std::endl << createedgedisplay(edge->getfirst()->getid(), edge->getsecond()->getid(), "inf", ++edgeidtmp);
    }
    _ss << std::endl << CLOSING_GRAPHTAG;
}

/*std::string PGraphCreator::createnodedisplay(int _id, std::string _label, std::string _bgcolor, bool _dotpenstyle, bool _ellipsenode)
{
    std::string value;
    value = "<node ";
    if (_dotpenstyle==true)
        value += " pen_style=\"dot\" ";
    value += "fill=";
    if (_bgcolor.compare("BLUE")==0)
        _bgcolor=std::string("\"#bddcdc\"");
    else if (_bgcolor.compare("AZURE")==0)
        _bgcolor=std::string("\"#33ccff\"");
    else if (_bgcolor.compare("MALLOW")==0)
        _bgcolor=std::string("\"#cc0033\"");
    else if (_bgcolor.compare("ROSE")==0)
        _bgcolor=std::string("\"#ff99ff\"");
    else
        _bgcolor=std::string("\"#fffbc2\"");    // Creme
    value += _bgcolor;
    value += " pen_width=\"1\" ";
    value += "id=\"";
    value += std::to_string(_id);
    value += "\"";
    if (_ellipsenode==true)
        value += " shape=\"ellipse\"";
    value += " label=\"";
    if(_label.empty())
        value += std::to_string(_id);
    else
        value += _label;
    value += "\"";
    value += "/>";
    return value;
}
*/

void PGraphCreator::createdisplay(std::shared_ptr<GraphEdge> _edgesrc, std::deque<std::shared_ptr<Node>> _selected_nodes, std::stringstream& _ss)
{
    const std::string OPENING_GRAPHTAG("<graph>"), CLOSING_GRAPHTAG("</graph>");
    int edgeidtmp = 2000000; // Offset for edgeid - used only when displaying

    std::deque<std::shared_ptr<Node>> vec__slctd = selected_nodes();

    _ss << OPENING_GRAPHTAG;
    /*
    value << std::endl << createnodedisplay(gnodesource->getid(), std::string("S"), std::string("BLUE"), true);
    value << std::endl << createnodedisplay(gnodedrain->getid(), std::string("T"), std::string("BLUE"), true);
    */
    for (auto node: list_nodes)
    {
        if (node==gnodesource)
            _ss << std::endl << createnodedisplay(node->getid(), "S", std::string("BLUECLAIR"), true, true);
        else if (node==gnodedrain)
            _ss << std::endl << createnodedisplay(node->getid(), "T", std::string("ROSECLAIR"), true, true);
        else
        {
            std::string bgcolor = "\"#cc0033\"";
            for (auto qnode: _selected_nodes){
                if(node->getid()==(2*qnode->getndata().getid())){
                    if (node->getcategory()=='s')
                        bgcolor = "BLUEFONCE";
                    else if (node->getcategory()=='t')
                        bgcolor = "ROSEFONCE";
                }
            }
            for (auto drn: list_drained){
                if(node->getid()==drn->getsecond()->getid()){
                    bgcolor = "GRIS";
                }
            }
            for (auto gnode: vec__slctd){
                if(!gnode->getchildren().empty()){
                    if(node->getid()==(2*gnode->getndata().getid())){
                        if (node->getcategory()=='s')
                            bgcolor = "BLUEFONCE";
                        else if (node->getcategory()=='t')
                            bgcolor = "ROSEFONCE";
                    }
                }
             }

            if(bgcolor == "\"#cc0033\""){
                if (node->getcategory()=='s')
                    bgcolor = "BLUECLAIR";
                else if (node->getcategory()=='t')
                    bgcolor = "ROSECLAIR";
            }

            _ss << std::endl << createnodedisplay(node->getid(), std::to_string(node->getid()), bgcolor, false);
        }

    }

    for (auto edge: list_edges)
    {
        if (edge->getcapacity()<1000000)
            _ss << std::endl << createedgedisplay(edge->getfirst()->getid(), edge->getsecond()->getid(), std::to_string((int)edge->getcapacity()), ++edgeidtmp);
        else
            _ss << std::endl << createedgedisplay(edge->getfirst()->getid(), edge->getsecond()->getid(), "inf", ++edgeidtmp);
    }
    _ss << std::endl << CLOSING_GRAPHTAG;
}

std::string PGraphCreator::createnodedisplay(int _id, std::string _label, std::string _bgcolor, bool _dotpenstyle, bool _ellipsenode)
{
    std::string value;
    value = "<node ";
    if (_dotpenstyle==true)
        value += " pen_style=\"dot\" ";
    value += "fill=";
    if (_bgcolor.compare("BLUE")==0)
        _bgcolor=std::string("\"#bddcdc\"");
    else if (_bgcolor.compare("AZURE")==0)
        _bgcolor=std::string("\"#33ccff\"");
    else if (_bgcolor.compare("MALLOW")==0)
        _bgcolor=std::string("\"#cc0033\"");
    else if (_bgcolor.compare("ROSE")==0)
        _bgcolor=std::string("\"#ff99ff\"");
    else if (_bgcolor.compare("BLUEFONCE")==0)
        _bgcolor=std::string("\"#3366ff\"");
    else if (_bgcolor.compare("BLUECLAIR")==0)
        _bgcolor=std::string("\"#bbeeff\"");
    else if (_bgcolor.compare("ROSEFONCE")==0)
        _bgcolor=std::string("\"#ff3377\"");
    else if (_bgcolor.compare("ROSECLAIR")==0)
        _bgcolor=std::string("\"#ffbbee\"");
    else if (_bgcolor.compare("GRIS")==0)
        _bgcolor=std::string("\"#999999\"");
    else
        _bgcolor=std::string("\"#fffbc2\"");    // Creme
    value += _bgcolor;
    value += " pen_width=\"1\" ";
    value += "id=\"";
    value += std::to_string(_id);
    value += "\"";
    if (_ellipsenode==true)
        value += " shape=\"ellipse\"";
    value += " label=\"";
    if(_label.empty())
        value += std::to_string(_id);
    else
        value += _label;
    value += "\"";
    value += "/>";
    return value;
}


std::string PGraphCreator::createedgedisplay(int _source, int _target, std::string _label, int _edgeid)
{
    std::string value;
    // We aim an unique ID for every edge, the IDs must not collide with node IDs

    value += "<node type=\"edge_label\" id=\"";
    value += std::to_string(_edgeid);
    value += "\" ";
    value += "label=\"";
    if (_label.empty())
        _label = std::string("0");
    value += _label;
    value += "\"";
    value += "/>";

    value += "\n";
    value += "<edge ";
    value += "target=\"";
    value += std::to_string(_edgeid);
    value += "\" ";
    value += "source=\"";
    value += std::to_string(_source);
    value += "\" ";
    value += "/>";

    value += "\n";
    value += "<edge ";
    value += "target=\"";
    value += std::to_string(_target);
    value += "\" ";
    value += "source=\"";
    value += std::to_string(_edgeid);
    value += "\" ";
    value += "/>";
    return value;
}

void PGraphCreator::graphcut()
{
    GraphType *g = new GraphType(/*estimated # of nodes*/ list_nodes.size()-2, /*estimated # of edges*/ list_edges.size());
    for (size_t i=0; i<list_nodes.size()-2; i++)
        g -> add_node();

    for (auto src: list_sourced)
    {
        //std::cout << std::endl << "Source Node ID:" << src->getsecond()->getid() << " , capacity=" << src->getcapacity();
        g -> add_tweights(src->getsecond()->getid(),   /* capacities */  src->getcapacity(), 0);
    }

    for (auto drn: list_drained)
    {
        //std::cout << std::endl << "Drain Node ID:" << drn->getsecond()->getid() << " , capacity=" << drn->getcapacity();
        g -> add_tweights(drn->getsecond()->getid(),   /* capacities */  0, GraphNs::INFINITE_LINK);
    }


    for (auto edge: list_edges)
    {
        if (edge->getfirst()!=gnodesource&&edge->getsecond()!=gnodedrain)
            g -> add_edge( edge->getfirst()->getid(), edge->getsecond()->getid(),    edge->getcapacity(),  0 );
    }

    int flow = g -> maxflow();

    std::cout << std::endl << "Flow = " << flow;
    std::cout << std::endl << "Minimum cut:";

    for (size_t i=0; i<list_nodes.size()-2; i++)
    {
//        if (g->what_segment(i) == GraphType::SOURCE)
//            std::cout << std::endl << "Node " << i << " is in the SOURCE set.";
//        else
//            std::cout << std::endl << "Node " << i << " is in the SINK set.";
        for (auto node: list_nodes)
        {
            if(node->getid()==i)
            {
                if (g->what_segment(i) == GraphType::SOURCE)
                    node->setcategory('s');
                else
                    node->setcategory('t');
                break;
            }
        }
    }

    delete g;
}

std::deque<int> PGraphCreator::read_selection(std::string _filename)
{
    std::deque<int> value;
    int n;
    std::string str;
    std::vector<std::string> vec_str;
    std::ifstream myfile(_filename);
    std::stringstream buffer;
    buffer << myfile.rdbuf();
    str = buffer.str();
    boost::split(vec_str, str, boost::is_any_of(","));
    for (auto s: vec_str)
    {
        std::istringstream(s) >> n;
        value.push_back(n);
    }
    myfile.close();

    return value;
}

std::shared_ptr<GraphEdge> PGraphCreator::edge_from_treenode(std::shared_ptr<Node> _node)
{
    std::shared_ptr<GraphEdge> value;
    if (_node)
    {
        try
        {
            value = maptg.left.at(_node);
        }
        catch (...)
        {
            std::cerr << std::endl << "WARNING: No correspondence for node #" << _node->getndata().getid();
        }
    }
    else
    {
        std::cerr << std::endl << "WARNING: Requesting match for null node!";
    }
    return value;
}

std::shared_ptr<Node> PGraphCreator::treenode_from_edge(std::shared_ptr<GraphEdge> _gedge)
{
    std::shared_ptr<Node> value;
    if (_gedge)
    {
        try
        {
            value = maptg.right.at(_gedge);
        }
        catch (...)
        {
            std::cerr << std::endl << "WARNING: No correspondence for the requested graph edge!";
        }
    }
    else
    {
        std::cerr << std::endl << "WARNING: Requesting match for null graph edge!";
    }
    return value;
}

std::shared_ptr<Node> PGraphCreator::treenode_from_dh(PDerivHistoric* _pdh)
{
    std::shared_ptr<Node> value;
    if (_pdh)
    {
        try
        {
            value = mapdn.left.at(_pdh);
        }
        catch(...)
        {
            std::cerr << std::endl << "WARNING: No n-ary tree correspondence for derivation historic instance!";
        }
    }
    else
    {
        std::cerr << std::endl << "WARNING: Requesting match for unexisting derivation historic!";
    }
    return value;
}

PDerivHistoric* PGraphCreator::dh_from_treenode(std::shared_ptr<Node> _treenode)
{
    PDerivHistoric* value=0;
    if (_treenode)
    {
        try
        {
            value = mapdn.right.at(_treenode);
        }
        catch(...)
        {
            std::cerr << std::endl << "WARNING: No historic correspondence for tree node!";
        }
    }
    else
    {
        std::cerr << std::endl << "WARNING: Requesting match for null n-ary tree node!";
    }
    return value;
}


std::list<float> PGraphCreator::captyprogression()
{
    std::list<float> value;
    float alpha0, alphak;
    int maxtreedepth = 10;

    alpha0 = pow(2,maxtreedepth);
    value.push_back(alpha0);
    for (int i=0; i<maxtreedepth-1; )
    {
        alphak = value.back() - pow(2,++i);
        value.push_back(alphak);
    }
    return value;
}

std::list<float> PGraphCreator::captyprogression(float _selection_weight)
{
    std::list<float> value;
    float alpha0, alphak;
    int maxtreedepth = 10;

    alpha0 = pow(2,maxtreedepth);
    value.push_back(alpha0);
    for (int i=0; i<maxtreedepth-1; i++)
    {
        alphak = value.back() - (pow(2,i)/_selection_weight);
        value.push_back(alphak);
    }
    return value;
}

std::list<float> PGraphCreator::captyprogression(int _selection_weight)
{
    std::list<float> value;
    float alpha0, alphak;
    float ratio;
    int maxtreedepth = 10;

    if (_selection_weight>=0 && _selection_weight<=100)
        ratio = _selection_weight*0.01f;
    else ratio = 0.5f;

    alpha0 = pow(2,maxtreedepth);
    value.push_back(alpha0);
    for (int i=0; i<maxtreedepth-1; i++)
    {
        alphak = value.back() - (pow(2,i)/ratio);
        value.push_back(alphak);
    }
    return value;
}


std::shared_ptr<Node> PGraphCreator::createntree(PDerivHistoric* _pdh, PAlphabet* _alphabet)
{
    std::shared_ptr<Node> value, nodetmp;
    std::vector<PDerivHistoric*> vec_kids;
    static int n=0;
    if (_pdh)
    {
        if (_alphabet->is_alphabet(_pdh->getWord()))
        {
            //For debug:
            //std::cout << std::endl << "alphabet #" << n++ << ": " << _pdh->getWord()->getText();
            value = std::shared_ptr<Node>(new Node(treenode_nb++));
            mapdn.insert(bimapdn_match(_pdh, value));
            vec_kids = _pdh->getVecChildren();
            for (auto kid: vec_kids)
            {
                nodetmp = createntree(kid, _alphabet);
                if (nodetmp)
                    connect_treenodes(value,  nodetmp);
            }
        }
    }
    return value;
}

std::deque< std::shared_ptr<GraphEdge> > PGraphCreator::selected_edges()
{
    std::deque< std::shared_ptr<GraphEdge> > value;
    for (auto edge: list_edges)
    {
        if ((edge->getfirst()->getcategory()=='s')&&(edge->getsecond()->getcategory()=='t'))
        value.push_back(edge);
    }
    return value;
}

std::deque< std::shared_ptr<Node> > PGraphCreator::selected_nodes()
{
    std::deque< std::shared_ptr<Node> > value;
    std::shared_ptr<Node> nodetmp;
    std::deque< std::shared_ptr<GraphEdge> > dqslctdedges = selected_edges();

    for (auto edge:dqslctdedges)
    {
        nodetmp = treenode_from_edge(edge);
        if (nodetmp)
            value.push_back(nodetmp);
    }
    return value;
}

std::vector<PDerivHistoric*> PGraphCreator::selected_derivhistoric()
{
    std::vector<PDerivHistoric*> value;

    std::deque< std::shared_ptr<Node> > dqnodes = selected_nodes();
    for (auto node:dqnodes)
    {
        PDerivHistoric* pdh=0;
        pdh = dh_from_treenode(node);
        if (pdh)
            value.push_back(pdh);
    }
    return value;
}


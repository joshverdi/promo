#ifndef PSHAPEINITIALIZER_H
#define PSHAPEINITIALIZER_H

#include <string>
#include <map>
#include "engine/procedural.h"
#include "engine/ptextparser.h"
#include "engine/ptextmap.h"

class PLSystemParametric;

class PShapeInitializer
{
    std::string text;
    unsigned int depth, max_depth, index;
    bool is_leaf; // For depth max and object at the end of brackets
    std::list<float> list_ray;
    float basis, top, ray;
public:
    static float delta;
    static float rate;
public:
    PShapeInitializer();
    PShapeInitializer(std::string _text);
    PShapeInitializer(const PShapeInitializer& _psi);
    std::string getText() const {return text;}
    unsigned int getIndex() const {return index;}
    void setIndex(unsigned int _index){index = _index;}
    unsigned int getDepth() const {return depth;}
    void setDepth(unsigned int _depth){depth = _depth;}
    unsigned int getMaxDepth() const {return max_depth;}
    void setMaxDepth(unsigned int _max_depth){max_depth = _max_depth;}
    bool getIsLeaf() const {return is_leaf;}
    void setIsLeaf(bool _is_leaf) {is_leaf = _is_leaf;}
    void refresh_state(char _c, unsigned int _index);
    float getBasis() const {return basis;};
    void setBasis(float _basis) {basis = _basis;}
    float getTop() const {return top;}
    void setTop(float _top) {top = _top;}
    float getRay() const {return ray;}
    void setRay(float _ray) {ray = _ray;}
public:
    void compute_ray(int _method);
    void compute_ray(std::string _text, unsigned int _idx, float& _basis,  float& _top, int _method);
    void subb_heurist();
    void subb_heurist(std::string _text, unsigned int _idx, float& _basis,  float& _top);
    void sibling_heurist(std::string _text, unsigned int _idx, float& _basis,  float& _top);
    void sibling_heurist();
    unsigned int post_sibling(std::string _text, unsigned int _idx, unsigned int _branching_depth);
    unsigned int post_sibling();
    bool is_extremity();
    bool is_extremity(std::string _text, unsigned int _idx);
    std::vector< std::map<unsigned int, unsigned int> > map_str();
    void check_sibling(std::vector< std::map<unsigned int, unsigned int> >& _vec_map, unsigned int _index, unsigned int& _depth, unsigned int& _siblings);
    //std::map<unsigned int, std::pair<float, float> > precompute_rays(); // output: <index, basis, top>
    std::map<unsigned int, std::pair<float, float> > precompute_rays(std::vector<PTextMap*>& _vec_tm);
    bool check_rays(std::map<unsigned int, std::pair<float, float> >& _src, unsigned int _index, float& _basis, float& _top);
    //void map(std::string _text, std::vector<PTextMap*>& _vec_map);
    void map(std::string _text, std::vector<PTextMap*>& _vec_map, PLSystemParametric* _lsys);
};

#endif // PSHAPEINITIALIZER_H

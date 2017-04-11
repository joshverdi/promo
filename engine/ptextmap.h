#ifndef PTEXTMAP_H
#define PTEXTMAP_H
#include <vector>
#include <string>
#include <map>

class PTextMap
{
    PTextMap* parent;
    unsigned int index;
    unsigned int depth;
    unsigned int mindepth, maxdepth;
    std::vector<PTextMap*> vec_children;
    std::vector<float> vec_dimension; // we put basis and top in case of a conic shape, otherwise the dimension could be other types
    //bool is_leaf;
    //std::string text; // replace by index78
public:
    PTextMap();
    PTextMap(unsigned int _index);
    PTextMap(PTextMap* _parent, unsigned int _index);
    PTextMap(unsigned int _depth, unsigned int _mindepth, unsigned int _maxdepth);
    PTextMap(PTextMap* _parent, unsigned int _index, unsigned int _depth, unsigned int _mindepth, unsigned int _maxdepth);
//    std::string getText() const {return text;}
//    void setText(std::string _text) {text = _text;}
    unsigned int getIndex() const {return index;}
    void setIndex(unsigned int _index) {index = _index;}
    unsigned int getDepth() const {return depth;}
    void setDepth(unsigned int _depth) {depth = _depth;}
    unsigned int getMinDepth() const {return mindepth;}
    void setMinDepth(unsigned int _mindepth) {mindepth = _mindepth;}
    unsigned int getMaxDepth() const {return maxdepth;}
    void setMaxDepth(unsigned int _maxdepth) {maxdepth = _maxdepth;}
    PTextMap* getParent() const {return parent;}
    void setParent(PTextMap* _parent){parent = _parent;}
//    void setIsLeaf(bool _is_leaf){is_leaf = _is_leaf;}
//    bool getIsLeaf() const {return is_leaf;}
    void add_children(PTextMap* _tm) {/*is_leaf =false;*/vec_children.push_back(_tm);}
    void clear_children(){vec_children.clear();}
    void update_childmax();
    void update_childmin();
    void update_max();
    void update_min();
    void add_dimension(float _dim){vec_dimension.push_back(_dim);}
    void clear_dimension(){vec_dimension.clear();}
    void apply_conic(float _basis_ray, std::map<unsigned int, std::pair<float, float> >& _map_ray);
    std::vector<unsigned int> nodes();
};

#endif // PTEXTMAP_H

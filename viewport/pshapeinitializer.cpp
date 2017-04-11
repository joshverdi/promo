#include <list>

#include "pshapeinitializer.h"
#include "grammar/palphabet.h"
#include "engine/ptextparser.h"
#include "engine/ptextmap.h"
#include "engine/plsystemparametric.h"

float PShapeInitializer::delta = 0.1f*PROCEDURAL_UNIT;
float PShapeInitializer::rate = 0.995; // Avoiding ray equal to 0

PShapeInitializer::PShapeInitializer()
{
    index = 0;
    max_depth = depth = 0;
    is_leaf = false;
    ray=top=basis=delta;
    //top=delta*(1-rate);
}

PShapeInitializer::PShapeInitializer(std::string _text)
{
    index = 0;
    max_depth = depth = 0;
    text = _text;
    is_leaf = false;
    ray=top=basis=delta;
    //top=delta*(1-rate);
}

PShapeInitializer::PShapeInitializer(const PShapeInitializer& _psi)
{
    depth     = _psi.getDepth();
    max_depth = _psi.getMaxDepth();
    text      = _psi.getText();
    is_leaf   = _psi.getIsLeaf();
    basis     = _psi.getBasis();
    top       = _psi.getTop();
    ray       = _psi.getRay();
    index     = _psi.getIndex();
}

void PShapeInitializer::refresh_state(char _c, unsigned int _index)
{
    unsigned int parenth_closer;
    PTextParser parser(text);
    PAlphabet alphabet;

    index = _index;
    switch(_c)
    {
    case '[':
        depth++;
        max_depth=PROCEDURAL_MAX(max_depth, depth);
        list_ray.push_back(top);
        break;
    case ']':
        depth--;
        ray=list_ray.back();
        list_ray.pop_back();
        break;
    default:
        if (alphabet.is_alphabet(text[index]))
        {
            parenth_closer = 0;
            parenth_closer = parser.find_bracket_closer(index+1);
            if (parenth_closer!=0&&parenth_closer<text.size())
            {
                if ((parenth_closer==text.size())||(text[parenth_closer+1]==']'))
                    is_leaf=true;
            }
            else is_leaf = false;
            compute_ray(SIBLING_METHOD);
        }
    }
}

void PShapeInitializer::compute_ray(int _method)
{
    switch (_method)
    {
    case SIBLING_METHOD:
        sibling_heurist();
        break;
    }
}

void PShapeInitializer::compute_ray(std::string _text, unsigned int _idx, float& _basis,  float& _top, int _method)
{
    switch (_method)
    {
    case BRANCHING_DEPTH_METHOD:
        subb_heurist(_text, _idx, _basis, _top);
        break;
    case SIBLING_METHOD:
        sibling_heurist(_text, _idx, _basis, _top);
        break;
    }
}

void PShapeInitializer::subb_heurist()
{
    bool is_leaf = false;

    if (index>=text.size()||text.empty())
    {
        basis = top = 0.1f;
        return;
    }

    if (max_depth==0)
    {
        basis = delta;
        top = delta*(1-rate);
        return;
    }

    top   = is_leaf?(delta*(1-rate)):delta*(1 -   depth   * rate/max_depth);
    basis = delta*(1 - ((((float)depth)-1.0f) * rate/max_depth));
}

void PShapeInitializer::subb_heurist(std::string _text, unsigned int _idx, float& _basis,  float& _top)
{
    unsigned int my_max_depth=0;
    unsigned int my_depth=0;
    unsigned int i=0;
    unsigned int parenth_closer;

    bool is_leaf = false;
    PTextParser parser(_text);
    std::string::iterator iter;

    parenth_closer = parser.find_bracket_closer(_idx+1);
    if (_idx>=_text.size()||_text.empty())
    {
        _basis=_top=0.1f;
        return;
    }
    for (iter=_text.begin(); iter!=_text.end(); iter++)
    {
        switch(*iter)
        {
        case '[':
            my_depth++;
            my_max_depth=PROCEDURAL_MAX(my_max_depth, my_depth);
            break;
        case ']':
            my_depth--;
            break;
        }
    }
    if (my_max_depth==0)
    {
        _basis = delta;
        _top = delta*(1-rate);
        return;
    }
    for (iter=_text.begin(), i=0, my_depth=0; iter!=_text.end(); iter++, i++)
    {
        if (i==_idx)
        {
            if (parenth_closer!=0&&parenth_closer<_text.size())
            {
                if ((parenth_closer==_text.size())||(_text[parenth_closer+1]==']'))
                {
                    is_leaf=true;
                }
            }
            break;
        }
        switch(*iter)
        {
        case '[':
            my_depth++;
            break;
        case ']':
            my_depth--;
            break;
        }
    }
    _top   = is_leaf?(delta*(1-rate)):delta*(1 -   my_depth   * rate/my_max_depth);
    _basis = delta*(1 - ((((float)my_depth)-1.0f) * rate/my_max_depth));
}

void PShapeInitializer::sibling_heurist()
{
    unsigned int sibling = post_sibling();
    basis = ray;
    if (is_extremity())
    {
        top = basis*(1-rate);
    }
    else
    {
        top = basis*(1-(rate/(sibling+1)));
        ray = top;
    }
}

void PShapeInitializer::sibling_heurist(std::string _text, unsigned int _idx, float& _basis,  float& _top)
{
    unsigned int sibling = 0;
    unsigned int offset=1;
    unsigned int prnthss_closer;
    unsigned int my_depth=0;
    PAlphabet alphabet;
    PTextParser textparser(_text);

    _basis=delta;
    _top=delta*(1-rate);
    if (_idx>=_text.size()||_text.empty())
    {
        _basis=_top=0.1f;
        return;
    }
    for (unsigned int i=0; i<_idx; i+=offset)
    {
        offset=1;
        if (_text[i]=='[')
        {
            my_depth++;
            list_ray.push_back(_basis);
        }
        else if (_text[i]==']')
        {
            my_depth--;
            _basis=list_ray.back();
            list_ray.pop_back();
        }
        else if (alphabet.is_alphabet(_text[i]))
        {
            sibling = post_sibling(_text, i, my_depth);
            _basis = _basis*(1-(rate/(float)(sibling+1)));
            prnthss_closer = textparser.find_bracket_closer(i+1);
            if (prnthss_closer!=0)
                offset = prnthss_closer-i+1;
        }
    }
    sibling = post_sibling(_text, _idx, my_depth);
    _top = is_extremity(_text, _idx)?_basis*(1-rate):_basis*(1-(rate/(sibling+1)));
}

unsigned int PShapeInitializer::post_sibling()
{
    PAlphabet alphabet;
    PTextParser textparser(text);
    unsigned int value=0;
    unsigned int my_depth=depth;
    unsigned int offset=1;
    unsigned int prnthss_closer=0;

    if (text.empty()||index>=text.size()) return value;
    for (unsigned int i=index; i<text.size(); i+=offset)
    {
        offset=1;
        switch(text[i])
        {
        case '[':
            my_depth++;
            break;
        case ']':
            my_depth--;
            break;
        default:
            if ((my_depth==depth)&&(alphabet.is_alphabet(text[i])))
            {
                value++;
                prnthss_closer = textparser.find_bracket_closer(i+1);
                if (prnthss_closer!=0)
                    offset = prnthss_closer-i+1;
            }
        }
    }
    return value;
}

unsigned int PShapeInitializer::post_sibling(std::string _text, unsigned int _idx, unsigned int _branching_depth)
{
    PAlphabet alphabet;
    PTextParser textparser(_text);
    unsigned int value=0;
    unsigned int my_depth=_branching_depth;
    unsigned int offset=1;
    unsigned int prnthss_closer=0;

    if (_text.empty()||_idx>=_text.size()) return value;
    for (unsigned int i=_idx; i<_text.size(); i+=offset)
    {
        offset=1;
        switch(_text[i])
        {
        case '[':
            my_depth++;
            break;
        case ']':
            my_depth--;
            break;
        default:
            if ((my_depth==_branching_depth)&&(alphabet.is_alphabet(_text[i])))
            {
                value++;
                prnthss_closer = textparser.find_bracket_closer(i+1);
                if (prnthss_closer!=0)
                    offset = prnthss_closer-i+1;
            }
        }
    }
    return value;
}

bool PShapeInitializer::is_extremity()
{
    bool value = false;
    unsigned int prnthss_closer, offset;
    PTextParser textparser(text);
    PAlphabet alphabet;

    offset = 1;
    if (index>=text.size())
        value = true;
    else if (alphabet.is_alphabet(text[index]))
    {
        prnthss_closer = textparser.find_bracket_closer(index+1);
        if (prnthss_closer!=0)
            offset = prnthss_closer-index+1;
        if (index+offset<text.size())
        {
            if (text[index+offset]==']')
            {
                value = true;
            }
        }
    }
    return value;
}

bool PShapeInitializer::is_extremity(std::string _text, unsigned int _idx)
{
    bool value = false;
    unsigned int prnthss_closer, offset;
    PTextParser textparser(_text);
    PAlphabet alphabet;

    offset = 1;
    if (_idx>=_text.size())
        value = true;
    else if (alphabet.is_alphabet(_text[_idx]))
    {
        prnthss_closer = textparser.find_bracket_closer(_idx+1);
        if (prnthss_closer!=0)
            offset = prnthss_closer-_idx+1;
        if (_idx+offset<_text.size())
        {
            if (_text[_idx+offset]==']')
            {
                value = true;
            }
        }
    }
    return value;
}

std::vector< std::map<unsigned int, unsigned int> > PShapeInitializer::map_str()
{
    std::vector< std::map<unsigned int, unsigned int> > vec_incomplete_map;
    std::vector< std::map<unsigned int, unsigned int> > value;
    std::map<unsigned int, unsigned int> map_sibling; // index, depth
    unsigned int my_depth=0;
    unsigned int offset;
    PTextParser tp(text);
    PAlphabet alphabet;

    for (unsigned int i=0; i<text.size(); i+=offset)
    {
        offset=1;
        switch(text[i])
        {
        case '[':
            my_depth++;
            vec_incomplete_map.push_back(map_sibling);
            map_sibling.clear();
            break;
        case ']':
            //vec_incomplete_map.push_back(map_sibling);
            value.push_back(map_sibling); // For clarity but not efficient
            map_sibling.clear();
            if (!vec_incomplete_map.empty())
            {
                map_sibling = vec_incomplete_map.back();
                vec_incomplete_map.pop_back();
            }
            my_depth--;
            break;
        default:
            unsigned int prnthss_closer = tp.find_bracket_closer(i+1);
            if (prnthss_closer!=0)
                offset = prnthss_closer-i+1;
            if (alphabet.is_alphabet(text[i]))
            {
                map_sibling.insert(std::pair<unsigned int, unsigned int>(i,my_depth));
            }
        }
    }
    if (!map_sibling.empty())
    {
        vec_incomplete_map.push_back(map_sibling);
    }
    while (!vec_incomplete_map.empty())
    {
        value.push_back(vec_incomplete_map.back());
        vec_incomplete_map.pop_back();
    }
    return value;
}

void PShapeInitializer::check_sibling(std::vector< std::map<unsigned int, unsigned int> >& _vec_map, unsigned int _index, unsigned int& _depth, unsigned int& _siblings)
{
    std::vector< std::map<unsigned int, unsigned int> >::iterator iter = _vec_map.begin();
    std::map<unsigned int, unsigned int>::iterator iter_idxfound;

    _depth = _siblings = 0;
    while(iter != _vec_map.end())
    {
        iter_idxfound = (*iter).find(_index);
        if (iter_idxfound!=(*iter).end())
        {
            _depth = iter_idxfound->second;
            _siblings = (*iter).size();
        }
        iter++;
    }
}

//// IDSR will stand for index, depth, sibling
//void PShapeInitializer::check_sibling(std::vector< std::map<unsigned int, unsigned int> >& _vec_map_in, std::map<unsigned int, std::pair<unsigned int, unsigned int> >& _map_ids_out)
//{
//    std::vector< std::map<unsigned int, unsigned int> >::iterator iter_map;
//    std::map<unsigned int, unsigned int>::iterator iter_idxdpth;

//    _map_ids_out.clear();
//    iter_map = _vec_map_in.begin();
//    while(iter_map != _vec_map_in.end())
//    {
//        for (iter_idxdpth=(*iter_map).begin(); iter_idxdpth!=(*iter_map).end(); iter_idxdpth++)
//            _map_ids_out.insert( std::make_pair( iter_idxdpth->first, std::make_pair(iter_idxdpth->second, (*iter_map).size())) );
//        iter_map++;
//    }
//}

//std::map<unsigned int, std::pair<float, float> > PShapeInitializer::precompute_rays()
//{
//    unsigned int my_maxdepth, my_index, my_depth, my_siblings, my_younger, my_elder;
//    float my_basis, my_top;
//    std::map<unsigned int, unsigned int>::iterator iter_idxdpth;
//    std::vector< std::map<unsigned int, unsigned int> > vec_map;
//    std::vector< std::map<unsigned int, unsigned int> >::iterator iter_map;

//    std::map<unsigned int, std::pair<float, float> > value;
//    std::map<unsigned int, float> depth_basis, depth_;
//    std::list<float> basis_stack;

//    my_depth = my_siblings = my_maxdepth = my_elder = my_younger =0;
//    my_basis = my_top = delta;
//    vec_map = map_str();
//    iter_map = vec_map.begin();

//    basis_stack.push_back(delta);
//    // All depths are equal for one range of map
//    while (iter_map != vec_map.end())
//    {
//        my_maxdepth = PROCEDURAL_MAX((*iter_map).begin()->second, my_maxdepth);
//        //std::cout << std::endl << (*iter_map).begin()->first << " <-> " << (*iter_map).begin()->second;
//        iter_map++;
//    }
//    for (unsigned int i=0; i<my_maxdepth; i++)
//    {
//        depth_basis.insert(std::make_pair(i, delta*(1 - ((((float)i)-1.0f) * rate/my_maxdepth))));
//        //depth_top.insert(std::make_pair(i, my_basis));
//    }
//    for (iter_map = vec_map.begin(); iter_map != vec_map.end(); iter_map++)
//    {
//        unsigned int sibling_indexer=0;
//        for (iter_idxdpth=(*iter_map).begin(); iter_idxdpth!=(*iter_map).end(); iter_idxdpth++, sibling_indexer++)
//        {
//            if (my_depth<iter_idxdpth->second)
//            {
//                basis_stack.push_back(my_top);
//                my_basis = my_top;
//            }
//            else if (my_depth>iter_idxdpth->second)
//            {
//                my_basis = basis_stack.back();
//                basis_stack.pop_back();
//            }
//            else // Equal
//            {
//                my_basis = my_top;
//            }
//            my_index = iter_idxdpth->first;
//            my_depth = iter_idxdpth->second;
//            my_siblings = (*iter_map).size();
//            my_younger = my_siblings - sibling_indexer - 1;
//            my_elder = sibling_indexer;
//            my_top = my_basis*(1-(rate/(my_younger+1+(my_maxdepth-my_depth))));
//            value.insert(std::make_pair(my_index, std::make_pair(my_basis, my_top)));
//            std::cout << std::endl << "index:" << iter_idxdpth->first << "\tdepth:" << iter_idxdpth->second << "\tbasis:" << my_basis << "\ttop:" << my_top;
//        }
//    }

//    return value;
//}

bool PShapeInitializer::check_rays(std::map<unsigned int, std::pair<float, float> >& _src, unsigned int _index, float& _basis, float& _top)
{
    std::map<unsigned int, std::pair<float, float> >::iterator iter;
    std::pair<float, float> my_rays;
    bool value(false);

    iter = _src.find(_index);
    if (iter!=_src.end())
    {
        my_rays = iter->second;
        _basis = my_rays.first;
        _top = my_rays.second;
        value = true;
    }
    return value;
}

//void PShapeInitializer::map(std::string _text, std::vector<PTextMap*>& _vec_map)
//{
//    PTextParser tp(_text);
//    PTextMap *tm, *tm_root;
//    PAlphabet alphabet;
//    std::list<PTextMap*> stack_tm;
//    unsigned int sq_depth, offset;

//    sq_depth = 0;
//    tm_root = tm = 0;
//    for (unsigned int i=0; i<_text.size(); i+=offset)
//    {
//        //tm = new PTextMap
//        offset=1;
//        switch(_text[i])
//        {
//        case '[':
//            sq_depth++;
//            if (tm_root!=0)
//            {
//                stack_tm.push_back(tm_root);
//            }
//            break;
//        case ']':
//            if (!stack_tm.empty())
//            {
//                tm_root = stack_tm.back();
//                stack_tm.pop_back();
//            }
//            else tm_root = tm;
//            sq_depth--;
//            break;
//        default:
//            unsigned int prnthss_closer = tp.find_bracket_closer(i+1);
//            if (prnthss_closer!=0)
//                offset = prnthss_closer-i+1;
//            if (alphabet.is_alphabet(_text[i]))
//            {
//                if (tm_root!=0)
//                {
//                    tm = new PTextMap(tm_root, i);
//                }
//                else
//                {
//                    tm  = new PTextMap(i);
//                    _vec_map.push_back(tm);
//                }
//                tm_root = tm;
//            }
//        }
//    }
//    for (unsigned int i=0; i<_vec_map.size(); i++)
//    {
//        _vec_map[i]->update_childmax();
//        _vec_map[i]->update_childmin();
//    }
//}

void PShapeInitializer::map(std::string _text, std::vector<PTextMap*>& _vec_map, PLSystemParametric* _lsys)
{
    PTextParser tp(_text);
    PTextMap *tm, *tm_root;
    PAlphabet alphabet;
    size_t alphabet_size;
    std::list<PTextMap*> stack_tm;
    unsigned int sq_depth, offset;

    sq_depth = 0;
    tm_root = tm = 0;
    _lsys->init_map_offset();
    for (unsigned int i=0; i<_text.size(); i+=offset)
    {
        //tm = new PTextMap
        offset=1;
        switch(_text[i])
        {
        case '[':
            sq_depth++;
            if (tm_root!=0)
            {
                stack_tm.push_back(tm_root);
            }
            break;
        case ']':
            if (!stack_tm.empty())
            {
                tm_root = stack_tm.back();
                stack_tm.pop_back();
            }
            else tm_root = tm;
            sq_depth--;
            break;
        default:
            offset = _lsys->current_offset(i);
            alphabet_size = _lsys->getAlphabet()->search(_text, i);
            if (alphabet_size!=0)
            {
                if (tm_root!=0)
                {
                    tm = new PTextMap(tm_root, i);
                }
                else
                {
                    tm  = new PTextMap(i);
                    _vec_map.push_back(tm);
                }
                tm_root = tm;
            }
        }
    }
    for (unsigned int i=0; i<_vec_map.size(); i++)
    {
        _vec_map[i]->update_childmax();
        _vec_map[i]->update_childmin();
    }
}

std::map<unsigned int, std::pair<float, float> > PShapeInitializer::precompute_rays(std::vector<PTextMap*>& _vec_tm)
{
    PTextMap* tm=0;
    std::map<unsigned int, std::pair<float, float> > value;
    for (unsigned int i=0; i<_vec_tm.size(); i++)
    {
        tm = _vec_tm[i];
        tm->apply_conic(delta, value);
    }
    return value;
}

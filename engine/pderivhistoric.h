#ifndef PDERIVHISTORIC_H
#define PDERIVHISTORIC_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <QtXml>
#include "grammar/pword.h"
#include "grammar/ppreproc.h"
#include "pnumeric.h"
#include "pruleparametric.h"

class PDerivHistoric
{
private:
    int idx_into_gen;           // Index into the generator string
    int deriv_num;              // Derivation number where the alphabet was created
    int idx_into_deriv;         // Index in the derived string
    int rule_gen_num;           // Rule that generates the word
    int idx_within_rule;        // Index in the successor within the rule

    double stochastic_param; // Stochastic parameters
    PWord* word;
    PDerivHistoric* parent;
    std::vector<PDerivHistoric*> vec_children;
    std::vector<float> vec_params;
    std::vector<PNumeric*> vec_numeric;
    PPreproc* preproc;

    struct match_idx_dn
    {
        int deriv_num;
        int idx_into_deriv;
    };
    std::vector<match_idx_dn> vec_match_idx_dn;
    std::vector<match_idx_dn> vec_offset;

public:
    PDerivHistoric();
    PDerivHistoric( PWord* _word, PPreproc* _preproc, std::vector<float> _vec_params,
                    int _idx_into_gen, int _deriv_num,  int _idx_into_deriv, int _rule_gen_num, int _idx_within_rule,
                    double _stochastic_param);
    PDerivHistoric( PDerivHistoric* _parent, PWord* _word, PPreproc* _preproc, std::vector<PNumeric*> _vec_numeric, std::vector<float> _vec_params,
                    int _deriv_num, int _idx_into_deriv, int _rule_gen_num, int _idx_within_rule,
                    double _stochastic_param);
    PDerivHistoric( PDerivHistoric* _parent, PWord* _word, PPreproc* _preproc, std::vector<PNumeric*> _vec_numeric, std::vector<float> _vec_params,
                    int _deriv_num, int _idx_into_deriv, int _idx_into_gen, int _rule_gen_num, int _idx_within_rule,
                    double _stochastic_param);
    ~PDerivHistoric();
public:
    PWord* getWord() const {return word;}
    int getIdxIntoGen() const {return idx_into_gen;}
    int getIdxIntoDeriv() const {return idx_into_deriv;}
    int getDerivNum() const {return deriv_num;}
    int getRuleGenNum() const {return rule_gen_num;}
    int getIdxWithinRule() const {return idx_within_rule;}
    PPreproc* getPreproc() const {return preproc;}
    std::vector<PDerivHistoric*> getVecChildren() const {return vec_children;}
    PDerivHistoric* getParent() const {return parent;}
    std::vector<float> getVecParams() const {return vec_params;}
    std::vector<PNumeric*> getVecNumeric() const {return vec_numeric;}
    double getStochasticParam() const {return stochastic_param;}
    void setIdxIntoGen(int _idx_into_gen) {idx_into_gen = _idx_into_gen;}
    //void setWord(PWord* _word) {word = _word;}
    void setStochasticParam(double _stochastic_param) {stochastic_param = _stochastic_param;}
    void setIdxIntoDeriv(int _idx_into_deriv) {idx_into_deriv = _idx_into_deriv;}
    void setDerivNum(int _deriv_num) {deriv_num = _deriv_num;}
    void setRuleGenNum(int _rule_gen_num) {rule_gen_num = _rule_gen_num;}
    void setIdxWithinRule(int _idx_within_rule) {idx_within_rule = _idx_within_rule;}
    void setParent(PDerivHistoric* _parent) {parent = _parent;}
    void setVecNumeric(std::vector<PNumeric*> _vec_numeric) {vec_numeric = _vec_numeric;}
    void setVecParams(std::vector<float> _vec_params) {vec_params = _vec_params;}
    PDerivHistoric* getRoot(unsigned int _stages);
    std::vector<PDerivHistoric*> getRoots();
    std::vector<PDerivHistoric*> getRoots(unsigned int _stages);
    bool is_ramification(PDerivHistoric* _pdh);
    bool is_root(PDerivHistoric* _pdh);
    PDerivHistoric* clone(PDerivHistoric* _pdh);
public:
    inline void add_child(PDerivHistoric* _dh_child){vec_children.push_back(_dh_child);}
    inline void add_children(std::vector<PDerivHistoric*> _vec_children){vec_children = _vec_children;}
    inline void pop_match() {vec_match_idx_dn.pop_back();}
    void del_child(PDerivHistoric* _dh_child);
    void del_child();
    unsigned int children_count() const {return vec_children.size();}
    unsigned int dist_to_leaf(unsigned int _offset);    // return the number of PDerivHistoric to cross to get to the leaf
    void add_match_idx_dn(int _deriv_num, int _idx_into_deriv);
    void append_word_xml(QDomDocument& _doc, QDomElement& _root);
    inline unsigned int left_boundary(void){return (children_count()?vec_children.front()->left_boundary():getIdxIntoDeriv());}
    inline unsigned int right_boundary(void){return (children_count()?vec_children.back()->right_boundary():getIdxIntoDeriv());}
    std::vector<PDerivHistoric*> leaves();
    std::vector<PDerivHistoric*> leaves(const std::vector<PDerivHistoric*> _vecdh); // _vecdh is the list of PDerivHistoric to be verified. Returns which of the input vecs are in the list
public:
    bool is_tied(std::string& _var_name); // this function verifies dependance on variable
    void update_value(std::string& _const_name, float _new_value, std::vector<std::string>& _vec_var_name, PPreproc* _preproc);
    void update_value(std::string& _const_name, float _new_value, std::vector<std::string>& _vec_var_name);
    void update_leaves_const(std::string& _const_name, float _new_value, std::vector<PRuleParametric*>& _vec_rules); // CAUTION: this function updates only leaves const
    float request_const_value(std::string _name) const;
    inline void copy_const(PPreproc* _preproc){*preproc = *_preproc;}
};

#endif // PDERIVHISTORIC_H

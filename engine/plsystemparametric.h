#ifndef PLSYSTEMPARAMETRIC_H
#define PLSYSTEMPARAMETRIC_H

#include <string>
#include <vector>
#include <boost/bimap.hpp>
#include "unordered_map"

#include "grammar/pvariable.h"
#include "procedural.h"
#include "pruleparametric.h"
#include "plsystem.h"

class PLSystemParametric : public PLSystem
{
    std::vector<PVariable*>         vec_var;
    std::vector<PRuleParametric*>   vec_rules;
    std::vector< std::vector<PDerivHistoric*> > vec_vec_dh_v2;
    std::map<unsigned int, unsigned int> map_offset;
    std::map<unsigned int, std::vector<float> > map_params;
    // Debugging issues
    std::string                     dbg_previous_text;
    std::map<unsigned int, unsigned int>::iterator map_offset_iter;
    std::map<unsigned int, std::vector<float> >::iterator map_iter;

public:
    PLSystemParametric();
    PLSystemParametric(const PLSystemParametric& _lsys, bool _start_derive=false);
    ~PLSystemParametric();
    int     load(std::string _productions);
    void    init();
    void    init_dh();
    boost_bimap_dh                              getBmDh()               const {return bm_dh;}
    std::vector< std::vector<PDerivHistoric*> > getVecVecDhV2()         const {return vec_vec_dh_v2;}
    std::string                                 getDbgPreviousText()    const {return dbg_previous_text;}
    void                                        setDbgPreviousText(std::string _dbg_previous_text) {dbg_previous_text = _dbg_previous_text;}
    std::vector<PRuleParametric*>               getVecRules()           const {return vec_rules;}
    std::map<unsigned int, std::vector<float> > getMapParams()          const {return map_params;}
    unsigned int current_offset(unsigned int _index_to_confirm);
    std::vector<float> current_map_params();
    std::vector<float> current_map_params(unsigned int _index_to_confirm);
    void init_map_params(){map_iter = map_params.begin();}
    void init_map_offset(){map_offset_iter = map_offset.begin();}
    void update_map_offset();
    void insert_map(unsigned _index, unsigned int _offset, std::vector<float> _vec_params);
    void drop_map(unsigned _index);
    std::map<unsigned int, std::vector<float> > get_map_params() const {return map_params;}
    void dbg_bimap(const boost_bimap_dh _my_bimap);
    void dbg_map_params();
    void dbg_map_offset();
    void dbg_string();
    void to_std_stream(std::stringstream& _ss);
    void update_leaves();
public:
    void                def_const(std::vector<std::string>& _vec_params, std::vector<float>& _vec_values);          // request constant values
    void                update_global_const(std::string _const_name, float _new_value);                             // Change the global value of the constant with the name indicated in argument
    void                update_global_const(PPreproc* _preproc);
    void                update_local_const(         PDerivHistoric* _pdh,
                                                    std::string _const_name,
                                                    float _new_value);
    void                update_local_const(PPreproc* _preproc, PDerivHistoric* _pdh);// Change a local value of the constant, more precisely the constant in PDerivHistoric* _pdh
    void                update_child_const(         PDerivHistoric* _pdh,
                                                    std::string _const_name,
                                                    float _new_value,
                                                    unsigned int _stages);
    void                update_child_const(         PPreproc* _preproc,
                                                    PDerivHistoric *_pdh,
                                                    unsigned int _back_stages);
    void                reinit_preproc(PPreproc* _src);
    void                reinit_global_const(std::vector<std::string> _vec_const_name, std::vector<float> _vec_const_val);
public:
    std::vector<float>  pick_params(unsigned int _index);                                                           // _index point to the character(then not in the params) which the params is required,
    std::vector<float>  pick_params(std::string& _src);
    std::vector<float>  pick_params(std::string& _src, unsigned int _index);
    std::vector<float>  pick_params(std::string& _src, unsigned int _index, unsigned int& _offset_out);
    std::string         pick_char_at(unsigned int& _index, unsigned int& _char_size);
    std::string         pick_char_at(   unsigned int& _index,
                                        unsigned int& _char_size,
                                        std::vector<float>& _vec_params );
    std::string         apply_prod(     std::string& _src,
                                        unsigned int& _index,
                                        unsigned int& _rule_number,
                                        std::vector<float> _vec_params,
                                        boost::bimap<unsigned int, unsigned int>& _bm_succidx_prodidx);             // Apply a rule
    std::string         copie(PLSystemParametric* lsystem_c);
    void     createcopie(PDerivHistoric* _parent, std::vector<PDerivHistoric*> _pdh);
    std::string         derive();
    std::string         derive(unsigned int _index);
    std::string         derive(unsigned int _start, unsigned int _end);
    std::string         derive(unsigned int _start, unsigned int _end, unsigned int _n_times);
    std::string         derive(QList<int> _qlist_idx);
    inline PDerivHistoric* select_historic(unsigned int _index){try {return bm_dh.left.at(_index);} catch(...) {return 0;} return 0;}
    void                select(         std::vector<unsigned int>& _vec_start,
                                        std::vector<unsigned int>& _vec_end,
                                        unsigned int _index );
    std::string         str_from_historic();
    PDerivHistoric*     historic_root(std::vector<PDerivHistoric*> _vec_dh);                                        // Refers to Lowest Common Ancestor
    void                drop(PDerivHistoric*& _root_output, unsigned int _idx, unsigned int _depth=1);              // _root_output is an output
    void                drop(std::vector<PDerivHistoric*>& _vec_roots_out, QList<int> _list_idx, unsigned int _depth=1);
    std::vector<PDerivHistoric*> historic_leaves();                                                                 // This method returns the leaves in historic trees
    std::string         highlight_text( std::string& _src,
                                        std::vector<unsigned int> _vec_start,
                                        std::vector<unsigned int> _vec_end,
                                        unsigned int _index );
    void                shift_vector(   std::vector<unsigned int>& _vec_src,
                                        unsigned int _start_idx,
                                        unsigned int _offset);
    void                locate_str_diff(std::string& _src,
                                        std::string& _modified_str,
                                        unsigned int& _modif_start,
                                        unsigned int& _modif_end);                                                  // Modified string should be longer than the source string
    void                regenerate(     unsigned int _idx,
                                        unsigned int _back_stages,
                                        unsigned int _forward_stages,
                                        PDerivHistoric*& _dhroot_output);                                           // _dhroot_output is the root when we go back
    void                regenerate(     QList<int> _list_idx,
                                        unsigned int _back_stages,
                                        unsigned int _forward_stages);
    void                generate(       unsigned int _idx,
                                        unsigned int _forward_stages
                                        );
    void                generate(       QList<int> _list_idx,
                                        unsigned int _forward_stages
                                        );
    void                inspect_selection(  QList<int> _list_idx, unsigned int _back_stages);
    void                eval_selection(     const std::vector<PDerivHistoric*>& _inputvec_dhconfig,
                                            QList<int> _inputlist_selection,
                                            std::vector<int>& _outputvec_selection,
                                            std::vector<int>& _outputvec_unwantedslctn,
                                            std::vector<int>& _outputvec_ignoredslctn);
    unsigned int        count_alphabet(     const std::vector<PDerivHistoric*>& _vec_dhconfig);
    std::vector<unsigned int> sub_branches(); //
    std::vector<unsigned int> sub_branches(std::string _alphabet);
    std::vector<unsigned int> allvalue();
    std::vector<unsigned int> sub_branches(unsigned int _selection);
    std::vector<unsigned int> sub_branches(QList<int>_list_selection);
    std::vector<PDerivHistoric*> multiselect_historic(std::vector<unsigned int> _vec_selection);
    std::vector<PDerivHistoric*> multiselect_historic(QList<int> _list_selection);
    void update_sub_branches(unsigned int _selection, PPreproc* _preproc);
    void update_sub_branches(QList<int> _list_selection, PPreproc* _preproc);
    void quick_load(std::string& _text);
public:
    std::vector<PPreproc> generate_preproc(); // Generate multiple constant configurations
    void gen_preproc(unsigned int nb_param, std::vector<std::vector<float>> param_values, std::vector<PPreproc> *preprocs, const PPreproc _preproc); //generate preproc with different configuration for generate_preproc
    void rewrite_rule(unsigned int _rule_nb, PPreproc* _new_preproc);
    void clear_rules();
};
#endif // PLSYSTEMPARAMETRIC_H

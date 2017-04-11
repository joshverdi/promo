#ifndef PMEDIATOR_H
#define PMEDIATOR_H

#include <QStandardItemModel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include "engine/plsystemparametric.h"
#include "engine/pderivhistoric.h"
#include "engine/piorecord.h"
#include "grammar/ppreproc.h"
#include "viewport/pboundingbox.h"
#include "viewport/pshapeindexer.h"
#include "viewport/scene.h"
#include "engine/pmetric.h"

class Viewer;

/*
  The Mediator pattern comes up very lately in the project,
  Only few classes may benefit from the design
 */

class PMediator
{
    PLSystemParametric* lsystem;
    PDerivHistoric* dh;
    PPreproc* preproc;
    Scene* scene;
    //unsigned int selection; // The current selection, working with the old way of single selection, will be replaced with list_selection
    std::vector<unsigned int> vec_selection_reviewed; // contains multiple indices that we got from the current selection, added by the indices of the elements affected by regeneration
    std::vector<PMetric> vec_metric;
public:
    PMediator();
    ~PMediator();
    PMediator(PLSystemParametric* _lsystem);
    PMediator(Scene* _scene);
    PMediator(PLSystemParametric* _lsystem, Scene* _scene);
    PLSystemParametric* getLSystem() const {return lsystem;}
    void setLSystem(PLSystemParametric* _lsystem){lsystem = _lsystem;}
    PDerivHistoric* getDh() const {return dh;}
    void setDh(PDerivHistoric* _dh){dh=_dh;}
    PPreproc* getPreproc() const {return preproc;}
    void setPreproc(PPreproc* _preproc) {preproc = _preproc;}
    QList<int> check_selection() const {QList<int> value; if (scene) value=scene->getListSelection(); return value;}
    //void setSelection(unsigned int _selection) {selection = _selection;}
    void setScene(Scene* _scene){scene = _scene;}
    Scene* getScene() const {return scene;}

    std::vector<unsigned int> getVecSelectionReviewed() const {return vec_selection_reviewed;}
    void setVecSelectionReviewed(std::vector<unsigned int> _vec_selection_reviewed){vec_selection_reviewed=_vec_selection_reviewed;}
    inline void copy_preproc(PPreproc& _preproc){*preproc = _preproc;}
    inline void clear_selection_reviewed(){vec_selection_reviewed.clear();}
    inline void add_selection(unsigned int _name){vec_selection_reviewed.push_back(_name);}
    inline void add_selection(QList<int> _list_selection)
    {   for (size_t i=0; i<_list_selection.size(); i++)
            if (std::find(vec_selection_reviewed.begin(), vec_selection_reviewed.end(), _list_selection.at(i))==vec_selection_reviewed.end())
                vec_selection_reviewed.push_back(_list_selection.at(i));
    }
    void set_img_res(Viewer* _v, int _resolut_type);
public:
    void setup_cst_model(PPreproc* _preproc, QStandardItemModel* _qsim);
    PPreproc setup_preproc(QStandardItemModel* _sim);
    void setup_combobox(QComboBox* _cb, QDoubleSpinBox* _dsb, PPreproc* _preproc);
public:
    std::vector<PShapeIndexer*> select_indexer(Scene* _scene, std::vector<unsigned int> _vec_name);
    void add_obb(Scene* _scene, PBoundingBox& _obb, std::vector<CGLA::Vec3f>& _vec_vertices, bool is_reset=false);
public:
    QStandardItemModel* setup_table_model(PMetric* _metric);
    PMetric compute_metric(     PLSystemParametric* _lsys,
                                Scene* _scene,
                                PBoundingBox& _obb);
    void select_preproc(std::vector<PPreproc>& _src_in, unsigned int _in_rule_nb, std::vector<unsigned int>& _vec_idx_in, std::vector<PPreproc>& _vec_pp_out, std::vector<unsigned int>& _vec_rule_idx,bool is_ar=false);
    template <class T> void remove_duplicates(std::vector<T>& _src)
    {
        std::sort(_src.begin(), _src.end());
        _src.erase(std::unique(_src.begin(), _src.end()), _src.end());
    }
};

#endif // PMEDIATOR_H

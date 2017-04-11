#include "pmediator.h"
#include "viewport/viewer.h"

#include <vector>

using namespace std;

PMediator::PMediator()
{
    preproc = new PPreproc();
    scene = 0;
}

PMediator::PMediator(PLSystemParametric* _lsystem)
{
    preproc = new PPreproc();
    lsystem = _lsystem;
    scene = 0;
}

PMediator::PMediator(Scene* _scene)
{
    preproc = new PPreproc();
    lsystem = 0;
    scene = _scene;
}

PMediator::PMediator(PLSystemParametric* _lsystem, Scene* _scene)
{
    preproc = new PPreproc();
    lsystem = _lsystem;
    scene = _scene;
}

PMediator::~PMediator()
{
    delete preproc;
}

void PMediator::setup_cst_model(PPreproc* _preproc, QStandardItemModel* _qsim)
{
    std::list<QString> lst_name, lst_val;
    QStandardItem *item_name, *item_val;
    QString qs_tmp;
    int row_idx;

    _qsim->clear();
    if (_preproc)
    {
        for (unsigned int i=0; i<_preproc->getVecDefine().size(); i++)
        {
            lst_name.push_back(_preproc->getVecDefine()[i]->getConstantName().c_str());
            lst_val.push_back(qs_tmp.setNum(_preproc->getVecDefine()[i]->getConstantValue()));
        }
        row_idx=0;
        while((!lst_name.empty())&&(!lst_val.empty()))
        {
            item_name   = new QStandardItem(lst_name.front());
            item_val    = new QStandardItem(lst_val.front());
            _qsim->setItem(row_idx, 0, item_name);
            _qsim->setItem(row_idx, 1, item_val);
            lst_name.pop_front();
            lst_val.pop_front();
            row_idx++;
        }
        _qsim->setHeaderData(0, Qt::Horizontal, QObject::tr("Constant"));
        _qsim->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
    }

}

void PMediator::setup_combobox(QComboBox* _cb, QDoubleSpinBox* _dsb, PPreproc* _preproc)
{
    if (_cb&&_dsb&&_preproc==0) return;
    _cb->clear();
    for (unsigned int i=0; i<_preproc->getVecDefine().size(); i++)
    {
        _cb->addItem(QString(QObject::tr(_preproc->getVecDefine()[i]->getConstantName().c_str())));
    }
    if (!_preproc->getVecDefine().empty())
    {
        _dsb->setValue((double)_preproc->getVecDefine().front()->getConstantValue());
    }
}

PPreproc PMediator::setup_preproc(QStandardItemModel* _sim)
{
    PPreproc value;
    string name_tmp;
    float val_tmp;

    int row_count = _sim->rowCount();
    for (int i=0; i<row_count; i++)
    {
        name_tmp = _sim->item(i, 0)->data(Qt::DisplayRole).toString().toStdString();
        val_tmp  = _sim->item(i, 1)->data(Qt::DisplayRole).toFloat();
        value.push_define(new PDefine(name_tmp, val_tmp));
    }
    return value;
}

std::vector<PShapeIndexer*> PMediator::select_indexer(Scene* _scene, std::vector<unsigned int> _vec_name)
{
    std::vector<PShapeIndexer*> value;
    value.clear();
    PShapeIndexer* tmp;
    for (unsigned int i=0; i<_scene->getVecShapeIndexer().size(); i++)
    {
        tmp = _scene->getVecShapeIndexer().at(i);
        if (std::find(_vec_name.begin(), _vec_name.end(), tmp->getName()) != _vec_name.end())
            value.push_back(tmp);
    }
    return value;
}

void PMediator::add_obb(Scene* _scene, PBoundingBox& _obb, vector<CGLA::Vec3f>& _vec_vertices, bool is_reset)
{
    if (is_reset) _scene->clear_obb();
    _obb.compute_corner(_vec_vertices);
    _scene->add_obb(_obb, is_reset);
}

QStandardItemModel* PMediator::setup_table_model(PMetric* _metric)
{
    QStandardItemModel* value;
    QStandardItem* item_descript, *item_val;
    std::list<QString> lst_descript;
    std::list<QString> lst_val;
    int row_idx=0;

    if (!_metric) return 0;
    value = new QStandardItemModel();

    QString qs_obbvol("OBB Volume");
    QString qs_obbsurf("OBB Surface");
    QString qs_brchs_surf("Branches Surface");
    QString qs_brchs_vol("Branches Volume");
    QString qs_brchs_nb("Branches Number");
    QString qs_brch_density("Branches per Vol Unit");
    QString qs_slctn_rate("Selection Rate");
    QString qs_fill_rate("Box Filling Rate");

    lst_descript.push_back(qs_obbvol);
    lst_descript.push_back(qs_obbsurf);
    lst_descript.push_back(qs_brchs_surf);
    lst_descript.push_back(qs_brchs_vol);
    lst_descript.push_back(qs_brchs_nb);
    lst_descript.push_back(qs_brch_density);
    lst_descript.push_back(qs_slctn_rate);
    lst_descript.push_back(qs_fill_rate);

    qs_obbvol.setNum(_metric->getObbVol());
    qs_obbsurf.setNum(_metric->getObbSurf());
    qs_brchs_surf.setNum(_metric->getBrchsSurf());
    qs_brchs_vol.setNum(_metric->getBrchsVol());
    qs_brchs_nb.setNum(_metric->getBrchsNb());
    qs_brch_density.setNum(_metric->getBrchDensity());
    qs_slctn_rate.setNum(_metric->getSlctnRate());
    qs_fill_rate.setNum(_metric->getFillRate());

    lst_val.push_back(qs_obbvol);
    lst_val.push_back(qs_obbsurf);
    lst_val.push_back(qs_brchs_surf);
    lst_val.push_back(qs_brchs_vol);
    lst_val.push_back(qs_brchs_nb);
    lst_val.push_back(qs_brch_density);
    lst_val.push_back(qs_slctn_rate);
    lst_val.push_back(qs_fill_rate);

    while((!lst_descript.empty())&&(!lst_val.empty()))
    {
        item_descript   = new QStandardItem(lst_descript.front());
        item_val        = new QStandardItem(lst_val.front());
        value->setItem(row_idx, 0, item_descript);
        value->setItem(row_idx, 1, item_val);
        lst_descript.pop_front();
        lst_val.pop_front();
        row_idx++;
    }

    value->setHeaderData(0, Qt::Horizontal, QObject::tr("Description"));
    value->setHeaderData(1, Qt::Horizontal, QObject::tr("Value"));
    return value;
}

PMetric PMediator::compute_metric( PLSystemParametric* _lsys,
                                   Scene* _scene,
                                   PBoundingBox& _obb)
{
    PMetric value;
    std::vector<unsigned int> vec_all_slctn;
    std::vector<PShapeIndexer*> vec_shape_idxr_slctd, vec_all_idxr;
    vec_all_slctn = _lsys->sub_branches();
    vec_shape_idxr_slctd = select_indexer(_scene, vec_selection_reviewed);
    vec_all_idxr         = select_indexer(_scene, vec_all_slctn);
    value.obb_volume(_obb);
    value.obb_surface(_obb);
    value.branches_number(vec_selection_reviewed);
    value.branches_volume(vec_shape_idxr_slctd);
    value.branches_surface(vec_shape_idxr_slctd);
    value.branch_per_vol(vec_selection_reviewed, _obb);
    value.selection_ratio(vec_shape_idxr_slctd, vec_all_idxr);
    value.filling_ratio(vec_shape_idxr_slctd, _obb);
    value.branches_length(vec_shape_idxr_slctd);
    value.average_length(vec_shape_idxr_slctd);
    value.averay(vec_shape_idxr_slctd);
    return value;
}

void PMediator::select_preproc(std::vector<PPreproc>& _src_in, unsigned int _in_rule_nb, std::vector<unsigned int>& _vec_idx_in, std::vector<PPreproc>& _vec_pp_out, std::vector<unsigned int>& _vec_ruleidx_out,bool is_ar)
{
    unsigned int ruleidx_tmp, ppidx_tmp;
    _vec_pp_out.clear();
    _vec_ruleidx_out.clear();
    if (_in_rule_nb==0)
        return;
    for (std::vector<unsigned int>::iterator iter=_vec_idx_in.begin(); iter!=_vec_idx_in.end(); iter++)
    {
        if(is_ar)
        {
            ruleidx_tmp=1001;
            ppidx_tmp = (unsigned int)(*iter);
        }
        else
        {
            ruleidx_tmp = (*iter)%_in_rule_nb;
            ppidx_tmp = (unsigned int)((*iter)/_in_rule_nb);
        }
        _vec_pp_out.push_back(_src_in.at(ppidx_tmp));
        _vec_ruleidx_out.push_back(ruleidx_tmp);
    }
}

void PMediator::set_img_res(Viewer* _v, int _resolut_type)
{
    switch(_resolut_type)
    {
    case RES_600x400:
        _v->resize(600, 400);
        break;
    case RES_1024x768:
        _v->resize(1024, 768);
        break;
    case RES_1200x780:
        _v->resize(1200, 780);
        break;
    }
}

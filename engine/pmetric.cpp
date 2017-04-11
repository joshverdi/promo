#include "pmetric.h"
#include "engine/procedural.h"
#include "viewport/pcylinder.h"

PMetric::PMetric()
{
}

double PMetric::obb_volume(PBoundingBox& _obb)
{
    double value, length, width, height;
    length = sqrt(  pow(_obb.getVecCorner()[0][0]-_obb.getVecCorner()[1][0],2)
                   +pow(_obb.getVecCorner()[0][1]-_obb.getVecCorner()[1][1],2)
                   +pow(_obb.getVecCorner()[0][2]-_obb.getVecCorner()[1][2],2)
                    );
    width  = sqrt(  pow(_obb.getVecCorner()[1][0]-_obb.getVecCorner()[2][0],2)
                   +pow(_obb.getVecCorner()[1][1]-_obb.getVecCorner()[2][1],2)
                   +pow(_obb.getVecCorner()[1][2]-_obb.getVecCorner()[2][2],2)
                    );
    height = sqrt(  pow(_obb.getVecCorner()[4][0]-_obb.getVecCorner()[0][0],2)
                   +pow(_obb.getVecCorner()[4][1]-_obb.getVecCorner()[0][1],2)
                   +pow(_obb.getVecCorner()[4][2]-_obb.getVecCorner()[0][2],2)
                    );
    value = obb_vol = length*width*height;
    return value;
}

double PMetric::obb_surface(PBoundingBox& _obb)
{
    double value, length, width, height;
    length = sqrt(  pow(_obb.getVecCorner()[0][0]-_obb.getVecCorner()[1][0],2)
                   +pow(_obb.getVecCorner()[0][1]-_obb.getVecCorner()[1][1],2)
                   +pow(_obb.getVecCorner()[0][2]-_obb.getVecCorner()[1][2],2)
                    );
    width  = sqrt(  pow(_obb.getVecCorner()[1][0]-_obb.getVecCorner()[2][0],2)
                   +pow(_obb.getVecCorner()[1][1]-_obb.getVecCorner()[2][1],2)
                   +pow(_obb.getVecCorner()[1][2]-_obb.getVecCorner()[2][2],2)
                    );
    height = sqrt(  pow(_obb.getVecCorner()[4][0]-_obb.getVecCorner()[0][0],2)
                   +pow(_obb.getVecCorner()[4][1]-_obb.getVecCorner()[0][1],2)
                   +pow(_obb.getVecCorner()[4][2]-_obb.getVecCorner()[0][2],2)
                    );
    value = obb_surf = ((length*width)+(length*height)+(width*height))*2;
    return value;
}

double PMetric::branches_surface(PShapeIndexer* _shape_indexer)
{
    double value;
    PCylinder* cyl=NULL;
    PShape* shape = _shape_indexer->getShape();
    try
    {
        cyl = dynamic_cast<PCylinder*>(shape);
    }
    catch (...)
    {
        return 0;
    }

    if (cyl)
    {
        brchs_surf=value=2*M_PI*((cyl->getBase()+cyl->getTop())/2)*cyl->getHeight();
        return value;
    }
    else
    {
        brchs_surf = 0;
        return 0;
    }
}

double PMetric::branches_surface(std::vector<PShapeIndexer*> _vec_shape_indexer)
{
    double value=0;
    for (std::vector<PShapeIndexer*>::iterator iter=_vec_shape_indexer.begin(); iter!=_vec_shape_indexer.end(); iter++)
        value+=branches_surface(*iter);
    brchs_surf = value;
    return value;
}

double PMetric::branches_volume(PShapeIndexer* _shape_indexer)
{
    double value=0;
    PCylinder* cyl;
    PShape* shape = _shape_indexer->getShape();
    cyl = dynamic_cast<PCylinder*>(shape);
    if (cyl)
    {
        brchs_vol = value = M_PI*pow((double)((cyl->getBase()+cyl->getTop())/2), 2)*cyl->getHeight();
        return value;
    }
    else
    {
        brchs_vol = 0;
        return 0;
    }
}

double PMetric::branches_volume(std::vector<PShapeIndexer*> _vec_shape_indexer)
{
    double value=0;
    for (std::vector<PShapeIndexer*>::iterator iter=_vec_shape_indexer.begin(); iter!=_vec_shape_indexer.end(); iter++)
        value+=branches_volume(*iter);
    brchs_vol = value;
    return value;
}

double PMetric::branches_length(PShapeIndexer* _shape_indexer)
{
    double value=0;
    PCylinder* cyl;
    PShape* shape = _shape_indexer->getShape();
    cyl = dynamic_cast<PCylinder*>(shape);
    if (cyl)
    {
        value = cyl->getHeight();
        return value;
    }
    brchs_length = 0;
    return 0;
}

double PMetric::branches_length(std::vector<PShapeIndexer*> _vec_shape_indexer)
{
    double value=0;
    for (std::vector<PShapeIndexer*>::iterator iter=_vec_shape_indexer.begin(); iter!=_vec_shape_indexer.end(); iter++)
        value+=branches_length(*iter);
    brchs_length = value;
    return value;
}

double PMetric::average_length(std::vector<PShapeIndexer*> _vec_shape_indexer)
{
    avrg_length = ((branches_length(_vec_shape_indexer))/(_vec_shape_indexer.size()));
    return avrg_length;
}

double PMetric::averay(PShapeIndexer* _shape_indexer)
{
    PShape* shape = _shape_indexer->getShape();
    PCylinder* cyl = dynamic_cast<PCylinder*>(shape);
    if (cyl)
    {
        brchs_averay = (cyl->getBase()+cyl->getTop())/2;
        return brchs_averay;
    }
    brchs_averay = 0;
    return 0;
}

double PMetric::averay(std::vector<PShapeIndexer*> _vec_shape_indexer)
{
    double value=0;
    for (std::vector<PShapeIndexer*>::iterator iter=_vec_shape_indexer.begin(); iter!=_vec_shape_indexer.end(); iter++)
        value+=averay(*iter);
    value/=_vec_shape_indexer.size();
    brchs_averay = value;
    return value;
}

double PMetric::branch_per_vol(std::vector<unsigned int>& _vec_name, PBoundingBox& _obb)
{
    double value = ((double)_vec_name.size())/obb_volume(_obb);
    brch_density = value;
    return value;
}

double PMetric::selection_ratio(std::vector<PShapeIndexer*> _vec_slctd_shp, std::vector<PShapeIndexer*> _vec_full_shp)
{
    double value = ((double)_vec_slctd_shp.size())/((double)_vec_full_shp.size());
    slctn_rate = value;
    return value;
}

double PMetric::filling_ratio(std::vector<PShapeIndexer*> _vec_shape_indexer, PBoundingBox& _obb)
{
    double value = branches_volume(_vec_shape_indexer)/obb_volume(_obb);
    fill_rate = value;
    return value;
}

std::vector<float> PMetric::compute_diff(PMetric& _metric)
{
    std::vector<float> value;
    value.push_back(Procedural::compute_diff(obb_vol,       _metric.getObbVol()));
    value.push_back(Procedural::compute_diff(obb_surf,      _metric.getObbSurf()));
    value.push_back(Procedural::compute_diff(brchs_surf,    _metric.getBrchsSurf()));
    value.push_back(Procedural::compute_diff(brchs_vol,     _metric.getBrchsVol()));
    value.push_back(Procedural::compute_diff((float)brchs_nb,(float)_metric.getBrchsNb()));
    value.push_back(Procedural::compute_diff(brch_density,  _metric.getBrchDensity()));
    value.push_back(Procedural::compute_diff(slctn_rate,    _metric.getSlctnRate()));
    value.push_back(Procedural::compute_diff(fill_rate,     _metric.getFillRate()));
    value.push_back(Procedural::compute_diff(brchs_length,  _metric.getBrchsLength()));
    value.push_back(Procedural::compute_diff(avrg_length,   _metric.getAvrgLength()));
    value.push_back(Procedural::compute_diff(brchs_averay,  _metric.getBrchsAveray()));
    return value;
}



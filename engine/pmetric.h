#ifndef PMETRIC_H
#define PMETRIC_H

#include <vector>
#include "viewport/pboundingbox.h"
#include "viewport/pshapeindexer.h"

class PMetric
{
    double obb_vol;         // OBB Volume
    double obb_surf;        // OBB Surface
    double brchs_surf;      // (All) Branches Surface
    double brchs_vol;       // (All) Branches Volume
    double brch_density;    // (All) Branches Number per OBB Volume
    double slctn_rate;      // The number of the selected branches divided by the number of all branches
    double fill_rate;       // The volume of the branches divided by the volume of the OBB
    double brchs_length;    // Branch(es) Length
    double avrg_length;     // Branches Average Length
    double brchs_averay;       // Branch(es) Average Ray
    unsigned int brchs_nb;  // Branches Number
public:
    PMetric();
    double getObbVol() const {return obb_vol;}
    double getObbSurf() const {return obb_surf;}
    double getBrchsSurf() const {return brchs_surf;}
    double getBrchsVol() const {return brchs_vol;}
    double getBrchDensity() const {return brch_density;}
    double getSlctnRate() const {return slctn_rate;}
    double getFillRate() const {return fill_rate;}
    double getBrchsLength() const {return brchs_length;}
    double getAvrgLength() const {return avrg_length;}
    double getBrchsAveray() const {return brchs_averay;}
    unsigned int getBrchsNb() const {return brchs_nb;}

public:
    double obb_volume(PBoundingBox& _obb);
    double obb_surface(PBoundingBox& _obb);
    //inline unsigned int count_branches(std::vector<unsigned int>& _vec_name){nb_brchs=_vec_name.size(); return _vec_name.size();}
    double branches_surface(PShapeIndexer* _shape_indexer);
    double branches_surface(std::vector<PShapeIndexer*> _vec_shape_indexer);
    double branches_volume(PShapeIndexer* _shape_indexer);
    double branches_volume(std::vector<PShapeIndexer*> _vec_shape_indexer);
    double branches_length(PShapeIndexer* _shape_indexer);
    double branches_length(std::vector<PShapeIndexer*> _vec_shape_indexer);
    double average_length(std::vector<PShapeIndexer*> _vec_shape_indexer);
    double averay(PShapeIndexer* _shape_indexer);
    double averay(std::vector<PShapeIndexer*> _vec_shape_indexer);
    unsigned int branches_number(std::vector<unsigned int>& _vec_name){return brchs_nb=_vec_name.size();}
    double branch_per_vol(std::vector<unsigned int>& _vec_name, PBoundingBox& _obb);
    double selection_ratio(std::vector<PShapeIndexer*> _vec_slctd_shp, std::vector<PShapeIndexer*> _vec_full_shp);
    double filling_ratio(std::vector<PShapeIndexer*> _vec_shape_indexer, PBoundingBox& _obb);
    std::vector<float> compute_diff(PMetric& _metric); // Percentage of changes
};

#endif // PMETRIC_H

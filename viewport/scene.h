#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>

#include "CGLA/Vec3f.h"
#include "CGLA/Mat3x3f.h"
#include "engine/plsystem.h"
#include "engine/plsystemparametric.h"
#include "viewport/pboundingbox.h"
#include "viewport/pshapeindexer.h"
#include "viewport/pcglaprocessor.h"
#include "viewport/pmodelingconfig.h"

class Scene
{
    QList<int> list_selection;
    //int selection;
    int geometry_type;
    bool selection_changed;
    bool enabled_impact;
    char* raw_data;
    float matrix_modelview[16];
    float matrix_model[16];
    float matrix_view[16];
    CGLA::Vec3f init_left, init_head, init_up;
    CGLA::Vec3f turtle_left, turtle_head, turtle_up;
    PLSystemParametric* lsystem;
    PModelingConfig* mod_config;
    PCglaProcessor cgla_proc;
    //GLUquadric* qobj;
    std::vector<GLuint> vec_display_list;
    std::vector<PShapeIndexer*> vec_shape_indexer;
    std::vector<PBoundingBox> vec_bounding_box;
    // Debug:
    std::vector<CGLA::Vec3f> vec_vertices;
    std::vector<CGLA::Vec3f> vec_specific_vert;
    std::vector<CGLA::Mat4x4f> vec_basis3d;
    // Debug functs:
public:
    void add_vertices(CGLA::Vec3f& _vertex){vec_vertices.push_back(_vertex);}
    void add_vertices(std::vector<CGLA::Vec3f>& _vec_vertices){vec_vertices.insert(vec_vertices.end(), _vec_vertices.begin(), _vec_vertices.end());}
    void add_vertices(std::vector< std::vector<float> >& _vec_vertices);
    void draw_vertices();
    void add_specific_vert(CGLA::Vec3f& _vertex){vec_specific_vert.push_back(_vertex);}
    void add_specific_vert(std::vector<CGLA::Vec3f>& _vec_specific_vert){vec_specific_vert.insert(vec_specific_vert.end(), _vec_specific_vert.begin(), _vec_specific_vert.end());}
    void draw_specific_vert();
    void add_basis(CGLA::Mat4x4f& _basis){vec_basis3d.push_back(_basis);}
    void add_basis(CGLA::Vec3f _origin, CGLA::Mat3x3f _basis);
public:
    void setSelection(QList<int>  _selection){list_selection = _selection;}
    void setSelection(std::vector<unsigned int> _vec_list_selection);
    void addIdToSelection(int _id);
    void removeIdFromSelection(int _id);
    bool is_selected(int _id){return list_selection.indexOf(_id)!=-1?true:false;}
    //int getSelection(void) const {return selection;}
    QList<int> getListSelection() const {return list_selection;}
    inline void clear_list_selection() {list_selection.clear();}
    //void setSelection(int _selection){selection = _selection;}
    bool getSelectionChanged(void) const {return selection_changed;}
    void setSelectionChanged(bool _selection_changed){selection_changed = _selection_changed;}
    bool getEnabledImpact(void) const {return enabled_impact;}
    void setEnabledImpact(bool _enabled_impact){enabled_impact = _enabled_impact;}
    std::vector<PShapeIndexer*> getVecShapeIndexer() const {return vec_shape_indexer;}
    void setVecShapeIndexer(std::vector<PShapeIndexer*> _vec_shape_indexer){vec_shape_indexer = _vec_shape_indexer;}
    void setLSystem(PLSystemParametric* _lsys){lsystem = _lsys;}
    void setGeometryType(int _geometry_type){geometry_type = _geometry_type;}
    PLSystemParametric* getLSystem(void) const {return lsystem;}
    CGLA::Vec3f getInitLeft() const{return init_left;}
    CGLA::Vec3f getInitHead() const{return init_head;}
    CGLA::Vec3f getInitUp() const{return init_up;}
    void setModConfig(PModelingConfig* _mod_config){mod_config = _mod_config;}
    void clear_shape_idxr();
public:
    inline void clear_obb(){vec_bounding_box.clear();}
    inline void add_obb(PBoundingBox _obb, bool _clear_previous=false){if(_clear_previous) vec_bounding_box.clear();vec_bounding_box.push_back(_obb);}
public:
    Scene();
    Scene(PLSystemParametric* _lsystem);
    ~Scene();
    void init();
    void draw(bool _names);
    void draw_obb();
    bool state_changed(const char* _c_str);
    void check_angle(GLfloat& _angle, unsigned int _index, unsigned int& _str_size);
    void check_angle(GLfloat& _angle, unsigned int _index, unsigned int& _str_size, char _symbol, CGLA::Vec3f& _axis);
    void check_angle(GLfloat& _angle_out, CGLA::Vec3f& _axis_out, unsigned int& _offset_out, char _c_in, std::unordered_map<unsigned int, std::vector<float> >& _map_params_in, unsigned _index_in);
    void check_axis(char _symbol, CGLA::Vec3f& _axis);
    void check_params(std::vector<GLfloat>& _vec_params, unsigned int _index, unsigned int& _str_size);
    void check_params(std::vector<GLfloat>& _vec_params_out, unsigned int& _offset_out, unsigned int _index_in, std::unordered_map<unsigned int, std::vector<float> >& _map_params_in);
    std::string glmatrix_to_str(float* _matrix);
    std::string modelview_to_str();
    std::string model_to_str();
    std::string view_to_str();
    std::string left_to_str();
    std::string head_to_str();
    std::string up_to_str();
    std::string vec3f_to_str(CGLA::Vec3f& _cglavec);
    void check_turtle_from(GLfloat* _view, GLfloat* _modelview);
public:
    // Matrix Operation:
    void compute_basis(float* _matrix, CGLA::Vec3f& _origin, CGLA::Vec3f& _left, CGLA::Vec3f& _head, CGLA::Vec3f& _up);
    void draw_pt3d(std::vector< std::vector<float> >& _vecvec3);
    void draw_pt3d(std::vector<float> _vec3);
    void draw_pt3d(CGLA::Vec3f _pt3d); // this function is supposed to be called after the model has been drawn, but before the view matrix applied
    void draw_pt3d(CGLA::Vec3f _pt3d, GLfloat _red, GLfloat _green, GLfloat _blue);
    void draw_line(CGLA::Vec3f _pt1, CGLA::Vec3f _pt2);
    void draw_box(PBoundingBox& _obb);
    //void draw_ogl(bool _names);
    void draw_cgla(bool _names);
    void draw_vec(CGLA::Vec3f _origin, CGLA::Vec3f _orientation, GLfloat _red, GLfloat _green, GLfloat _blue);
    void draw_basis(CGLA::Vec3f _origin, CGLA::Mat3x3f _units);
    void draw_basis(CGLA::Mat4x4f& _basis);
    void draw_basis();
    void rot_to_vert();
    PCglaProcessor compute_cgla(unsigned int _idx);
};

#endif // SCENE_H

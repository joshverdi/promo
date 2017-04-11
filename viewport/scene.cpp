#include "CGLA/Vec3f.h"
#include "CGLA/Quaternion.h"
#include "CGLA/Mat4x4f.h"
#include "engine/procedural.h"
#include "engine/plsystem.h"
#include "engine/ptextparser.h"
#include "viewport/scene.h"
#include "viewport/pshape.h"
#include "viewport/pshapeindexer.h"
#include "viewport/pshapeinitializer.h"
#include "viewport/pline.h"
#include "viewport/pcylinder.h"
#include "viewport/psimplisticleaf.h"
#include "assert.h"

#include "gettimeofday.h"

using namespace std;
using namespace CGLA;
using namespace Procedural;

Scene::Scene()
{
    lsystem    = 0;
    raw_data   = 0;
    mod_config = 0;
    init();
}

Scene::Scene(PLSystemParametric* _lsystem)
{
    raw_data = 0;
    mod_config = 0;
    lsystem = _lsystem;
    init();
}

Scene::~Scene()
{
    delete [] raw_data;
    clear_shape_idxr();
}

void Scene::init()
{
    mod_config = 0;
    glLineWidth(5.0f);
    //selection = -1;
    selection_changed=false;enabled_impact=true;
    for (int i=0; i<16; i++)
    {
        if (i%5==0) matrix_view[i] = matrix_model[i] = matrix_modelview[i] = 1;
        else matrix_view[i] = matrix_model[i] = matrix_modelview[i] = 0;
    }

    turtle_left = Vec3f(-1.0, 0.0, 0.0);
    turtle_head = Vec3f(0.0,  1.0, 0.0);
    turtle_up   = Vec3f(0.0,  0.0, 1.0);

    init_left = CGLA::Vec3f(-1.0, 0.0, 0.0);
    init_head = CGLA::Vec3f(0.0,  1.0, 0.0);
    init_up   = CGLA::Vec3f(0.0,  0.0, 1.0);

    geometry_type = CONIC_SHAPE;
}

void Scene::clear_shape_idxr()
{
    for (vector<PShapeIndexer*>::iterator iter=vec_shape_indexer.begin(); iter!=vec_shape_indexer.end(); iter++)
    {
        delete (*iter);
    }
    vec_shape_indexer.clear();
}

void Scene::draw(bool _names)
{
    if (lsystem)
    {
        if (state_changed(lsystem->getText().c_str())||(getSelectionChanged()))
        {
            vec_display_list.clear();
            GLuint dl = glGenLists(1);
            glNewList(dl, GL_COMPILE_AND_EXECUTE);
            //draw_ogl(_names);
            draw_cgla(_names);
            glEndList();
            vec_display_list.push_back(dl);
        }
        else
        {
            for (vector<GLuint>::iterator iter_display_list=vec_display_list.begin(); iter_display_list!=vec_display_list.end(); iter_display_list++)
            {
                if (glIsList(*iter_display_list))
                    glCallList(*iter_display_list);
            }
        }
        draw_obb();
        // For debug:
        draw_vertices();
        draw_specific_vert();
        draw_basis();
    }
    draw_vertices();
}

bool Scene::state_changed(const char* _c_str)
{
    bool value = true;
    if (!_c_str) return false;
    if (raw_data)
        value=strcmp(raw_data, _c_str)?true:false;
    if (value==false) return value;
    delete [] raw_data;
    raw_data = new char[lsystem->getText().size()+1];
    strcpy(raw_data, lsystem->getText().c_str());
    return value;
}

void Scene::check_angle(GLfloat& _angle, unsigned int _index, unsigned int& _str_size)
{
    PTextParser textparser;
    unsigned int idx_closer;
    vector<GLfloat> vec_params = lsystem->pick_params(_index);
    if (!vec_params.empty())
    {
        idx_closer = textparser.find_bracket_closer(lsystem->getText(), _index+1);
        _str_size  = idx_closer - _index + 1;
        _angle     = vec_params.back();
    }
}

void Scene::check_angle(GLfloat& _angle, unsigned int _index, unsigned int& _str_size, char _symbol, CGLA::Vec3f& axis)
{
    PTextParser textparser;
    //unsigned int idx_closer;
    //lsystem->dbg_map_params();
    //vector<GLfloat> vec_params = lsystem->pick_params(_index);
    _str_size = 1;
    vector<GLfloat> vec_params = lsystem->current_map_params(_index);
    // The two next lines may be the source of malfunction
    _str_size  = lsystem->current_offset(_index);
    _angle=fabs(_angle);
    if (!vec_params.empty())
        _angle=vec_params.back();

    switch(_symbol)
    {
    case '+':
        axis = init_up;
        break;
    case '-':
        _angle*=(-1);
        axis = init_up;
        break;
    case '&':
        axis = init_left;
        break;
    case '^':
        _angle*=(-1);
        axis = init_left;
        break;
    case '\\':
        axis = init_head;
        break;
    case '/':
        _angle*=(-1);
        axis = init_head;
        break;
    case '|':
        _angle=180;
        axis = init_up;
        break;
    }
}

void Scene::check_angle(GLfloat& _angle_out, CGLA::Vec3f& _axis_out, unsigned int& _offset_out, char _c_in, std::unordered_map<unsigned int, std::vector<float> >& _map_params_in, unsigned _index_in)
{
    PTextParser textparser;
    _angle_out = 0;

    unsigned int idx_closer = textparser.find_bracket_closer(lsystem->getText(), _index_in+1);
    _offset_out = idx_closer - _index_in + 1;
    _angle_out = _map_params_in[_index_in].back();
    switch(_c_in)
    {
    case '+':
        _axis_out = init_up;
        break;
    case '-':
        _angle_out*=(-1);
        _axis_out = init_up;
        break;
    case '&':
        _axis_out = init_left;
        break;
    case '^':
        _angle_out*=(-1);
        _axis_out = init_left;
        break;
    case '\\':
        _axis_out = init_head;
        break;
    case '/':
        _angle_out*=(-1);
        _axis_out = init_head;
        break;
    case '|':
        _angle_out=180;
        _axis_out = init_up;
        break;
    }
}

void Scene::check_params(vector<GLfloat>& _vec_params, unsigned int _index, unsigned int& _str_size)
{
    PTextParser textparser;
    unsigned int idx_closer;
    //lsystem->dbg_map_params();
//    _vec_params = lsystem->pick_params(_index);
    //_vec_params = lsystem->current_map_params(_index);
    if (!_vec_params.empty())
    {
        idx_closer = textparser.find_bracket_closer(lsystem->getText(), _index+1);
        _str_size  = idx_closer - _index + 1;
    }
}

void Scene::check_params(std::vector<GLfloat>& _vec_params_out, unsigned int& _offset_out, unsigned int _index_in, std::unordered_map<unsigned int, std::vector<float> >& _map_params_in)
{
    PTextParser textparser;

    unsigned int idx_closer = textparser.find_bracket_closer(lsystem->getText(), _index_in+1);
    try
    {
        _vec_params_out = _map_params_in[_index_in];
        if (!_vec_params_out.empty())
            _offset_out = idx_closer - _index_in + 1;
    }
    catch (...)
    {
        _vec_params_out.clear();
        _offset_out = 1;
    }
}

string Scene::glmatrix_to_str(float* _matrix)
{
    PTextParser textparser;
    string buffer;
    string value;

    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            buffer = textparser.float_to_stdstr(_matrix[j*4+i]);
            value += buffer;
            value += '\t';
        }
        value += '\n';
    }
    return value;
}

string Scene::modelview_to_str()
{
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix_modelview);
    return glmatrix_to_str(matrix_modelview);
}

std::string Scene::model_to_str()
{
    return glmatrix_to_str(matrix_model);
}

std::string Scene::view_to_str()
{
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix_modelview);

    Mat4x4f cglamat_modelview = Mat4x4f(matrix_modelview);
    Mat4x4f cglamat_model = Mat4x4f(matrix_model);
    Mat4x4f cglamat_view = cglamat_modelview*invert(cglamat_model);
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            matrix_view[i*4+j]=cglamat_view[i][j];
        }
    }
    return glmatrix_to_str(matrix_view);
}

std::string Scene::left_to_str()
{
    turtle_left[0] = (-1)*matrix_model[0];
    turtle_left[1] = (-1)*matrix_model[1];
    turtle_left[2] = (-1)*matrix_model[2];
    return vec3f_to_str(turtle_left);
}

std::string Scene::head_to_str()
{
    turtle_head[0] = matrix_model[4];
    turtle_head[1] = matrix_model[5];
    turtle_head[2] = matrix_model[6];
    return vec3f_to_str(turtle_head);
}
std::string Scene::up_to_str()
{
    turtle_up[0] = matrix_model[8];
    turtle_up[1] = matrix_model[9];
    turtle_up[2] = matrix_model[10];
    //std::cout << std::endl << "init_up:"<<init_up[2];
    return vec3f_to_str(turtle_up);
}

string Scene::vec3f_to_str(Vec3f& _cglavec)
{
    PTextParser textparser;
    string value;
    for (int i=0; i<3; i++)
    {
        float buffer = _cglavec[i];
        value+=textparser.float_to_stdstr(buffer);
        value+='\t';
    }
    return value;
}

void Scene::check_turtle_from(GLfloat* _view, GLfloat* _modelview)
{
    CGLA::Mat4x4f cgla_view(_view);
    CGLA::Mat4x4f cgla_inv_view = CGLA::invert(cgla_view);
    CGLA::Mat4x4f cgla_modelview(_modelview);
    CGLA::Mat4x4f cgla_tmp = cgla_modelview*cgla_inv_view;

    turtle_left[0] = (-1)*cgla_tmp[0][0];
    turtle_left[1] = (-1)*cgla_tmp[0][1];
    turtle_left[2] = (-1)*cgla_tmp[0][2];

    turtle_head[0] = cgla_tmp[1][0];
    turtle_head[1] = cgla_tmp[1][1];
    turtle_head[2] = cgla_tmp[1][2];

    turtle_up[0] = cgla_tmp[2][0];
    turtle_up[1] = cgla_tmp[2][1];
    turtle_up[2] = cgla_tmp[2][2];
}

void Scene::rot_to_vert()
{
    float angle_deg;
    float dot_product_tmp=0;
    Vec3f cross_product_tmp;
    Vec3f turtle_left_new, turtle_up_new;
    Vec3f v(init_head[0], init_head[1], init_head[2]);

    glGetFloatv(GL_MODELVIEW_MATRIX, matrix_modelview);
    check_turtle_from(matrix_view, matrix_modelview);

    turtle_left_new = cross(v, turtle_head);
    turtle_up_new = cross(turtle_head, turtle_up);
    dot_product_tmp = dot(turtle_left, turtle_left_new);
    if (fabs(dot_product_tmp)>1) return;
    angle_deg = acos(dot_product_tmp)*180/3.1415926535;

    dot_product_tmp=0;
    cross_product_tmp = cross(turtle_left, turtle_left_new);
    dot_product_tmp   = dot(cross_product_tmp, turtle_head);
    if (dot_product_tmp>0)
        angle_deg = fabs(angle_deg);
    else angle_deg = (-1)*fabs(angle_deg);

    glRotatef(angle_deg, init_head[0], init_head[1], init_head[2]);
}

void Scene::compute_basis(float* _matrix, CGLA::Vec3f& _origin, CGLA::Vec3f& _left, CGLA::Vec3f& _head, CGLA::Vec3f& _up)
{
    // Turtle left, head, and up should always be refered by init_left, init_head, init_up
    CGLA::Mat4x4f cgla_view(_matrix);
    CGLA::Mat4x4f cgla_inv_view = CGLA::invert(cgla_view);
    CGLA::Mat4x4f cgla_modelview(_matrix);
    CGLA::Mat4x4f cgla_tmp = cgla_modelview*cgla_inv_view;

    _origin = CGLA::Vec3f(cgla_tmp[3][0],       cgla_tmp[3][1],         cgla_tmp[3][2]);
    _left   = CGLA::Vec3f((-1)*cgla_tmp[0][0],  (-1)*cgla_tmp[0][1],    (-1)*cgla_tmp[0][2]);
    _head   = CGLA::Vec3f(cgla_tmp[1][0],       cgla_tmp[1][1],         cgla_tmp[1][2]);
    _up     = CGLA::Vec3f(cgla_tmp[2][0],       cgla_tmp[2][1],         cgla_tmp[2][2]);
}

void Scene::draw_pt3d(std::vector< std::vector<float> >& _vecvec3)
{
    if (_vecvec3.empty()) return;
    if ((_vecvec3.front().size()!=2)||(_vecvec3.front().size()!=3)) return;
    for (std::vector< std::vector<float> >::iterator iter = _vecvec3.begin(); iter!=_vecvec3.end(); iter++)
        draw_pt3d(*iter);
}

void Scene::draw_pt3d(std::vector<float> _vec3)
{
    if ((_vec3.size()!=2)||(_vec3.size()!=3)||(_vec3.empty()))
        return;
    glPushAttrib(GL_CURRENT_BIT);

    glColor3f(0.117, 0.564, 1.0);
    glPointSize(5);
    glBegin(GL_POINTS);
    (_vec3.size()==3)?glVertex3f(_vec3[0], _vec3[1], _vec3[2]):glVertex3f(_vec3[0], _vec3[1], 0);
    glEnd();

    glPopAttrib();
}

void Scene::draw_pt3d(CGLA::Vec3f _pt3d)
{
    glPushAttrib(GL_CURRENT_BIT);

    glColor3f(0.117, 0.564, 1.0);
    glPointSize(5);
    glBegin(GL_POINTS);
    glVertex3f(_pt3d[0], _pt3d[1], _pt3d[2]);
    glEnd();

    glPopAttrib();
}

void Scene::draw_pt3d(CGLA::Vec3f _pt3d, GLfloat _red, GLfloat _green, GLfloat _blue)
{
    glPushAttrib(GL_CURRENT_BIT);

    glColor3f(_red,_green,_blue); //Psychedelic Purple
    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3f(_pt3d[0], _pt3d[1], _pt3d[2]);
    glEnd();

    glPopAttrib();
}

void Scene::draw_vec(CGLA::Vec3f _origin, CGLA::Vec3f _orientation, GLfloat _red, GLfloat _green, GLfloat _blue)
{
    glPushAttrib(GL_CURRENT_BIT);

    glLineWidth(2);
    glBegin(GL_LINES);
        glColor3f(_red, _green, _blue);
        glVertex3f(_origin[0], _origin[1], _origin[2]);
        glVertex3f(_origin[0]+_orientation[0], _origin[1]+_orientation[1], _origin[2]+_orientation[2]);
    glEnd();
    glLineWidth(1);

    // draw arrows(actually big square dots)
    glPointSize(5);
    glBegin(GL_POINTS);
        glVertex3f(_origin[0]+_orientation[0], _origin[1]+_orientation[1], _origin[2]+_orientation[2]);
    glEnd();
    glPointSize(1);

    glPopAttrib();
}

void Scene::draw_basis(CGLA::Vec3f _origin, CGLA::Mat3x3f _units)
{
    draw_vec(_origin, _units[0], 0.874, 0.0, 1.0);
    draw_vec(_origin, _units[1], 0.486, 0.988, 0);
    draw_vec(_origin, _units[2], 0.0f, 1.0f, 1.0f);
}

void Scene::draw_basis(CGLA::Mat4x4f& _basis)
{
    CGLA::Vec3f orig(_basis[3][0], _basis[3][1], _basis[3][2]);
    CGLA::Vec3f u(_basis[0][0], _basis[0][1], _basis[0][2]);
    CGLA::Vec3f v(_basis[1][0], _basis[1][1], _basis[1][2]);
    CGLA::Vec3f w(_basis[2][0], _basis[2][1], _basis[2][2]);
    CGLA::Mat3x3f m(u, v, w);
    draw_basis(orig, m);
}

void Scene::draw_vertices()
{
    for (std::vector<CGLA::Vec3f>::iterator iter=vec_vertices.begin(); iter!=vec_vertices.end(); iter++)
        draw_pt3d(*iter);
//    vec_mean_pt.clear();
//    for (std::vector<PBoundingBox>::iterator iter=vec_bounding_box.begin(); iter!=vec_bounding_box.end(); iter++)
//    {
//        vec_mean_pt.push_back((*iter).);
//    }
}

void Scene::draw_line(CGLA::Vec3f _pt1, CGLA::Vec3f _pt2)
{
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1.0,0.0,0.0);

    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex3f(_pt1[0], _pt1[1], _pt1[2]);
    glVertex3f(_pt2[0], _pt2[1], _pt2[2]);
    glEnd();

    glPopAttrib();
}

void Scene::draw_box(PBoundingBox& _obb)
{
    if (_obb.getVecCorner().size()!=8) return;
    draw_line(_obb.getVecCorner()[0], _obb.getVecCorner()[1]);
    draw_line(_obb.getVecCorner()[1], _obb.getVecCorner()[2]);
    draw_line(_obb.getVecCorner()[2], _obb.getVecCorner()[3]);
    draw_line(_obb.getVecCorner()[3], _obb.getVecCorner()[0]);

    draw_line(_obb.getVecCorner()[4], _obb.getVecCorner()[5]);
    draw_line(_obb.getVecCorner()[5], _obb.getVecCorner()[6]);
    draw_line(_obb.getVecCorner()[6], _obb.getVecCorner()[7]);
    draw_line(_obb.getVecCorner()[7], _obb.getVecCorner()[4]);

    draw_line(_obb.getVecCorner()[0], _obb.getVecCorner()[4]);
    draw_line(_obb.getVecCorner()[1], _obb.getVecCorner()[5]);
    draw_line(_obb.getVecCorner()[2], _obb.getVecCorner()[6]);
    draw_line(_obb.getVecCorner()[3], _obb.getVecCorner()[7]);
}

//void Scene::draw_ogl(bool _names)
//{
//    vector<unsigned int> vec_left_impact, vec_right_impact;
//    vector<GLfloat> vec_params;
//    PCylinder* shape_cyl;
//    PLine* shape_lin;
//    PShapeIndexer* shape_idxr;
//    PCglaProcessor cgla_proc;
//    PShapeInitializer si;
//    CGLA::Vec3f posit3d, rot_axis;
//    CGLA::Mat4x4f cgla_view, cgla_model, cgla_modelview, cgla_tmp;
//    GLfloat shift, angle;
//    GLfloat impact_red, impact_green, impact_blue;
//    unsigned int i, offset;
//    int impact_val, geometry_type;
//    float impact_ratio, basis, top;

//    geometry_type = CYLINDER_SHAPE;
//    angle = lsystem->getAngle();
//    shift = PROCEDURAL_UNIT;

//    glColor3f(0.0,1.0,0.0);
//    glMatrixMode(GL_MODELVIEW);
//    glGetFloatv(GL_MODELVIEW_MATRIX, matrix_view);
//    cgla_tmp = cgla_view = CGLA::Mat4x4f(matrix_view);
//    glPushMatrix();
//    vec_shape_indexer.clear();
//    impact_val = -1;
//    if ((selection!=(-1))&&(enabled_impact))
//        lsystem->select(vec_left_impact, vec_right_impact, selection);
//    if (_names) glInitNames();
//    for (i=0; i<lsystem->getText().size(); i+=offset)
//    {
//        vec_params.clear();
//        offset = 1;
//        switch(lsystem->at(i))
//        {
//        case '+':case '-':case '&':case '^':case '\\':case '/':case '|':
//            check_angle(angle, i, offset, lsystem->at(i), rot_axis);
//            glRotatef(angle, rot_axis[0], rot_axis[1], rot_axis[2]);
//            break;
//        case '[':
//            glPushMatrix();
//            break;
//        case ']':
//            glPopMatrix();
//            break;
//        case '$':
//            rot_to_vert();
//            break;
//        default:
//            glGetFloatv(GL_MODELVIEW, matrix_model);
//            // This function should be reevaluated :
//            compute_basis(matrix_model, posit3d, turtle_left, turtle_head, turtle_up);
//            check_params(vec_params, i, offset);
//            if (lsystem->getAlphabet()->is_shifting(lsystem->at(i)))
//            {
//                glTranslatef(0.0f, shift, 0.0f);
//            }
//            else if (lsystem->getAlphabet()->is_alphabet(lsystem->at(i)))
//            {
//                if ((selection!=(-1))&&(enabled_impact))
//                {
//                    impact_val = eval_impact(vec_left_impact, vec_right_impact, selection, i);
//                    if (impact_val!=(-1))
//                    {
//                        impact_ratio = impact_to_ratio(vec_left_impact.size(), impact_val);
//                        rgb_from_impact(impact_ratio, impact_red, impact_green, impact_blue);
//                        glPushAttrib(GL_CURRENT_BIT);
//                        glColor3f(impact_red, impact_green, impact_blue);
//                    }
//                }
//                if (selection == (signed)i)
//                {
//                    glPushAttrib(GL_CURRENT_BIT);
//                    glColor3f(1.0,0.0,0.0);
//                    glLineWidth(3);
//                }
//                if (_names) glPushName(i);

//                // shape indexing
//                //shape_idxr.init();
//                switch (geometry_type)
//                {
//                case CYLINDER_SHAPE:
//                    if (!vec_params.empty())
//                        shape_cyl = new PCylinder(vec_params.front());
//                    else
//                        shape_cyl = new PCylinder();
//                    shape_cyl->draw();
//                    shape_idxr = new PShapeIndexer(i, posit3d, turtle_left, turtle_head, turtle_up, shape_cyl);
//                    vec_shape_indexer.push_back(shape_idxr);
//                    break;
//                case CONIC_SHAPE:
//                    si.compute_ray(lsystem->getText(), i, basis, top, SIBLING_METHOD);
//                    if (!vec_params.empty())
//                        shape_cyl = new PCylinder(vec_params.front(), basis, top);
//                    else
//                        shape_cyl = new PCylinder();
//                    shape_cyl->draw();
//                    shape_idxr = new PShapeIndexer(i, posit3d, turtle_left, turtle_head, turtle_up, shape_cyl);
//                    vec_shape_indexer.push_back(shape_idxr);
//                    break;
//                default:
//                    shape_lin = new PLine();
//                    shape_lin->draw();
//                    shape_idxr = new PShapeIndexer(i, posit3d, turtle_left, turtle_head, turtle_up, shape_lin);
//                    vec_shape_indexer.push_back(shape_idxr);
//                }
//                if (_names) glPopName();
//                if (selection == (signed)i) glPopAttrib();
//            }
//            break;
//        }
//        if ((enabled_impact)&&(impact_val!=(-1))) glPopAttrib();
//    }
//    glPopMatrix();
//    if (getSelectionChanged()) setSelectionChanged(false);
//}
void Scene::setSelection(std::vector<unsigned int> _vec_list_selection)
{

    list_selection.clear();
    unsigned int i;
    for (i=0; i<_vec_list_selection.size(); i++)
    {
        list_selection.push_back(_vec_list_selection[i]);
    }
}

void Scene::addIdToSelection(int _id)
{
    // The next condition was created to avoid a bug likely from libQGLViewer
    // and could create an unexpected behaviour one of these days
    if ((_id>=0)&&(_id<=lsystem->getText().size()))
    {
        if (!list_selection.contains(_id))
            list_selection.push_back(_id);
        else
            list_selection.erase(std::find(list_selection.begin(), list_selection.end(), _id));
    }
}

void Scene::removeIdFromSelection(int _id)
{
    list_selection.removeAll(_id);
}

void Scene::draw_cgla(bool _names)
{
    vector<unsigned int> vec_left_impact, vec_right_impact;
    vector<GLfloat> vec_params;
    vector<float> vec_color_tmp;
    PSimplisticLeaf* shape_leaf=NULL;
    PCylinder* shape_cyl=NULL;
    PLine* shape_lin=NULL;
    PShapeInitializer si(lsystem->getText());
    CGLA::Vec3f vec3f_tmp, posit3d, rot_axis;
    GLfloat shift, angle;
    GLfloat impact_red, impact_green, impact_blue;
    //std::map<unsigned int, std::vector<float> > map_params_tmp;
    std::string alphabet_found, alphabet_shape;
    size_t alphabet_size;
    unsigned int i, offset;
    int impact_val;
    float impact_ratio;

    /*
    struct timeval dbg_now, dbg_before, dbg_after;
    unsigned int dbg_idx_step=1000;
    unsigned int dbg_idx_old=0;
    unsigned int dbg_idx_curr=0;
    unsigned int dbg_idx_delta=0;
    gettimeofday(&dbg_now, NULL);
    dbg_before = dbg_now;
    */

//   struct timeval dbg_rayc_before, dbg_rayc_after;
   struct timeval dbg_init_before, dbg_init_after;
   struct timeval dbg_default_before, dbg_default_after, dbg_default_mid1, dbg_default_mid2;
   struct timeval dbg_case1_before, dbg_case1_after, dbg_case1_mid;
   struct timeval dbg_case2_before, dbg_case2_after;
   struct timeval dbg_case3_before, dbg_case3_after;
   struct timeval dbg_case4_before, dbg_case4_after;
   float dbg_default_total=0;float dbg_default_part1=0;float dbg_default_part2=0;float dbg_default_part3=0;
   float dbg_case1_total=0;float dbg_case1_part1=0;float dbg_case1_part2=0;
   float dbg_case2_total=0;
   float dbg_case3_total=0;
   float dbg_case4_total=0;

    angle = lsystem->getAngle();
    shift = PROCEDURAL_UNIT;
    //clear_shape_idxr();
    cgla_proc.init();

    std::cout << std::endl << "Draw initialization... :\t";
    gettimeofday(&dbg_init_before, NULL);
    std::cout << 0.000001*(dbg_init_before.tv_usec + dbg_init_before.tv_sec*1000000 - DBG_NOW);

    vec_shape_indexer.clear();
    vec_shape_indexer.reserve(lsystem->getText().size());
    //if ((selection!=(-1))&&(enabled_impact)) lsystem->select(vec_left_impact, vec_right_impact, selection);
    if ((list_selection.size()==1)&&(enabled_impact))
        lsystem->select(vec_left_impact, vec_right_impact, list_selection.front());
    if (_names) glInitNames();

    std::map<unsigned int, std::pair<float, float> > map_ibt;
    std::vector<PTextMap*> vec_tm;
    float my_basis, my_top;

    if (mod_config!=0)
    {
        if (!mod_config->getVecShapeStr().empty())
            setGeometryType(CONFIGURED_SHAPE);
    }
    if ((geometry_type == CONIC_SHAPE)||(geometry_type == CONFIGURED_SHAPE))
    {
        si.map(lsystem->getText(), vec_tm, lsystem);

        std::cout << std::endl << "Ray mapping done... :\t\t";
        gettimeofday(&dbg_init_after, NULL);
        std::cout << 0.000001*(dbg_init_after.tv_usec + dbg_init_after.tv_sec*1000000 - DBG_NOW);
        map_ibt = si.precompute_rays(vec_tm);
        for (std::vector<PTextMap*>::iterator iter = vec_tm.begin(); iter!=vec_tm.end(); iter++)
        {
            delete (*iter);
        }
        vec_tm.clear();
        lsystem->init_map_offset();
        lsystem->init_map_params();
    }

//    std::cout << std::endl << "Draw Initialization and Mapping Time : ";
//    std::cout << 0.000001*(dbg_init_after.tv_usec + dbg_init_after.tv_sec*1000000 - dbg_init_before.tv_usec - dbg_init_before.tv_sec*1000000);

    std::cout << std::endl << "Ray computation done... :\t";
    gettimeofday(&dbg_init_after, NULL);
    std::cout << 0.000001*(dbg_init_after.tv_usec + dbg_init_after.tv_sec*1000000 - DBG_NOW);

    impact_val = -1;
    glColor3f(0.0f,1.0f,0.0f);
    for (i=0; i<lsystem->getText().size(); i+=offset)
    {
        vec_params.clear();
        offset = 1;

        //si.refresh_state(lsystem->at(i), i);
        switch(lsystem->at(i))
        {
        case '+':case '-':case '&':case '^':case '\\':case '/':case '|':
            gettimeofday(&dbg_case1_before, NULL);
            //map_params_tmp = lsystem->getMapParams();
            //check_angle(angle, rot_axis, offset, lsystem->at(i), map_params_tmp, i);
            check_angle(angle, i, offset, lsystem->at(i), rot_axis);
            gettimeofday(&dbg_case1_mid, NULL);
            cgla_proc.cgla_rotate(angle, rot_axis);
            gettimeofday(&dbg_case1_after, NULL);
            dbg_case1_total+=(0.000001*(dbg_case1_after.tv_usec + dbg_case1_after.tv_sec*1000000 - dbg_case1_before.tv_usec - dbg_case1_before.tv_sec*1000000));
            dbg_case1_part1+=(0.000001*(dbg_case1_mid.tv_usec + dbg_case1_mid.tv_sec*1000000 - dbg_case1_before.tv_usec - dbg_case1_before.tv_sec*1000000));
            dbg_case1_part2+=(0.000001*(dbg_case1_after.tv_usec + dbg_case1_after.tv_sec*1000000 - dbg_case1_mid.tv_usec - dbg_case1_mid.tv_sec*1000000));
            break;
        case '[':
            gettimeofday(&dbg_case2_before, NULL);
            cgla_proc.cgla_push_matrix();
            gettimeofday(&dbg_case2_after, NULL);
            dbg_case2_total+=(0.000001*(dbg_case2_after.tv_usec + dbg_case2_after.tv_sec*1000000 - dbg_case2_before.tv_usec - dbg_case2_before.tv_sec*1000000));
            break;
        case ']':
            gettimeofday(&dbg_case3_before, NULL);
            cgla_proc.cgla_pop_matrix();
            gettimeofday(&dbg_case3_after, NULL);
            dbg_case3_total+=(0.000001*(dbg_case3_after.tv_usec + dbg_case3_after.tv_sec*1000000 - dbg_case3_before.tv_usec - dbg_case3_before.tv_sec*1000000));
            break;
        case '$':
            gettimeofday(&dbg_case4_before, NULL);
            cgla_proc.rot_to_vert();
            gettimeofday(&dbg_case4_after, NULL);
            dbg_case4_total+=(0.000001*(dbg_case4_after.tv_usec + dbg_case4_after.tv_sec*1000000 - dbg_case4_before.tv_usec - dbg_case4_before.tv_sec*1000000));
            break;
        default:
            gettimeofday(&dbg_default_before, NULL);
            cgla_proc.compute_basis(posit3d, turtle_left, turtle_head, turtle_up);
            gettimeofday(&dbg_default_mid1, NULL);

            vec_params = lsystem->current_map_params(i);
            offset = lsystem->current_offset(i);
            //check_params(vec_params, i, offset);
            //check_params(vec_params, offset, i, map_params_tmp);
            gettimeofday(&dbg_default_mid2, NULL);
            if ((alphabet_size=lsystem->getAlphabet()->search(lsystem->getText(), i))!=0)
            {
                if (mod_config!=0)
                {
                    alphabet_found = lsystem->getText().substr(i, alphabet_size);
                    vec_color_tmp = mod_config->check_color(alphabet_found);
                    alphabet_shape = mod_config->check_shape(alphabet_found);
                    glPushAttrib(GL_CURRENT_BIT); // Beware this is the first of two pushes on some cases
                    glColor4f(vec_color_tmp[0], vec_color_tmp[1], vec_color_tmp[2], vec_color_tmp[3]);
                }
                if ((list_selection.size()==1)&&(enabled_impact))
                //if ((selection!=(-1))&&(enabled_impact))
                {
                    impact_val = eval_impact(vec_left_impact, vec_right_impact, list_selection.front(), i);
                    if (impact_val!=(-1))
                    {
                        impact_ratio = impact_to_ratio(vec_left_impact.size(), impact_val);
                        rgb_from_impact(impact_ratio, impact_red, impact_green, impact_blue);
                        //glPushAttrib(GL_CURRENT_BIT);
                        glColor3f(impact_red, impact_green, impact_blue);
                    }
                }

                if (is_selected(i))
                //if (selection == (signed)i)
                {
                    glPushAttrib(GL_CURRENT_BIT);
                    glColor3f(1.0,0.0,0.0);
                    glLineWidth(3);
                }
                if (_names) glPushName(i);

                switch (geometry_type)
                {
                case CYLINDER_SHAPE:
                    if (!vec_params.empty())
                        shape_cyl = new PCylinder(vec_params.front());
                    else
                        shape_cyl = new PCylinder();
                    shape_cyl->draw(cgla_proc);
                    vec_shape_indexer.push_back(new PShapeIndexer(i, posit3d, turtle_left, turtle_head, turtle_up, shape_cyl));
                    delete shape_cyl;
                    break;
                    // unclean code:
                case CONIC_SHAPE:
                    if (!si.check_rays(map_ibt, i, my_basis, my_top))
                    {
                        std::cout << std::endl << "Failed to relate text to the graphic model!";
                        break;
                    }
                    if (!vec_params.empty())
                        //shape_cyl = new PCylinder(vec_params.front(), si.getBasis(), si.getTop());
                        shape_cyl = new PCylinder(vec_params.front());
                    else
                        shape_cyl = new PCylinder();
                    shape_cyl->setBase(my_basis);
                    shape_cyl->setTop(my_top);
                    shape_cyl->draw(cgla_proc);
                    vec_shape_indexer.push_back(new PShapeIndexer(i, posit3d, turtle_left, turtle_head, turtle_up, shape_cyl));
                    delete shape_cyl;
                    break;
                case CONFIGURED_SHAPE:
                    if (!si.check_rays(map_ibt, i, my_basis, my_top))
                    {
                        std::cout << std::endl << "Failed to relate text to the graphic model!";
                        break;
                    }
                    if (mod_config!=0)
                    {
                        if (alphabet_shape.compare("leaf")==0)
                        {
                            shape_leaf = (!vec_params.empty())?new PSimplisticLeaf(vec_params.front()):new PSimplisticLeaf();
                            shape_leaf->setBase(my_basis);
                            shape_leaf->setTop(my_top);
                            shape_leaf->draw(cgla_proc);
                            vec_shape_indexer.push_back(new PShapeIndexer(i, posit3d, turtle_left, turtle_head, turtle_up, shape_leaf));
                            delete shape_leaf;
                        }
                        else
                        {
                            shape_cyl = (!vec_params.empty())?new PCylinder(vec_params.front()):new PCylinder();
                            shape_cyl->setBase(my_basis);
                            shape_cyl->setTop(my_top);
                            shape_cyl->draw(cgla_proc);
                            vec_shape_indexer.push_back(new PShapeIndexer(i, posit3d, turtle_left, turtle_head, turtle_up, shape_cyl));
                            delete shape_cyl;
                        }
                    }
                    else
                    {
                        shape_cyl = (!vec_params.empty())?new PCylinder(vec_params.front()):new PCylinder();

                        shape_cyl->setBase(my_basis);
                        shape_cyl->setTop(my_top);
                        shape_cyl->draw(cgla_proc);
                        vec_shape_indexer.push_back(new PShapeIndexer(i, posit3d, turtle_left, turtle_head, turtle_up, shape_cyl));
                        delete shape_cyl;
                    }

                    break;
                default:
                    shape_lin = new PLine();
                    shape_lin->draw(cgla_proc);
                    vec_shape_indexer.push_back(new PShapeIndexer(i, posit3d, turtle_left, turtle_head, turtle_up, shape_lin));
                    delete shape_lin;
                }
                if (_names) glPopName();
                //if ((enabled_impact)&&(impact_val!=(-1))) glPopAttrib();
                //if (selection == (signed)i) glPopAttrib();
                if (is_selected(i)) glPopAttrib();
                if (mod_config!=0) glPopAttrib(); // Beware of two pops when modifying this section
            }
            else if (lsystem->getAlphabet()->is_shifting(lsystem->at(i)))
            {
                vec3f_tmp=Vec3f(0.0f, shift, 0.0f);
                cgla_proc.cgla_translate(vec3f_tmp);
            }
            gettimeofday(&dbg_default_after, NULL);
            dbg_default_total+=(0.000001*(dbg_default_after.tv_usec + dbg_default_after.tv_sec*1000000 - dbg_default_before.tv_usec - dbg_default_before.tv_sec*1000000));
            dbg_default_part1+=(0.000001*(dbg_default_mid1.tv_usec + dbg_default_mid1.tv_sec*1000000 - dbg_default_before.tv_usec - dbg_default_before.tv_sec*1000000));
            dbg_default_part2+=(0.000001*(dbg_default_mid2.tv_usec + dbg_default_mid2.tv_sec*1000000 - dbg_default_mid1.tv_usec - dbg_default_mid1.tv_sec*1000000));
            dbg_default_part3+=(0.000001*(dbg_default_after.tv_usec + dbg_default_after.tv_sec*1000000 - dbg_default_mid2.tv_usec - dbg_default_mid2.tv_sec*1000000));
        }
//        std::cout << std::endl << "End loop... : ";
//        gettimeofday(&dbg_default_after, NULL);
//        std::cout << 0.000001*(dbg_default_after.tv_usec + dbg_default_after.tv_sec*1000000 - DBG_NOW);
    }
    if (getSelectionChanged()) setSelectionChanged(false);
//    gettimeofday(&dbg_loop_after, NULL);
//    std::cout << std::endl << " Drawing loop exec time : ";
//    std::cout << 0.000001*(dbg_loop_after.tv_usec + dbg_loop_after.tv_sec*1000000 - dbg_loop_before.tv_usec - dbg_loop_before.tv_sec*1000000);

    std::cout << std::endl << " Case Symbol exec time :\t"  << dbg_case1_total;
    std::cout << "\t[String part = " << dbg_case1_part1 << ", Ray Part =" << dbg_case1_part2 << "]";
    std::cout << std::endl << " Case Push exec time :\t\t"    << dbg_case2_total;
    std::cout << std::endl << " Case Pop exec time :\t\t"     << dbg_case3_total;
    std::cout << std::endl << " Case VertRot exec time :\t" << dbg_case4_total;
    std::cout << std::endl << " Case Default exec time :\t" << dbg_default_total;
    std::cout << "\t[Matrix part = " << dbg_default_part1 << ", String part =" << dbg_default_part2 << ", Drawing Part =" << dbg_default_part3 << "]";
}

PCglaProcessor Scene::compute_cgla(unsigned int _idx)
{
    PCglaProcessor value;
    vector<unsigned int> vec_left_impact, vec_right_impact;
    vector<GLfloat> vec_params;
    CGLA::Vec3f vec3f_tmp, posit3d, rot_axis;
    GLfloat shift, angle;
    float f;
    unsigned int i, offset;

    lsystem->init_map_offset();
    lsystem->init_map_params();
    angle = lsystem->getAngle();
    shift = PROCEDURAL_UNIT;
    if (_idx>lsystem->getText().size()) return value;

    value.cgla_load_identity4x4f();

//    if ((selection!=(-1))&&(enabled_impact))
//        lsystem->select(vec_left_impact, vec_right_impact, selection);
    if ((list_selection.size()==1)&&(enabled_impact))
        lsystem->select(vec_left_impact, vec_right_impact, list_selection.front());
    i=0;
    while (i<_idx)
    {
        vec_params.clear();
        offset = 1;
        switch(lsystem->at(i))
        {
        case '+':case '-':case '&':case '^':case '\\':case '/':case '|':
            check_angle(angle, i, offset, lsystem->at(i), rot_axis);
            value.cgla_rotate(angle, rot_axis);
        case '[':
            value.cgla_push_matrix();
            break;
        case ']':
            value.cgla_pop_matrix();
            break;
        case '$':
            value.rot_to_vert();
            break;
        default:
            value.compute_basis(posit3d, turtle_left, turtle_head, turtle_up);
            check_params(vec_params, i, offset);
            if (lsystem->getAlphabet()->is_shifting(lsystem->at(i)))
            {
                vec3f_tmp=Vec3f(0.0f, shift, 0.0f);
                value.cgla_translate(vec3f_tmp);
            }
            else if (lsystem->getAlphabet()->is_alphabet(lsystem->at(i)))
            {
                f = (vec_params.empty())?PROCEDURAL_UNIT:vec_params.front();
                vec3f_tmp = CGLA::Vec3f(0,f,0);
                value.cgla_translate(vec3f_tmp);
            }
            break;
        }
        i+=offset;
    }
    return value;
}

void Scene::draw_obb()
{
    for (std::vector<PBoundingBox>::iterator iter=vec_bounding_box.begin(); iter!=vec_bounding_box.end(); iter++)
    {
        draw_box(*iter);
    }
}

void Scene::add_basis(CGLA::Vec3f _origin, CGLA::Mat3x3f _basis)
{
    CGLA::Vec4f r0(_basis[0], 0);
    CGLA::Vec4f r1(_basis[1], 0);
    CGLA::Vec4f r2(_basis[2], 0);
    CGLA::Vec4f r3(_origin, 1);
    CGLA::Mat4x4f m(r0, r1, r2, r3);
    vec_basis3d.push_back(m);
}

void Scene::draw_basis()
{
    for (std::vector<CGLA::Mat4x4f>::iterator iter=vec_basis3d.begin(); iter!=vec_basis3d.end(); iter++)
    {
        draw_basis(*iter);
    }
}

void Scene::draw_specific_vert()
{
    for (std::vector<CGLA::Vec3f>::iterator iter=vec_specific_vert.begin(); iter!=vec_specific_vert.end(); iter++)
    {
        draw_pt3d(*iter, 1.0f, 1.0f, 0.0f);
    }
}

void Scene::add_vertices(std::vector< std::vector<float> >& _vec_vertices)
{
    if (_vec_vertices.empty()) return;
    if (!((_vec_vertices.front().size()==2)||(_vec_vertices.front().size()==3))) return;
    for (std::vector< std::vector<float> >::iterator iter=_vec_vertices.begin(); iter!=_vec_vertices.end(); iter++)
    {
        ((*iter).size()==3)?vec_vertices.push_back(CGLA::Vec3f((*iter)[0], (*iter)[1], (*iter)[2])):vec_vertices.push_back(CGLA::Vec3f((*iter)[0], (*iter)[1], 0.0f));
    }
}

#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QKeyEvent>
#include <vector>
#include <algorithm>
#include <string>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include "ptext3d.h"
#include "CGLA/Vec3f.h"
#include "pboundingbox.h"

#define PROCEDURAL_UNIT 1.0f

enum {STD_ORIENT, CUSTOM_ORIENT1, CUSTOM_ORIENT2, CUSTOM_ORIENT3, CUSTOM_ORIENT4, CUSTOM_ORIENT5, REAL_TARGETS, REAL_PTS, REAL_GAP, RESAMPLED};
enum {DISABLE_DASHD, DASH_METHOD1, DASH_METHOD2, DASH_METHOD3, DASH_MANUAL};

class Viewer : public QGLViewer
{
    std::vector< std::vector<GLfloat> > vecs_imgid;         // Most of the times it will contain the bounding box
    std::vector< std::vector<GLfloat> > vecs_std_imgid;
    std::vector< std::vector<GLfloat> > vecs_absoluteimgid; // The 3D real coordinates of the images
    std::vector<uint> vec_absolresampld1, vec_absolresampld2, vec_absolresampld3;
    std::vector<uint> vec_absolresampld_man;
    std::vector< std::vector<GLfloat> > vecs_scatterplot;
    std::vector<PText3D*> vec_t3d;
    std::vector<PBoundingBox> vec_obb;
    std::vector< std::vector< std::vector<GLfloat> > > vecs_obb_clouds;
    std::vector<bool> vec_obbdisplayed;
    std::vector<float> vec_usefulns;
    std::vector<float> vec_diff;
    std::vector<float> vec_xqlfr, vec_yqlfr, vec_zqlfr;
    std::vector<std::string> vec_qlfrgrmr, vec_allqlfrs, vec_qlfrfilter;
    QVector<QStringList*> qvec_qlfrs;
    QColor bgcolor_default;

    float absolutexmin, absoluteymin, absolutezmin;
    float absolutexmax, absoluteymax, absolutezmax;
    float xmin, ymin, zmin;
    float xmax, ymax, zmax;

    float step;
    float idxr_radius;
    int idxr_fontsize;
    int box_orientation;
    int dash_method;
    bool is_realtarget_drawn;
    bool is_qualifier_drawn;
    bool is_scattered_drawn;
    bool is_allqlfrs_drawn;

    size_t init_scattersize, final_scattersize;

    std::string fn_manmatch;
    // Stats use:
    std::vector<GLfloat> vec_obbvol, vec_obbvol_resampled;
    std::vector< std::vector<GLfloat> > vecs_obbcenter;
    std::vector< std::vector< std::vector<GLfloat> > > vecs_obb_cloudsresampled;
    std::vector<PBoundingBox> vec_obb_resampled;
    bool is_obb_drawn, is_obbresampled_drawn;
public:
    Viewer();
    Viewer(int _type, QWidget* _parent=0, const QGLWidget* _shareWidget=NULL, Qt::WindowFlags _flags = 0);
    ~Viewer();
    QVector<QStringList*> getqvecqlfrs() const {return qvec_qlfrs;}
    std::vector<uint> getvecabsolresampld1() const {return vec_absolresampld1;}
    std::vector<uint> getvecabsolresampld2() const {return vec_absolresampld2;}
    std::vector<uint> getvecabsolresampld3() const {return vec_absolresampld3;}
    std::vector<uint> getvecabsolresampldman() const {return vec_absolresampld_man;}
    inline void setidxrradius(float _idxr_radius){idxr_radius=_idxr_radius;}
    inline void setrealtargetdrawn(bool _is_realtarget_drawn){is_realtarget_drawn=_is_realtarget_drawn;}
    inline void setqualifierdrawn(bool _is_qualifier_drawn){is_qualifier_drawn=_is_qualifier_drawn;}
    inline void setscattereddrawn(bool _is_scattered_drawn){is_scattered_drawn=_is_scattered_drawn;}
    inline void setallqlfrsdrawn(bool _is_allqlfrs_drawn){is_allqlfrs_drawn=_is_allqlfrs_drawn;}
    inline void setqvecqlfrs(QVector<QStringList*> _qvec_qlfrs){qvec_qlfrs=_qvec_qlfrs;}
    inline void setvecusefulns(std::vector<float> _vec_usefulns){vec_usefulns = _vec_usefulns;}
    inline void setvecdiff(std::vector<float> _vec_diff){vec_diff = _vec_diff;}
    inline void setextremums(std::vector<float> _vec_extremum){if (_vec_extremum.size()>=6)
        {absolutexmin=_vec_extremum[0];absoluteymin=_vec_extremum[1];absolutezmin=_vec_extremum[2];
         absolutexmax=_vec_extremum[3];absoluteymax=_vec_extremum[4];absolutezmax=_vec_extremum[5];}}
    void setvecsabsolute(std::vector< std::vector<GLfloat> > _vecs_absoluteimgid){vecs_absoluteimgid=_vecs_absoluteimgid;}
    void setvecsscatterplot(std::vector< std::vector<GLfloat> > _vecs_scatteredplot){vecs_scatterplot=_vecs_scatteredplot;scattercount();}
    void setfnmanmatch(std::string _fn_manmatch){fn_manmatch = _fn_manmatch;}
    void setvecxqlfr(std::vector<float> _vec_xqlfr){vec_xqlfr=_vec_xqlfr;}
    void setvecyqlfr(std::vector<float> _vec_yqlfr){vec_yqlfr=_vec_yqlfr;}
    void setveczqlfr(std::vector<float> _vec_zqlfr){vec_zqlfr=_vec_zqlfr;}
    void setvecqlfrgrmr(std::vector<std::string> _vec_qlfrgrmr){vec_qlfrgrmr=_vec_qlfrgrmr;}
    void setvecallqlfrs(std::vector<std::string> _vec_allqlfrs){vec_allqlfrs=_vec_allqlfrs;}
    void setvecobb(std::vector<PBoundingBox> _vec_obb){vec_obb=_vec_obb;}
    void setvecobbdisplayed(std::vector<bool> _vec_obbdisplayed){vec_obbdisplayed = _vec_obbdisplayed;}
    void setvecsobbclouds(std::vector< std::vector< std::vector<GLfloat> > > _vecs_obb_clouds){vecs_obb_clouds = _vecs_obb_clouds;}
    std::vector< std::vector<float> > getclouds();
    //float scalepca(float _x);

public:
    virtual void draw();
    virtual void init();
    void init_extremums();
    //void init_extremums(std::vector<float>& _vec_extremum);
    void init_imgids(bool _draw_real=false);
    void drawpt3d(std::vector<float>& _vec3);
    void drawline(std::vector<GLfloat>& _pt1,std::vector<GLfloat>& _pt2);
    void drawline(std::vector<GLfloat>& _pt1,std::vector<GLfloat>& _pt2, GLfloat* _color);
    void drawline(CGLA::Vec3f _pt1, CGLA::Vec3f _pt2);
    void drawline(CGLA::Vec3f _pt1, CGLA::Vec3f _pt2, GLfloat _red, GLfloat _green, GLfloat _blue);
    void drawdashedline(std::vector<GLfloat>& _pt1,std::vector<GLfloat>& _pt2, GLfloat* _color);
    void drawbox(std::vector< std::vector<GLfloat> >& _corners);
    void drawtext(QString& _text, int _size=5);
    void drawtext(QString& _text, GLfloat* _color, int _size=5);
    void drawsphere(std::vector<GLfloat>& _center, GLfloat _radius);
    void drawsphere(std::vector<GLfloat>& _center, GLfloat _radius, GLfloat* _color);
    void drawsphere(std::vector<GLfloat>& _center, GLfloat _radius, GLfloat _red, GLfloat _green, GLfloat _blue);
    void draw_imgid(unsigned int _idx);
    void draw_imgwire(bool _draw_inside=true);
    void draw_qlfrs();
    void draw_usefn();
    void draw_diff();
    void draw_allqlfrs();
    void update_orientation();
    void draw_obb(PBoundingBox& _obb);
    void draw_obb(PBoundingBox& _obb, GLfloat _red, GLfloat _green, GLfloat _blue);

public:
    void resampledist_method1();
    void resampledist_method2();
    void resampledist_method3();
    void scattercount();
    GLfloat euclidist(const std::vector<float>& _pt1, const std::vector<float>& _pt2);
    GLfloat maxdist(std::vector< std::vector<GLfloat> >& _src);
    void load_matching(std::string _filename);
    void update_qlfrfilter();
    std::vector<size_t> check_neighbours(std::vector< std::vector<float> >& _vecs_clouds, std::vector<float> _target, unsigned int _neighbours);
    void update_obbdisplay();
    void update_obbdisplay(std::vector<bool>& _vec_obbdisplayed);

    template <typename T>
    std::vector<size_t> ordered(std::vector<T> const& _values)
    {
       using namespace boost::phoenix;
       using namespace boost::phoenix::arg_names;

       std::vector<size_t> indices(_values.size());
       int i = 0;
       std::transform(_values.begin(), _values.end(), indices.begin(), ref(i)++);
       std::sort(indices.begin(), indices.end(), ref(_values)[arg1] < ref(_values)[arg2]);
       return indices;
    }

public: // stats use
    std::vector< GLfloat > mean(const std::vector< std::vector<GLfloat> >& _vec_clouds);
    std::vector< std::vector<GLfloat> > compute_center(const std::vector< std::vector< std::vector<GLfloat> > >& _vecs_clouds);
    void save_obbcenter(std::string _filename, const std::vector<PBoundingBox>& _vec_obb);
    void save_obbcenter(std::string _filename);
    void save_obbcorner(std::string _filenameprefix, const std::vector<PBoundingBox>& _vec_obb);
    std::vector< std::vector<GLfloat> > remove_furthest(std::vector< std::vector<GLfloat> > _vecs_clouds, std::vector<float> _target, unsigned int _remove_neighbours);
    void resample_obb();
    void save_obbvol();
    bool is_within(PBoundingBox& _obb, std::vector<GLfloat> _pt);
    float belong_rates(PBoundingBox& _obb, std::vector < std::vector<GLfloat> >& _vec_pts);
    void save_belongrates();
    void save_belongrates_regen();
    void save_averages_dist();
    void save_center_dist();
protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // VIEWER_H

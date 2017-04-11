#ifndef VIEWERRAW_H
#define VIEWERRAW_H

#include <QGLViewer/qglviewer.h>
#include <QMenu>
#include "engine/procedural.h"
#include "ptext3d.h"
#include "pimgdata.h"



class ViewerRaw : public QGLViewer
{
    QVector< QVector<PImgData>* > qvecvec_imgdata;

    GLuint* texture_names;
    GLfloat box_length, box_width, box_height;
    int highlight_idx;
    int nb_column;
    GLfloat snapshot_spacing;
    bool name_drawn, label_drawn;
    GLfloat title_border;
    GLfloat highlighting_thickness;
    QMenu *contextual;
    QString qstr_csv_fn; 
    std::vector<int> vec_selection;
    std::vector<int> vec_main_img;

    QHash<QString,int> *hash_save; //hashtab to link line with an image
    QString id;// id du test
    QString test_name; // Name of the csv file
    QString date_test; //date of the test

protected:
    qglviewer::Vec origin, direction, selected_point;
protected:
    virtual void mousePressEvent(QMouseEvent* e);
public:
    ViewerRaw(int _type, QWidget* _parent, const QGLWidget* _share_widget=NULL);
    ViewerRaw(QString _csv_filename, int _type, QWidget* _parent, const QGLWidget* _share_widget=NULL);
    ViewerRaw(QString _csv_filename, QString _id_user, QString _date, int _type, QWidget* _parent, const QGLWidget* _share_widget=NULL);
    ~ViewerRaw();
    virtual void init();
    virtual void postSelection(const QPoint& _point);
    virtual void drawWithNames();

    void setNbColumn(int nb);
    void setContextual(QMenu* _contextual){contextual = _contextual;}
    void setCsvFilename(QString _qstr_csv_fn){qstr_csv_fn=_qstr_csv_fn;}
    QString getCsvFilename() const {return qstr_csv_fn;}
    int getHighlightIdx() const {return highlight_idx;}
    GLfloat getBoxWidth() const {return box_width;}
    GLfloat getBoxHeight() const {return box_height;}
    void add_selection(int _idx, bool _reset=false);
    bool empty_selection(){return vec_selection.empty()?true:false;}
    void add_main_img(int _idx, bool _reset=false){if (_reset==true) vec_main_img.clear(); if (_idx>=0) vec_main_img.push_back(_idx);}


public:
    virtual void draw();
    void draw_box(GLfloat _length, GLfloat _width, GLfloat _height);
    void draw_box(GLuint _texture_name, GLint _texture_face, GLfloat _length, GLfloat _width, GLfloat _height, bool _label_drawn=false);
    void compute_vertices(GLfloat _out_vertices[8][3], GLfloat _out_normals[6][3], GLint _out_faces[6][4], GLfloat _length, GLfloat _width, GLfloat _height);
    void draw_border(int _size, QString _str_group_name);
    void text_glut(QString& _text);
    void text_3d(QString& _text);
    bool is_selected(int _elt_idx);
    bool is_selected_main(int _elt_idx);
public:
    PImgData& select_imgdt(unsigned int _gl_id);
    std::vector<PImgData*> multiselect_imgdt(std::vector<int> _vec_slctn);
    void draw_group(const QVector<PImgData>& _qvec_imgdata);
    void draw_group(const QVector<PImgData>* _qvec_imgdata, unsigned int _group_id);
    void move_to(unsigned int _group_id);
    void multimove_to(unsigned int _group_id);
    void main_tree();
    int load_image_list(QString file_basename, QString _extension,
                        QStringList& _qsl_filename,
                        QVector<QImage>& _qvec_image);
    int load_image_list(QString _csv_filename);
};

#endif // VIEWERRAW_H

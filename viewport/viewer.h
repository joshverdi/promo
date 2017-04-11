#ifndef VIEWER_H
#define VIEWER_H

#include <QGLViewer/qglviewer.h>
#include <QMenu>
#include <vector>
#include "scene.h"

class Viewer : public QGLViewer
{
    QWidget* wnd_root;
    bool selection_occured;
    bool multiselect_activated;
    bool horizgrid_drawn;
    QRect rectangle;
    enum SelectionMode {NONE, ADD, REMOVE, SINGLE};
    SelectionMode selectMode;
    QColor bgcolor_default;
public:
    Viewer();
    Viewer(Scene* _pScene, int type, QWidget* parent, const QGLWidget* shareWidget=NULL, Qt::WindowFlags flags = 0, bool _draw_grid = true, bool _draw_axis = false);
    ~Viewer();
    void setScene(Scene* _scn){scene = _scn;}
    void setHorizGridDrawn(bool _horizgrid_drawn){horizgrid_drawn = _horizgrid_drawn;}
    void drawSelectionRectangle() const;
public:
    virtual void draw();
    virtual void drawWithNames();
    virtual void init();
    //virtual void postSelection(const QPoint& point);
    virtual void endSelection(const QPoint&);

    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
public:
    bool getSelectionOccured() const {return selection_occured;}
    void setSelectionOccured(bool _value) {selection_occured = _value;}
    void take_snapshot(int _resolut, QString _filename);
    void take_snapshot(int _resolut, QString _filename, QString _qs_format);
    float radius_from_str(std::string _src);
    void set_pointSize();
    void draw_horiz_grid();
    void saveVectorialSnapshot();
    static void progressFunction(float f, const std::string& info);
    void save_camera();
    void load_camera();
    void save_selection();
    void load_selection();

public:
    Scene* scene;
    qglviewer::Vec origin, direction, selected_point;
};

#endif // VIEWER_H

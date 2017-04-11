#ifndef DIALOGVIEWER_H
#define DIALOGVIEWER_H

#include <QDialog>
#include <QMenu>
#include <QSignalMapper>

class ViewerRaw;
class Viewer;
class Scene;

namespace Ui {
class DialogViewer;
}

class DialogViewer : public QDialog
{
    Q_OBJECT
    ViewerRaw* viewer;
    Viewer* snapshot_viewer;
    Scene* scene;

    QMenu *contextual;
    //QMenu *sub_add_to;
    QAction *act_maingrp;    
    QAction *act_maintree;
    QAction *act_view3d;
    QList<QAction*> list_act_grp;
    QSignalMapper* signal_mapper;
    QString input_filename;
    bool is_test;

public:
    explicit DialogViewer(QWidget *parent = 0);
    ~DialogViewer();
    ViewerRaw* getViewer() {return viewer;}
    void setInputFilename(QString _input_filename){input_filename=_input_filename;}
    QString getInputFilename() const {return input_filename;}
    void updateViewer3d(QString _id_user, QString _date);
    void setTest(bool _is_test) {is_test=_is_test;}
    void closeEvent(QCloseEvent *);


public:
    int num_check(int _in);

protected:
    void mousePressEvent(QMouseEvent* e);
    void keyPressEvent(QKeyEvent* e);

protected slots:
    void act_maingrp_triggered();    
    void act_maintree_triggered();
    void generic_act_triggered(int);
    void act_view3d_triggered();
    void on_windowSize_valueChanged(int);

private slots:
    void on_windowSize_editingFinished();

private:
    Ui::DialogViewer *ui;
};

#endif // DIALOGVIEWER_H

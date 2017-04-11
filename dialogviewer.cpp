#include <QFileDialog>
#include <QMouseEvent>
#include <QMessageBox>

#include "dialogviewer.h"
#include "ui_dialogviewer.h"
#include "viewport/viewerraw.h"
#include "viewport/viewer.h"
#include "viewport/scene.h"
#include "engine/procedural.h"
#include "engine/piorecord.h"

DialogViewer::DialogViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogViewer)
{
    QAction*  my_act;
    scene = new Scene();
    viewer = 0;
    is_test = false;
    //viewer = new ViewerRaw(LOCK_VIEW_ON_Y, this);
    snapshot_viewer = new Viewer(scene, CONSTRAINTS_FREE_VIEW, this);
    snapshot_viewer->setVisible(false);

    ui->setupUi(this);

    ui->verticalLayoutButtons->addWidget(snapshot_viewer);
    //connect(ui->pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));
    signal_mapper = new QSignalMapper(this);
    contextual  = new QMenu(this);
    //sub_add_to  = contextual->addMenu("Add To");
    //act_maingrp = sub_add_to->addAction("Main Group");
    act_maingrp = contextual->addAction("Main Group");

    for (int i=0; i<MAX_GROUP_NUM; i++)
    {
        //my_act = sub_add_to->addAction(QString("Group %1").arg(i+1));
        my_act = contextual->addAction(QString("Group %1").arg(i+1));
        my_act->setShortcut(QKeySequence(Qt::Key_0+i+1));
        list_act_grp.push_back(my_act);
        signal_mapper->setMapping(my_act, list_act_grp.size());
        list_act_grp.back()->setShortcutContext(Qt::ApplicationShortcut);
        addAction(list_act_grp.back());
        connect(my_act, SIGNAL(triggered()), signal_mapper, SLOT(map()));
    }
    act_maintree = contextual->addAction(QString("Main tree"));
    //act_maintree->setShortcut(QKeySequence(Qt::Key_0));
    connect(act_maintree, SIGNAL(triggered()), this, SLOT(act_maintree_triggered()));

    act_maingrp->setShortcut(QKeySequence(Qt::Key_0));

    act_view3d = contextual->addAction("View in 3D");

    connect(act_maingrp, SIGNAL(triggered()), this, SLOT(act_maingrp_triggered()));
    connect(act_view3d, SIGNAL(triggered()), this, SLOT(act_view3d_triggered()));
    connect(signal_mapper, SIGNAL(mapped(int)), this, SLOT(generic_act_triggered(int)));
}

DialogViewer::~DialogViewer()
{
    delete viewer;
    delete snapshot_viewer;
    delete signal_mapper;
    delete ui;
}

void DialogViewer::mousePressEvent(QMouseEvent* e)
{
    if ((e->button() == Qt::RightButton) && (e->modifiers() == Qt::NoButton))
    {
        if (viewer->empty_selection()==false)
            contextual->exec(e->globalPos());
    }
    else
    {
        QDialog::mousePressEvent(e);
    }
}

void DialogViewer::keyPressEvent(QKeyEvent * e)
{

    if (e->modifiers()==Qt::KeypadModifier)
    {
        int my_numpad = num_check(e->key());
        if (my_numpad != -1)
        {
            //QMessageBox::warning( this, "Keypad Status", QString("%1 pressed!").arg(num_check(e->key())));
            generic_act_triggered(my_numpad);
        }
        else QDialog::keyPressEvent(e);
    }
    else QDialog::keyPressEvent(e);
}

void DialogViewer::act_maintree_triggered()
{
    viewer->main_tree();
    viewer->updateGL();
}

void DialogViewer::act_maingrp_triggered()
{
    viewer->multimove_to(0);
    viewer->updateGL();
}

void DialogViewer::generic_act_triggered(int _arg)
{
    viewer->multimove_to(_arg);
    viewer->updateGL();
}

void DialogViewer::act_view3d_triggered()
{
    PLSystemParametric* lsys = new PLSystemParametric();
    PModelingConfig* mc;
    std::stringstream ss;
    std::string str;
    std::string color_fn, shape_fn;
    PIORecord io;
    QString fn;
    PImgData imgdt = viewer->select_imgdt(viewer->getHighlightIdx());
    fn = imgdt.getGrammarFilename().c_str();
    QFile file(fn);
    if (file.exists())
    {
        //QFile file("/home/kryptorkid/lab/src/promo/examples/parametric7.txt");
        io.load(file, ss);
        
        str = ss.str();
        lsys->quick_load(str);
        mc = new PModelingConfig(*lsys->getAlphabet());
        scene = new Scene(lsys);
        color_fn = mc->smart_load_color(fn.toStdString());
        shape_fn = mc->smart_load_shape(fn.toStdString());
        if ((!color_fn.empty())||(!shape_fn.empty()))
            scene->setModConfig(mc);

        snapshot_viewer = new Viewer(scene, CONSTRAINTS_FREE_VIEW, 0, 0, 0, false, false);
        snapshot_viewer->setHorizGridDrawn(true);
        //connect(snapshot_viewer, SIGNAL(on_close()), snapshot_viewer, SLOT(snapshot_viewer->hide()));
        //snapshot_viewer->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        snapshot_viewer->updateGL();
        snapshot_viewer->showEntireScene();
        snapshot_viewer->show();
        //delete mc;
    }
    else QMessageBox::warning(this, "Loading Status", "Failed to load file");
}

int DialogViewer::num_check(int _in)
{
    const char numpad[] = {'0','1','2','3','4','5','6','7','8','9'};
    int value = -1;
    for (int i=0; i<10; i++)
    {
        if (_in==numpad[i])
            return i;
    }
    return value;
}

void DialogViewer::on_windowSize_valueChanged(int nb){
    if (viewer)
    {
        viewer->setNbColumn(nb);
        viewer->updateGL();
    }

}

void DialogViewer::updateViewer3d(QString _id_user, QString _date)
{
    if (viewer)
    {
        ui->verticalLayoutViewer->removeWidget(viewer);
        delete viewer;
    }

    viewer = new ViewerRaw(input_filename, _id_user, _date, LOCK_VIEW_ON_Y, this);
    viewer->setContextual(contextual);
    ui->verticalLayoutViewer->addWidget(viewer);
}

void DialogViewer::closeEvent(QCloseEvent * event){

    QDialog::closeEvent(event);
    if(is_test)
        exit(0);
}

void DialogViewer::on_windowSize_editingFinished()
{
    if (viewer)
        viewer->updateGL();
}

#include "ppcaviewer.h"
#include "ui_ppcaviewer.h"
#include "viewport/scene.h"
#include "viewport/viewer.h"

PPcaViewer::PPcaViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PPcaViewer)
{
    ui->setupUi(this);
    pMainScene  = new Scene();
    pMainViewer = new Viewer(pMainScene, CONSTRAINTS_FREE_VIEW, ui->frame);
    pMainViewer->resize(ui->frame->width(), ui->frame->height());
    pMainViewer->showEntireScene();
}

PPcaViewer::~PPcaViewer()
{
    delete ui;
}

void PPcaViewer::update_pt3d(std::vector< std::vector<float> >& _src)
{
    float scene_radius;
    if (_src.empty()) return;
    scene_radius = (float)fabs((float)_src.front().front());
    for (std::vector< std::vector<float> >::iterator iter1 = _src.begin(); iter1!=_src.end(); iter1++)
    {
        for (std::vector<float>::iterator iter2 = (*iter1).begin();  iter2!=(*iter1).end(); iter2++)
        {
            scene_radius = PROCEDURAL_MAX(scene_radius, (float)fabs(*iter2));
        }
    }
    pMainViewer->setSceneRadius(scene_radius);
    pMainScene->add_vertices(_src);
    pMainViewer->updateGL();
}
void PPcaViewer::set_pointSize(){
    pMainViewer->set_pointSize();
}

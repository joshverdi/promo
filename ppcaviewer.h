#ifndef PPCAVIEWER_H
#define PPCAVIEWER_H

#include <QDialog>

class Viewer;
class Scene;

namespace Ui {
class PPcaViewer;
}

class PPcaViewer : public QDialog
{
    Q_OBJECT
    Viewer* pMainViewer;
    Scene*  pMainScene;
public:
    explicit PPcaViewer(QWidget *parent = 0);
    ~PPcaViewer();
    Scene* getScene() const {return pMainScene;}
    Viewer* getViewer() const {return pMainViewer;}
    void set_pointSize();
    void update_pt3d(std::vector< std::vector<float> >& _src);
    
private:
    Ui::PPcaViewer *ui;
};

#endif // PPCAVIEWER_H

#ifndef PIMGBROWSER_H
#define PIMGBROWSER_H

#include <QDialog>
#include <QLabel>
#include <QResizeEvent>
#include <QMouseEvent>
#include "viewport/viewer.h"
#include "grammar/ppreproc.h"

class PLSystemParametric;

namespace Ui {
class PImgBrowser;
}

class PImgBrowser : public QDialog
{
    Q_OBJECT
    int lbl_width;
    int lbl_height;
    int current_img_index;
    PLSystemParametric* lsystem;
    Viewer* viewer;
    Scene*  scene;
    QLabel* imgLbl[27];
    QStringList img_file_list;
    QImage mainImg;
    std::vector<PPreproc> vec_preproc;
    std::vector<unsigned int> vec_modif_rules_idx;
public:
    void setLSystem(PLSystemParametric* _lsys){lsystem = _lsys;}
    PLSystemParametric* getLSystem(void) const {return lsystem;}
    void setVecPreproc(std::vector<PPreproc>& _vec_preproc){vec_preproc = _vec_preproc;}
    void setVecModifRulesIdx(std::vector<unsigned int>& _vec_modif_rules_idx){ vec_modif_rules_idx = _vec_modif_rules_idx;}
    //void update_lsystem(PLSystemParametric* _lsys){scene->setLSystem(_lsys);}
public:
    explicit PImgBrowser(QWidget *parent = 0);
    ~PImgBrowser();
    void init_file_list();
    void snaphot_resize(int _width, int _height);
protected:
    void resizeEvent(QResizeEvent* _re);
    //void mousePressEvent(QMouseEvent* _me);
    bool eventFilter(QObject *_o, QEvent *_e);

private slots:
    void on_pushButtonLoadSnapshot_clicked();

private:
    Ui::PImgBrowser *ui;
};

#endif // PIMGBROWSER_H

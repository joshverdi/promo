#ifndef DIALOGQLFR_H
#define DIALOGQLFR_H

#include <QDialog>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QVector>
#include <QString>
#include <vector>
#include "viewer.h"

namespace Ui {
class DialogQlfr;
}

class DialogQlfr : public QDialog
{
    Q_OBJECT
    QVector<QCheckBox*> qvec_checkboxes;
    QVector<QPalette> qvec_palette;
    QVBoxLayout* vbox;
    Viewer* viewer;
    QStringList qsl_qlfr;
public:
    explicit DialogQlfr(QWidget *parent = 0);
    ~DialogQlfr();
    void setviewer(Viewer* _viewer){viewer = _viewer;}
    void setqslqlfr(QStringList _qsl_qlfr){qsl_qlfr = _qsl_qlfr;}
    void add_checkbox(QString _caption, float _r, float _g, float _b);
    void add_checkbox(const QStringList& _captionlist, const std::vector< std::vector<float> >& _vec_rgb);
    std::vector<bool> cb_states();
private:
    Ui::DialogQlfr *ui;
protected:
    bool eventFilter(QObject *_o, QEvent *_e);
};

#endif // DIALOGQLFR_H

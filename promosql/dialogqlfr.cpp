#include "dialogqlfr.h"
#include "ui_dialogqlfr.h"

DialogQlfr::DialogQlfr(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogQlfr)
{
    ui->setupUi(this);
    vbox = new QVBoxLayout;
    ui->groupBox->setLayout(vbox);

    QLabel* qlbl = new QLabel(" ");
    qlbl->setMinimumHeight(2);
    qlbl->setMaximumHeight(3);
    ui->verticalLayoutIndicator->addWidget(qlbl);
    ui->groupBox->installEventFilter(this);
}

DialogQlfr::~DialogQlfr()
{
    delete vbox;
    delete ui;
}

void DialogQlfr::add_checkbox(QString _caption, float _r, float _g, float _b)
{
    QLabel* qlbl = new QLabel("");
    QString qstr_stylesheet("QLabel {background-color : rgb(");
    int red     = 255*_r;
    int green   = 255*_g;
    int blue    = 255*_b;
    qstr_stylesheet += QString::number(red);    qstr_stylesheet += ",";
    qstr_stylesheet += QString::number(green);  qstr_stylesheet += ",";
    qstr_stylesheet += QString::number(blue);   qstr_stylesheet += ");}";

    qlbl->setMinimumHeight(8);
    qlbl->setMaximumHeight(10);
    qlbl->setStyleSheet(qstr_stylesheet);
    ui->verticalLayoutIndicator->addWidget(qlbl);
    for (QVector<QCheckBox*>::iterator iter = qvec_checkboxes.begin(); iter != qvec_checkboxes.end(); iter++)
    {
        if (QString::compare( _caption, (*iter)->text(), Qt::CaseInsensitive)==0)
            return;
    }


    QCheckBox *cb = new QCheckBox(_caption);
    cb->setChecked(true);
    cb->installEventFilter(this);
    vbox->addWidget(cb);

    qvec_checkboxes.push_back(cb);
}

void DialogQlfr::add_checkbox(const QStringList& _captionlist, const std::vector< std::vector<float> >& _vec_rgb)
{
    QStringListIterator qsli(_captionlist);
    QString qlfr_tmp;
    int i = 0;
    while (qsli.hasNext() && i < _vec_rgb.size())
    {
        add_checkbox(qsli.next(), _vec_rgb[i][0], _vec_rgb[i][1], _vec_rgb[i][2]);
        ++i;
    }
}

std::vector<bool> DialogQlfr::cb_states()
{
    std::vector<bool> vec_value;
    for (QVector<QCheckBox*>::iterator iter_cb = qvec_checkboxes.begin(); iter_cb != qvec_checkboxes.end(); iter_cb++)
    {
        vec_value.push_back((*iter_cb)->isChecked());
    }
    return vec_value;
}

bool DialogQlfr::eventFilter(QObject *_o, QEvent *_e)
{
    for (QVector<QCheckBox*>::iterator iter_cb = qvec_checkboxes.begin(); iter_cb != qvec_checkboxes.end(); iter_cb++)
    {
        if ((*iter_cb) == _o && _e->type() == QEvent::MouseButtonPress)
        {
            (*iter_cb)->setChecked(!(*iter_cb)->isChecked());
            std::vector<bool> vec_obbdisplayed = cb_states();
            viewer->update_obbdisplay(vec_obbdisplayed);
            viewer->updateGL();
            return true;
        }
    }
    return QWidget::eventFilter(_o, _e);
}


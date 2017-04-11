#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H

#include <QDialog>
//#include "viewport/scene.h"

namespace Ui {
    class OutputDialog;
}

class OutputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OutputDialog(QWidget *parent = 0);
    ~OutputDialog();
    void setCurrDerivText(QString _curr_deriv);
    void setCurrDerivHtmlText(QString _curr_html_deriv);
    void setMatrixModelviewText(QString _qs_matrix_modelview){matrix_modelview_text = _qs_matrix_modelview;}
    void setMatrixModelText(QString _qs_matrix_model){matrix_model_text=_qs_matrix_model;}
    void setMatrixViewText(QString _qs_matrix_view){matrix_view_text = _qs_matrix_view;}
    void setLeftText(QString _left_text){left_text = _left_text;}
    void setHeadText(QString _head_text){head_text = _head_text;}
    void setUpText(QString _up_text){up_text = _up_text;}
    QString settle_output();
private:
    Ui::OutputDialog *ui;
    QString curr_deriv_text;
    QString matrix_modelview_text;
    QString matrix_model_text;
    QString matrix_view_text;
    QString left_text;
    QString head_text;
    QString up_text;
    QString raw_output_text;

private slots:
    void on_textEditCurrDevOutput_cursorPositionChanged();
};

#endif // OUTPUTDIALOG_H

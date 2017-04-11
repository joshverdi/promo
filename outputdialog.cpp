#include "outputdialog.h"
#include "ui_outputdialog.h"

OutputDialog::OutputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);
    connect(ui->pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));
}

void OutputDialog::setCurrDerivText(QString _curr_deriv)
{
    curr_deriv_text = _curr_deriv;
    ui->textEditCurrDevOutput->setText(_curr_deriv);
}

void OutputDialog::setCurrDerivHtmlText(QString _curr_html_deriv)
{
    curr_deriv_text = _curr_html_deriv;
    ui->textEditCurrDevOutput->setHtml(_curr_html_deriv);
}

OutputDialog::~OutputDialog()
{
    delete ui;
}

QString OutputDialog::settle_output()
{
    QString raw_output_text_tmp;
    raw_output_text_tmp+="[GL_MODELVIEW]\n";
    raw_output_text_tmp+=matrix_modelview_text;
    raw_output_text_tmp+="\n\n[GL_MODEL]\n";
    raw_output_text_tmp+=matrix_model_text;
    raw_output_text_tmp+="\n\n[GL_VIEW]\n";
    raw_output_text_tmp+=matrix_view_text;
    raw_output_text_tmp+="\n\n[L]\n";
    raw_output_text_tmp+=left_text;
    raw_output_text_tmp+="\n\n[H]\n";
    raw_output_text_tmp+=head_text;
    raw_output_text_tmp+="\n\n[U]\n";
    raw_output_text_tmp+=up_text;
    if (raw_output_text_tmp!=raw_output_text)
    {
        ui->textBrowserTurtle->setText(raw_output_text_tmp);
        raw_output_text = raw_output_text_tmp;
    }
    return raw_output_text;
}

void OutputDialog::on_textEditCurrDevOutput_cursorPositionChanged()
{
    QString buffer;
    buffer.setNum(ui->textEditCurrDevOutput->textCursor().anchor());
    this->ui->lineEditCursorPos->setText(buffer);
}

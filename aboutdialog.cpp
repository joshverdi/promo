#include <QPixmap>

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);
    connect(ui->pushButtonOk, SIGNAL(clicked()), this, SLOT(close()));

    QPixmap pixmap;
    if (!pixmap.load( ":/images/ets_logo.png" ))
    {
        qWarning("Failed to load /images/ets_logo.png");
    }
    this->ui->labelLogo->setPixmap( pixmap );

    ui->labelLogo->setScaledContents(true);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}


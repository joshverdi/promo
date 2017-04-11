#include <QMessageBox>

#include "mainwindow.h"
#include "treeviewdialog.h"
#include "ui_treeviewdialog.h"
#include "historicitem.h"
#include "historictreemodel.h"
#include "viewport/viewer.h"

TreeViewDialog::TreeViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TreeViewDialog)
{
    QStringList qsl_pv;
    qsl_pv<<tr("Property")<<tr("Value");

    // The lines below are about the treeview
    QStringList qsl_db;
    qsl_db<<tr("Derivation Background");

    htm = new HistoricTreeModel;
    htm->setHorizontalHeaderLabels(qsl_db);

    ui->setupUi(this);
    ui->treeView->setModel(htm);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

TreeViewDialog::~TreeViewDialog()
{
    delete htm;
    delete ui;
}

void TreeViewDialog::on_treeView_clicked(const QModelIndex &index)
{
    MainWindow* mw = dynamic_cast<MainWindow*>(this->parent());
    HistoricItem* hi_clicked;
    QStandardItemModel* table_model;
    unsigned int idx_lstring;
    hi_clicked = htm->select(index);
    idx_lstring = hi_clicked->getPdh()->getIdxIntoDeriv();
    try
    {
        if (mw)
        {
            mw->getScene()->clear_list_selection();
            mw->getScene()->addIdToSelection(idx_lstring);
//            mw->getScene()->setSelection(idx_lstring);
            mw->getScene()->setSelectionChanged(true);
            mw->getViewer()->drawWithNames();
        }
    }
    catch(...)
    {
        qDebug() << "FAILED TO RETRIEVE MAINWINDOW ID!";
    }

    table_model = htm->setup_table(hi_clicked);
    table_model->setHeaderData(0, Qt::Horizontal, tr("Property"));
    table_model->setHeaderData(1, Qt::Horizontal, tr("Value"));
    /*  QSize treeview_size;
        treeview_size = ui->treeView->size();
        treeview_size.setWidth(2*treeview_size.width());
        ui->treeView->resize(treeview_size);
        ui->treeView->resizeColumnToContents(index.column());   */
    ui->treeView->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeView->header()->setStretchLastSection(false);
    ui->tableView->setModel(table_model);
    ui->tableView->show();
}

void TreeViewDialog::setCurrentIndex(const QModelIndex& _index)
{
    ui->treeView->setCurrentIndex(_index);
}

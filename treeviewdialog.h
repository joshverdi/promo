#ifndef TREEVIEWDIALOG_H
#define TREEVIEWDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QHeaderView>

//#include "engine/plsystemparametric.h"
//#include "engine/pderivhistoric.h"
//#include "historicitem.h"
//#include "historictreemodel.h"
class HistoricItem;
class HistoricTreeModel;

// See map in STL
namespace Ui {
    class TreeViewDialog;
}

class TreeViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TreeViewDialog(QWidget *parent = 0);
    ~TreeViewDialog();
private:
    Ui::TreeViewDialog* ui;
    HistoricItem*       hi;
    HistoricTreeModel*  htm;

public:
    HistoricTreeModel* getHistoricTreeModel() const {return htm;}
    void setCurrentIndex(const QModelIndex& _index);

private slots:
    void on_treeView_clicked(const QModelIndex &index);
};

#endif // TREEVIEWDIALOG_H

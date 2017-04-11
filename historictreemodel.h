#ifndef HISTORICTREEMODEL_H
#define HISTORICTREEMODEL_H

#include <QStandardItemModel>
#include "engine/plsystemparametric.h"
#include "engine/pderivhistoric.h"
#include "historicitem.h"

class HistoricTreeModel : public QStandardItemModel
{
public:
    explicit HistoricTreeModel();
    void generate_tree(PLSystemParametric* _lsp);
    void add_children(PDerivHistoric* _dh_root, HistoricItem* _hi_root);
    QStandardItemModel* setup_table(HistoricItem* _hi);
    HistoricItem* select(QModelIndex _index);
    HistoricItem* select(PDerivHistoric* _dh);

signals:
    
public slots:
    
};

#endif // HISTORICTREEMODEL_H

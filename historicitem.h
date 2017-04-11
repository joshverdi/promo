#ifndef HISTORICITEM_H
#define HISTORICITEM_H

#include <QStandardItem>

class PDerivHistoric;

class HistoricItem : public QStandardItem
{
    PDerivHistoric* pdh;
public:
    explicit HistoricItem(PDerivHistoric* _pdh);
    PDerivHistoric* getPdh() const{return pdh;}
    int hierarchy_count();

signals:
    
public slots:
    
};

#endif // HISTORICITEM_H

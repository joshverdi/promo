#include "historicitem.h"
#include "engine/pderivhistoric.h"

HistoricItem::HistoricItem(PDerivHistoric* _pdh) :
    QStandardItem(_pdh->getWord()->compute_str().c_str())
{
    pdh = _pdh;
}

int HistoricItem::hierarchy_count()
{
    HistoricItem* tmp;
    int value = 0;
    while ((tmp=static_cast<HistoricItem*>(this->parent())) != 0)
        value++;
    return value;
}

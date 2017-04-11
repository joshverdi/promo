#include "historictreemodel.h"
#include "historicitem.h"

HistoricTreeModel::HistoricTreeModel():QStandardItemModel()
{
}

void HistoricTreeModel::generate_tree(PLSystemParametric* _lsp)
{
    // roots are located at the axiom
    // somewhere where the derivation has never begun
    std::vector<PDerivHistoric*>::iterator iter;
    std::vector<PDerivHistoric*> vec_root=_lsp->getVecVecDhV2().front();
    HistoricItem* hi;

    removeRows(0,rowCount());
    for (iter=vec_root.begin(); iter!=vec_root.end(); iter++)
    {
        hi = new HistoricItem(*iter);
        appendRow(hi);
        add_children(*iter, hi);
    }
}

void HistoricTreeModel::add_children(PDerivHistoric *_dh_root, HistoricItem *_hi_root)
{
    PDerivHistoric* dh_child=0;
    HistoricItem* hi_child;

    if (_dh_root)
    {
        for (unsigned int i=0; i<_dh_root->getVecChildren().size(); i++)
        {
            dh_child = _dh_root->getVecChildren().at(i);
            hi_child = new HistoricItem(dh_child);
            _hi_root->appendRow(hi_child);
            add_children(dh_child, hi_child);
        }
    }
}

QStandardItemModel* HistoricTreeModel::setup_table(HistoricItem* _hi)
{
    QStandardItemModel* ret_val;
    QStandardItem *item_prop, *item_val;
    QString qs_tmp;
    std::list<QString> lst_prop, lst_val;
    int row_idx;

    ret_val = new QStandardItemModel();

    lst_prop.push_back("Appearance Derivation Number");
    lst_prop.push_back("Index into the Predecessor String");
    lst_prop.push_back("Index into the Successor String");
    lst_prop.push_back("Production Rule Number");
    lst_prop.push_back("Index into the Generating Rule");
    lst_prop.push_back("Stochastic Parameter");
    lst_prop.push_back("[Deriv N#, Index]");

    qs_tmp.setNum(_hi->getPdh()->getDerivNum());lst_val.push_back(qs_tmp);
    qs_tmp.setNum(_hi->getPdh()->getIdxIntoGen());lst_val.push_back(qs_tmp);
    qs_tmp.setNum(_hi->getPdh()->getIdxIntoDeriv());lst_val.push_back(qs_tmp);
    qs_tmp.setNum(_hi->getPdh()->getRuleGenNum());lst_val.push_back(qs_tmp);
    qs_tmp.setNum(_hi->getPdh()->getIdxWithinRule());lst_val.push_back(qs_tmp);
    qs_tmp.setNum(_hi->getPdh()->getStochasticParam());lst_val.push_back(qs_tmp);

    row_idx=0;
    while((!lst_prop.empty())&&(!lst_val.empty()))
    {
        item_prop = new QStandardItem(lst_prop.front());
        item_val = new QStandardItem(lst_val.front());
        ret_val->setItem(row_idx, 0, item_prop);
        ret_val->setItem(row_idx, 1, item_val);
        lst_prop.pop_front();
        lst_val.pop_front();
        row_idx++;
    }

    return ret_val;
}

HistoricItem* HistoricTreeModel::select(QModelIndex _index)
{
    HistoricItem* value;
    value = static_cast<HistoricItem*>(itemFromIndex(_index));
    return value;
}

HistoricItem* HistoricTreeModel::select(PDerivHistoric* _dh)
{
    HistoricItem* value=0;
    std::string search_str;
    QList<QStandardItem*> lst_search_results;
    QList<QStandardItem*>::iterator iter;
    try
    {
        search_str = _dh->getWord()->compute_str();
        lst_search_results = findItems(search_str.c_str(), Qt::MatchContains|Qt::MatchRecursive);
        //unsigned int result_size_dbg = lst_search_results.size();
        iter = lst_search_results.begin();
        while (iter!=lst_search_results.end())
        {
            value = static_cast<HistoricItem*> (*iter);
            if (value->getPdh()==_dh)
                return value;
            iter++;
        }
    }
    catch(...)
    {
        qDebug() << "FAILED TO FIND A MATCH FOR A DERIVATION HISTORIC!";
    }

    return value;

}

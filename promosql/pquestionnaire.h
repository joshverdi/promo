#ifndef PQUESTIONNAIRE_H
#define PQUESTIONNAIRE_H

#include "pqualifier.h"
#include "pdbsetup.h"
#include "main.h"

#include <QVector>
#include <QString>

class PQuestionnaire
{
    int user_id;
    QString grammar;
    QVector<PQualifier> qvec_qualifiers;
public:
    PQuestionnaire();
    PQuestionnaire(int _user_id, QString _grammar);
    inline int getuserid() const {return user_id;}
    void setuserid(int _user_id){user_id=_user_id;}
    inline QString getgrammar() const {return grammar;}
    void setgrammar(QString _grammar){grammar=_grammar;}
    uint check_usflnss(int _grpid);
    uint check_diff(int _grpid);
    uint check_ambdr(int _grpid);
    QString check_qlfr (int _grpid);
    void add_qualifier(PQualifier _qualifier){qvec_qualifiers.push_back(_qualifier);}
    QVector< QVector<float> > diff_matrix();
public:
    void writedb(QString _tablename=QLFCTN_TBLNAME);
    int generate_pkey(PQualifier& _qualifier);
public:
    void load_table(int _user_id, QString _grammar, const QString _tablename=QLFCTN_TBLNAME);
};

#endif // PQUESTIONNAIRE_H

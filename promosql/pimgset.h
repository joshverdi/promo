#ifndef PIMGSET_H
#define PIMGSET_H

#include <QVector>
#include "putils.h"
#include "main.h"
#include "pdbsetup.h"
#include "pgrouprecord.h"

class PImgSet
{
    int user_id;
    QString grammar;
    QVector<PGroupRecord> qvec_grouprecord;
//public:
//    static const QString TABLENAME;
public:
    PImgSet();
    PImgSet(QTextStream& _textstream);
    inline int getuserid() const {return user_id;}
    inline QString getgrammar() const {return grammar;}
    unsigned int records_size() const {return qvec_grouprecord.size();}
public:
    int id_from_pngfile(const QString& _qstr);
    void writedb(QString _tablename);
    QVector< QVector<int> > group_matrix();
    int generate_pkey(PGroupRecord& _group_record);
    void load_table(int _user_id, QString _grammar, const QString _tablename=IMGGRP_TBLNAME);
public:
    void main_check(); // Check if an image is a representative image
    std::vector<uint> read_groupnb(int _user_id, QString _grammar, QString _tablename=IMGGRP_TBLNAME);
    int check_groupid(int _imgid);
};

#endif // PIMGSET_H

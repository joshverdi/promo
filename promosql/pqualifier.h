#ifndef PQUALIFIER_H
#define PQUALIFIER_H

#include <QString>
#include <QVector>

class PQualifier
{
    int group_id;
    unsigned int usefulness;
    unsigned int difference;
    unsigned int ambassador;
    QString qualifier;
public:
    PQualifier();
    PQualifier( int _group_id,
                unsigned int _usefulness,
                unsigned int _diff,
                unsigned int _ambassador,
                QString _qualifier
                );
    PQualifier( int _group_id,
                QString _usefulness,
                QString _diff,
                QString _ambassador,
                QString _qualifier
                );
    int getgroupid() const {return group_id;}
    uint getusefulness() const {return usefulness;}
    uint getdifference() const {return difference;}
    uint getambassador() const {return ambassador;}
    QString getqualifier() const {return qualifier;}
    QString getusefulness_str() const {return str_rate(usefulness);}
    QString getdifference_str() const {return str_rate(difference);}
    QString getambassador_str() const {return str_rate(ambassador);}
    QVector<QString> getvecqualifiers();
public:
    static QString str_rate(unsigned int _value);
    static int n_rate(QString _in);
};

#endif // PQUALIFIER_H

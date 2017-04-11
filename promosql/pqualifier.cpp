#include "pqualifier.h"

#include <QStringList>

PQualifier::PQualifier()
{
    group_id   = 0;
    usefulness = 1;
    difference = 1;
    ambassador = 1;
    qualifier = "nil";
}

PQualifier::PQualifier( int _group_id,
                        unsigned int _usefulness,
                        unsigned int _diff,
                        unsigned int _ambassador,
                        QString _qualifier )
{
    group_id   = _group_id;
    usefulness = _usefulness;
    difference = _diff;
    ambassador = _ambassador;
    qualifier  = _qualifier;
}

PQualifier::PQualifier( int _group_id,
                        QString _usefulness,
                        QString _diff,
                        QString _ambassador,
                        QString _qualifier
                        )
{
    group_id   = _group_id;
    usefulness = n_rate(_usefulness);
    difference = n_rate(_diff);
    ambassador = n_rate(_ambassador);
    qualifier  = _qualifier;
}

QString PQualifier::str_rate(unsigned int _value)
{
    QString value("Undefined");
    switch (_value)
    {
    case 4:
        value = "Excellent";
        break;
    case 3:
        value = "Good";
        break;
    case 2:
        value = "Fair";
        break;
    case 1:
        value = "Bad";
        break;
    }
    return value;
}

int PQualifier::n_rate(QString _in)
{
    if (_in.compare("Excellent", Qt::CaseInsensitive)==0)
        return 4;
    if (_in.compare("Good", Qt::CaseInsensitive)==0)
        return 3;
    if (_in.compare("Fair", Qt::CaseInsensitive)==0)
        return 2;
    if (_in.compare("Bad", Qt::CaseInsensitive)==0)
        return 1;
    //std::cout << "Qualifier " << _in.toStdString() << " not recognized.";
    return 0;
}

QVector<QString> PQualifier::getvecqualifiers()
{
    QVector<QString> value;
    QStringList qsl = qualifier.split(";");
    for (int i=0; i<value.size(); i++)
        value.push_back(qsl.at(i));
    return value;
}

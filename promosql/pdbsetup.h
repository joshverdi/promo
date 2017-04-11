#ifndef PDBSETUP_H
#define PDBSETUP_H

#include <QString>
#include <QSqlDatabase>

class PDbSetup
{
    QSqlDatabase db;
    QString hostname;
    QString dbname;
    QString username;
    QString password;
public:
    PDbSetup();
    PDbSetup(QString _hostname, QString _dbname, QString _username, QString _password);
    PDbSetup(QString _dbname);
    ~PDbSetup();
    void init();
    void set_dbname(std::string _dbname){dbname=QString(_dbname.c_str());}
public:
    QSqlDatabase get_db() const {return db;}
};

#endif // PDBSETUP_H

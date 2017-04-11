#include "pdbsetup.h"
#include "main.h"

#include <iostream>
#include <QSqlError>
#include <QDebug>

PDbSetup::PDbSetup()
{
    hostname = DFLT_HOSTNAME;
    dbname   = DFLT_DBNAME;
    username = DFLT_USERNAME;
    password = DFLT_PASSWORD;
}

PDbSetup::PDbSetup(QString _hostname, QString _dbname, QString _username, QString _password)
{
    hostname = _hostname;
    dbname   = _dbname;
    username = _username;
    password = _password;
}

PDbSetup::PDbSetup(QString _dbname)
{
    hostname = DFLT_HOSTNAME;
    dbname   = _dbname;
    username = DFLT_USERNAME;
    password = DFLT_PASSWORD;
}

PDbSetup::~PDbSetup()
{
    db.close();
}

void PDbSetup::init()
{
    db = QSqlDatabase::addDatabase(DB_TYPE);
    db.setHostName(hostname);
    db.setDatabaseName(dbname);
    db.setUserName(username);
    db.setPassword(password);
    if( !db.open() )
    {
        std::cout << std::endl << db.lastError().text().toStdString();
        qDebug() << db.lastError();
        qFatal( "Failed to connect." );
    }
}

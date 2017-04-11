#include "pquestionnaire.h"
#include "putils.h"

#include <sstream>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>

PQuestionnaire::PQuestionnaire()
    :user_id(0), grammar("")
{
}

PQuestionnaire::PQuestionnaire(int _user_id, QString _grammar)
    :user_id(_user_id), grammar(_grammar)
{
}

void PQuestionnaire::writedb(QString _tablename)
{
    QSqlQuery qry;
    QString qry_str("CREATE TABLE IF NOT EXISTS ");

    qry_str+=_tablename;

    qry_str+=" (";
    qry_str+="pk           INTEGER PRIMARY KEY, ";
    qry_str+="user_id      INTEGER, ";
    qry_str+="grammar      VARCHAR(30), ";
    qry_str+="group_id     INTEGER, ";
    qry_str+="usefulness   VARCHAR(10), ";
    qry_str+="difference   VARCHAR(10), ";
    qry_str+="ambassador   VARCHAR(10), ";
    qry_str+="qualifier    VARCHAR(80) ";
    qry_str+=")";

    if (!qry.exec(qry_str))
    {
        std::cout << std::endl << "Failed to execute : " << qry_str.toStdString();
        std::cout << std::endl << qry.lastError().text().toStdString();
    }

    for (QVector<PQualifier>::iterator iter=qvec_qualifiers.begin(); iter!=qvec_qualifiers.end(); iter++)
    {
        qry_str.clear();
        qry_str = QString("INSERT INTO " + _tablename + " VALUES (:pk, :user_id, :grammar, :group_id, :usefulness, :difference, :ambassador, :qualifier)");
        qry.prepare(qry_str);
        qry.bindValue(":pk",        generate_pkey(*iter));
        qry.bindValue(":user_id",   user_id);
        qry.bindValue(":grammar",   grammar);
        qry.bindValue(":group_id",    (*iter).getgroupid());
        qry.bindValue(":usefulness",  (*iter).getusefulness_str());
        qry.bindValue(":difference",  (*iter).getdifference_str());
        qry.bindValue(":ambassador",  (*iter).getambassador_str());
        qry.bindValue(":qualifier",   (*iter).getqualifier());

        if (!qry.exec())
        {
            std::cout << std::endl << "Group ID " << (*iter).getgroupid() << " Insertion FAILED!";
            std::cout << std::endl << qry.lastError().text().toStdString();
            std::cout << std::endl << "Query String : " << qry_str.toStdString();
        }
    }
}

int PQuestionnaire::generate_pkey(PQualifier& _qualifier)
{
    // See PImgSet::generate_pkey() for further information
    std::stringstream ss;
    int hash = 0;
    for (int i=0; i<grammar.size(); i++)
    {
        hash += grammar.toStdString()[i];
    }
    hash%=100;

    int value=user_id;
    // First part is the user id
    ss << user_id;

    // Second part is the hash
    if (hash<10)
        ss << 0;
    ss << hash;

    // third part is the group number, it is <9
    ss << _qualifier.getgroupid();

    try
    {
        ss >> value;
    }
    catch(...)
    {
        value = 0;
        std::cout << std::endl << "Failed to generate primary key from Qualifier!";
    }
    return value;
}

void PQuestionnaire::load_table(int _user_id, QString _grammar, const QString _tablename)
{
    std::vector< std::vector<std::string> > vecs_datas;
    const QString tmp_filename(_tablename+"_"+_grammar+".tmp");
    QString qry_str(    "SELECT * FROM "
                        + _tablename
                        + " WHERE user_id="
                        + QString::number(_user_id)
                        + " AND grammar=\""
                        + _grammar
                        + "\"");
    std::stringstream ss;
    const int TABLEROW = 8;
    const int GRPIDROW = 3;
    const int USEFROW  = 4;
    const int DIFFROW  = 5;
    const int AMBDROW  = 6;
    const int QLFRROW  = 7;

    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(qry_str);
    PUtils::model_to_csv(tmp_filename.toStdString(), model);
    PUtils::load(tmp_filename, ss);
    PUtils::csv_to_data(vecs_datas, ss);

    if (vecs_datas.size()>1)
    {
        user_id = _user_id;
        grammar = _grammar;
        qvec_qualifiers.clear();
        vecs_datas.erase(vecs_datas.begin());
    }
    for (std::vector< std::vector<std::string> >::iterator iter = vecs_datas.begin(); iter!=vecs_datas.end(); iter++)
    {
        uint group_tmp, usef, diff, ambd;
        QString qs_usef, qs_diff, qs_ambd, qs_qlfr;

        if ((*iter).size()!=(uint)TABLEROW)
            continue;

        try
        {
            std::istringstream((*iter)[GRPIDROW]) >> group_tmp;
            qs_usef=QString((*iter)[USEFROW].c_str());
            qs_diff=QString((*iter)[DIFFROW].c_str());
            qs_ambd=QString((*iter)[AMBDROW].c_str());
            qs_qlfr=QString((*iter)[QLFRROW].c_str());
            qvec_qualifiers.push_back(PQualifier(group_tmp, qs_usef, qs_diff, qs_ambd, qs_qlfr));
        }
        catch(...)
        {
            unsigned int fail = std::distance(vecs_datas.begin(), iter); // (unsigned int)(iter-vecs_datas.begin());
            std::cout << std::endl << "WARNING: Failed to retrieve datas from User ID "
                      << user_id << " with the grammar '"
                      << grammar.toStdString() << "' on line "
                      << fail    << ".";
        }
    }
    QFile::remove(tmp_filename);
}

uint PQuestionnaire::check_usflnss(int _grpid)
{
    for (QVector<PQualifier>::iterator iter=qvec_qualifiers.begin(); iter!=qvec_qualifiers.end(); iter++)
    {
        if ((*iter).getgroupid()==_grpid)
            return (*iter).getusefulness();
    }
    qDebug() << "Requesting usefulness for Group ID " << _grpid << " failed.";
    return 0;
}

uint PQuestionnaire::check_diff(int _grpid)
{
    for (QVector<PQualifier>::iterator iter=qvec_qualifiers.begin(); iter!=qvec_qualifiers.end(); iter++)
    {
        if ((*iter).getgroupid()==_grpid)
            return (*iter).getdifference();
    }
    qDebug() << "Requesting usefulness for Group ID " << _grpid << " failed.";
    return 0;
}

uint PQuestionnaire::check_ambdr(int _grpid)
{
    for (QVector<PQualifier>::iterator iter=qvec_qualifiers.begin(); iter!=qvec_qualifiers.end(); iter++)
    {
        if ((*iter).getgroupid()==_grpid)
            return (*iter).getambassador();
    }
    qDebug() << "Requesting ambassador for Group ID " << _grpid << " failed.";
    return 0;
}

QString PQuestionnaire::check_qlfr(int _grpid)
{
    for (QVector<PQualifier>::iterator iter=qvec_qualifiers.begin(); iter!=qvec_qualifiers.end(); iter++)
    {
        if ((*iter).getgroupid()==_grpid)
            return (*iter).getqualifier();
    }
    qDebug() << "Requesting usefulness for Group ID " << _grpid << " failed.";
    return 0;
}

QVector< QVector<float> > PQuestionnaire::diff_matrix()
{
    QVector< QVector<float> > value (IMAGE_NB, QVector<float>(IMAGE_NB, 0));
    return value;
}

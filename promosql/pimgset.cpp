#include "pimgset.h"
#include "putils.h"
#include "main.h"

#include <iostream>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QTableView>

//const QString PImgSet::TABLENAME(IMGGRP_TBLNAME);

PImgSet::PImgSet()
{
    user_id=0;
    grammar="nofileselected";
}

PImgSet::PImgSet(QTextStream& _textstream)
{
    std::vector<std::string> vec_line, vec_strcell;
    std::stringstream ss;
    std::string line_tmp;
    int num;

    ss << _textstream.readAll().toStdString();
    if (ss.str().empty()==false)
    {
        while (std::getline(ss, line_tmp))
            vec_line.push_back(line_tmp);

        for (std::vector<std::string>::iterator iter = vec_line.begin(); iter!=vec_line.end(); iter++)
        {
            vec_strcell.clear();
            boost::split(vec_strcell, (*iter), boost::is_any_of(","));
            if (!vec_strcell.empty())
            {
                while (vec_strcell.back().empty()==true)
                {
                    vec_strcell.pop_back();
                    if (vec_strcell.empty()==true)
                        break;
                }
                if (vec_strcell.front().compare("Test:")==0)
                {
                    try
                    {
                        grammar = QString(vec_strcell[1].c_str());
                        if (grammar.toStdString().find("_main.csv")!=std::string::npos)
                        {
                            size_t idx_grammar = grammar.length()-std::string("_main.csv").length();
                            grammar = QString(grammar.toStdString().substr(0, idx_grammar).c_str());
                        }
                    }
                    catch (...)
                    {
                        std::cout << std::endl << "Image Group Name Is Empty!";
                    }
                }
                else if (vec_strcell.front().find("User:")==0)
                {
                    try
                    {
                        std::istringstream (vec_strcell.front().substr(std::string("User:").size())) >> user_id;
                    }
                    catch (...)
                    {
                        std::cout << std::endl << "Impossible to retrieve User ID!";
                    }
                }
                else
                {
                    num = id_from_pngfile(QString(vec_strcell.front().c_str()));
                    if (num!=-1)
                    {
                        try
                        {
                            PGroupRecord pgr(num, boost::lexical_cast<uint>(vec_strcell[1]));
                            if (vec_strcell.size()>=2)
                            {
                                if (vec_strcell[2].find("main_tree")!=std::string::npos)
                                    pgr.set_is_main(true);
                            }
                            qvec_grouprecord.push_back(pgr);
                        }
                        catch (...)
                        {
                            std::cout << std::endl << "Group Record Failure!";
                        }
                    }
                }
            }
        }
    }
    main_check();
}

int PImgSet::id_from_pngfile(const QString& _qstr)
{
    int value = -1;
    const std::string suffix[] = {  "_0.png", "_1.png", "_2.png",
                            "_3.png", "_4.png", "_5.png",
                            "_6.png", "_7.png", "_8.png",
                            "_9.png", "_10.png", "_11.png",
                            "_12.png", "_13.png", "_14.png",
                            "_15.png", "_16.png", "_17.png",
                            "_18.png", "_19.png", "_20.png",
                            "_21.png", "_22.png", "_23.png",
                            "_24.png", "_25.png", "_26.png"};
    for (int i=0; i<27; i++)
    {
        if (_qstr.toStdString().find(suffix[i])!=std::string::npos)
        {
            value = i+1;
            break;
        }
    }
    return value;
}

void PImgSet::writedb(QString _tablename)
{
    QSqlQuery qry;
    QString qry_str("CREATE TABLE IF NOT EXISTS ");

    qry_str+=_tablename;
    qry_str+=" (";
    qry_str+="pk        INTEGER PRIMARY KEY, ";
    qry_str+="user_id   INTEGER, ";
    qry_str+="grammar   VARCHAR(30), ";
    qry_str+="img_id    INTEGER, ";
    qry_str+="group_id  INTEGER, ";
    qry_str+="main_pic  INTEGER)";

    if (!qry.exec(qry_str))
    {
        std::cout << std::endl << "Failed to execute : " << qry_str.toStdString();
        std::cout << std::endl << qry.lastError().text().toStdString();
    }

    for (QVector<PGroupRecord>::iterator iter = qvec_grouprecord.begin(); iter!=qvec_grouprecord.end(); iter++)
    {
        qry_str.clear();
        qry_str = QString("INSERT INTO " + _tablename + " VALUES (:pk, :user_id, :grammar, :img_id, :group_id, :main_pic)");
        qry.prepare(qry_str);
        qry.bindValue(":pk",        generate_pkey(*iter));
        qry.bindValue(":user_id",   user_id);
        qry.bindValue(":grammar",   grammar);
        qry.bindValue(":img_id",    (*iter).get_img_id());
        qry.bindValue(":group_id",    (*iter).get_group());
        qry.bindValue(":main_pic",    (*iter).get_is_main()==true?1:0);

        if (!qry.exec())
        {
            std::cout << std::endl << "Img ID " << (*iter).get_img_id() << " Insertion FAILED!";
            std::cout << std::endl << qry.lastError().text().toStdString();
            std::cout << std::endl << "Query String : " << qry_str.toStdString();
        }
    }

}

QVector< QVector<int> > PImgSet::group_matrix()
{
    QVector< QVector<int> > value(IMAGE_NB, QVector<int>(IMAGE_NB, 0));
    for (QVector<PGroupRecord>::const_iterator iter = qvec_grouprecord.begin(); iter!= qvec_grouprecord.end(); iter++)
    {
        for (QVector<PGroupRecord>::const_iterator jter = qvec_grouprecord.begin(); jter!= qvec_grouprecord.end(); jter++)
        {
            if ((*jter).get_group() == (*iter).get_group())
                value[(*iter).get_img_id()-1][(*jter).get_img_id() - 1] = value[(*jter).get_img_id()-1][(*iter).get_img_id() - 1] = 1;
        }
    }
    return value;
}

int PImgSet::generate_pkey(PGroupRecord& _group_record)
{
    // We are working on a 3 digits User ID and 27 images
    // This function should be updated if those formats
    // are subjects to change
    int hash = 0;
    for (int i=0; i<grammar.size(); i++)
    {
        hash += grammar.toStdString()[i];
    }
    hash%=100;

    int value=_group_record.get_img_id();
    std::stringstream ss;
    ss << user_id;
    if (hash<10)
        ss << 0;
    ss << hash;

    if (value<10)
        ss << 0;
    try
    {
        ss << value;
        ss >> value;
    }
    catch(...)
    {
        value = 0;
        std::cout << std::endl << "Failed to generate primary key from Group Record!";
    }

    return value;
}

// Check if a group has a single image, in which case that single image is a representative image by definition
void PImgSet::main_check()
{
    int n;
    std::vector<size_t> vec_mains;
    QVector<PGroupRecord> qvec_tmp;
    for (int i=0; i<qvec_grouprecord.size(); i++)
    {
        n=0;
        for (int j=0; j<qvec_grouprecord.size(); j++)
        {
            if (qvec_grouprecord.at(i).get_group()==qvec_grouprecord.at(j).get_group())
                n++;
        }
        if (n==1)
        {
            vec_mains.push_back(i);
        }
    }
    if (!vec_mains.empty())
    {
        for (int i=0; i<qvec_grouprecord.size(); i++)
        {
            if (std::find(vec_mains.begin(), vec_mains.end(), i)!=vec_mains.end())
            {
                qvec_tmp.push_back(PGroupRecord(qvec_grouprecord.at(i).get_img_id(), qvec_grouprecord.at(i).get_group(), true));
            }
            else qvec_tmp.push_back(qvec_grouprecord.at(i));
        }
        qvec_grouprecord = qvec_tmp;
    }
}

void PImgSet::load_table(int _user_id, QString _grammar, const QString _tablename)
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
    const int TABLEROW = 6;
    const int IMGIDROW = 3;
    const int GRPIDROW = 4;
    const int MNIDROW  = 5;

    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(qry_str);
    PUtils::model_to_csv(tmp_filename.toStdString(), model);
    PUtils::load(tmp_filename, ss);
    PUtils::csv_to_data(vecs_datas, ss);

    if (vecs_datas.size()>1)
    {
        user_id = _user_id;
        grammar = _grammar;
        qvec_grouprecord.clear();
        vecs_datas.erase(vecs_datas.begin());
    }
    for (std::vector< std::vector<std::string> >::iterator iter = vecs_datas.begin(); iter!=vecs_datas.end(); iter++)
    {
        int img_id_tmp;
        unsigned int group_tmp;
        bool is_main_tmp; int main_tmp;

        if ((*iter).size()!=(uint)TABLEROW)
            continue;

        try
        {
            std::istringstream((*iter)[IMGIDROW]) >> img_id_tmp;
            std::istringstream((*iter)[GRPIDROW]) >> group_tmp;
            std::istringstream((*iter)[MNIDROW]) >> main_tmp; is_main_tmp = (main_tmp==1)?true:false;
            qvec_grouprecord.push_back(PGroupRecord(img_id_tmp, group_tmp, is_main_tmp));
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

std::vector<uint> PImgSet::read_groupnb(int _user_id, QString _grammar, QString _tablename)
{
    std::vector<uint> value;
    load_table(_user_id, _grammar, _tablename);
    for (QVector<PGroupRecord>::iterator iter=qvec_grouprecord.begin(); iter!=qvec_grouprecord.end(); iter++)
    {
        if (std::find(value.begin(), value.end(), (*iter).get_group())==value.end())
        {
            value.push_back((*iter).get_group());
        }
    }
    std::sort(value.begin(), value.end());
    return value;
}

int PImgSet::check_groupid(int _imgid)
{
    for (QVector<PGroupRecord>::iterator iter=qvec_grouprecord.begin(); iter!=qvec_grouprecord.end(); iter++)
    {
        if ((*iter).get_img_id() == _imgid)
            return (*iter).get_group();
    }
    return 0;
}

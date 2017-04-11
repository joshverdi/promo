#ifndef PIORECORD_H
#define PIORECORD_H

#include <QFile>
#include <QFileDialog>
#include <QDomDocument>
#include <QDomElement>
#include <QMessageBox>
#include <QString>

#include <vector>
#include <string>
#include <sstream>

#include "grammar/ppreproc.h"
#include "pderivhistoric.h"
#include "pmetric.h"
#include "plsystem.h"
#include "plsystemparametric.h"

class PIORecord
{
    QDomDocument                doc;
    QDomProcessingInstruction   instruction;
    QDomElement                 root;
    QFile                       file;
    QString                     error;
    QString                     filename;
    QByteArray                  content;
    int indentation;
public:
    PIORecord();
public:
    static void save(QFile& _file, std::stringstream& _ss);
    void save_to_xml(QWidget* _widget);
    void save_to_xml(QFile& _file, QString& _filename, QDomDocument& _doc);

    void save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, PMetric& _metric);
    void save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, PMetric& _metric, PPreproc* _preproc);
    void save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, std::vector<PMetric>& _vec_metric, std::vector<PPreproc>& _vec_preproc);
    void save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, std::vector<PMetric>& _vec_metric, std::vector<PPreproc>& _vec_preproc,
                     std::vector<unsigned int>& _vec_modified_rule, std::vector<std::vector<float> >& _vec_change_rate);
    void save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, std::vector<PPreproc>& _vec_preproc,
                     std::vector<unsigned int>& _vec_modified_rule, std::vector<std::vector<float> >& _vec_change_rate);
    void save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, std::vector<std::vector<float> >& _comat, std::vector<std::vector<float> >& _eigenvec, std::vector<float>& _eigenval);
//    void save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, std::vector<PMetric>& _vec_metric, std::vector<PPreproc>& _vec_preproc,
//                     std::vector<unsigned int>& _vec_modified_rule,std::vector<std::vector<float> >& _vec_internalchange_rate, std::vector< std::vector<unsigned int> >& _vecs_rules_count);
    void create_xml_deriv(std::vector<PDerivHistoric*> _vec_dh, QDomDocument& _doc, QDomElement& _root);
    void create_xml_deriv(PLSystemParametric* _lsysp);
    void load(QString& _raw_qstr, PLSystemParametric* _lsysp);
    void load(QWidget* _widget, QString& _filename, QString& _qs_rules, QString& _qs_preproc, QString& _qs_axiom, float& _angle, int& _n_deriv);
    static void load(QFile& _file, std::stringstream& _ss);
    static void csv_to_data(std::vector<std::vector <std::string> >& _datas, std::stringstream& _src);

    template <class T> void generic_save(QWidget *_widget, QFile &_file, QString &_filename, std::vector<std::vector<T> >& _src, std::vector<std::string> _hor_label, std::vector<std::string> _ver_label, std::string _label, std::string _intro)
    {
        std::stringstream ss;
        std::vector<std::string>::iterator iter_str;

        _file.setFileName(_filename);
        if (!_file.open(QFile::WriteOnly|QFile::Text))
        {
            QMessageBox::warning( _widget, "Saving", "Failed to save file." );
            return;
        };
        if (!_intro.empty())
            ss << _intro << std::endl;
        if ((!_label.empty())&&(!_hor_label.empty()))
        {
            ss << _label;
            for (std::vector<std::string>::iterator iter = _hor_label.begin(); iter!=_hor_label.end(); iter++)
                ss << "," << (*iter);
            ss << std::endl;
        }
        if (!_ver_label.empty())
            iter_str = _ver_label.begin();
        typename std::vector< std::vector<T> >::iterator iter1;
        typename std::vector<T>::iterator iter2;
        for (iter1 = _src.begin(); iter1!=_src.end(); iter1++, iter_str++)
        {
            if (!_ver_label.empty())
                if (iter_str!=_ver_label.end())
                    ss << (*iter_str) << ",";
            for (iter2 = (*iter1).begin(); iter2!=(*iter1).end(); iter2++)
            {
                ss << (*iter2);
                if (iter2+1!=(*iter1).end())
                    ss << ",";
            }
            ss << "\n";
        }
        _file.write(QString(ss.str().c_str()).toUtf8());
        _file.close();

    }

    template <class T> static void data_to_csv(std::stringstream& _dest, const QVector< QVector<T> >& _src)
    {
        typename QVector< QVector<T> >::const_iterator iter1;
        typename QVector<T>::const_iterator iter2;
        for (iter1 = _src.begin(); iter1!=_src.end(); iter1++)
        {
            for (iter2 = (*iter1).begin(); iter2!=(*iter1).end(); iter2++)
            {
                _dest << (*iter2);
                if (iter2+1!=(*iter1).end())
                    _dest << ",";
            }
            _dest << "\n";
        }
        //std::cout << std::endl << "Textstream Content Debug: " << _dest.readAll().toStdString();
    }
};

#endif // PIORECORD_H

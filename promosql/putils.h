#ifndef PUTILS_H
#define PUTILS_H

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QSqlQueryModel>
#include <QVector>
#include <sstream>
#include <iostream>

class PUtils
{
public:
    PUtils();
    static void model_to_csv(std::string _outputfilename, QSqlQueryModel* _model);
    static std::vector<std::string> csvline_to_vector(std::string& _strline);
    static void save(QString _filename, std::stringstream& _ss);
    static void load(QString _filename, std::stringstream& _ss);

    static void hsv_to_rgb( float *r, float *g, float *b, float h, float s, float v );
    static void generate_color(float _ratio_criterion, float& _red, float& _green, float& _blue);
    static float stdstr_to_float(std::string& _src);
    static std::string float_to_stdstr(float _val);

    template <class T> static void put_numheaders(std::stringstream& _ss, QVector< QVector<T> > _datas)
    {
        try
        {
            int rows = _datas.size();
            int cols = _datas.back().size();
            _ss.clear();
            for (int i=0; i<=cols; i++)
            {
                if (i>0)
                    _ss << ",";
                _ss << i;
            }
            _ss << "\n";
            for (int i=0; i<rows; i++)
            {
                _ss << i+1 << ",";
                for (int j=0; j<cols; j++)
                {
                    if (j>0)
                        _ss << ",";
                    _ss << _datas[i][j];
                }
                _ss << "\n";
            }
        }
        catch(...)
        {
            std::cout << std::endl << "Matrix was not uniformed to be generically entitled.";
        }
    }

    template <class T, class U> static void put_numheaders(std::stringstream& _ss, QVector< QVector<T> > _datas, QVector<U> _headers)
    {
        typedef typename QVector<U>::iterator QVecIter;
        QVecIter iter;
        try
        {
            int rows = _datas.size();
            int cols = _datas.back().size();
            _ss.clear();
            _ss << "0";
            for (iter = _headers.begin(); iter!=_headers.end(); iter++)
            {
                _ss << ",";
                _ss << (*iter);
            }
            _ss << "\n";
            iter = _headers.begin();
            for (int i=0; i<rows; i++, iter++)
            {
                _ss << (*iter) << ",";
                for (int j=0; j<cols; j++)
                {
                    if (j>0)
                        _ss << ",";
                    _ss << _datas[i][j];
                }
                _ss << "\n";
                if (iter==_headers.end())
                    iter = _headers.begin();
            }
        }
        catch(...)
        {
            std::cout << std::endl << "Matrix was not uniformed to be generically entitled.";
        }
    }

    template <class T> static QVector< QVector<T> > matrix_add(QVector< QVector<T> > _m1, QVector< QVector<T> > _m2)
    {
        QVector< QVector<T> > value(_m1.size(), QVector<T>(_m1.front().size(), 0));
        try
        {
            for (int i=0; i<_m1.size(); i++)
                for (int j=0; j<_m1.front().size(); j++)
                    value[i][j] = _m1[i][j]+_m2[i][j];
        }
        catch(...)
        {
            std::cout << std::endl << "Matrix calculation error! Please check if the sizes of your matrices are corrects.";
        }

        return value;
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

    template <class T> static void data_to_csv(std::stringstream& _dest, const std::vector< std::vector<T> >& _src)
    {
        typename std::vector< std::vector<T> >::const_iterator iter1;
        typename std::vector<T>::const_iterator iter2;
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

    static void csv_to_data(std::vector<std::vector <std::string> >& _datas, std::stringstream& _src);
    static QVector<int> load_vector(QString _vectorfn)
    {
        QVector<int> value;
        std::stringstream ss;
        std::vector<std::vector <std::string> > vecs_datas;
        PUtils::load(_vectorfn, ss);
        PUtils::csv_to_data(vecs_datas, ss);
        if (vecs_datas.size()==1)
        {
            for (std::vector<std::string>::iterator iter=vecs_datas.front().begin(); iter!=vecs_datas.front().end(); iter++)
            {
                value.push_back(QString((*iter).c_str()).toFloat());
            }
        }
        return value;
    }

    static std::vector< float > mean(const std::vector< std::vector<float> >& _vec_clouds);
};

#endif // PUTILS_H

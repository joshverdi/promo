#include "putils.h"
#include <QSqlRecord>
#include <QSqlField>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

PUtils::PUtils()
{
}

//PUtils::PUtils(QString& _filename, QTextStream& _ts)
//{
//    QFile my_file(_filename);
//    if (!my_file.open(QFile::ReadOnly|QFile::Text))
//    {
//        std::cout << std::endl << _filename.toStdString() << " not found!";
//    }
//    else
//    {
//        _ts << QTextStream(&my_file).readAll();
//    }
//}

std::vector<std::string> PUtils::csvline_to_vector(std::string& _strline)
{
    std::vector<std::string> value;
    boost::split(value, _strline, boost::is_any_of(","));
    return value;
}

void PUtils::save(QString _filename, std::stringstream& _ss)
{
    QFile file(_filename);
    if (!file.open(QFile::WriteOnly|QFile::Text))
    {
        std::cout << std::endl << "Unable To Save: " << _filename.toStdString();
        return;
    }
    file.write(_ss.str().c_str());
    file.close();
}

void PUtils::load(QString _filename, std::stringstream& _ss)
{
    QFile file(_filename);
    if (!file.open(QFile::ReadOnly|QFile::Text))
    {
        std::cout << std::endl << "Unable To load: " << _filename.toStdString();
        return;
    }
    _ss << QTextStream(&file).readAll().toStdString();
    file.close();
}

void PUtils::model_to_csv(std::string _outputfilename, QSqlQueryModel* _model)
{
    QFile file(_outputfilename.c_str());
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream ts(&file);
        for (int i=0; i<_model->columnCount(); i++)
        {
            if (i>0)
                ts << ",";
            ts << _model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        }
        ts << "\n";
        while (_model->canFetchMore())
             _model->fetchMore();
        //std::cout << std::endl << _model->rowCount() << " rows from model";
        for (int i=0; i<_model->rowCount(); i++)
        {
            QSqlRecord record = _model->record(i);
            for (int j=0; j<record.count(); j++)
            {
                if (j>0)
                    ts << ",";
                ts << record.field(j).value().toString();
            }
            ts << "\n";
        }
        file.close();
    }
}

void PUtils::csv_to_data(std::vector<std::vector <std::string> >& _datas, std::stringstream& _src)
{
    std::vector<std::string> vec_line, vec_strcell;
    std::string line_tmp;
    while (std::getline(_src, line_tmp))
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
            _datas.push_back(vec_strcell);
        }
    }
    //std::cout << "Debug Info: Collected " << _datas.size() << " lines.";
}

void PUtils::generate_color(float _ratio_criterion, float& _red, float& _green, float& _blue)
{
    float r, g, b;
    float h, s, v;
    s = v = 1;
    h = 240*(1-_ratio_criterion);
    PUtils::hsv_to_rgb(&r, &g, &b, h, s, v);
    _red=r; _green=g; _blue=b;
}

void PUtils::hsv_to_rgb( float *r, float *g, float *b, float h, float s, float v )
{
    int i;
    float f, p, q, t;
    if( s == 0 ) {
        // achromatic (grey)
        *r = *g = *b = v;
        return;
    }
    h /= 60;			// sector 0 to 5
    i = floor( h );
    f = h - i;			// factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );
    switch( i ) {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:		// case 5:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
}

float PUtils::stdstr_to_float(std::string& _src)
{
    float value;
    try
    {
        value = boost::lexical_cast<float>(_src);
    }
    catch(boost::bad_lexical_cast const&)
    {
        std::cout<<std::endl<<"Boost Lexical Cast Exception";
        return 0;
    }
    return value;
}

std::string PUtils::float_to_stdstr(float _val)
{
    std::ostringstream buffer_stream;
    buffer_stream<<_val;
    return buffer_stream.str();
}

std::vector< float > PUtils::mean(const std::vector< std::vector<float> >& _vec_clouds)
{
    std::vector< float > value;
    size_t s;
    if (!_vec_clouds.empty())
    {
        s = _vec_clouds.front().size();

        if (s!= 0)
        {
            for (int i=0; i < s; i++)
                value.push_back(0);
            for (std::vector< std::vector<float> >::const_iterator iter = _vec_clouds.begin(); iter != _vec_clouds.end(); iter++)
            {
                if ((*iter).size() == s)
                    for (int i=0; i < s; i++)
                    {
                        value[i] += (*iter)[i];
                    }
            }

            for (int i=0; i < s; i++)
                value[i] /= _vec_clouds.size();
        }
    }
    return value;
}

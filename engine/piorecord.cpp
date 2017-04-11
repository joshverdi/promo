#include <vector>
#include <string>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <QMessageBox>

#include "piorecord.h"
#include "ptextparser.h"
#include "engine/pmetric.h"

using namespace std;

PIORecord::PIORecord()
{
    doc = QDomDocument(QObject::tr("modeling"));
    indentation = 4;
    instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"ISO-8859-1\"");
    filename = QString("promo.xml");
    root = doc.createElement("promo");
    doc.appendChild(root);
}

void PIORecord::save_to_xml(QWidget* _widget)
{
    filename = QFileDialog::getSaveFileName(_widget, QObject::tr("Promo Output File Name"), QObject::tr(""), QObject::tr("XML Files(*.xml);;All Files(*)"));
    file.setFileName(filename);

    if (!file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning( _widget, "Saving", "Failed to save file." );
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    doc.insertBefore(instruction, doc.firstChild());
    doc.save(out, indentation);
    QString doc_debug = doc.toString();
    QString* debug_str = out.string();
    if (debug_str) debug_str->clear();
    file.close();
}

void PIORecord::save_to_xml(QFile& _file, QString& _filename, QDomDocument& _doc)
{
    _file.setFileName(_filename);
    QTextStream out(&_file);
    _doc.save(out, indentation);
}

void PIORecord::save_to_csv(QWidget* _widget, QFile& _file, QString& _filename, PMetric& _metric)
{
    PTextParser parser;
    _file.setFileName(_filename);
    if (!_file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning( _widget, "Saving", "Failed to save file." );
        return;
    }
    std::stringstream ss;
    float ov = (float) _metric.getObbVol();
    float os = (float) _metric.getObbSurf();
    float bs = (float) _metric.getBrchsSurf();
    float bv = (float) _metric.getBrchsVol();
    float bn = (float) _metric.getBrchsNb();
    float bd = (float) _metric.getBrchDensity();
    float sr = (float) _metric.getSlctnRate();
    float fr = (float) _metric.getFillRate();
    float bl = (float) _metric.getBrchsLength();
    float al = (float) _metric.getAvrgLength();
    float ar = (float) _metric.getBrchsAveray();

    string str_ov(parser.float_to_stdstr(ov));
    string str_os(parser.float_to_stdstr(os));
    string str_bs(parser.float_to_stdstr(bs));
    string str_bv(parser.float_to_stdstr(bv));
    string str_bn(parser.float_to_stdstr(bn));
    string str_bd(parser.float_to_stdstr(bd));
    string str_sr(parser.float_to_stdstr(sr));
    string str_fr(parser.float_to_stdstr(fr));
    string str_bl(parser.float_to_stdstr(bl));
    string str_al(parser.float_to_stdstr(al));
    string str_ar(parser.float_to_stdstr(ar));

    string str_ovd("OBB Volume");
    string str_osd("OBB Surface");
    string str_bsd("Branches Surface");
    string str_bvd("Branches Volume");
    string str_bnd("Branches Number");
    string str_bdd("Branches per Vol Unit");
    string str_srd("Selection Rate");
    string str_frd("Box Filling Rate");
    string str_bld("Branches Length");
    string str_ald("Average Length");
    string str_ard("Average Ray");

    ss << "Description" << "," << "value" << endl;
    ss << str_ovd       << "," << str_ov  << endl;
    ss << str_osd       << "," << str_os  << endl;
    ss << str_bsd       << "," << str_bs  << endl;
    ss << str_bvd       << "," << str_bv  << endl;
    ss << str_bnd       << "," << str_bn  << endl;
    ss << str_bdd       << "," << str_bd  << endl;
    ss << str_srd       << "," << str_sr  << endl;
    ss << str_frd       << "," << str_fr  << endl;
    ss << str_bld       << "," << str_bl  << endl;
    ss << str_ald       << "," << str_al  << endl;
    ss << str_ard       << "," << str_ar  << endl;

    _file.write(QString(ss.str().c_str()).toUtf8());
    _file.close();
}

void PIORecord::save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, PMetric& _metric, PPreproc* _preproc)
{
    PTextParser parser;
    std::stringstream ss;

    _file.setFileName(_filename);
    if (!_file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning( _widget, "Saving", "Failed to save file." );
        return;
    }
    if ((!_preproc)||(_preproc->getVecDefine().empty()))
        return;

    float ov = (float) _metric.getObbVol();
    float os = (float) _metric.getObbSurf();
    float bs = (float) _metric.getBrchsSurf();
    float bv = (float) _metric.getBrchsVol();
    float bn = (float) _metric.getBrchsNb();
    float bd = (float) _metric.getBrchDensity();
    float sr = (float) _metric.getSlctnRate();
    float fr = (float) _metric.getFillRate();
    float bl = (float) _metric.getBrchsLength();
    float al = (float) _metric.getAvrgLength();
    float ar = (float) _metric.getBrchsAveray();

    string str_ov(parser.float_to_stdstr(ov));
    string str_os(parser.float_to_stdstr(os));
    string str_bs(parser.float_to_stdstr(bs));
    string str_bv(parser.float_to_stdstr(bv));
    string str_bn(parser.float_to_stdstr(bn));
    string str_bd(parser.float_to_stdstr(bd));
    string str_sr(parser.float_to_stdstr(sr));
    string str_fr(parser.float_to_stdstr(fr));
    string str_bl(parser.float_to_stdstr(bl));
    string str_al(parser.float_to_stdstr(al));
    string str_ar(parser.float_to_stdstr(ar));

    string str_ovd("OBB Volume");
    string str_osd("OBB Surface");
    string str_bsd("Branches Surface");
    string str_bvd("Branches Volume");
    string str_bnd("Branches Number");
    string str_bdd("Branches per Vol Unit");
    string str_srd("Selection Rate");
    string str_frd("Box Filling Rate");
    string str_bld("Branches Length");
    string str_ald("Average Length");
    string str_ard("Average Ray");

    for (unsigned int i=0; i<_preproc->getVecDefine().size(); i++)
    {
        ss << _preproc->getVecDefine().at(i)->getConstantName();
        ss << ",";
    }

    ss << str_ovd << "," ;
    ss << str_osd << "," ;
    ss << str_bsd << "," ;
    ss << str_bvd << "," ;
    ss << str_bnd << "," ;
    ss << str_bdd << "," ;
    ss << str_srd << "," ;
    ss << str_frd << "," ;
    ss << str_bld << "," ;
    ss << str_ald << "," ;
    ss << str_ard << endl;

    for (unsigned int i=0; i<_preproc->getVecDefine().size(); i++)
    {
        ss << _preproc->getVecDefine().at(i)->getConstantValue();
        ss << ",";
    }

    ss << str_ov << "," ;
    ss << str_os << "," ;
    ss << str_bs << "," ;
    ss << str_bv << "," ;
    ss << str_bn << "," ;
    ss << str_bd << "," ;
    ss << str_sr << "," ;
    ss << str_fr << "," ;
    ss << str_bl << "," ;
    ss << str_al << "," ;
    ss << str_ar << endl;

    _file.write(QString(ss.str().c_str()).toUtf8());
    _file.close();
}

void PIORecord::save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, std::vector<PMetric>& _vec_metric, std::vector<PPreproc>& _vec_preproc)
{
    PTextParser parser;
    unsigned int size = _vec_metric.size();
    std::stringstream ss;
    float ov, os, bs, bv, bn, bd, sr, fr, bl, al, ar;

    _file.setFileName(_filename);
    if (!_file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning( _widget, "Saving", "Failed to save file." );
        return;
    }
    if ((_vec_metric.empty())||(_vec_preproc.empty()))
        return;
    if (_vec_metric.size()!=_vec_preproc.size())
        return;


    string str_ovd("OBB Volume");
    string str_osd("OBB Surface");
    string str_bsd("Branches Surface");
    string str_bvd("Branches Volume");
    string str_bnd("Branches Number");
    string str_bdd("Branches per Vol Unit");
    string str_srd("Selection Rate");
    string str_frd("Box Filling Rate");
    string str_bld("Branches Length");
    string str_ald("Average Length");
    string str_ard("Average Ray");

    for (unsigned int i=0; i<_vec_preproc.front().getVecDefine().size(); i++)
    {
        ss << _vec_preproc.front().getVecDefine().at(i)->getConstantName();
        ss << ",";
    }

    ss << str_ovd << "," ;
    ss << str_osd << "," ;
    ss << str_bsd << "," ;
    ss << str_bvd << "," ;
    ss << str_bnd << "," ;
    ss << str_bdd << "," ;
    ss << str_srd << "," ;
    ss << str_frd << "," ;
    ss << str_bld << "," ;
    ss << str_ald << "," ;
    ss << str_ard << endl;

    for (unsigned int j=0; j<size; j++)
    {
        PMetric  m = _vec_metric.at(j);
        PPreproc p = _vec_preproc.at(j);
        for (unsigned int k=0; k<p.getVecDefine().size(); k++)
        {
            ss << p.getVecDefine().at(k)->getConstantValue() << ",";
        }
        ov = (float) m.getObbVol();
        os = (float) m.getObbSurf();
        bs = (float) m.getBrchsSurf();
        bv = (float) m.getBrchsVol();
        bn = (float) m.getBrchsNb();
        bd = (float) m.getBrchDensity();
        sr = (float) m.getSlctnRate();
        fr = (float) m.getFillRate();
        bl = (float) m.getBrchsLength();
        al = (float) m.getAvrgLength();
        ar = (float) m.getBrchsAveray();

        string str_ov(parser.float_to_stdstr(ov));
        string str_os(parser.float_to_stdstr(os));
        string str_bs(parser.float_to_stdstr(bs));
        string str_bv(parser.float_to_stdstr(bv));
        string str_bn(parser.float_to_stdstr(bn));
        string str_bd(parser.float_to_stdstr(bd));
        string str_sr(parser.float_to_stdstr(sr));
        string str_fr(parser.float_to_stdstr(fr));
        string str_bl(parser.float_to_stdstr(bl));
        string str_al(parser.float_to_stdstr(al));
        string str_ar(parser.float_to_stdstr(ar));

        ss << str_ov << "," ;
        ss << str_os << "," ;
        ss << str_bs << "," ;
        ss << str_bv << "," ;
        ss << str_bn << "," ;
        ss << str_bd << "," ;
        ss << str_sr << "," ;
        ss << str_fr << "," ;
        ss << str_bl << "," ;
        ss << str_al << "," ;
        ss << str_ar << endl;
    }

    _file.write(QString(ss.str().c_str()).toUtf8());
    _file.close();
}

void PIORecord::save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, std::vector<PMetric>& _vec_metric, std::vector<PPreproc>& _vec_preproc,
                            std::vector<unsigned int>& _vec_modified_rule,std::vector<std::vector<float> >& _vec_change_rate)
{
    PTextParser parser;
    unsigned int size = _vec_metric.size();
    std::stringstream ss;
    float ov, os, bs, bv, bn, bd, sr, fr, bl, al, ar;

    _file.setFileName(_filename);
    if (!_file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning( _widget, "Saving", "Failed to save file." );
        return;
    }
    if ((_vec_metric.empty())||(_vec_preproc.empty()))
        return;
    if ((_vec_metric.size()!=_vec_preproc.size())||(_vec_metric.size()!=_vec_modified_rule.size()))
        return;

    string str_rule_idd("Modified Rule ID");
    string str_ovd("OBB Volume");
    string str_osd("OBB Surface");
    string str_bsd("Branches Surface");
    string str_bvd("Branches Volume");
    string str_bnd("Branches Number");
    string str_bdd("Branches per Vol Unit");
    string str_srd("Selection Rate");
    string str_frd("Box Filling Rate");
    string str_bld("Branches Length");
    string str_ald("Average Length");
    string str_ard("Average Ray");

    string str_ovc("% OV Change");
    string str_osc("% OS Change");
    string str_bsc("% BS Change");
    string str_bvc("% BV Change");
    string str_bnc("% BN Change");
    string str_bdc("% BD Change");
    string str_src("% SR Change");
    string str_frc("% FR Change");
    string str_blc("% BL Change");
    string str_alc("% AL Change");
    string str_arc("% AR Change");

    ss << str_rule_idd << "," ;

    for (unsigned int i=0; i<_vec_preproc.front().getVecDefine().size(); i++)
    {
        ss << _vec_preproc.front().getVecDefine().at(i)->getConstantName();
        ss << ",";
    }

    ss << str_ovd << "," ;
    ss << str_osd << "," ;
    ss << str_bsd << "," ;
    ss << str_bvd << "," ;
    ss << str_bnd << "," ;
    ss << str_bdd << "," ;
    ss << str_srd << "," ;
    ss << str_frd << "," ;
    ss << str_bld << "," ;
    ss << str_ald << "," ;
    ss << str_ard << "," ;

    ss << str_ovc << "," ;
    ss << str_osc << "," ;
    ss << str_bsc << "," ;
    ss << str_bvc << "," ;
    ss << str_bnc << "," ;
    ss << str_bdc << "," ;
    ss << str_src << "," ;
    ss << str_frc << "," ;
    ss << str_blc << "," ;
    ss << str_alc << "," ;
    ss << str_arc << endl;

    for (unsigned int j=0; j<size; j++)
    {
        PMetric  m = _vec_metric.at(j);
        PPreproc p = _vec_preproc.at(j);
        string str_rule_id(PTextParser((float)_vec_modified_rule.at(j)+1).getText());

        ss << str_rule_id << "," ;
        for (unsigned int k=0; k<p.getVecDefine().size(); k++)
        {
            ss << p.getVecDefine().at(k)->getConstantValue() << ",";
        }
        ov = (float) m.getObbVol();
        os = (float) m.getObbSurf();
        bs = (float) m.getBrchsSurf();
        bv = (float) m.getBrchsVol();
        bn = (float) m.getBrchsNb();
        bd = (float) m.getBrchDensity();
        sr = (float) m.getSlctnRate();
        fr = (float) m.getFillRate();
        bl = (float) m.getBrchsLength();
        al = (float) m.getAvrgLength();
        ar = (float) m.getBrchsAveray();

        string str_ov(parser.float_to_stdstr(ov));
        string str_os(parser.float_to_stdstr(os));
        string str_bs(parser.float_to_stdstr(bs));
        string str_bv(parser.float_to_stdstr(bv));
        string str_bn(parser.float_to_stdstr(bn));
        string str_bd(parser.float_to_stdstr(bd));
        string str_sr(parser.float_to_stdstr(sr));
        string str_fr(parser.float_to_stdstr(fr));
        string str_bl(parser.float_to_stdstr(bl));
        string str_al(parser.float_to_stdstr(al));
        string str_ar(parser.float_to_stdstr(ar));

        ss << str_ov << "," ;
        ss << str_os << "," ;
        ss << str_bs << "," ;
        ss << str_bv << "," ;
        ss << str_bn << "," ;
        ss << str_bd << "," ;
        ss << str_sr << "," ;
        ss << str_fr << "," ;
        ss << str_bl << "," ;
        ss << str_al << "," ;
        ss << str_ar << "," ;

        for (std::vector<float>::iterator iter = _vec_change_rate.at(j).begin(); iter!=_vec_change_rate.at(j).end(); iter++)
        {
            ss << PTextParser(*iter).getText() << "," ;
        }
        ss << endl;
    }
    _file.write(QString(ss.str().c_str()).toUtf8());
    _file.close();
}

void PIORecord::save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, std::vector<PPreproc>& _vec_preproc,
                 std::vector<unsigned int>& _vec_modified_rule, std::vector<std::vector<float> >& _vec_change_rate)
{
    PTextParser parser;
    unsigned int size = _vec_preproc.size();
    std::stringstream ss;
    float ov, os, bs, bv, bn, bd, sr, fr, bl, al, ar;

    _file.setFileName(_filename);
    if (!_file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning( _widget, "Saving", "Failed to save file." );
        return;
    }
    if (/*(_vec_metric.empty())||*/(_vec_preproc.empty()))
        return;
//    if ((_vec_metric.size()!=_vec_preproc.size())||(_vec_metric.size()!=_vec_modified_rule.size()))
//        return;

    string str_rule_idd("Modified Rule ID");
//    string str_ovd("OBB Volume");
//    string str_osd("OBB Surface");
//    string str_bsd("Branches Surface");
//    string str_bvd("Branches Volume");
//    string str_bnd("Branches Number");
//    string str_bdd("Branches per Vol Unit");
//    string str_srd("Selection Rate");
//    string str_frd("Box Filling Rate");
//    string str_bld("Branches Length");
//    string str_ald("Average Length");
//    string str_ard("Average Ray");

    string str_ovc("% OV Change");
    string str_osc("% OS Change");
    string str_bsc("% BS Change");
    string str_bvc("% BV Change");
    string str_bnc("% BN Change");
    string str_bdc("% BD Change");
    string str_src("% SR Change");
    string str_frc("% FR Change");
    string str_blc("% BL Change");
    string str_alc("% AL Change");
    string str_arc("% AR Change");

    ss << str_rule_idd << "," ;

    for (unsigned int i=0; i<_vec_preproc.front().getVecDefine().size(); i++)
    {
        ss << _vec_preproc.front().getVecDefine().at(i)->getConstantName();
        ss << ",";
    }

//    ss << str_ovd << "," ;
//    ss << str_osd << "," ;
//    ss << str_bsd << "," ;
//    ss << str_bvd << "," ;
//    ss << str_bnd << "," ;
//    ss << str_bdd << "," ;
//    ss << str_srd << "," ;
//    ss << str_frd << "," ;
//    ss << str_bld << "," ;
//    ss << str_ald << "," ;
//    ss << str_ard << "," ;

    ss << str_ovc << "," ;
    ss << str_osc << "," ;
    ss << str_bsc << "," ;
    ss << str_bvc << "," ;
    ss << str_bnc << "," ;
    ss << str_bdc << "," ;
    ss << str_src << "," ;
    ss << str_frc << "," ;
    ss << str_blc << "," ;
    ss << str_alc << "," ;
    ss << str_arc << endl;

    for (unsigned int j=0; j<size; j++)
    {
        //PMetric  m = _vec_metric.at(j);
        PPreproc p = _vec_preproc.at(j);
        string str_rule_id(PTextParser((float)_vec_modified_rule.at(j)+1).getText());

        ss << str_rule_id << "," ;
        for (unsigned int k=0; k<p.getVecDefine().size(); k++)
        {
            ss << p.getVecDefine().at(k)->getConstantValue() << ",";
        }
//        ov = (float) m.getObbVol();
//        os = (float) m.getObbSurf();
//        bs = (float) m.getBrchsSurf();
//        bv = (float) m.getBrchsVol();
//        bn = (float) m.getBrchsNb();
//        bd = (float) m.getBrchDensity();
//        sr = (float) m.getSlctnRate();
//        fr = (float) m.getFillRate();
//        bl = (float) m.getBrchsLength();
//        al = (float) m.getAvrgLength();
//        ar = (float) m.getBrchsAveray();

//        string str_ov(parser.float_to_stdstr(ov));
//        string str_os(parser.float_to_stdstr(os));
//        string str_bs(parser.float_to_stdstr(bs));
//        string str_bv(parser.float_to_stdstr(bv));
//        string str_bn(parser.float_to_stdstr(bn));
//        string str_bd(parser.float_to_stdstr(bd));
//        string str_sr(parser.float_to_stdstr(sr));
//        string str_fr(parser.float_to_stdstr(fr));
//        string str_bl(parser.float_to_stdstr(bl));
//        string str_al(parser.float_to_stdstr(al));
//        string str_ar(parser.float_to_stdstr(ar));

//        ss << str_ov << "," ;
//        ss << str_os << "," ;
//        ss << str_bs << "," ;
//        ss << str_bv << "," ;
//        ss << str_bn << "," ;
//        ss << str_bd << "," ;
//        ss << str_sr << "," ;
//        ss << str_fr << "," ;
//        ss << str_bl << "," ;
//        ss << str_al << "," ;
//        ss << str_ar << "," ;

        for (std::vector<float>::iterator iter = _vec_change_rate.at(j).begin(); iter!=_vec_change_rate.at(j).end(); iter++)
        {
            ss << PTextParser(*iter).getText() << "," ;
        }
        ss << endl;
    }
    _file.write(QString(ss.str().c_str()).toUtf8());
    _file.close();
}

void PIORecord::save_to_csv(QWidget *_widget, QFile &_file, QString &_filename, std::vector<std::vector<float> >& _comat, std::vector<std::vector<float> >& _eigenvec, std::vector<float>& _eigenval)
{
    std::stringstream ss;
    std::vector<std::string> vec_prop;
    const size_t nprop = _eigenvec.size(); // Number of properties, should be equal to _eigenvec size

    if (_eigenvec.size()!=nprop) return;
    ss.precision(2);
    vec_prop.push_back("OBB Volume");
    vec_prop.push_back("OBB Surface");
    vec_prop.push_back("Branches Surface");
    vec_prop.push_back("Branches Volume");
    vec_prop.push_back("Branches Number");
    vec_prop.push_back("Branches per Vol Unit");
    vec_prop.push_back("Selection Rate");
    vec_prop.push_back("Box Filling Rate");
    vec_prop.push_back("Branches Length");
    vec_prop.push_back("Average Length");
    vec_prop.push_back("Average Ray");

    ss << "Covariance matrix :" << std::endl;
    ss << "Property";
    for (unsigned int i=0; i<nprop; i++)
    {
        ss << "," << vec_prop.at(i) ;
    }

    for (unsigned int i=0; i<nprop; i++)
    {
        ss << std::endl << vec_prop.at(i) << ",";
        for (unsigned int j=0; j<nprop; j++)
        {
            ss << PTextParser(_comat[i][j]).format_value(_comat[i][j], 2) << ",";
        }
    }

    ss << std::endl << "Eigen Vectors :" << std::endl;

    for (unsigned int i=0; i<nprop; i++)
    {
        ss << "\t[" << i << "]" << ",";
    }

    for (unsigned int i=0; i<nprop; i++)
    {
        ss << std::endl;
        for (unsigned int j=0; j<nprop; j++)
        {
            ss << PTextParser(_eigenvec[j][i]).format_value(_eigenvec[j][i], 2) << ",";
        }
    }

    ss << std::endl << std::endl << "Eigen Values :" << std::endl;
    for (unsigned int i=0; i<nprop; i++)
    {
        ss << PTextParser(_eigenval[i]).format_value(_eigenval[i], 2) << ",";
    }
    ss << std::endl;
    _file.setFileName(_filename);
    if (!_file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning( _widget, "Saving", "Failed to save file." );
    }
    _file.write(QString(ss.str().c_str()).toUtf8());
    _file.close();
}

void PIORecord::create_xml_deriv(std::vector<PDerivHistoric*> _vec_dh, QDomDocument& _doc, QDomElement& _root)
{
    QDomElement dom_elt_derivation = _doc.createElement("derivation");
    QDomElement dom_elt_text       = _doc.createElement("text");
    //QDomText dom_txt_deriv_str = _doc.createTextNode(_deriv_str.c_str());
    dom_elt_derivation.appendChild(dom_elt_text);
    //dom_elt_text.appendChild(dom_txt_deriv_str);
    for (unsigned int i = 0; i<_vec_dh.size(); i++)
    {
        _vec_dh.at(i)->append_word_xml(_doc, dom_elt_derivation);
    }
    _root.appendChild(dom_elt_derivation);
}

void PIORecord::create_xml_deriv(PLSystemParametric* _lsysp)
{
    for (unsigned int i=0; i<_lsysp->getVecVecDhV2().size(); i++)
    {
        create_xml_deriv(_lsysp->getVecVecDhV2().at(i), doc, root);
    }
}

void PIORecord::load(QString& _raw_qstr, PLSystemParametric* _lsysp)
{
    PTextParser textparser;
    string s_preproc, s_axiom, s_rules, s_buffer;
    unsigned int n_deriv=1;
    float angle;
    vector<string> vec_str_line;
    istringstream raw(_raw_qstr.toStdString().c_str());

    while (!raw.eof())
    {
        getline(raw, s_buffer, '\n');
        vec_str_line.push_back(s_buffer);
    }
    for (unsigned int i=0; i<vec_str_line.size(); i++)
    {
        s_buffer = vec_str_line.at(i);
        if (s_buffer.find('#')==0)
        {
            if (!s_preproc.empty())
                s_preproc+="\n";
            s_preproc+=s_buffer;
        }
        else if (s_buffer.find("w:")==0)
        {
            if (s_buffer.size()>2)
                s_buffer = s_buffer.substr(2);
            s_axiom = s_buffer;
        }
        else if (s_buffer.find("n:")==0)
        {
            if (s_buffer.size()>2)
                s_buffer = s_buffer.substr(2);
            n_deriv = (unsigned int)textparser.stdstr_to_float(s_buffer);
        }
        else if (s_buffer.find("d:")==0)
        {
            if (s_buffer.size()>2)
                s_buffer = s_buffer.substr(2);
            angle = textparser.stdstr_to_float(s_buffer);
        }
        else if (s_buffer.find("->")!=string::npos)
        {
            if(!s_rules.empty())
                s_rules+="\n";
            s_rules+=s_buffer;
        }
    }
    _lsysp->update_params(s_preproc, s_axiom, angle, n_deriv);
    _lsysp->load(s_rules);
}

void PIORecord::load(QWidget* _widget, QString& _filename, QString& _qs_rules, QString& _qs_preproc, QString& _qs_axiom, float& _angle, int& _n_deriv)
{
    string s_preproc, s_axiom, s_rules, s_buffer;
    vector<string> vec_str_line;
    PTextParser textparser;

    _n_deriv = 1;
    _angle = 90;

    //Deplacer ce code dans main windowspour differencirr le drag and drop et l'ouverture via le menu
    //_filename = QFileDialog::getOpenFileName(_widget, QObject::tr("Load Procedural Modeling"), QObject::tr(""), QObject::tr("Text Files(*.txt);;All Files(*)"));
    QFile file(_filename);
    if (!file.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::warning( _widget, "Loading Procedural", "Failed to load file." );
        return;
    }
    QTextStream ts(&file);
    while (!ts.atEnd())
         vec_str_line.push_back(ts.readLine().toStdString());
    for (unsigned int i=0; i<vec_str_line.size(); i++)
    {
        s_buffer = vec_str_line.at(i);
        if (s_buffer.find('#')==0)
        {
            if (!s_preproc.empty())
                s_preproc+="\n";
            s_preproc+=s_buffer;
        }
        else if (s_buffer.find("w:")==0)
        {
            if (s_buffer.size()>2)
                s_buffer = s_buffer.substr(2);
           s_buffer.erase(std::remove(s_buffer.begin(), s_buffer.end(), ' '),s_buffer.end());
            s_axiom = s_buffer;
        }
        else if (s_buffer.find("n:")==0)
        {
            if (s_buffer.size()>2)
                s_buffer = s_buffer.substr(2);
            s_buffer.erase(std::remove(s_buffer.begin(), s_buffer.end(), ' '),s_buffer.end());
            _n_deriv = (int)textparser.stdstr_to_float(s_buffer);
        }
        else if (s_buffer.find("d:")==0)
        {
            if (s_buffer.size()>2)
                s_buffer = s_buffer.substr(2);
           s_buffer.erase(std::remove(s_buffer.begin(), s_buffer.end(), ' '),s_buffer.end());
            _angle = textparser.stdstr_to_float(s_buffer);
        }
        else if (s_buffer.find("->")!=string::npos)
        {
            if(!s_buffer.empty())
                s_buffer+="\n";
            s_buffer.erase(std::remove(s_buffer.begin(), s_buffer.end(), ' '),s_buffer.end());
            s_rules+=s_buffer;
        }
    }
    _qs_rules   = QString(s_rules.c_str());
    _qs_preproc = QString(s_preproc.c_str());
    _qs_axiom   = QString(s_axiom.c_str());
}

void PIORecord::load(QFile& _file, std::stringstream& _ss)
{
    if (!_file.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::warning( 0, "Loading Procedural File", "Failed to load file." );
        return;
    }
    QTextStream ts(&_file);
    while (!ts.atEnd())
        _ss<<ts.readLine().toStdString()<<endl;
}

void PIORecord::save(QFile& _file, std::stringstream& _ss)
{
    if (!_file.open(QFile::WriteOnly))
    {
        QMessageBox::warning( 0, "Saving Procedural File", "Failed to save file: output file cannot be opened." );
        return;
    }
    _file.write(QString(_ss.str().c_str()).toUtf8());
}

void PIORecord::csv_to_data(std::vector<std::vector <std::string> >& _datas, std::stringstream& _src)
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

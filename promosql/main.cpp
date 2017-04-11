#include <QCoreApplication>
#include <QApplication>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QVector>
#include <QTimer>
#include <QStringListIterator>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "main.h"
#include "putils.h"
#include "pimgset.h"
#include "pdbsetup.h"
#include "viewer.h"
#include "pquestionnaire.h"
#include "pboundingbox.h"
#include "dialogqlfr.h"


/*
    promosql is charged to execute 3 kinds of action
    1: Data insertion
    2: Data visualization in table and CSV format
    3: 3D Vizsualization

    Data Insertion is of two types:
    The automatic insertion of image groups with each user ID
    The semi-automatic insertion of group qualification
*/


static command promocmd;

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);
    QString filename;
    QString viewer_title;
    std::string cmd_str;
    std::vector<float> vec_extremum;
    std::vector< std::vector<float> > vec_nearestpt, vec_scattered;

    PDbSetup dbsetup;
    PImgSet imgset;

    cmd_str = parse_command(argc, argv);
    // QApplication::setQuitOnLastWindowClosed(true);

    // Show help usage
    if ((cmd_str.empty())||(cmd_str.find(HELP_CMD)!=std::string::npos))
    {
        showhelpstring();
        exit(0);
    }

    // Processing CSV Group Filename
    if (cmd_str.find(CSVG_CMD)!=std::string::npos)
    {
        if (cmd_str.find(DB_CMD)!=std::string::npos)
        {
            if (promocmd.dbname.empty())
            {
                std::cout << std::endl << "No database name was provided along the -" << DB_CMD << " option."
                          << std::endl << "Default name '" << DFLT_DBNAME << "' was set.";
                promocmd.dbname = DFLT_DBNAME;
            }
        }
        if (promocmd.csvinput.empty())
        {
            std::cout << std::endl << "No CSV file name was provided along the -c option."
                      << std::endl << "Use -" << "-" << vec_shortcommands[0] << " or --" << vec_commands[0] << " for more information.";
            exit(0);
        }

        QFile csvfile(promocmd.csvinput.c_str());
        if (!csvfile.open(QFile::ReadOnly|QFile::Text))
        {
            std::cout << std::endl << promocmd.csvinput.c_str() << ": Could not open file!. Please check if file exists!";
            std::cout << std::endl << "Process Aborted.";
            exit(0);
        }

        QTextStream csv_txtstream(&csvfile);
        dbsetup.set_dbname(promocmd.dbname);
        dbsetup.init();
        imgset = PImgSet(csv_txtstream);
        imgset.writedb(promocmd.imgtablename.c_str());

        exit(0);
    }

    // Showing Table with tablename provided
    if (cmd_str.find(SHTBL_CMD)!=std::string::npos)
    {
        if (cmd_str.find(DB_CMD)!=std::string::npos)
        {
            if (promocmd.dbname.empty())
            {
                std::cout << std::endl << "No database name was provided along the -" << DB_CMD << " option."
                          << std::endl << "Default name '" << DFLT_DBNAME << "' was set.";
                promocmd.dbname = DFLT_DBNAME;
            }
        }
        dbsetup.set_dbname(promocmd.dbname);
        dbsetup.init();
        if (promocmd.imgtablename.empty()==false)
        {
            showtable(promocmd.imgtablename);
            QApplication::exit(0);
        }
        else if ((cmd_str.find(ITBL_CMD)==std::string::npos)&&(cmd_str.find(QTBL_CMD)==std::string::npos))
        {
            std::cout << std::endl << "Please specify qualifier table or image group table."
                   << std::endl << "Use -" << "-" << vec_shortcommands[0] << " or --" << vec_commands[0] << " for more information.";
            exit(0);
        }
        else if (cmd_str.find(ITBL_CMD)!=std::string::npos)
        {
            if (promocmd.imgtablename.empty()==false)
                showtable(promocmd.imgtablename);
            QApplication::exit(0);
        }
        else if (cmd_str.find(QTBL_CMD)!=std::string::npos)
        {
            if (promocmd.qlftablename.empty()==false)
                showtable(promocmd.qlftablename);
            QApplication::exit(0);
        }
    }

    // Write Some Table in a CSV File
    if (cmd_str.find(CSVT_CMD)!=std::string::npos)
    {
        if (promocmd.csvoutput.empty())
        {
            std::cout << std::endl << "No output filename was provided along the -" << CSVT_CMD << " option."
                      << std::endl << "Default name '" << DFLT_CSVOUTPUT << "' will be set."
                      << std::endl << "Use -" << "-" << vec_shortcommands[0] << " or --" << vec_commands[0] << " for more information.";
            promocmd.csvoutput  = DFLT_CSVOUTPUT;
        }
        if (cmd_str.find(DB_CMD)!=std::string::npos)
        {
            if (promocmd.dbname.empty())
            {
                std::cout << std::endl << "No database name was provided along the -" << DB_CMD << " option."
                          << std::endl << "Default name '" << DFLT_DBNAME << "' was set.";
                promocmd.dbname = DFLT_DBNAME;
            }
        }
        dbsetup.set_dbname(promocmd.dbname);
        dbsetup.init();
        if ((cmd_str.find(ITBL_CMD)==std::string::npos)&&(cmd_str.find(QTBL_CMD)==std::string::npos))
        {
            std::cout << std::endl << "Please specify qualifier table or image group table."
                   << std::endl << "Use -" << "-" << vec_shortcommands[0] << " or --" << vec_commands[0] << " for more information.";
            exit(0);
        }
        else if (cmd_str.find(ITBL_CMD)!=std::string::npos)
        {
            if (promocmd.imgtablename.empty())
            {
                std::cout << std::endl << "No table name was provided along the -" << ITBL_CMD << " option."
                          << std::endl << "Random table '" << IMGGRP_TBLNAME << "' will be considered."
                          << std::endl << "Use -" << "-" << vec_shortcommands[0] << " or --" << vec_commands[0] << " for help.";
                promocmd.imgtablename = IMGGRP_TBLNAME;
            }
            csv_table(promocmd.csvoutput, promocmd.imgtablename);
        }
        else if (cmd_str.find(QTBL_CMD)!=std::string::npos)
        {
            if (promocmd.imgtablename.empty())
            {
                std::cout << std::endl << "No table name was provided along the -" << QTBL_CMD << " option."
                          << std::endl << "Random table '" << QLFCTN_TBLNAME << "' will be considered."
                          << std::endl << "Use -" << "-" << vec_shortcommands[0] << " or --" << vec_commands[0] << " for help.";
                promocmd.imgtablename = QLFCTN_TBLNAME;
            }
            csv_table(promocmd.csvoutput, promocmd.qlftablename);
        }

        /*  PLEASE REMOVE THE FOLLOWING LINES ONES QUALIFIERS DATAS ARE UNUSED */
        qlfrs_to_csv();
        qlfrs_to_csv_annotated();
        /*  ENDS OF LINES TO BE REMOVED*/
        exit(0);
    }


    //Generate matrix for specified grammar and/or user_id
    if (cmd_str.find(MX_CMD)!=std::string::npos)
    {

        QString qs_tablename(IMGGRP_TBLNAME);
        if (promocmd.matrixfilename.empty())
        {
            std::cout << std::endl << "No CSV output file specified. Process Aborted."
                      << std::endl << "Use -" << "-" << vec_shortcommands[0] << " or --" << vec_commands[0] << " for help.";
            exit(0);
        }
        if (cmd_str.find(ITBL_CMD)!=std::string::npos)
        {
            std::cout << std::endl << "No table name was provided along the -" << ITBL_CMD << " option."
                      << std::endl << "Random table '" << IMGGRP_TBLNAME << "' will be considered."
                      << std::endl << "Use -" << "-" << vec_shortcommands[0] << " or --" << vec_commands[0] << " for help.";
            promocmd.imgtablename = IMGGRP_TBLNAME;
            qs_tablename = IMGGRP_TBLNAME;
        }
        else if (cmd_str.find(QTBL_CMD)!=std::string::npos)
        {
            std::cout << std::endl << "No table name was provided along the -" << QTBL_CMD << " option."
                      << std::endl << "Random table '" << QLFCTN_TBLNAME << "' will be considered."
                      << std::endl << "Use -" << "-" << vec_shortcommands[0] << " or --" << vec_commands[0] << " for help.";
            promocmd.imgtablename = QLFCTN_TBLNAME;
            qs_tablename = QLFCTN_TBLNAME;
        }
        if (cmd_str.find(DB_CMD)!=std::string::npos)
        {
            if (promocmd.dbname.empty())
            {
                std::cout << std::endl << "No database name was provided along the -" << DB_CMD << " option."
                          << std::endl << "Default name '" << DFLT_DBNAME << "' was set.";
                promocmd.dbname = DFLT_DBNAME;
            }
        }
        dbsetup.set_dbname(promocmd.dbname);
        dbsetup.init();
        QVector< QVector< QVector<int> > > mtrcs = generate_switchedmatrix(promocmd.userid, promocmd.grammar.c_str(), qs_tablename);
        QVector< QVector<int> > mx = generate_matrix(promocmd.userid, promocmd.grammar.c_str(), qs_tablename);
        std::stringstream ss;
        PUtils::put_numheaders(ss, mx);
        PUtils::save(promocmd.matrixfilename.c_str(), ss);
        multimx_to_csv(mtrcs, promocmd.matrixfilename.c_str(), MX_ORDERING);
        exit(0);
    }

    // Begin qualifying process for a specific user and grammar
    if (cmd_str.find(QLF_CMD)!=std::string::npos)
    {
        if (cmd_str.find(DB_CMD)!=std::string::npos)
        {
            if (promocmd.dbname.empty())
            {
                std::cout << std::endl << "No database name was provided along the -" << DB_CMD << " option."
                          << std::endl << "Default name '" << DFLT_DBNAME << "' was set.";
                promocmd.dbname = DFLT_DBNAME;
            }
        }
        if (promocmd.grammar.empty())
        {
            std::cout << std::endl << "No grammar specified. Process Aborted.";
            exit(0);
        }
        if (promocmd.userid==0)
        {
            std::cout << std::endl << "User ID unspecified. Process Aborted.";
            exit(0);
        }
        if (promocmd.qlftablename.empty())
            promocmd.qlftablename = QLFCTN_TBLNAME;

        dbsetup.set_dbname(promocmd.dbname);
        dbsetup.init();

        std::vector<uint> vec_grps = imgset.read_groupnb(promocmd.userid, promocmd.grammar.c_str(), promocmd.imgtablename.c_str());

        qualify(promocmd.userid, promocmd.grammar.c_str(), vec_grps, promocmd.qlftablename.c_str());
        exit(0);
    }

    // Output Usefulness
    if (cmd_str.find(USEF_CMD)!=std::string::npos)
    {
        std::stringstream ss;

        if (cmd_str.find(DB_CMD)!=std::string::npos)
        {
            if (promocmd.dbname.empty())
            {
                std::cout << std::endl << "No database name was provided along the -" << DB_CMD << " option."
                          << std::endl << "Default name '" << DFLT_DBNAME << "' was set.";
                promocmd.dbname = DFLT_DBNAME;
            }
        }
        dbsetup.set_dbname(promocmd.dbname);
        dbsetup.init();
        if (promocmd.usefulfn.empty()==true)
            promocmd.usefulfn = DFLT_CSVOUTPUT;
        std::vector<float> vec_usefs = compute_usefulness(promocmd.userid, promocmd.grammar.c_str(), promocmd.imgtablename.c_str(), promocmd.qlftablename.c_str());
        for (uint i=0; i<vec_usefs.size(); i++)
        {
            ss << i+1 << "," << vec_usefs.at(i) << std::endl;
        }
        PUtils::save(promocmd.usefulfn.c_str(), ss);
        viewer_title = "Usefulness Viewer";
        if (promocmd.userid==0)
            viewer_title += " - All Users";
        else
        {
            viewer_title += " - User ID ";
            viewer_title += QString::number(promocmd.userid);
        }
        if (promocmd.grammar.empty()==true)
            viewer_title += " - All Grammars";
        else
        {
            viewer_title += " - ";
            viewer_title += promocmd.grammar.c_str();
        }
        load_wrappers(vec_extremum, vec_nearestpt);
        load_scattered(vec_scattered);
        Viewer* viewer = new Viewer(CONSTRAINTS_FREE_VIEW);
        if (!vec_nearestpt.empty())
            viewer->setvecsabsolute(vec_nearestpt);
        if (!vec_extremum.empty())
            viewer->setextremums(vec_extremum);
        if (!vec_scattered.empty())
            viewer->setvecsscatterplot(vec_scattered);
        viewer->setWindowTitle(viewer_title);
        viewer->setvecusefulns(vec_usefs);
        viewer->updateGL();
        viewer->show();

        QApplication::exit(0);
    }

    // Output Difference
    if (cmd_str.find(DIFF_CMD)!=std::string::npos)
    {
        std::stringstream ss, ss_hardcode;
        std::string mx_filename;
        std::vector< std::vector<uint> > vecs_respldidx;
        QVector<int> qvec_hardcodeidx;

        if (cmd_str.find(DB_CMD)!=std::string::npos)
        {
            if (promocmd.dbname.empty())
            {
                std::cout << std::endl << "No database name was provided along the -" << DB_CMD << " option."
                          << std::endl << "Default name '" << DFLT_DBNAME << "' was set.";
                promocmd.dbname = DFLT_DBNAME;
            }
        }
        dbsetup.set_dbname(promocmd.dbname);
        dbsetup.init();
        if (promocmd.differencefn.empty()==true)
            promocmd.differencefn = DFLT_CSVOUTPUT;
        //QVector< QVector<float> > qvecs_diffmx = generate_diffmatrix(promocmd.userid, promocmd.grammar.c_str(), promocmd.imgtablename.c_str(), promocmd.qlftablename.c_str());
        //PUtils::put_numheaders(ss, qvecs_diffmx);
//        if (promocmd.differencefn.find(".csv")!=std::string::npos)
//            mx_filename = promocmd.differencefn.substr(0, promocmd.differencefn.size()-4) + std::string("_mx.csv");
//        else
//            mx_filename = promocmd.differencefn + std::string("_mx.csv");
//        PUtils::save(mx_filename.c_str(), ss);
        ss.str(std::string());
        std::vector<float> vec_diff = compute_difference(promocmd.userid, promocmd.grammar.c_str(), promocmd.imgtablename.c_str(), promocmd.qlftablename.c_str());
        for (uint i=0; i<vec_diff.size(); i++)
        {
            ss << i+1 << "," << vec_diff.at(i) << std::endl;
        }
        PUtils::save(promocmd.differencefn.c_str(), ss);
        viewer_title = "Difference Viewer";
        if (promocmd.userid==0)
            viewer_title += " - All Users";
        else
        {
            viewer_title += " - User ID ";
            viewer_title += QString::number(promocmd.userid);
        }
        if (promocmd.grammar.empty()==true)
            viewer_title += " - All Grammars";
        else
        {
            viewer_title += " - ";
            viewer_title += promocmd.grammar.c_str();
        }
        load_wrappers(vec_extremum, vec_nearestpt);
        load_scattered(vec_scattered);
        Viewer* viewer = new Viewer(CONSTRAINTS_FREE_VIEW);
        if (!vec_nearestpt.empty())
            viewer->setvecsabsolute(vec_nearestpt);
        if (!vec_extremum.empty())
            viewer->setextremums(vec_extremum);
        if (!vec_scattered.empty())
            viewer->setvecsscatterplot(vec_scattered);
        viewer->setWindowTitle(viewer_title);
        viewer->setvecdiff(vec_diff);
        viewer->updateGL();
        viewer->show();

        viewer->resampledist_method1();
        viewer->resampledist_method2();
        viewer->resampledist_method3();
        std::vector<uint> vec_idxtmp;
        for (int i=0; i<IMAGE_NB; i++)
        {
            vec_idxtmp.push_back(i);
        }
        vecs_respldidx.push_back(vec_idxtmp);
        vecs_respldidx.push_back(viewer->getvecabsolresampld1());
        vecs_respldidx.push_back(viewer->getvecabsolresampld2());
        vecs_respldidx.push_back(viewer->getvecabsolresampld3());
        std::string mx_fn = promocmd.matrixfilename.empty()?promocmd.grammar:promocmd.matrixfilename;
        QVector< QVector< QVector<int> > > mtrcs_rspld = generate_resampldmatrix(vecs_respldidx, promocmd.userid, promocmd.grammar.c_str());        
        multimx_to_csv(mtrcs_rspld, mx_fn.c_str(), vecs_respldidx);

        //PUtils::data_to_csv(ss_hardcode, mxhardcode);
        /*
        QVector< QVector<int> > mxhardcode = generate_matrix("honda_sympodial_0912_nykomatch.csv", promocmd.userid, promocmd.grammar.c_str());
        qvec_hardcodeidx = PUtils::load_vector(QString("honda_sympodial_0912_nykomatch.csv"));
        PUtils::put_numheaders(ss_hardcode, mxhardcode, qvec_hardcodeidx);
        PUtils::save(QString("hardcode_")+promocmd.grammar.c_str()+QString(".csv"), ss_hardcode);
        */
        QApplication::exit(0);
    }

    // Output qualifiers
    if (cmd_str.find(QLFR_CMD)!=std::string::npos)
    {
        std::stringstream ss;
        if (cmd_str.find(DB_CMD)!=std::string::npos)
        {
            if (promocmd.dbname.empty())
            {
                std::cout << std::endl << "No database name was provided along the -" << DB_CMD << " option."
                          << std::endl << "Default name '" << DFLT_DBNAME << "' was set.";
                promocmd.dbname = DFLT_DBNAME;
            }
        }
        dbsetup.set_dbname(promocmd.dbname);
        dbsetup.init();
        if (promocmd.qualifierfn.empty()==true)
            promocmd.qualifierfn = DFLT_CSVOUTPUT;
        QVector<QStringList*> qvec_qlfrs = check_qualifiers(promocmd.userid, promocmd.grammar.c_str(), promocmd.imgtablename.c_str(), promocmd.qlftablename.c_str());
        for (uint i=0; i<qvec_qlfrs.size(); i++)
        {
            QStringListIterator qsliter(*qvec_qlfrs.at(i));
            ss << i+1;
            while (qsliter.hasNext())
            {
                ss << "," << qsliter.next().toStdString();
            }
            ss << std::endl;
        }
        PUtils::save(promocmd.qualifierfn.c_str(), ss);
        load_wrappers(vec_extremum, vec_nearestpt);
        load_scattered(vec_scattered);
        viewer_title = "Qualifiers Viewer";
        if (promocmd.userid==0)
            viewer_title += " - All Users";
        else
        {
            viewer_title += " - User ID ";
            viewer_title += QString::number(promocmd.userid);
        }
        if (promocmd.grammar.empty()==true)
            viewer_title += " - All Grammars";
        else
        {
            viewer_title += " - ";
            viewer_title += promocmd.grammar.c_str();
        }
        //save_qlfr_coords(viewer->get);
        Viewer* viewer = new Viewer(CONSTRAINTS_FREE_VIEW);
        viewer->setWindowTitle(viewer_title);
        viewer->setqvecqlfrs(qvec_qlfrs);
        if (!vec_nearestpt.empty())
            viewer->setvecsabsolute(vec_nearestpt);
        if (!vec_extremum.empty())
            viewer->setextremums(vec_extremum);
        if (!vec_scattered.empty())
            viewer->setvecsscatterplot(vec_scattered);
        if (!promocmd.grammar.empty())
        {
            viewer->setfnmanmatch(promocmd.grammar+"_nykomatch.csv");
            viewer->load_matching(promocmd.grammar+"_nykomatch.csv");
            //save_qlfr_coords(viewer->getvecabsolresampldman(), qvec_qlfrs);
        }

        viewer->updateGL();
        viewer->show();
        QApplication::exit(0);
    }

    // Draw qualifiers in 3D
    if (cmd_str.find(QCOO_CMD)!=std::string::npos)
    {
        std::ifstream file("qlfr_coords.csv");
        std::stringstream buffer;
        std::vector< std::vector<std::string> > vecs_datas;
        std::vector<std::string> vec_qlfrgrmrtmp, vec_allqlfrstmp;
        std::vector<float> vec_xqlfrtmp, vec_yqlfrtmp, vec_zqlfrtmp;

        buffer << file.rdbuf();
        file.close();
        PUtils::csv_to_data(vecs_datas, buffer);
        for (std::vector< std::vector<std::string> >::iterator iter = vecs_datas.begin(); iter != vecs_datas.end(); iter++)
        {
            if ((*iter).size()>=5)
            {
                vec_xqlfrtmp.push_back(PUtils::stdstr_to_float((*iter)[0]));
                vec_yqlfrtmp.push_back(PUtils::stdstr_to_float((*iter)[1]));
                vec_zqlfrtmp.push_back(PUtils::stdstr_to_float((*iter)[2]));
                vec_qlfrgrmrtmp.push_back((*iter)[3]);
                vec_allqlfrstmp.push_back((*iter)[4]);
            }
        }

        viewer_title = "Qualifiers Viewer";
        Viewer* viewer = new Viewer(CONSTRAINTS_FREE_VIEW);
        viewer->setWindowTitle(viewer_title);

        viewer->setvecxqlfr(vec_xqlfrtmp);
        viewer->setvecyqlfr(vec_yqlfrtmp);
        viewer->setveczqlfr(vec_zqlfrtmp);
        viewer->setvecqlfrgrmr(vec_qlfrgrmrtmp);
        viewer->setvecallqlfrs(vec_allqlfrstmp);
        viewer->update_qlfrfilter();

        viewer->updateGL(); // Need to call init first
        viewer->show();
        viewer->setrealtargetdrawn(false);
        viewer->setscattereddrawn(false);
        viewer->setqualifierdrawn(false);
        viewer->setallqlfrsdrawn(true);
        viewer->setidxrradius(2.0f);
        viewer->updateGL();

        std::vector< std::vector<float> > vecs_dbgclouds = viewer->getclouds();
        std::vector<float> vec_dbg_target;
        std::vector<size_t> vec_dbg_idx;
        vec_dbg_target.push_back(1.7f);
        vec_dbg_target.push_back(-9.2f);
        vec_dbg_target.push_back(4.6f);
        vec_dbg_idx = viewer->check_neighbours(vecs_dbgclouds, vec_dbg_target, 15);
        for (size_t i=0; i<vec_dbg_idx.size(); i++)
            std::cout << std::endl << "Neighbour #" << i <<":" << vec_dbg_idx[i];
        QApplication::exit(0);
    }

    // Draw bounding box
    if (cmd_str.find(BBOX_CMD)!=std::string::npos)
    {
        Viewer* viewer = new Viewer(CONSTRAINTS_FREE_VIEW);
        std::vector< std::vector< std::vector<GLfloat> > > vecs_obbclouds;
        DialogQlfr* dlg_qlfr = new DialogQlfr;
        QStringList qlfr_list = load_obbqlfr("qlfr_filterobb.csv");
        std::vector<PBoundingBox> vec_obb = qlfr_obb(qlfr_list, vecs_obbclouds);

        std::vector< std::vector<float> > vecs_rgb = ::rgb_colorgenerator(vec_obb.size());
        dlg_qlfr->setqslqlfr(qlfr_list);
        dlg_qlfr->add_checkbox(qlfr_list, vecs_rgb);
        viewer->setvecobb(vec_obb);
        viewer->setvecsobbclouds(vecs_obbclouds);
        viewer->update_obbdisplay();
        viewer->resample_obb();
        viewer->updateGL();
        viewer->show();
        //viewer->save_obbvol();
        viewer->save_obbcenter("center_resampled.csv");
        //viewer->save_obbcorner("nyko_cornerrequest", vec_obb);
        //viewer->save_belongrates();
        //viewer->save_belongrates_regen();
        //viewer->save_averages_dist();
        viewer->save_center_dist();
        dlg_qlfr->setviewer(viewer);
        dlg_qlfr->show();
    }


    // View in 3D
    if (cmd_str.find(V3D_CMD)!=std::string::npos)
    {
        Viewer* viewer = new Viewer(CONSTRAINTS_FREE_VIEW);
        viewer->updateGL();
        viewer->show();
    }

    return a.exec();
}

void showhelpstring()
{
    std::cout << std::endl;
    std::cout << std::endl << "promosql version 1.0 - Analyzing utilities for procedural modeling project"
              << std::endl << "USAGE: promosql [-option1] [argument1] ... [-optionN] [argumentN]"
              << std::endl << "Options:"
              << std::endl << "-" << (char)HELP_CMD   << " or --" << vec_commands[0] << "\t\t\t" << "Show Usage."
              << std::endl << "-" << (char)DB_CMD     << " or --" << vec_commands[1] << "\t\t"   << "Set database name."
              << std::endl << "-" << (char)MX_CMD     << " or --" << vec_commands[2] << "\t\t"   << "Show Table."
              << std::endl << "-" << (char)SHTBL_CMD  << " or --" << vec_commands[3] << "\t\t"   << "Set Group CSV filename."
              << std::endl << "-" << (char)CSVG_CMD   << " or --" << vec_commands[4] << "\t\t"   << "Set matrix output filename."
              << std::endl << "-" << (char)USR_CMD    << " or --" << vec_commands[5] << "\t\t"   << "Set Username for database file."
              << std::endl << "-" << (char)PWD_CMD    << " or --" << vec_commands[6] << "\t\t"   << "Set Password for database file."
              << std::endl << "-" << (char)CSVT_CMD   << " or --" << vec_commands[7] << "\t\t"   << "Output table in CSV File."
              << std::endl << "-" << (char)ITBL_CMD   << " or --" << vec_commands[8] << "\t\t"   << "Specify Image Group tablename."
              << std::endl << "-" << (char)GRAM_CMD   << " or --" << vec_commands[9] << "\t\t\t" << "Request grammar for specific User ID."
              << std::endl << "-" << (char)UID_CMD    << " or --" << vec_commands[10]<< "\t\t\t" << "Specify User ID."
              << std::endl << "-" << (char)QLF_CMD    << " or --" << vec_commands[11]<< "\t\t\t" << "Qualify groups from specified grammar."
              << std::endl << "-" << (char)V3D_CMD    << " or --" << vec_commands[12]<< "\t\t\t" << "View in 3D."
              << std::endl << "-" << (char)QTBL_CMD   << " or --" << vec_commands[13]<< "\t\t"   << "Specify Qualifier Tablename."
              << std::endl << "-" << (char)IMGNB_CMD  << " or --" << vec_commands[14]<< "\t\t\t"   << "Specify Image ID."
              << std::endl << "-" << (char)USEF_CMD   << " or --" << vec_commands[15]<< "\t\t"   << "Specify Usefulness Output Filename."
              << std::endl << "-" << (char)QLFR_CMD   << " or --" << vec_commands[16]<< "\t\t"   << "Return qualifiers for specific input config."
              << std::endl << "-" << (char)DIFF_CMD   << " or --" << vec_commands[17]<< "\t\t"   << "Specify Difference Output Filename."
              << std::endl << "-" << (char)QCOO_CMD   << " or --" << vec_commands[18]<< "\t\t"   << "Draw qualifiers in 3D space."
              << std::endl << "-" << (char)BBOX_CMD   << " or --" << vec_commands[19]<< "\t\t"   << "Draw bounding box related to qualifier(s)."
                 ;
    std::cout << std::endl << std::endl << std::endl;
}

std::string parse_command(int argc, char *argv[])
{
    std::string command_tmp, value;
    char shortcommand_tmp;

    promocmd.username    = DFLT_USERNAME;
    promocmd.password    = DFLT_PASSWORD;
    promocmd.usefulfn    = DFLT_CSVOUTPUT;
    promocmd.qualifierfn = DFLT_CSVOUTPUT;
    promocmd.differencefn= DFLT_CSVOUTPUT;
    promocmd.userid   = 0;
    promocmd.imgid    = 0;

    for (int i=1; i<argc; i++)
    {
        command_tmp = std::string(argv[i]);
        shortcommand_tmp = '\0';
        if (command_tmp.find("--")==0)
        {
            command_tmp = command_tmp.substr(2);
            if (std::find(vec_commands.begin(), vec_commands.end(), command_tmp)==vec_commands.end())
            {
                std::cout << std::endl << "Invalid option : --" << command_tmp;
                value = std::string(1, vec_shortcommands[0]);
                break;
            }
        }
        else if (command_tmp.find_first_of("-")==0)
        {
            if (command_tmp.size()==2)
            {
                shortcommand_tmp = command_tmp[1];
            }
            if (std::find(vec_shortcommands.begin(), vec_shortcommands.end(), shortcommand_tmp)==vec_shortcommands.end())
            {
                std::cout << std::endl << "Invalid option : -" << command_tmp[1];
                value = std::string(1, HELP_CMD);
                break;
            }
        }

        if ((shortcommand_tmp==vec_shortcommands[0])||(command_tmp.compare(vec_commands[0])==0))
        {
            value.push_back(vec_shortcommands[0]);
        }
        else
        {
            for (uint j=1; j<vec_commands.size(); j++)
            {
                if ((shortcommand_tmp==vec_shortcommands[j])||(command_tmp.compare(vec_commands[j])==0))
                {
                    value.push_back(vec_shortcommands[j]);
                    if (i+1<argc)
                    {
                        if (argv[i+1][0]!='-')
                        {
                            switch(j)
                            {
                            case 1:
                                promocmd.dbname     = argv[i+1];
                                break;
                            case 2:
                                promocmd.matrixfilename = argv[i+1];
                                break;
                            case 3:
                                promocmd.imgtablename  = argv[i+1];
                                break;
                            case 4:
                                promocmd.csvinput   = argv[i+1];
                                break;
                            case 5:
                                promocmd.username   = argv[i+1];
                                break;
                            case 6:
                                promocmd.password   = argv[i+1];
                                break;
                            case 7:
                                promocmd.csvoutput  = argv[i+1];
                                break;
                            case 8:
                                promocmd.imgtablename  = argv[i+1];
                                break;
                            case 9:
                                promocmd.grammar    = argv[i+1];
                                break;
                            case 10:
                                try{std::istringstream(argv[i+1]) >> promocmd.userid;}
                                catch(...){std::cout << std::endl << "Invalid User ID!";}
                                break;
                            case 11:
                                // Qualify Groups
                                break;
                            case 12:
                                // View in 3D
                                break;
                            case 13:
                                promocmd.qlftablename  = argv[i+1];
                                break;
                            case 14:
                                try{std::istringstream(argv[i+1]) >> promocmd.imgid;}
                                catch(...){std::cout << std::endl << "Invalid Image ID!";}
                                break;
                            case 15:
                                promocmd.usefulfn = argv[i+1];
                                break;
                            case 16:
                                promocmd.qualifierfn = argv[i+1];
                                break;
                            case 17:
                                promocmd.differencefn = argv[i+1];
                                break;
                            case 18:
                                // draw qualifiers in 3D
                                break;
                            case 19:
                                // draw bounding box
                                break;
                            }
                        }
                    }
                }
            }
        }

    }
    if (promocmd.dbname.empty())
        promocmd.dbname     = DFLT_DBNAME;
    if (promocmd.imgtablename.empty())
        promocmd.imgtablename  = IMGGRP_TBLNAME;
    if (promocmd.username.empty())
        promocmd.username   = DFLT_USERNAME;
    if (promocmd.password.empty())
        promocmd.password   = DFLT_PASSWORD;
    if (promocmd.csvoutput.empty())
        promocmd.csvoutput  = DFLT_CSVOUTPUT;
    if (promocmd.qlftablename.empty())
        promocmd.qlftablename  = QLFCTN_TBLNAME;
    return value;
}

/*  Symmetric matrix A => eigenvectors in columns of V, corresponding
    eigenvalues in d. */
void eigen_decomposition(double A[MATRIX_DIM][MATRIX_DIM], double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM])
{
    double e[MATRIX_DIM];
    for (int i = 0; i < MATRIX_DIM; i++)
    {
        for (int j = 0; j < MATRIX_DIM; j++)
        {
          V[i][j] = A[i][j];
        }
    }
    tred2(V, d, e);
    tql2(V, d, e);
}

// Symmetric tridiagonal QL algorithm.
void tql2(double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM], double e[MATRIX_DIM])
{
//  This is derived from the Algol procedures tql2, by
//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
//  Fortran subroutine in EISPACK.

    for (int i = 1; i < MATRIX_DIM; i++)
    {
        e[i-1] = e[i];
    }
    e[MATRIX_DIM-1] = 0.0;

    double f = 0.0;
    double tst1 = 0.0;
    double eps = pow(2.0,-52.0);
    for (int l = 0; l < MATRIX_DIM; l++)
    {
        // Find small subdiagonal element
        tst1 = PROCEDURAL_MAX(tst1,fabs(d[l]) + fabs(e[l]));
        int m = l;
        while (m < MATRIX_DIM)
        {
            if (fabs(e[m]) <= eps*tst1)
            {
                break;
            }
            m++;
        }

        // If m == l, d[l] is an eigenvalue,
        // otherwise, iterate.
        if (m > l)
        {
            int iter = 0;
            do
            {
                iter = iter + 1;  // (Could check iteration count here.)
                // Compute implicit shift

                double g = d[l];
                double p = (d[l+1] - g) / (2.0 * e[l]);
                double r = hypot2(p,1.0);
                if (p < 0)
                {
                  r = -r;
                }
                d[l] = e[l] / (p + r);
                d[l+1] = e[l] * (p + r);
                double dl1 = d[l+1];
                double h = g - d[l];
                for (int i = l+2; i < MATRIX_DIM; i++)
                {
                    d[i] -= h;
                }
                f = f + h;

                // Implicit QL transformation.
                p = d[m];
                double c = 1.0;
                double c2 = c;
                double c3 = c;
                double el1 = e[l+1];
                double s = 0.0;
                double s2 = 0.0;
                for (int i = m-1; i >= l; i--)
                {
                    c3 = c2;
                    c2 = c;
                    s2 = s;
                    g = c * e[i];
                    h = c * p;
                    r = hypot2(p,e[i]);
                    e[i+1] = s * r;
                    s = e[i] / r;
                    c = p / r;
                    p = c * d[i] - s * g;
                    d[i+1] = h + s * (c * g + s * d[i]);

                    // Accumulate transformation.

                    for (int k = 0; k < MATRIX_DIM; k++)
                    {
                        h = V[k][i+1];
                        V[k][i+1] = s * V[k][i] + c * h;
                        V[k][i] = c * V[k][i] - s * h;
                    }
                }
                p = -s * s2 * c3 * el1 * e[l] / dl1;
                e[l] = s * p;
                d[l] = c * p;

            // Check for convergence.
            } while (fabs(e[l]) > eps*tst1);
        }
        d[l] = d[l] + f;
        e[l] = 0.0;
    }

    // Sort eigenvalues and corresponding vectors.
    for (int i = 0; i < MATRIX_DIM-1; i++)
    {
        int k = i;
        double p = d[i];
        for (int j = i+1; j < MATRIX_DIM; j++)
        {
            if (d[j] < p)
            {
                k = j;
                p = d[j];
            }
        }
        if (k != i)
        {
            d[k] = d[i];
            d[i] = p;
            for (int j = 0; j < MATRIX_DIM; j++)
            {
               p = V[j][i];
               V[j][i] = V[j][k];
               V[j][k] = p;
            }
        }
    }
}

// Symmetric Householder reduction to tridiagonal form.
void tred2(double V[MATRIX_DIM][MATRIX_DIM], double d[MATRIX_DIM], double e[MATRIX_DIM])
{
//  This is derived from the Algol procedures tred2 by
//  Bowdler, Martin, Reinsch, and Wilkinson, Handbook for
//  Auto. Comp., Vol.ii-Linear Algebra, and the corresponding
//  Fortran subroutine in EISPACK.

    for (int j = 0; j < MATRIX_DIM; j++)
    {
        d[j] = V[MATRIX_DIM-1][j];
    }

    // Householder reduction to tridiagonal form.
    for (int i = MATRIX_DIM-1; i > 0; i--)
    {

        // Scale to avoid under/overflow.

        double scale = 0.0;
        double h = 0.0;
        for (int k = 0; k < i; k++)
        {
            scale = scale + fabs(d[k]);
        }
        if (scale == 0.0)
        {
            e[i] = d[i-1];
            for (int j = 0; j < i; j++)
            {
                d[j] = V[i-1][j];
                V[i][j] = 0.0;
                V[j][i] = 0.0;
            }
        }
        else
        {
        // Generate Householder vector.
            for (int k = 0; k < i; k++)
            {
                d[k] /= scale;
                h += d[k] * d[k];
            }
            double f = d[i-1];
            double g = sqrt(h);
            if (f > 0)
            {
                g = -g;
            }
            e[i] = scale * g;
            h = h - f * g;
            d[i-1] = f - g;
            for (int j = 0; j < i; j++)
            {
                e[j] = 0.0;
            }

            // Apply similarity transformation to remaining columns.
            for (int j = 0; j < i; j++)
            {
                f = d[j];
                V[j][i] = f;
                g = e[j] + V[j][j] * f;
                for (int k = j+1; k <= i-1; k++)
                {
                    g += V[k][j] * d[k];
                    e[k] += V[k][j] * f;
                }
                e[j] = g;
            }
            f = 0.0;
            for (int j = 0; j < i; j++)
            {
                e[j] /= h;
                f += e[j] * d[j];
            }
            double hh = f / (h + h);
            for (int j = 0; j < i; j++)
            {
                e[j] -= hh * d[j];
            }
            for (int j = 0; j < i; j++)
            {
                f = d[j];
                g = e[j];
                for (int k = j; k <= i-1; k++)
                {
                  V[k][j] -= (f * e[k] + g * d[k]);
                }
                d[j] = V[i-1][j];
                V[i][j] = 0.0;
            }
        }
        d[i] = h;
    }

    // Accumulate transformations.

    for (int i = 0; i < MATRIX_DIM-1; i++)
    {
        V[MATRIX_DIM-1][i] = V[i][i];
        V[i][i] = 1.0;
        double h = d[i+1];
        if (h != 0.0)
        {
            for (int k = 0; k <= i; k++)
            {
                d[k] = V[k][i+1] / h;
            }
            for (int j = 0; j <= i; j++)
            {
                double g = 0.0;
                for (int k = 0; k <= i; k++)
                {
                    g += V[k][i+1] * V[k][j];
                }
                for (int k = 0; k <= i; k++)
                {
                    V[k][j] -= g * d[k];
                }
            }
        }
        for (int k = 0; k <= i; k++)
        {
            V[k][i+1] = 0.0;
        }
    }
    for (int j = 0; j < MATRIX_DIM; j++)
    {
        d[j] = V[MATRIX_DIM-1][j];
        V[MATRIX_DIM-1][j] = 0.0;
    }
    V[MATRIX_DIM-1][MATRIX_DIM-1] = 1.0;
    e[0] = 0.0;
}

void hsv_to_rgb( float *r, float *g, float *b, float h, float s, float v )
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

CGLA::Vec3f translate(CGLA::Vec3f _point, CGLA::Vec3f _vec)
{
    return CGLA::Vec3f(_point[0]+_vec[0], _point[1]+_vec[1], _point[2]+_vec[2]);
}

double hypot2(double x, double y)
{
    return sqrt(x*x+y*y);
}

void showtable(std::string _tablename)
{
    QTableView *tableview;
    QSqlQueryModel* model = new QSqlQueryModel();
    QString qry_str("SELECT * FROM ");
    qry_str+=_tablename.c_str();

    model->setQuery(qry_str);

    tableview = new QTableView();
    tableview->setModel(model);
    tableview->show();
}

void csv_table(std::string _csvoutputfn, std::string _tablename)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM " + QString(_tablename.c_str()));
    PUtils::model_to_csv(_csvoutputfn, model);
}

std::vector<std::string> get_grammars(int _user_id, QString _tablename)
{
    std::vector<std::string> value;
    std::vector< std::vector<std::string> > vecs_datas;
    std::stringstream ss;
    const int GRAMMAR_IDX = 0;
    const QString tmp_filename(_tablename+".tmp");
    QString qry_str("SELECT grammar FROM " + _tablename + " WHERE user_id=");
    qry_str.append(QString::number(_user_id));
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(qry_str);

    PUtils::model_to_csv(tmp_filename.toStdString(), model);
    PUtils::load(tmp_filename, ss);
    PUtils::csv_to_data(vecs_datas, ss);

    for (std::vector< std::vector<std::string> >::iterator iter = vecs_datas.begin(); iter!=vecs_datas.end(); iter++)
    {
        try
        {
            std::string grammar_tmp = (*iter)[GRAMMAR_IDX];
            if (grammar_tmp.compare("grammar")!=0)
            {
                if (std::find(value.begin(), value.end(), grammar_tmp)==value.end())
                {
                    value.push_back(grammar_tmp);
                }
            }
        }
        catch(...)
        {
            std::cout << std::endl << "Failed to get grammar name on line " << std::distance(iter, vecs_datas.begin()) << ".";
        }
    }

    return value;
}

std::vector<int> get_users(QString _grammar, QString _tablename)
{
    std::vector<int> value;
    std::vector< std::vector<std::string> > vecs_datas;
    std::stringstream ss;
    const int UID_IDX = 0, UID_MIN=10, UID_MAX=1000;
    const QString tmp_filename(_tablename+".tmp");
    QString qry_str("SELECT user_id FROM " + _tablename + " WHERE grammar=\"" + _grammar+"\"");
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(qry_str);
    if (model->lastError().isValid())
         std::cout      << std::endl << "Error on " <<  qry_str.toStdString()
                        << model->lastError().text().toStdString();

    //std::cout << std::endl << "Debugging...";
    PUtils::model_to_csv(tmp_filename.toStdString(), model);
    PUtils::load(tmp_filename, ss);
    PUtils::csv_to_data(vecs_datas, ss);

    for (std::vector< std::vector<std::string> >::iterator iter = vecs_datas.begin(); iter!=vecs_datas.end(); iter++)
    {
        try
        {
            int uid_tmp;
            std::istringstream((*iter)[UID_IDX]) >> uid_tmp;
            if (uid_tmp>UID_MIN && uid_tmp<UID_MAX)
            {
                if (std::find(value.begin(), value.end(), uid_tmp)==value.end())
                {
                    value.push_back(uid_tmp);
                }
            }
        }
        catch(...)
        {
            std::cout << std::endl << "Failed to get user_id on line " << std::distance(iter, vecs_datas.begin()) << ".";
        }
    }
    return value;
}

QVector< QVector<int> > generate_matrix(int _user_id, QString _grammar, QString _tablename)
{
    QVector< QVector<int> > value;

    if ((_user_id==0)&&(_grammar.isEmpty()))
    {
        std::vector<PImgSet> vec_imgset = request_imgset(_user_id, QString(""), _tablename);
        value = QVector< QVector<int> >(vec_imgset.front().records_size(), QVector<int>(vec_imgset.front().records_size(), 0));
        for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
            value = PUtils::matrix_add(value, (*iter).group_matrix());
    }
    else if (_user_id==0)
    {
        QVector<PImgSet> qvec_imgset;
        std::vector<int> vec_users = get_users(_grammar, _tablename);
        for (std::vector<int>::iterator iter = vec_users.begin(); iter!=vec_users.end(); iter++)
        {
            PImgSet imgset_tmp;
            imgset_tmp.load_table((*iter), _grammar);
            qvec_imgset.push_back(imgset_tmp);
        }
        if (!qvec_imgset.isEmpty())
        {
            value = QVector< QVector<int> >(qvec_imgset.front().records_size(), QVector<int>(qvec_imgset.front().records_size(), 0));
            for (QVector<PImgSet>::iterator iter=qvec_imgset.begin(); iter!=qvec_imgset.end(); iter++)
                value = PUtils::matrix_add(value, (*iter).group_matrix());
        }
    }
    else if (_grammar.isEmpty())
    {
        std::vector<PImgSet> vec_imgset = request_imgset(_user_id, QString(""), _tablename);
        value = QVector< QVector<int> >(vec_imgset.front().records_size(), QVector<int>(vec_imgset.front().records_size(), 0));
        for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
            value = PUtils::matrix_add(value, (*iter).group_matrix());
    }
    return value;
}

QVector< QVector<int> > generate_matrix(QString _mxordering_inputfn, int _user_id, QString _grammar, QString _tablename)
{
    QVector< QVector<int> > value = QVector< QVector<int> >(IMAGE_NB, QVector<int>(IMAGE_NB,0));;
    QVector< QVector<int> > qvec_stdordering;
    QVector<int> qvec_loadedindices;
    std::vector<std::vector <std::string> > vecs_filedatas;
    std::stringstream ss;

    PUtils::load(_mxordering_inputfn, ss);
    PUtils::csv_to_data(vecs_filedatas, ss);

    if (vecs_filedatas.size()==1)
    {
        for (std::vector<std::string>::iterator iter=vecs_filedatas.front().begin(); iter!=vecs_filedatas.front().end(); iter++)
        {
            qvec_loadedindices.push_back(QString((*iter).c_str()).toInt());
        }
        if (qvec_loadedindices.size()==IMAGE_NB)
        {
            if ((_user_id==0)&&(_grammar.isEmpty()))
            {
                std::vector<PImgSet> vec_imgset = request_imgset(_user_id, QString(""), _tablename);
                qvec_stdordering = QVector< QVector<int> >(vec_imgset.front().records_size(), QVector<int>(vec_imgset.front().records_size(), 0));
                for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
                    qvec_stdordering = PUtils::matrix_add(qvec_stdordering, (*iter).group_matrix());
            }
            else if (_user_id==0)
            {
                QVector<PImgSet> qvec_imgset;
                std::vector<int> vec_users = get_users(_grammar, _tablename);
                for (std::vector<int>::iterator iter = vec_users.begin(); iter!=vec_users.end(); iter++)
                {
                    PImgSet imgset_tmp;
                    imgset_tmp.load_table((*iter), _grammar);
                    qvec_imgset.push_back(imgset_tmp);
                }
                if (!qvec_imgset.isEmpty())
                {
                    qvec_stdordering = QVector< QVector<int> >(qvec_imgset.front().records_size(), QVector<int>(qvec_imgset.front().records_size(), 0));
                    for (QVector<PImgSet>::iterator iter=qvec_imgset.begin(); iter!=qvec_imgset.end(); iter++)
                        qvec_stdordering = PUtils::matrix_add(qvec_stdordering, (*iter).group_matrix());
                }
            }
            else if (_grammar.isEmpty())
            {
                std::vector<PImgSet> vec_imgset = request_imgset(_user_id, QString(""), _tablename);
                qvec_stdordering = QVector< QVector<int> >(vec_imgset.front().records_size(), QVector<int>(vec_imgset.front().records_size(), 0));
                for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
                    qvec_stdordering = PUtils::matrix_add(qvec_stdordering, (*iter).group_matrix());
            }
            for (int j=0; j<IMAGE_NB; j++)
            {
                for (int k=0; k<IMAGE_NB; k++)
                {
                    value[j][k]=qvec_stdordering[qvec_loadedindices[j]-1][qvec_loadedindices[k]-1];
                }
            }
        }
        else
        {
            std::cout << std::endl << "Matrix Hardcoded Ordering Generation Failed!";
        }
    }

    return value;
}

QVector< QVector< QVector<int> > > generate_switchedmatrix(int _user_id, QString _grammar, QString _tablename)
{
    QVector< QVector< QVector<int> > > value;
    QVector< QVector<int> > qvec_stdordering, qvec_tmp;
    uint ordering[5][IMAGE_NB] = {  {1,  4,  7,  2,  5,  8,  3,  6,  9, 10, 13, 16, 11, 14, 17, 12, 15, 18, 19, 22, 25, 20, 23, 26, 21, 24, 27},
                                    {1,  4,  7, 10, 13, 16, 19, 22, 25,  2,  5,  8, 11, 14, 17, 20, 23, 26,  3,  6,  9, 12, 15, 18, 21, 24, 27},
                                    {1,  2,  3, 10, 11, 12, 19, 20, 21,  4,  5,  6, 13, 14, 15, 22, 23, 24,  7,  8,  9, 16, 17, 18, 25, 26, 27},
                                    {1, 10, 19,  2, 11, 20,  3, 12, 21,  4, 13, 22,  5, 14, 23,  6, 15, 24,  7, 16, 25,  8, 17, 26,  9, 18, 27},
                                    {1, 10, 19,  4, 13, 22,  7, 16, 25,  2, 11, 20,  5, 14, 23,  8, 17, 26,  3, 12, 21,  6, 15, 24,  9, 18, 27}
                           };

    if ((_user_id==0)&&(_grammar.isEmpty()))
    {
        std::vector<PImgSet> vec_imgset = request_imgset(_user_id, QString(""), _tablename);
        qvec_stdordering = QVector< QVector<int> >(vec_imgset.front().records_size(), QVector<int>(vec_imgset.front().records_size(), 0));
        for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
            qvec_stdordering = PUtils::matrix_add(qvec_stdordering, (*iter).group_matrix());
    }
    else if (_user_id==0)
    {
        QVector<PImgSet> qvec_imgset;
        std::vector<int> vec_users = get_users(_grammar, _tablename);
        for (std::vector<int>::iterator iter = vec_users.begin(); iter!=vec_users.end(); iter++)
        {
            PImgSet imgset_tmp;
            imgset_tmp.load_table((*iter), _grammar);
            qvec_imgset.push_back(imgset_tmp);
        }
        if (!qvec_imgset.isEmpty())
        {
            qvec_stdordering = QVector< QVector<int> >(qvec_imgset.front().records_size(), QVector<int>(qvec_imgset.front().records_size(), 0));
            for (QVector<PImgSet>::iterator iter=qvec_imgset.begin(); iter!=qvec_imgset.end(); iter++)
                qvec_stdordering = PUtils::matrix_add(qvec_stdordering, (*iter).group_matrix());
        }
    }
    else if (_grammar.isEmpty())
    {
        std::vector<PImgSet> vec_imgset = request_imgset(_user_id, QString(""), _tablename);
        qvec_stdordering = QVector< QVector<int> >(vec_imgset.front().records_size(), QVector<int>(vec_imgset.front().records_size(), 0));
        for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
            qvec_stdordering = PUtils::matrix_add(qvec_stdordering, (*iter).group_matrix());
    }
    value.push_back(qvec_stdordering);
    if (qvec_stdordering.size()==IMAGE_NB)
    {
        try
        {
            for (int i=0; i<5; i++)
            {
                qvec_tmp.clear();
                qvec_tmp = QVector< QVector<int> >(IMAGE_NB, QVector<int>(IMAGE_NB,0));
                for (int j=0; j<IMAGE_NB; j++)
                {
                    for (int k=0; k<IMAGE_NB; k++)
                    {
                        qvec_tmp[j][k]=qvec_stdordering[ordering[i][j]-1][ordering[i][k]-1];
                    }
                }
                value.push_back(qvec_tmp);
            }
        }
        catch(...)
        {
            std::cout << std::endl << "Matrix permutation failed!";
        }
    }

    return value;
}

QVector< QVector< QVector<int> > > generate_resampldmatrix(std::vector< std::vector<uint> > _vecs_indices, int _user_id, QString _grammar, QString _tablename)
{
    QVector< QVector< QVector<int> > > value;
    QVector< QVector<int> > qvec_stdordering, qvec_tmp;

    if ((_user_id==0)&&(_grammar.isEmpty()))
    {
        std::vector<PImgSet> vec_imgset = request_imgset(_user_id, QString(""), _tablename);
        qvec_stdordering = QVector< QVector<int> >(vec_imgset.front().records_size(), QVector<int>(vec_imgset.front().records_size(), 0));
        for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
            qvec_stdordering = PUtils::matrix_add(qvec_stdordering, (*iter).group_matrix());
    }
    else if (_user_id==0)
    {
        QVector<PImgSet> qvec_imgset;
        std::vector<int> vec_users = get_users(_grammar, _tablename);
        for (std::vector<int>::iterator iter = vec_users.begin(); iter!=vec_users.end(); iter++)
        {
            PImgSet imgset_tmp;
            imgset_tmp.load_table((*iter), _grammar);
            qvec_imgset.push_back(imgset_tmp);
        }
        if (!qvec_imgset.isEmpty())
        {
            qvec_stdordering = QVector< QVector<int> >(qvec_imgset.front().records_size(), QVector<int>(qvec_imgset.front().records_size(), 0));
            for (QVector<PImgSet>::iterator iter=qvec_imgset.begin(); iter!=qvec_imgset.end(); iter++)
                qvec_stdordering = PUtils::matrix_add(qvec_stdordering, (*iter).group_matrix());
        }
    }
    else if (_grammar.isEmpty())
    {
        std::vector<PImgSet> vec_imgset = request_imgset(_user_id, QString(""), _tablename);
        qvec_stdordering = QVector< QVector<int> >(vec_imgset.front().records_size(), QVector<int>(vec_imgset.front().records_size(), 0));
        for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
            qvec_stdordering = PUtils::matrix_add(qvec_stdordering, (*iter).group_matrix());
    }
    value.push_back(qvec_stdordering);

    if (qvec_stdordering.size()==IMAGE_NB)
    {
        try
        {
            for (int i=1; i<_vecs_indices.size(); i++)
            {
                if (_vecs_indices.at(i).size()!=IMAGE_NB)
                {
                    std::cout << std::endl << "Unexpected number of images...";
                    continue;
                }
                qvec_tmp.clear();
                qvec_tmp = QVector< QVector<int> >(IMAGE_NB, QVector<int>(IMAGE_NB,0));
                for (int j=0; j<IMAGE_NB; j++)
                {
                    for (int k=0; k<IMAGE_NB; k++)
                    {
                        qvec_tmp[j][k]=qvec_stdordering[_vecs_indices[i][j]][_vecs_indices[i][k]];
                    }
                }
                value.push_back(qvec_tmp);
            }
        }
        catch(...)
        {
            std::cout << std::endl << "Matrix permutation failed!";
        }
    }

    return value;
}

//QVector< QVector<float> >   generate_diffmatrix(int _user_id, QString _grammar, QString _imggrptable, QString _qlfrtable)
//{
//    QVector< QVector<float> > value(IMAGE_NB, QVector<float>(IMAGE_NB, 0.0f));
//    std::vector<PImgSet> vec_imgset;
//    std::vector<PQuestionnaire> vec_quest;
//    uint grpid_tmp = 0;
//    int uid_tmp = 0;
//    QString grammar_tmp;

//    vec_imgset = request_imgset(_user_id, _grammar, _imggrptable);
//    vec_quest  = request_questnr(_user_id, _grammar, _qlfrtable);

//    for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
//    {
//        QVector< QVector<float> > mx_tmp(IMAGE_NB, QVector<float>(IMAGE_NB, 0.0f));
//        PImgSet imgset = (*iter);
//        uid_tmp = (*iter).getuserid();
//        grammar_tmp = (*iter).getgrammar();
//        try
//        {
//            for (int i=0; i<IMAGE_NB; i++)
//            {
//                for (int j=0; j<IMAGE_NB; j++)
//                {
//                    if ((*iter).check_groupid(i+1)==(*iter).check_groupid(j+1))
//                    {
//                        mx_tmp[i][j]=1;
//                    }
//                    else
//                    {
//                        std::vector<PQuestionnaire>::iterator jter;
//                        for (jter=vec_quest.begin(); jter!=vec_quest.end(); jter++)
//                        {
//                            if (((*jter).getuserid()==(*iter).getuserid())&&((*iter).getgrammar()==(*jter).getgrammar()))
//                                break;
//                        }
//                        mx_tmp[i][j]=1;
//                        if (jter!=vec_quest.end())
//                        {
//                            mx_tmp[i][j]=(float)PROCEDURAL_MAX((*jter).check_diff( (*iter).check_groupid(i+1)), (*jter).check_diff( (*iter).check_groupid(j+1)));
//                        }
//                    }
//                }

//            }
//            value = PUtils::matrix_add(value, mx_tmp);
//        }
//        catch (...)
//        {
//            std::cout << std::endl << "Difference matrix failed for user ID "
//                      << uid_tmp   << " with the grammar " << grammar_tmp.toStdString() << ".";
//        }
//    }
//    if (vec_imgset.size()!=0)
//    {
//        try
//        {
//            for (int i=0; i<IMAGE_NB; i++)
//            {
//                for (int j=0; j<IMAGE_NB; j++)
//                {
//                    value[i][j]/=vec_imgset.size();
//                }
//            }
//        }
//        catch(...)
//        {
//            std::cout << std::endl << "Matrix elements were not calculated for average difference!";
//        }
//    }
//    return value;
//}

std::vector<PImgSet> request_imgset(int _user_id, QString _grammar, QString _tablename)
{
    std::vector<PImgSet> value;
    std::vector< std::vector<std::string> >::iterator iter_data, jter_data;
    std::vector< std::vector<std::string> > vecs_datas;
    std::vector<std::string> vec_grmmrstr;
    std::vector<int> vec_usrs;
    std::stringstream ss;
    QString qry_str("SELECT * FROM " + _tablename);
    const QString tmp_filename(_tablename+".tmp");
    const int UID_IDX=1, GRMMR_IDX=2;
    QSqlQueryModel* model = new QSqlQueryModel();

    model->setQuery(qry_str);
    PUtils::model_to_csv(tmp_filename.toStdString(), model);
    PUtils::load(tmp_filename, ss);
    PUtils::csv_to_data(vecs_datas, ss);

    jter_data = iter_data = vecs_datas.end();
    if (!vecs_datas.empty())
    {
        if (
             (vecs_datas.front()[GRMMR_IDX].compare("grammar")==0)
             &&
             (vecs_datas.front()[UID_IDX].compare("user_id")==0)
           )
            jter_data = iter_data = vecs_datas.begin()+1;
    }


    if ((_user_id == 0)&&(_grammar.isEmpty()))
    {
        while (iter_data!=vecs_datas.end())
        {
            try
            {
                int uid_tmp = QString((*iter_data)[UID_IDX].c_str()).toInt();
                if (std::find(vec_usrs.begin(), vec_usrs.end(), uid_tmp) == vec_usrs.end())
                {
                    vec_usrs.push_back(uid_tmp);
                }
            }
            catch(...)
            {
                std::cout << std::endl << "Could not load User ID from Table!";
            }
            iter_data++;
        }

        for (std::vector<int>::iterator iter_uid = vec_usrs.begin(); iter_uid!=vec_usrs.end(); iter_uid++)
        {
            try
            {
                vec_grmmrstr = get_grammars(*iter_uid);
                for (std::vector<std::string>::iterator iter_grmmr=vec_grmmrstr.begin(); iter_grmmr!=vec_grmmrstr.end(); iter_grmmr++)
                {
                    PImgSet imgset;
                    imgset.load_table((*iter_uid), (*iter_grmmr).c_str(), _tablename);
                    value.push_back(imgset);
                }
            }
            catch(...)
            {
                std::cout << std::endl << "Table Loading Failure on User " << (iter_uid-vec_usrs.begin()) << ".";
            }
        }
    }
    else if (_user_id == 0)
    {
        vec_usrs = get_users(_grammar);
        for (std::vector<int>::iterator iter_uid = vec_usrs.begin(); iter_uid!=vec_usrs.end(); iter_uid++)
        {
            PImgSet imgset;
            imgset.load_table((*iter_uid), _grammar);
            value.push_back(imgset);
        }
    }
    else if (_grammar.isEmpty())
    {
        iter_data = jter_data;
        vec_grmmrstr = get_grammars(_user_id);
        for (std::vector<std::string>::iterator iter_grmmr=vec_grmmrstr.begin(); iter_grmmr!=vec_grmmrstr.end(); iter_grmmr++)
        {
            try
            {
                PImgSet imgset;
                imgset.load_table(_user_id, (*iter_grmmr).c_str(), _tablename);
                value.push_back(imgset);
            }
            catch(...)
            {
                 std::cout << std::endl << "Table Loading Failure on User " << _user_id << ".";
            }
        }
    }
    else
    {
        try
        {
            PImgSet imgset;
            imgset.load_table(_user_id, _grammar, _tablename);
            value.push_back(imgset);
        }
        catch(...)
        {
             std::cout << std::endl << "Table Loading Failure on User " << _user_id << ".";
        }
    }
    return value;
}

std::vector<PQuestionnaire> request_questnr(int _user_id, QString _grammar, QString _tablename)
{
    std::vector<PQuestionnaire> value;
    std::vector< std::vector<std::string> >::iterator iter_data, jter_data;
    std::vector< std::vector<std::string> > vecs_datas;
    std::vector<std::string> vec_grmmrstr;
    std::vector<int> vec_usrs;
    std::stringstream ss;
    QString qry_str("SELECT * FROM " + _tablename);
    const QString tmp_filename(_tablename+".tmp");
    const int UID_IDX=1, GRMMR_IDX=2;
    QSqlQueryModel* model = new QSqlQueryModel();

    model->setQuery(qry_str);
    PUtils::model_to_csv(tmp_filename.toStdString(), model);
    PUtils::load(tmp_filename, ss);
    PUtils::csv_to_data(vecs_datas, ss);

    jter_data = iter_data = vecs_datas.end();
    if (!vecs_datas.empty())
    {
        if (
             (vecs_datas.front()[GRMMR_IDX].compare("grammar")==0)
             &&
             (vecs_datas.front()[UID_IDX].compare("user_id")==0)
           )
            jter_data = iter_data = vecs_datas.begin()+1;
    }


    if ((_user_id == 0)&&(_grammar.isEmpty()))
    {
        while (iter_data!=vecs_datas.end())
        {
            try
            {
                int uid_tmp = QString((*iter_data)[UID_IDX].c_str()).toInt();
                if (std::find(vec_usrs.begin(), vec_usrs.end(), uid_tmp) == vec_usrs.end())
                {
                    vec_usrs.push_back(uid_tmp);
                }
            }
            catch(...)
            {
                std::cout << std::endl << "Could not load User ID from Table!";
            }
            iter_data++;
        }

        for (std::vector<int>::iterator iter_uid = vec_usrs.begin(); iter_uid!=vec_usrs.end(); iter_uid++)
        {
            try
            {
                vec_grmmrstr = get_grammars(*iter_uid);
                for (std::vector<std::string>::iterator iter_grmmr=vec_grmmrstr.begin(); iter_grmmr!=vec_grmmrstr.end(); iter_grmmr++)
                {
                    PQuestionnaire pquest;
                    pquest.load_table((*iter_uid), (*iter_grmmr).c_str(), _tablename);
                    value.push_back(pquest);
                }
            }
            catch(...)
            {
                std::cout << std::endl << "Table Loading Failure on User " << (iter_uid-vec_usrs.begin()) << ".";
            }
        }
    }
    else if (_user_id == 0)
    {
        vec_usrs = get_users(_grammar);
        for (std::vector<int>::iterator iter_uid = vec_usrs.begin(); iter_uid!=vec_usrs.end(); iter_uid++)
        {
            PQuestionnaire pquest;
            pquest.load_table((*iter_uid), _grammar);
            value.push_back(pquest);
        }
    }
    else if (_grammar.isEmpty())
    {
        iter_data = jter_data;
        vec_grmmrstr = get_grammars(_user_id);
        for (std::vector<std::string>::iterator iter_grmmr=vec_grmmrstr.begin(); iter_grmmr!=vec_grmmrstr.end(); iter_grmmr++)
        {
            try
            {
                PQuestionnaire pquest;
                pquest.load_table(_user_id, (*iter_grmmr).c_str(), _tablename);
                value.push_back(pquest);
            }
            catch(...)
            {
                 std::cout << std::endl << "Table Loading Failure on User " << _user_id << ".";
            }
        }
    }
    else
    {
        try
        {
            PQuestionnaire pquest;
            pquest.load_table(_user_id, _grammar, _tablename);
            value.push_back(pquest);
        }
        catch(...)
        {
             std::cout << std::endl << "Table Loading Failure on User " << _user_id << ".";
        }
    }
    return value;
}

void qualify(int _user_id, QString _grammar, std::vector<uint> _vec_groups, const QString _tablename)
{
    PQuestionnaire quest(_user_id, _grammar);
    PQualifier qlf;
    std::stringstream ss;
    char c;

    ss  << std::endl
        << "'E' or '4' for Excellent,"
        << "\t'G' or '3' for Good," << std::endl
        << "'F' or '2' for Fair,"
        << "\t\t'B' or '1' for Bad.";

    std::cout << std::endl << "Processing qualifiers for User ID "
                 << _user_id << " with the grammar '"
                 << _grammar.toStdString() << "'.";
    std::cout << std::endl << _vec_groups.size() << " groups will be examined.";
    std::cout << std::endl << "To enter the ratings for the 3 first questions, you can type: ";
    std::cout << ss.str();

    for (std::vector<uint>::iterator iter=_vec_groups.begin(); iter!=_vec_groups.end(); iter++)
    {
        int uf, diff, amb;
        uint gid = (*iter);
        std::string qlfr_str;
        std::cout << std::endl << std::endl << ">>>>";
        std::cout << std::endl;
        std::cout << std::endl << "Please enter the Usefulness of the group " << *iter << ">";
        uf = get_qlfr();

        std::cout << std::endl;
        std::cout << std::endl << "Please enter the Difference ratio for the group " << *iter << ">";
        diff = get_qlfr();

        std::cout << std::endl;
        std::cout << std::endl << "Please rate the Ambassador of the group " << *iter << ">";
        amb = get_qlfr();

        std::cout << std::endl;
        std::cout << std::endl << "Please enter user specified qualifier for group " << *iter << ">";
        qlfr_str = get_customqlfr();

        qlf = PQualifier(gid, uf, diff, amb, qlfr_str.c_str());
        quest.add_qualifier(qlf);
    }

    std::cout << std::endl << std::endl << ">>>>";
    std::cout << std::endl << "Please confirm if Yes [y] or No [n], you want to update the database>";
    std::cin >> c;

    while  ((toupper(c)!='Y')&&(c!='N'))
    {
        std::cout << std::endl << "Please type 'y' for Yes, or 'n' for No>";
        std::cin >> c;
    }
    if (toupper(c)=='Y')
    {
        quest.writedb(_tablename);
    }
}


int get_qlfr()
{
    int value=0;
    char c;
    std::stringstream ss;
    ss  << "Please Type:" << std::endl
        << "'E' or '4' for Excellent,"
        << "\t'G' or '3' for Good," << std::endl
        << "'F' or '2' for Fair,"
        << "\t\t'B' or '1' for Bad."
        << std::endl;

    std::cin >> c;

    while  (
                (toupper(c)!='E')&&(c!='4')
                &&
                (toupper(c)!='G')&&(c!='3')
                &&
                (toupper(c)!='F')&&(c!='2')
                &&
                (toupper(c)!='B')&&(c!='1')
          )
    {
        std::cout << std::endl << ss.str();
        std::cin >> c;
    }
    if ((toupper(c)=='E')||(c=='4'))
    {
        value = 4;
        std::cout << "[Excellent]";
    }
    else if ((toupper(c)=='G')||(c=='3'))
    {
        value = 3;
        std::cout << "[Good]";
    }
    else if ((toupper(c)=='F')||(c=='2'))
    {
        value = 2;
        std::cout << "[Fair]";
    }
    else if ((toupper(c)=='B')||(c=='1'))
    {
        value = 1;
        std::cout << "[Bad]";
    }
    return value;
}

std::string get_customqlfr()
{
    std::string value;
    std::cout << std::endl << "Note that the qualifiers, if more than one, should be separated by semicolon."
              << std::endl << "For database and CSV purpose, the usage of comma is not permitted!";
    std::cout << std::endl;
    std::cin.clear();
    std::cin.ignore(1);
    std::getline(std::cin, value);
    while (value.find(',')!=std::string::npos)
    {
        std::cout << std::endl << "Comma (',') is not allowed. Please reenter qualifier(s):" << std::endl ;
        std::getline(std::cin, value);
    }
    return value;
}

std::vector<float> compute_usefulness(int _user_id, QString _grammar, QString _imggrptable, QString _qlfrtable)
{
    std::vector<float> value(IMAGE_NB, 0);
    std::vector<PImgSet> vec_imgset;
    std::vector<PQuestionnaire> vec_quest;
    uint grpid_tmp = 0;
    int uid_tmp = 0;
    QString grammar_tmp;

    vec_imgset = request_imgset(_user_id, _grammar, _imggrptable);
    vec_quest  = request_questnr(_user_id, _grammar, _qlfrtable);

    for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
    {
        PImgSet imgset = (*iter);
        uid_tmp = (*iter).getuserid();
        grammar_tmp = (*iter).getgrammar();
        try
        {
            for (int i=0; i<IMAGE_NB; i++)
            {
                grpid_tmp = imgset.check_groupid(i+1);
                for (std::vector<PQuestionnaire>::iterator jter=vec_quest.begin(); jter!=vec_quest.end(); jter++)
                {
                    if ( ((*jter).getuserid()==uid_tmp) && ((*jter).getgrammar().compare(grammar_tmp)==0) )
                    {
                        value[i]+=(*jter).check_usflnss(grpid_tmp);
                        break;
                    }
                }
            }
        }
        catch (...)
        {
            std::cout << std::endl << "Usefulness computation failed for user ID "
                      << uid_tmp   << " with the grammar " << grammar_tmp.toStdString() << ".";
        }
    }
    try
    {
        for (std::vector<float>::iterator iter=value.begin(); iter!=value.end(); iter++)
            (*iter) /= vec_imgset.size();
    }
    catch(...)
    {
        std::cout << std::endl << "Warning: average usefulness calculation failed!";
    }

    return value;
}

std::vector<float> compute_difference(int _user_id, QString _grammar, QString _imggrptable, QString _qlfrtable)
{
    std::vector<float> value(IMAGE_NB, 0);
    std::vector<PImgSet> vec_imgset;
    std::vector<PQuestionnaire> vec_quest;
    uint grpid_tmp = 0;
    int uid_tmp = 0;
    QString grammar_tmp;

    vec_imgset = request_imgset(_user_id, _grammar, _imggrptable);
    vec_quest  = request_questnr(_user_id, _grammar, _qlfrtable);

    for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
    {
        PImgSet imgset = (*iter);
        uid_tmp = (*iter).getuserid();
        grammar_tmp = (*iter).getgrammar();
        try
        {
            for (int i=0; i<IMAGE_NB; i++)
            {
                grpid_tmp = imgset.check_groupid(i+1);
                for (std::vector<PQuestionnaire>::iterator jter=vec_quest.begin(); jter!=vec_quest.end(); jter++)
                {
                    if ( ((*jter).getuserid()==uid_tmp) && ((*jter).getgrammar().compare(grammar_tmp)==0) )
                    {
                        value[i]+=(*jter).check_diff(grpid_tmp);
                        break;
                    }
                }
            }
        }
        catch (...)
        {
            std::cout << std::endl << "Difference computation failed for user ID "
                      << uid_tmp   << " with the grammar " << grammar_tmp.toStdString() << ".";
        }
    }
    try
    {
        for (std::vector<float>::iterator iter=value.begin(); iter!=value.end(); iter++)
            (*iter) /= vec_imgset.size();
    }
    catch(...)
    {
        std::cout << std::endl << "Warning: average difference calculation failed!";
    }
    return value;
}

QVector<QStringList*> check_qualifiers(int _user_id, QString _grammar, QString _imggrptable, QString _qlfrtable)
{
    QVector<QStringList*> value;
    std::vector<PImgSet> vec_imgset;
    std::vector<PQuestionnaire> vec_quest;
    uint grpid_tmp = 0;
    int uid_tmp = 0;
    QString grammar_tmp;

    vec_imgset = request_imgset(_user_id, _grammar, _imggrptable);
    vec_quest  = request_questnr(_user_id, _grammar, _qlfrtable);

    for (int i=0; i<IMAGE_NB; i++)
        value.push_back(new QStringList());

    for (std::vector<PImgSet>::iterator iter=vec_imgset.begin(); iter!=vec_imgset.end(); iter++)
    {
        PImgSet imgset = (*iter);
        uid_tmp = (*iter).getuserid();
        grammar_tmp = (*iter).getgrammar();
        try
        {
            for (int i=0; i<IMAGE_NB; i++)
            {
                grpid_tmp = imgset.check_groupid(i+1);
                for (std::vector<PQuestionnaire>::iterator jter=vec_quest.begin(); jter!=vec_quest.end(); jter++)
                {
                    if ( ((*jter).getuserid()==uid_tmp) && ((*jter).getgrammar().compare(grammar_tmp)==0) )
                    {
                        *(value.at(i)) << (*jter).check_qlfr(grpid_tmp);
                        break;
                    }
                }
            }
        }
        catch (...)
        {
            std::cout << std::endl << "Qualifiers researching failed for user ID "
                      << uid_tmp   << " with the grammar " << grammar_tmp.toStdString() << ".";
        }
    }
    return value;
}

void load_wrappers(std::vector<float>& _vec_extremum, std::vector< std::vector<float> >& _vec_nearestpt)
{
    QString qs_targetfn(promocmd.grammar.c_str()), qs_nearestfn(promocmd.grammar.c_str());
    std::stringstream ss_targetwrap, ss_wrapper;
    std::vector<std::vector <std::string> > vecs_targetwrapstr, vecs_wrapstr;
    std::vector <float> vec_tmp;

    qs_targetfn+="_targetwrap.tmp";
    qs_nearestfn+="_wrapper.tmp";
    PUtils::load(qs_targetfn, ss_targetwrap);
    PUtils::load(qs_nearestfn, ss_wrapper);

    PUtils::csv_to_data(vecs_targetwrapstr, ss_targetwrap);
    PUtils::csv_to_data(vecs_wrapstr, ss_wrapper);

    for (std::vector<std::vector <std::string> >::iterator iter=vecs_targetwrapstr.begin();iter!=vecs_targetwrapstr.end();iter++)
    {
        if ((*iter).size()<6)
        {
            qDebug() << "One Target Wrapper was not processed!";
        }
        else
        {
            _vec_extremum.clear();
            _vec_extremum.push_back( QString((*iter).at(0).c_str()).toFloat() );
            _vec_extremum.push_back( QString((*iter).at(1).c_str()).toFloat() );
            _vec_extremum.push_back( QString((*iter).at(2).c_str()).toFloat() );
            _vec_extremum.push_back( QString((*iter).at(3).c_str()).toFloat() );
            _vec_extremum.push_back( QString((*iter).at(4).c_str()).toFloat() );
            _vec_extremum.push_back( QString((*iter).at(5).c_str()).toFloat() );
        }
    }
    _vec_nearestpt.clear();
    for (std::vector<std::vector <std::string> >::iterator iter=vecs_wrapstr.begin();iter!=vecs_wrapstr.end();iter++)
    {
        vec_tmp.clear();
        if ((*iter).size()<4)
        {
            qDebug() << "One PCA coordinate was not processed!";
        }
        else
        {
            vec_tmp.push_back(QString((*iter).at(1).c_str()).toFloat());
            vec_tmp.push_back(QString((*iter).at(2).c_str()).toFloat());
            vec_tmp.push_back(QString((*iter).at(3).c_str()).toFloat());
            _vec_nearestpt.push_back(vec_tmp);
        }
    }
}

void load_scattered(std::vector< std::vector<float> >& _vec_scatterplot)
{
    QString qs_scatterplotfn(promocmd.grammar.c_str());
    std::stringstream ss_scatterplot;
    std::vector<std::vector <std::string> > vecs_scatterplotstr;
    std::vector <float> vec_tmp;

    qs_scatterplotfn+="_scatterplot.tmp";
    PUtils::load(qs_scatterplotfn, ss_scatterplot);
    PUtils::csv_to_data(vecs_scatterplotstr, ss_scatterplot);

    _vec_scatterplot.clear();
    for (std::vector<std::vector <std::string> >::iterator iter=vecs_scatterplotstr.begin();iter!=vecs_scatterplotstr.end();iter++)
    {
        vec_tmp.clear();
        if ((*iter).size()<3)
        {
            qDebug() << "One PCA coordinate from the cloud was not processed!";
        }
        else
        {
            vec_tmp.push_back(QString((*iter).at(0).c_str()).toFloat());
            vec_tmp.push_back(QString((*iter).at(1).c_str()).toFloat());
            vec_tmp.push_back(QString((*iter).at(2).c_str()).toFloat());
            _vec_scatterplot.push_back(vec_tmp);
        }
    }
}

void multimx_to_csv(const QVector< QVector< QVector<int> > >& _matrices, QString _filebasename, const uint _mx_ordering[6][IMAGE_NB])
{
    QStringList qsl_ext, qsl_fn;
    std::stringstream ss;
    qsl_ext<<QObject::tr("_1.csv")<<QObject::tr("_2.csv")<<QObject::tr("_3.csv")<<QObject::tr("_4.csv")<<QObject::tr("_5.csv")<<QObject::tr("_6.csv");
    if (_filebasename.endsWith(".csv", Qt::CaseInsensitive)==true)
    {
        _filebasename = _filebasename.mid(0, _filebasename.size()-4);
    }
    if (_matrices.size()==qsl_ext.size())
    {
        for (int i=0; i<qsl_ext.size(); i++)
        {
            qsl_fn << _filebasename + qsl_ext.at(i);
            ss.str(std::string());
            ss << 0;
            for (int j=0; j<IMAGE_NB; j++)
            {
                ss << "," << _mx_ordering[i][j];
            }
            ss << std::endl;
            for (int j=0; j<IMAGE_NB; j++)
            {
                ss << _mx_ordering[i][j];
                for (int k=0; k<IMAGE_NB; k++)
                {
                    ss << "," << _matrices[i][j][k];
                }
                ss << std::endl;
            }
            PUtils::save(qsl_fn[i],ss);
        }
    }
}

void multimx_to_csv(const QVector< QVector< QVector<int> > >& _matrices, QString _filebasename, std::vector< std::vector<uint> > _vec_headeridx)
{
    QStringList qsl_ext, qsl_fn;
    std::stringstream ss;
    if (_filebasename.isEmpty())
        _filebasename = QString("tmp");
    for (int i=0; i<_vec_headeridx.size(); i++)
        qsl_ext << QString("_mx_%1.csv").arg(i+1);

    if (_filebasename.endsWith(".csv", Qt::CaseInsensitive)==true)
    {
        _filebasename = _filebasename.mid(0, _filebasename.size()-4);
    }
    if (_matrices.size()==qsl_ext.size())
    {
        for (int i=0; i<qsl_ext.size(); i++)
        {
            qsl_fn << _filebasename + qsl_ext.at(i);
            ss.str(std::string());
            ss << 0;
            for (int j=0; j<IMAGE_NB; j++)
            {
                ss << "," << _vec_headeridx[i][j];
            }
            ss << std::endl;
            for (int j=0; j<IMAGE_NB; j++)
            {
                ss << _vec_headeridx[i][j];
                for (int k=0; k<IMAGE_NB; k++)
                {
                    ss << "," << _matrices[i][j][k];
                }
                ss << std::endl;
            }
            PUtils::save(qsl_fn[i],ss);
        }
    }
}

void qlfrs_to_csv(QString _tablename)
{
    const QString QS_BEFORE(";"), QS_AFTER("\n");
    QString qs_outputfn("qlfrs_only.csv");
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT qualifier FROM " + _tablename);

    QFile file(qs_outputfn);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream ts(&file);
        while (model->canFetchMore())
             model->fetchMore();
        //std::cout << std::endl << _model->rowCount() << " rows from model";
        for (int i=0; i<model->rowCount(); i++)
        {
            QSqlRecord record = model->record(i);
            for (int j=0; j<record.count(); j++)
            {

                QString qs_record = record.field(j).value().toString();
                qs_record.replace(QS_BEFORE, QS_AFTER);
                if (j>0)
                    ts << ",";
                ts << qs_record;
            }
            ts << "\n";
        }
        file.close();
    }
}

void qlfrs_to_csv_annotated(QString _tablename)
{
    const QString QS_BEFORE(";"), QS_AFTER("\n");
    const QString tmp_filename(_tablename+".tmp");
    std::vector< std::vector<std::string> > vecs_datas;
    const int USF_IDX=0, QLFR_IDX=1, UID_IDX=2, GRM_IDX=3;
    QString qs_outputfn("qlfrs_only_annotated.csv");
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT usefulness,qualifier,user_id,grammar FROM " + _tablename);

    QFile file(qs_outputfn);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream ts(&file);
        while (model->canFetchMore())
             model->fetchMore();
        //std::cout << std::endl << _model->rowCount() << " rows from model";
        for (int i=0; i<model->rowCount(); i++)
        {
            QSqlRecord record = model->record(i);
            //std::cout << std::endl << record.count() << " records.";
            if (record.count()>=4)
            {
                QString qs_newafter(    ","+record.field(USF_IDX).value().toString()+
                                        ","+record.field(UID_IDX).value().toString()+
                                        ","+record.field(GRM_IDX).value().toString()+
                                        QS_AFTER);
                //std::cout << std::endl << record.field(USF_IDX).value().toString().toStdString() << " & " << record.field(QLFR_IDX).value().toString().toStdString();
                QString qs_record = record.field(QLFR_IDX).value().toString();
                qs_record.replace(QS_BEFORE, qs_newafter);

                if (!qs_record.endsWith("\n"))
                {
                    qs_record+=",";
                    qs_record+=record.field(USF_IDX).value().toString()+",";
                    qs_record+=record.field(UID_IDX).value().toString()+",";
                    qs_record+=record.field(GRM_IDX).value().toString()+QS_AFTER;
                }
                ts << qs_record;
            }
        }
    }
    file.close();
}

void save_qlfr_coords(std::vector<uint> _vec_resampld_idx, QVector<QStringList*> _qvec_qlfrs)
{
    /*
        We will process reading using C++ standards but will perform writing using Qt interface
    */
    QString qfn_qlfrcoords("qlfr_coords.csv");

    std::string fn_wrapper;
    std::stringstream ss;
    std::vector< std::vector<std::string> > vecs_datas;
    std::vector<float> vec_x, vec_y, vec_z;

    //if (promocmd.userid==0) return;
    if (promocmd.grammar.empty()==true) return;
    if (_qvec_qlfrs.size()!=_vec_resampld_idx.size()) return;

    fn_wrapper = promocmd.grammar + "_wrapper.tmp";
    std::ifstream wrapperfile(fn_wrapper.c_str());
    ss << wrapperfile.rdbuf();
    wrapperfile.close();
    PUtils::csv_to_data(vecs_datas, ss);
    for (std::vector< std::vector<std::string> >::iterator iter = vecs_datas.begin(); iter != vecs_datas.end(); iter++)
    {
        if ((*iter).size()>=4)
        {
            vec_x.push_back(PUtils::stdstr_to_float((*iter)[1]));
            vec_y.push_back(PUtils::stdstr_to_float((*iter)[2]));
            vec_z.push_back(PUtils::stdstr_to_float((*iter)[3]));
        }
    }


    QFile file_qlfrcoords(qfn_qlfrcoords);
    if (file_qlfrcoords.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        for (size_t i=0; i<_qvec_qlfrs.size(); i++)
        {
            // Per user isolation
            QStringListIterator qsliter(*_qvec_qlfrs.at(i));
            while (qsliter.hasNext())
            {
                // Per qualifier isolation
                QString qlfr_tmp = qsliter.next();
                QStringList qsl_monouserqlfr = qlfr_tmp.split(";");
                QStringListIterator qsljter(qsl_monouserqlfr);
                while (qsljter.hasNext())
                {
                    file_qlfrcoords.write(PUtils::float_to_stdstr(vec_x.at(_vec_resampld_idx.at(i)-1)).c_str());
                    file_qlfrcoords.write(",");
                    file_qlfrcoords.write(PUtils::float_to_stdstr(vec_y.at(_vec_resampld_idx.at(i)-1)).c_str());
                    file_qlfrcoords.write(",");
                    file_qlfrcoords.write(PUtils::float_to_stdstr(vec_z.at(_vec_resampld_idx.at(i)-1)).c_str());
                    file_qlfrcoords.write(",");
                    file_qlfrcoords.write(promocmd.grammar.c_str());
                    file_qlfrcoords.write(",");
                    file_qlfrcoords.write(qsljter.next().toStdString().c_str());
                    file_qlfrcoords.write("\n");
                }
            }
        }
        file_qlfrcoords.close();
    }
}

std::vector<PBoundingBox> qlfr_obb(const QStringList &_qlfr_list, std::vector<std::vector<std::vector<GLfloat> > > &_vecs_obbclouds)
{
    std::vector<PBoundingBox> value;

    QString qlfr_tmp;
    QStringListIterator qsli(_qlfr_list);
    std::ifstream file("qlfr_coords.csv");
    std::stringstream buffer;
    std::vector< std::vector<std::string> > vecs_datas;
    std::vector<CGLA::Vec3f> vec_vertices;

    buffer << file.rdbuf();
    file.close();
    PUtils::csv_to_data(vecs_datas, buffer);

    //_vecs_obbclouds.clear();
    while (qsli.hasNext())
    {
        PBoundingBox* pobb = new PBoundingBox();
        std::vector< std::vector<GLfloat> > vecs_tmp;
        qlfr_tmp = qsli.next();
        vec_vertices.clear();
        for (std::vector< std::vector<std::string> >::iterator iter = vecs_datas.begin(); iter != vecs_datas.end(); iter++)
        {
            if ((*iter).size()>=5)
            {
                std::string qlfr_data = (*iter)[4];
                boost::algorithm::to_lower(qlfr_data);
                //if (qlfr_data.find(qlfr_tmp.toStdString())!=std::string::npos)
                if (qlfr_data.compare(qlfr_tmp.toStdString())==0)
                {
                    vec_vertices.push_back( CGLA::Vec3f(    PUtils::stdstr_to_float((*iter)[0]),
                                                            PUtils::stdstr_to_float((*iter)[1]),
                                                            PUtils::stdstr_to_float((*iter)[2]) )
                                );
                    //vec_allqlfrstmp.push_back((*iter)[4]);
                }
                // synonyms
                else if (synonyms(qlfr_data, qlfr_tmp.toStdString()))
                {
                    vec_vertices.push_back( CGLA::Vec3f(    PUtils::stdstr_to_float((*iter)[0]),
                                                            PUtils::stdstr_to_float((*iter)[1]),
                                                            PUtils::stdstr_to_float((*iter)[2]) )
                                );
                }
            }
        }
        if (!vec_vertices.empty())
        {
            pobb->compute_corner(vec_vertices);
            value.push_back(*pobb);

            std::vector<GLfloat> vec_tmp;
            for (std::vector<CGLA::Vec3f>::iterator iter_vertices = vec_vertices.begin(); iter_vertices != vec_vertices.end(); iter_vertices++)
            {
                vec_tmp.clear();
                vec_tmp.push_back((*iter_vertices)[0]);
                vec_tmp.push_back((*iter_vertices)[1]);
                vec_tmp.push_back((*iter_vertices)[2]);
                vecs_tmp.push_back(vec_tmp);
            }
//            vec_tmp.push_back(vec_vertices[0]);
//            vec_tmp.push_back(vec_vertices[1]);
//            vec_tmp.push_back(vec_vertices[2]);

            _vecs_obbclouds.push_back(vecs_tmp);

        }
        delete pobb;
    }

    return value;
}

QStringList load_obbqlfr(QString _obbfilter_filename)
{
    QStringList value;
    QFile filterfile(_obbfilter_filename);
    std::stringstream ss;
    std::vector< std::vector<std::string> > vecs_datas;

    if (!filterfile.open(QFile::ReadOnly|QFile::Text))
    {
        std::cout << std::endl << _obbfilter_filename.toStdString().c_str() << ": Could not open file!. Please check if file exists!";
        std::cout << std::endl << "Process Aborted.";
        exit(0);
    }
    QTextStream txtstream(&filterfile);
    ss << txtstream.readAll().toStdString();
    PUtils::csv_to_data(vecs_datas, ss);
    for (std::vector< std::vector<std::string> >::iterator iter = vecs_datas.begin(); iter != vecs_datas.end(); iter++)
    {
        for (std::vector<std::string>::iterator jter = (*iter).begin(); jter!=(*iter).end(); jter++)
        {
            std::string qlfr_tmp = (*jter);
            boost::algorithm::to_lower(qlfr_tmp);
            value << qlfr_tmp.c_str();
        }
    }
    return value;
}

std::vector< std::vector<float> > hsv_colorgenerator(unsigned int _number)
{
    std::vector< std::vector<float> > vec_retval;
    std::vector<float> vec_tmp;
    float saturation = 1, value = 1;
    float hue;
    for (int i = 0; i < _number; i++)
    {
        vec_tmp.clear();
        hue = 240*(1-(float)(i+1)/_number);
        vec_tmp.push_back(hue);
        vec_tmp.push_back(saturation);
        vec_tmp.push_back(value);
        vec_retval.push_back(vec_tmp);
    }
    return vec_retval;
}

std::vector< std::vector<float> > rgb_colorgenerator(unsigned int _number)
{
    std::vector< std::vector<float> > vec_retval, vec_hsvval;
    std::vector<float> vec_tmp;
    float r, g, b;
    vec_hsvval = hsv_colorgenerator(_number);
    for (std::vector< std::vector<float> >::iterator iter = vec_hsvval.begin(); iter != vec_hsvval.end(); iter++)
    {
        if ((*iter).size()>=3)
        {
            vec_tmp.clear();
            hsv_to_rgb(&r, &g, &b, (*iter)[0], (*iter)[1], (*iter)[2]);
            vec_tmp.push_back(r);
            vec_tmp.push_back(g);
            vec_tmp.push_back(b);
            vec_retval.push_back(vec_tmp);
        }
    }

    return vec_retval;
}


QString rgb_to_qstr(float _r, float _g, float _b)
{
    QString value("rgb(");
    int r, g, b;
    r = 255*_r;
    g = 255*_g;
    b = 255*_b;
    value += QString::number(r); value += ",";
    value += QString::number(g); value += ",";
    value += QString::number(b); value += ")";
    return value;
}

bool synonyms(std::string _word1, std::string _word2)
{
    bool value = false;
    std::vector< std::vector<std::string> > vecs_synonyms;
    std::vector<std::string> vec_tmp;

    vec_tmp = boost::assign::list_of("petit")
                                    ("court")
                                    ("sommet court")
                                    ("small")
                                    ("short")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("grand")
                                    ("eleve")
                                    ("large")
                                    ("tall")
                                    ("longue")
                                    ("long")
                                    ("elance")
                                    ("sommet eleve")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("epars")
                                    ("non dense")
                                    ("sparse")
                                    ("sparse leaves")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("dense")
                                    ("touffu")
                                    ("branchu")
                                    ("thick")
                                    ("thin branches")
                                    ("dense branches")
                                    ("thick branches")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("symetrique")
                                    ("bien equilibre")
                                    ("uniforme")
                                    ("symmetric")
                                    ("symmetrical")
                                    ("symetrical")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("asymetrique")
                                    ("asymmetric")
                                    ("asymmetrical")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("realiste")
                                    ("realistic")
                                    ("reel")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("irrealiste")
                                    ("irreel")
                                    ("abstrait")
                                    ("unrealistic")
                                    ("irrealistic")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("sapin")
                                    ("conifere")
                                    ("pointu")
                                    ("fleche")
                                    ("fir")
                                    ("christmas tree")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("mince")
                                    ("thin")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("normal")
                                    ("ordinaire")
                                    ("standard")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("gros")
                                    ("full")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("mince")
                                    ("fin")
                                    ("elance")
                                    ("fine")
                                    ("narrow")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    vec_tmp = boost::assign::list_of("rond")
                                    ("boule")
                                    ("arrondi")
                                    ("port arrondi")
    ;
    vecs_synonyms.push_back(vec_tmp);
    vec_tmp.clear();

    for (std::vector< std::vector<std::string> >::iterator iter = vecs_synonyms.begin(); iter != vecs_synonyms.end(); iter++)
    {
        vec_tmp = (*iter);
        if (    std::find(vec_tmp.begin(), vec_tmp.end(), _word1)!=vec_tmp.end()
                &&
                std::find(vec_tmp.begin(), vec_tmp.end(), _word2)!=vec_tmp.end())
        {
            value = true;
            break;
        }
    }
    return value;
}

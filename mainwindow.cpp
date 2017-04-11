#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <boost/algorithm/string/replace.hpp>

#include <QElapsedTimer>
#include <QMessageBox>
#include <QPalette>
#include <QColorDialog>
#include <QList>

#include "engine/procedural.h"
#include "engine/pderivhistoric.h"
#include "engine/ptextparser.h"
#include "engine/ppcaeval.h"
#include "engine/pgraphcreator.h"
#include "grammar/ppreproc.h"
#include "grammar/palphabet.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "historicitem.h"

#include "viewport/viewer.h"
#include "viewport/viewerraw.h"
#include "viewport/pboundingbox.h"
#include "engine/piorecord.h"
#include "engine/plsystem.h"
#include "engine/plsystemparametric.h"
#include "engine/pmediator.h"

#include "aboutdialog.h"
#include "outputdialog.h"
#include "treeviewdialog.h"
#include "windowtitlebar.h"
#include "ppcaviewer.h"
#include "pimgbrowser.h"
#include "dialogviewer.h"

#include "historicitem.h"
#include "historictreemodel.h"

using namespace std;
using namespace boost::numeric;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    titleBar = new WindowTitleBar(this);
    //ui->labelDebug->setVisible(false);

    timer = new QTimer(this);
    timer->setInterval(500);
    timer->start();
    lsystem     = new PLSystemParametric();
    in_out_rec  = new PIORecord();

    pMainScene  = new Scene(lsystem);
    pMainViewer = new Viewer(pMainScene, CONSTRAINTS_FREE_VIEW, this);
    mediator    = new PMediator(lsystem, pMainScene);
    mod_config  = new PModelingConfig(*lsystem->getAlphabet());
    ui->verticalLayoutViewer->addWidget(pMainViewer);
    pMainViewer->showEntireScene();

    aboutDlg    = new AboutDialog(this);
    outputDlg   = new OutputDialog(this);
    treeviewDlg = new TreeViewDialog(this);
    pcaViewer   = new PPcaViewer(this);
    imgBrowser  = new PImgBrowser(this);
    dlgViewer   = new DialogViewer(this);
    qstd_imodel = new QStandardItemModel();


    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->setMargin(0);  // No space between window's element and the border
    mainLayout->setSpacing(0); // No space between window's element
    setWindowFlags(Qt::FramelessWindowHint);
    ui->menubar->setLayout(mainLayout);
    ui->menubar->setWindowTitle(this->windowTitle());
    ui->tableViewConstVal->verticalHeader()->hide();
    ui->tableViewConstVal->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewConstVal->resizeRowsToContents();
    ui->tableViewMetric->verticalHeader()->hide();
    ui->tableViewMetric->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewMetric->resizeRowsToContents();

    //centralWidget()->setLayout(mainLayout);
    mainLayout->addWidget(titleBar, 0, 0, 1, 1);
    mainLayout->setRowStretch(1, 1); // Put the title bar at the top of the window
    ui->widgetColor->setStyleSheet("border: 1px solid black");

    ui->textEditPreproc->setReadOnly(true);
    ui->textEditProduction->setReadOnly(true);
    ui->lineEditAxiom->setReadOnly(true);

    ui->comboBoxlineEditShape->addItem("cylinder");
    ui->comboBoxlineEditShape->addItem("leaf");
    load_targetqlfr();
    val = true;
    connect( ui->actionExit, SIGNAL(triggered()), this, SLOT(close()) );
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    connect(titleBar, SIGNAL(on_close()), this, SLOT(close()));
    connect(titleBar, SIGNAL(on_minimize()), this, SLOT(minimize()));

}

MainWindow::~MainWindow()
{
    delete lsystem;
    delete in_out_rec;
    delete mediator;
    delete mod_config;
    delete pMainScene;
    delete timer;
    delete pMainViewer;
    delete pcaViewer;
    delete titleBar;
    delete dlgViewer;
    delete qstd_imodel;
    delete ui;
}

/*Drag and drop project management*/
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

//drag and drop de fichier
void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> droppedUrls = event->mimeData()->urls();
    int droppedUrlCnt = droppedUrls.size();
    //on liste les données drag and droppées
    for(int i = 0; i < droppedUrlCnt; i++)
    {
        if(i>0) break; //traiter 1 seul element

        QString localPath = droppedUrls[i].toLocalFile();
        QFileInfo fileInfo(localPath);

        //si l'element est un fichier
        if(fileInfo.isFile()) {
            //QMessageBox::information(this, tr("Dropped file"), fileInfo.absoluteFilePath());
            LoadProjectFile( fileInfo.absoluteFilePath() );
        }
    }

    event->acceptProposedAction();
}

void MainWindow::minimize()
{
    this->setWindowState(Qt::WindowMinimized);
}

//  Load project file
void MainWindow::LoadProjectFile(QString filename)
{
    QString qs_rules, qs_preproc, qs_axiom;
    float angle;
    int n_deriv=1;

    if(filename.trimmed().isEmpty()) return;

    reinit();

    current_filename = filename.toStdString();
    in_out_rec->load(this, filename, qs_rules, qs_preproc, qs_axiom, angle, n_deriv);
    std::string rules=qs_rules.toStdString();// ui->textEditProduction->toPlainText().toStdString();
    qs_axiom = qs_axiom.simplified();
    qs_axiom.replace(" ","");
    ui->textEditPreproc->setText(qs_preproc);
    ui->textEditPreproc->setReadOnly(true);
    qs_rules =qs_rules.toHtmlEscaped();
    qs_rules = qs_rules.replace("\n","<br\>");
    ui->textEditProduction->setText(qs_rules);
    ui->textEditProduction->setReadOnly(true);
    ui->lineEditAxiom->setText(qs_axiom);
    ui->lineEditAxiom->setReadOnly(true);
    ui->spinBoxAngle->setValue(angle);
    ui->spinBoxDepth->setValue(n_deriv);

    string preproc_buffer;
    preproc_buffer = ui->textEditPreproc->toPlainText().toStdString();
    pMainScene->setGeometryType((ui->radioButtonRecursiveRay->isChecked()?CONIC_SHAPE:CYLINDER_SHAPE));

    lsystem->init();
    lsystem->update_params( preproc_buffer,
                            ui->lineEditAxiom->text().toStdString(),
                            (float)ui->spinBoxAngle->value(),
                            ui->spinBoxDepth->value()
                            );
    lsystem->load(rules);
    lsystem->init_dh();
    color_config_fn = mod_config->smart_load_color(filename.toStdString());
    if (!color_config_fn.empty())
    {
        QFile my_file(color_config_fn.c_str());
        std::stringstream ss;
        ui->lineEditConfigFile->setText(color_config_fn.c_str());
        if (my_file.exists())
        {
            mod_config->load_color(color_config_fn);
            shape_config_fn = mod_config->load_shape_from_colorfn(color_config_fn);
            pMainScene->setModConfig(mod_config);
            pMainScene->setSelectionChanged(true); // Force 3d scene refresh
            pMainViewer->updateGL();

            PIORecord::load(my_file, ss);
            ui->textEditConfigContent->setText(QString(ss.str().c_str()));
            update_color_cb();
            update_shape_line();
        }
    }
    else
    {
        mod_config->setAlphabet(*lsystem->getAlphabet());
        mod_config->init_color();
    }

    mediator->getPreproc()->copy_const(lsystem->getPreproc());
    mediator->setup_cst_model(lsystem->getPreproc(), qstd_imodel);
    ui->tableViewConstVal->setModel(qstd_imodel);
    mediator->setup_combobox(ui->comboBoxConst, ui->doubleSpinBoxConstValue, lsystem->getPreproc());
}

void MainWindow::on_actionAbout_Promo_triggered()
{
    aboutDlg->show();
    aboutDlg->raise();
    aboutDlg->activateWindow();
}

void MainWindow::on_actionHistoric_triggered()
{
    in_out_rec->create_xml_deriv(lsystem);
    in_out_rec->save_to_xml(this);
}

void MainWindow::on_actionOutput_triggered()
{
    outputDlg->show();
    outputDlg->raise();
    outputDlg->activateWindow();
}

void MainWindow::on_actionLoad_triggered()
{
    if( remove( "selectfile.txt" ) != 0 )
        perror( "Error deleting file" );
    else
        puts( "File successfully deleted" );

    std::ofstream fileselect("selectfile.txt", std::ofstream::out|std::ofstream::app);
    std::stringstream ss_select;

    QString filename = QFileDialog::getOpenFileName(this, QObject::tr("Load Procedural Modeling project"), QObject::tr(""), QObject::tr("Text Files(*.txt);;All Files(*)"));
    LoadProjectFile(filename);
    file_basename = Procedural::file_basename(current_filename);
    ss_select << file_basename << std::endl;
    file_basename = setup_filename_prefix(file_basename);
    std::cout << std::endl << "Current file base name : " << file_basename;


    if (fileselect.is_open())
    {
        fileselect << ss_select.str();
        fileselect.close();
    }
    else
    {
        std::cout << std::endl << "WARNING: Could not open selectfile.txt...";
    }
}

void MainWindow::on_actionExecute_triggered()
{
    QString folder(QDir::currentPath());
    vector<string> vec_str_line;
    vector<QString> s_qualifiers;
    vector<int> s_smartselect;
    vector<QList<int>> s_seletion;
    QList<int> list;
    QString s_grammar;
    string s_buffer;
    int s_iteration, select;

    QString filename = QFileDialog::getOpenFileName(this, QObject::tr("Load select file"), QObject::tr(""), QObject::tr("Text Files(*.txt);;All Files(*)"));

    if(filename.isEmpty()) {
        // Error ...
        return;
    }

    //Read file
    QFile file(filename);
    if (!file.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::warning( this, "Loading Procedural", "Failed to load file." );
        return;
    }
    QTextStream ts(&file);
    while (!ts.atEnd())
         vec_str_line.push_back(ts.readLine().toStdString());
    if(vec_str_line.size()>0)
        s_grammar = QString::fromStdString(vec_str_line.at(0));
    if(vec_str_line.size()>1)
    s_iteration = atoi(vec_str_line.at(1).c_str());
    //std::cout << " " << s_grammar << std::endl;
    //std::cout << " " << s_iteration << std::endl;
    if(vec_str_line.size()>2)
        for (unsigned int i=2; i<vec_str_line.size(); i++)
        {
            s_qualifiers.push_back(QString::fromStdString(vec_str_line.at(i)));
            i++;
            s_smartselect.push_back(atoi(vec_str_line.at(i).c_str()));
            i++;
            s_buffer = vec_str_line.at(i);
            list.clear();
            while(s_buffer.length()>0)
            {

                select = atoi(s_buffer.substr(0, s_buffer.find(',')).c_str());
                s_buffer = s_buffer.substr(s_buffer.find(',')+1);
                list.push_back(select);

            }
            /*foreach(int element , list)
                {
                std::cout<< "" << element << ",";
                }
            std::cout<< std::endl;*/
            s_seletion.push_back(list);
        }

    if(!s_grammar.isEmpty()){
        //Load grammar
        LoadProjectFile(folder + "/" + s_grammar+".txt");
        file_basename = Procedural::file_basename(current_filename);
        file_basename = setup_filename_prefix(file_basename);
        std::cout << std::endl << "Current file base name : " << file_basename;
    }

    if(s_iteration > 1){
        //Iterations
        ui->spinBoxDepth->setValue(s_iteration);
        on_pushButtonUpdate_clicked();
    }
    /*
     * NYKOLAS'S VERSION
    int i = 0;
    for(std::vector<QString>::iterator iter = s_qualifiers.begin(); iter !=s_qualifiers.end();iter++){

        val = true;
        ui->comboBoxTargetQualifier->setCurrentIndex(ui->comboBoxTargetQualifier->findText((*iter)));
        list_idx_slt = s_seletion.at(i);
        ui->spinBoxSmartSelection->setValue(s_smartselect.at(i));
        //myCombo->setCurrentIndex(myCombo->findText(rs.value(0).toString()));
        on_pushButtonApplyQualifier_clicked();
        i++;
    }

    pMainScene= new Scene(lsystem);
    mediator = new PMediator(lsystem, pMainScene);

    std::vector<unsigned int> vec_slct_tmp;
    vec_slct_tmp = lsystem->sub_branches();

    mediator->setVecSelectionReviewed(vec_slct_tmp);

    pMainScene->setModConfig(mod_config);
    pMainScene->draw(true);
    pMainViewer->setScene(pMainScene);

    pMainScene->setSelection(list_idx_slt);
    pMainScene->setSelectionChanged(true);
    pMainViewer->setFocus();
    pMainViewer->updateGL();
    */

    // A. SALOMON'S VERSION

    int i = 0;
    for(std::vector<QString>::iterator iter = s_qualifiers.begin(); iter !=s_qualifiers.end();iter++){

        val = true;
        ui->comboBoxTargetQualifier->setCurrentIndex(ui->comboBoxTargetQualifier->findText((*iter)));
        list_idx_slt = s_seletion.at(i);
        ui->spinBoxSmartSelection->setValue(s_smartselect.at(i));
        //myCombo->setCurrentIndex(myCombo->findText(rs.value(0).toString()));
        //on_pushButtonApplyQualifier_clicked();
        i++;
    }

    pMainScene= new Scene(lsystem);
    mediator = new PMediator(lsystem, pMainScene);

    std::vector<unsigned int> vec_slct_tmp;
//    vec_slct_tmp = lsystem->sub_branches();

//    mediator->setVecSelectionReviewed(vec_slct_tmp);

    pMainScene->setModConfig(mod_config);
    pMainScene->draw(true);
    pMainViewer->setScene(pMainScene);

    pMainScene->setSelection(list_idx_slt);
    pMainScene->setSelectionChanged(true);
    pMainViewer->setFocus();
    pMainViewer->updateGL();

}

void MainWindow::on_actionDerivation_Tree_triggered()
{
    treeviewDlg->show();
}

void MainWindow::on_actionPCA_Plot_triggered()
{
    pcaViewer->set_pointSize();
    pcaViewer->show();
}

void MainWindow::on_actionReset_triggered()
{
    reinit();
    ui->tabWidget->setCurrentWidget(ui->tabEdit);
}

void MainWindow::on_actionOutput_Img_Browser_triggered(QString _filename, QString _id_user, QString _date)
{
    QString my_filename;  
    dlgViewer = new DialogViewer(this);

       if(_filename=="")
       {
           my_filename = QFileDialog::getOpenFileName(this, QObject::tr("Load CSV Data Images"), QObject::tr(""), QObject::tr("CSV Files(*.csv);;All Files(*)"));
       }
       else
       {
           my_filename=_filename;
           dlgViewer->setTest(true);
       }

    dlgViewer->setInputFilename(my_filename);
    dlgViewer->updateViewer3d(_id_user, _date);
    //dlgViewer->getViewer()->load_image_list(my_filename);
    //imgBrowser->show();
    dlgViewer->show();
}

void MainWindow::on_comboBoxActions_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0: // Delete
        break;
    case 1: // Regenerate
        break;
    }
}

void MainWindow::on_pushButtonResetConfig_clicked()
{
    QString qstr;
    std::stringstream my_colorss;

    ui->lineEditConfigFile->clear();
    ui->textEditConfigContent->clear();

    mod_config->init();
    color_config_fn = mod_config->smart_save_color(current_filename);
    shape_config_fn = mod_config->smart_save_shape(current_filename);
    pMainScene->setModConfig(mod_config);
    pMainScene->setSelectionChanged(true); // Force 3d scene refresh
    pMainViewer->updateGL();

    ui->lineEditConfigFile->setText(color_config_fn.c_str());
    ui->textEditConfigContent->clear();

    QFile my_colorfile(color_config_fn.c_str());
    PIORecord::load(my_colorfile, my_colorss);
    ui->textEditConfigContent->setText(my_colorss.str().c_str());

    update_color_cb();
    update_shape_line();
}

void MainWindow::on_pushButtonSaveConfig_clicked()
{
    std::stringstream my_ss;

    if (!ui->textEditConfigContent->toPlainText().toStdString().empty())
    {
        my_ss << ui->textEditConfigContent->toPlainText().toStdString();
        mod_config->load_color(my_ss);
        if (ui->lineEditConfigFile->text().isEmpty())
        {
            color_config_fn = mod_config->smart_save_color(current_filename);
        }
        else
        {
            mod_config->save_color(ui->lineEditConfigFile->text().toStdString());
        }
        pMainScene->setModConfig(mod_config);
        pMainScene->setSelectionChanged(true); // Force 3d scene refresh
        pMainViewer->updateGL();
        update_color_cb();
    }
}

void MainWindow::on_pushButtonConfigBrowse_clicked()
{
    // This button loads contents from a selected file, then it is much more like a load button
    stringstream ss;
    string my_shape_filename;
    QString filename = QFileDialog::getOpenFileName(this, QObject::tr("Loading Procedural Config File"), QObject::tr(""), QObject::tr("Csv Files(*.csv);;All Files(*)"));

    if (!filename.isEmpty())
    {
        QFile file(filename);
        ui->lineEditConfigFile->setText(filename);
        mod_config->load_color(filename.toStdString());
        if (!mod_config->getVecColor().empty())
        {
            mod_config->load_shape_from_colorfn(filename.toStdString());
            pMainScene->setModConfig(mod_config);

            PIORecord::load(file, ss);
            ui->textEditConfigContent->setText(QString(ss.str().c_str()));
            update_color_cb();
            update_shape_line();
        }
        else
        {
            mod_config->load_shape(filename.toStdString());
            if (!mod_config->getVecShapeStr().empty())
            {
                ui->comboBoxlineEditShape->setCurrentText(mod_config->check_shape(ui->comboBoxAlphabet->currentIndex()).c_str());
                update_color_cb();
                update_shape_line();
            }
        }
        pMainScene->setSelectionChanged(true); // Force 3d scene refresh
        pMainViewer->updateGL();
    }
}

void MainWindow::on_pushButtonApply_clicked()
{
    PDerivHistoric* pdh_tmp=0;
    HistoricItem* hi;

    unsigned int back_stage=ui->spinBoxBackStage->value();
    unsigned int forward_stage=ui->spinBoxForwardStage->value();
    std::string html_text;
    std::vector<unsigned int> vec_start_tmp, vec_end_tmp;
    std::vector<PDerivHistoric*> vec_pdh_tmp;


    if (getScene()->getListSelection().size()==1)
    {
        switch(ui->comboBoxActions->currentIndex())
        {
        case 0: // Delete
            lsystem->drop(pdh_tmp, getScene()->getListSelection().front(), back_stage);
            mediator->setDh(pdh_tmp);
            treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
            hi=treeviewDlg->getHistoricTreeModel()->select(mediator->getDh());
            getTreeViewDialog()->setCurrentIndex(hi->index());
            hist_drop_dbg();
            break;
        case 1: // Generate
//                dbg_map_offset();
//                lsystem->dbg_map_params();
//                lsystem->dbg_string();
            lsystem->generate(getScene()->getListSelection().front(), forward_stage);
//                lsystem->dbg_map_params();
//                lsystem->dbg_string();
            treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
            break;
        case 2: // Regenerate
            lsystem->regenerate(getScene()->getListSelection().front(), back_stage, forward_stage, pdh_tmp);
            mediator->setDh(pdh_tmp);
            treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
            break;
        }
        pMainViewer->setFocus();
        pMainViewer->updateGL();
        html_text=lsystem->getText();

        lsystem->select(vec_start_tmp, vec_end_tmp, getScene()->getListSelection().front());
        lsystem->highlight_text(html_text, vec_start_tmp, vec_end_tmp, getScene()->getListSelection().front());
        outputDlg->setCurrDerivHtmlText(QString(html_text.c_str()));
    }
    else
    {
        switch(ui->comboBoxActions->currentIndex())
        {
        case 0: // Delete
            lsystem->drop(vec_pdh_tmp, getScene()->getListSelection(), back_stage);
            mediator->setDh(pdh_tmp);
            treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
            if (pdh_tmp)
            {
                hi=treeviewDlg->getHistoricTreeModel()->select(mediator->getDh());
                getTreeViewDialog()->setCurrentIndex(hi->index());
            }
            hist_drop_dbg();
            break;
        case 1: // Generate
            lsystem->generate(getScene()->getListSelection(), forward_stage);
            treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
            break;
        case 2: // Regenerate
            lsystem->inspect_selection(getScene()->getListSelection(), back_stage);
            lsystem->regenerate(getScene()->getListSelection(), back_stage, forward_stage);
            treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
            break;
        }
        pMainViewer->setFocus();
        pMainViewer->updateGL();


        if (getScene()->getListSelection().size()==1)
        {
            html_text=lsystem->getText();
            lsystem->select(vec_start_tmp, vec_end_tmp, getScene()->getListSelection().front());
            lsystem->highlight_text(html_text, vec_start_tmp, vec_end_tmp, getScene()->getListSelection().front());
            outputDlg->setCurrDerivHtmlText(QString(html_text.c_str()));
        }
    }

    pMainViewer->setFocus();
    pMainViewer->updateGL();
}

void MainWindow::on_pushButtonUpdate_clicked()
{
    std::ofstream fileselect("selectfile.txt", std::ofstream::out|std::ofstream::app);
    std::stringstream ss_select;
    std::vector<unsigned int> rules_count;
    std::vector<float> infl_factor;

    QElapsedTimer time_elapsed;

    int derivation_number = ui->spinBoxDepth->value();
    pMainScene->setGeometryType((ui->radioButtonRecursiveRay->isChecked()?CONIC_SHAPE:CYLINDER_SHAPE));
    lsystem->setTargetDerivNum(derivation_number);
    time_elapsed.start();
    PPreproc * pp_tmp=lsystem->getPreproc();
    time_t seed=pp_tmp->getSeed();
    if (seed==0)
    {
        pp_tmp->setSeed(time(NULL));
        seed=pp_tmp->getSeed();
        lsystem->setPreproc(pp_tmp);
    }
    srand(seed);
    for (int i=0 ; i<derivation_number; i++)
    {
        time_elapsed.restart();
        lsystem->derive();
        pMainViewer->updateGL();
        rules_count.clear();
        for (int l=0; l<lsystem->getVecRules().size(); l++)
        {
            rules_count.push_back(lsystem->getVecRules().at(l)->getapplycount());
        }
        vec_rules_count.push_back(rules_count);
        //vec_metrics.push_back(check_metrics());
    }
    pMainScene->clear_obb();
    treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
    //pMainViewer->updateGL();
    pMainViewer->showEntireScene();
    outputDlg->setCurrDerivText(QString(lsystem->getText().c_str()));

    //std::cout << lsystem->getText().c_str();
    ss_select << derivation_number << std::endl;

    if (fileselect.is_open())
    {
        fileselect << ss_select.str();
        fileselect.close();
    }
    else
    {
        std::cout << std::endl << "WARNING: Could not open selectfile.txt...";
    }

    // for debug
    //std::cout << std::endl << vec_metrics.size() << " metrics set have been computed.";
    std::cout << std::endl << "Rule usage statistics:";
    for (unsigned int j=0; j<vec_rules_count.size(); j++)
    {
        std::cout << std::endl;
        float total = 0;
        for(unsigned int i=0; i<vec_rules_count.at(j).size(); i++)
            total+=vec_rules_count.at(j).at(i);
        for (unsigned int i=0; i<vec_rules_count.at(j).size(); i++)
        {
            infl_factor.push_back(vec_rules_count.at(j).at(i)/total);
            std::cout << std::endl << "Rule " << i+1 <<":" << vec_rules_count.at(j).at(i);/*lsystem->getVecRules().at(i)->getapplycount();*/
        }
        for(unsigned int i=0; i<infl_factor.size(); i++)
            std::cout << std::endl << "Influence Factor " << i+1 <<":" << infl_factor.at(i)*100;
        vec_infl_factor.push_back(infl_factor);
        infl_factor.clear();
    }
    std::cout << std::endl;

}

void MainWindow::hist_str_dbg()
{
    QString orig_str(this->lsystem->getText().c_str());
    QString intro_str("\nHistoric generated string:\n");
    QString generated_str(this->lsystem->str_from_historic().c_str());
    QString conclusion_str(orig_str.compare(generated_str)==0?tr("\nSame string!"):tr("\nDifferent string!"));
    outputDlg->setCurrDerivText(orig_str+intro_str+generated_str+conclusion_str);
}

void MainWindow::hist_drop_dbg()
{
    string curr_txt = lsystem->getText();
    string prev_txt = lsystem->getDbgPreviousText();
    QString qs_curr(curr_txt.c_str());
    QString qs_prev(prev_txt.c_str());
    QString intro_curr("\nCurrent string:\n");
    QString intro_prev("\nString before modification:\n");
    QString value = intro_prev+qs_prev+intro_curr+qs_curr;
    if (curr_txt.compare(prev_txt)!=0)
    {
        outputDlg->setCurrDerivText(value);
        lsystem->setDbgPreviousText(curr_txt);
    }
}

void MainWindow::hist_read_dbg()
{
    PTextParser tp;
    string curr_txt;
    string prev_txt;

    curr_txt = tp.make_readable(lsystem->getText());
    prev_txt = tp.make_readable(lsystem->getDbgPreviousText());
    QString qs_curr(curr_txt.c_str());
    QString qs_prev(prev_txt.c_str());
    QString intro_curr("\nCurrent cleaned string:\n");
    QString intro_prev("\nCleaned tring before modification:\n");
    QString value = intro_prev+qs_prev+intro_curr+qs_curr;
    if (lsystem->getText().compare(lsystem->getDbgPreviousText())!=0)
    {
        outputDlg->setCurrDerivText(value);
        lsystem->setDbgPreviousText(lsystem->getText());
    }
}

void MainWindow::timerTimeout()
{
    unsigned int idx_from_viewer;
    string html_text, text_tmp;
    QString qs_selection;
    PDerivHistoric* pdh=0;
    HistoricItem* hi;
    vector<unsigned int> vec_start_tmp, vec_end_tmp;

    QString qs_modelview_tmp(pMainScene->modelview_to_str().c_str());
    QString qs_model_tmp(pMainScene->model_to_str().c_str());
    QString qs_view_tmp(pMainScene->view_to_str().c_str());
    QString qs_left_tmp(pMainScene->left_to_str().c_str());
    QString qs_head_tmp(pMainScene->head_to_str().c_str());
    QString qs_up_tmp(pMainScene->up_to_str().c_str());
    outputDlg->setMatrixModelviewText(qs_modelview_tmp);
    outputDlg->setMatrixModelText(qs_model_tmp);
    outputDlg->setMatrixViewText(qs_view_tmp);
    outputDlg->setLeftText(qs_left_tmp);
    outputDlg->setHeadText(qs_head_tmp);
    outputDlg->setUpText(qs_up_tmp);
    outputDlg->settle_output();

    if (getViewer()->getSelectionOccured()==true)
    {
        if (getScene()->getListSelection().size()==1)
        {
            //mediator->setSelection(getScene()->getListSelection().front());
            idx_from_viewer = getScene()->getListSelection().front();
            pdh = lsystem->select_historic(idx_from_viewer);
            mediator->setDh(pdh);

            if (!pdh)
            {
                mediator->setup_cst_model(lsystem->getPreproc(), qstd_imodel);
                ui->tableViewConstVal->setModel(qstd_imodel);
                mediator->setup_combobox(ui->comboBoxConst, ui->doubleSpinBoxConstValue, lsystem->getPreproc());
                if (lsystem->getPreproc())
                    *mediator->getPreproc() = *lsystem->getPreproc();
            }
            else
            {
                mediator->setup_cst_model(pdh->getPreproc(), qstd_imodel);
                ui->tableViewConstVal->setModel(qstd_imodel);
                mediator->setup_combobox(ui->comboBoxConst, ui->doubleSpinBoxConstValue, pdh->getPreproc());
                getViewer()->setSelectionOccured(false);

                text_tmp = html_text=lsystem->getText();
                hi = getTreeViewDialog()->getHistoricTreeModel()->select(pdh);
                getTreeViewDialog()->setCurrentIndex(hi->index());
                lsystem->select(vec_start_tmp, vec_end_tmp, idx_from_viewer);
                html_text = lsystem->highlight_text(text_tmp, vec_start_tmp, vec_end_tmp, idx_from_viewer);
                outputDlg->setCurrDerivHtmlText(QString(html_text.c_str()));
                qs_selection = hi->data(Qt::DisplayRole).toString();
                ui->lineEditCurrentSelection->setText(qs_selection);
                ui->spinBoxBackStage->setValue(hi->getPdh()->getDerivNum());
            }
        }
//        std::cout << std::endl << getScene()->getListSelection().size() << " selection(s) :";
//        for (size_t i=0; i<getScene()->getListSelection().size(); i++)
//            std::cout << getScene()->getListSelection().at(i) << "\t";
    }
}

void MainWindow::on_comboBoxConst_currentIndexChanged(const QString &arg1)
{
    string s = arg1.toStdString();
    if (s.empty()) return;
    for (unsigned int i=0; i<mediator->getPreproc()->getVecDefine().size(); i++)
    {
        if (!s.compare(mediator->getPreproc()->getVecDefine()[i]->getConstantName()))
        {
            ui->doubleSpinBoxConstValue->setValue((double) mediator->getPreproc()->getVecDefine()[i]->getConstantValue());
            break;
        }
    }
}

void MainWindow::on_doubleSpinBoxConstValue_valueChanged(double arg)
{
    string const_name = ui->comboBoxConst->currentText().toStdString();
    float const_val = (float) ui->doubleSpinBoxConstValue->value();

    mediator->getPreproc()->redefine_const(const_name, const_val);
    mediator->setup_cst_model(mediator->getPreproc(), qstd_imodel);
    ui->tableViewConstVal->setModel(qstd_imodel);
}

void MainWindow::on_pushButtonConstGlobalScope_clicked()
{
    PPreproc* preproc_tmp;
    QAbstractItemModel* aim;
    QStandardItemModel* sim;
    aim = ui->tableViewConstVal->model();
    sim = dynamic_cast<QStandardItemModel*>(aim);

    if (!sim) return;
    preproc_tmp = new PPreproc();
    *preproc_tmp = mediator->setup_preproc(sim);
    mediator->copy_preproc(*preproc_tmp);
    lsystem->update_global_const(preproc_tmp);
    treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
    pMainViewer->updateGL();
    delete preproc_tmp;
}

void MainWindow::on_pushButtonConstLocalScope_clicked()
{
    PPreproc* preproc_tmp;
    QAbstractItemModel* aim;
    QStandardItemModel* sim;
    aim = ui->tableViewConstVal->model();
    sim = dynamic_cast<QStandardItemModel*>(aim);

    if (!sim) return;
//    if (pMainScene->getSelection()<0) return;
//    if ((unsigned)pMainScene->getSelection()>=lsystem->getText().size()) return;

    preproc_tmp = new PPreproc();
    *preproc_tmp = mediator->setup_preproc(sim);
    mediator->getPreproc()->copy_const(preproc_tmp);
    lsystem->update_local_const(preproc_tmp, mediator->getDh());

    treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
    pMainViewer->updateGL();
    delete preproc_tmp;
}

void MainWindow::on_pushButtonConstChildScope_clicked()
{
    PPreproc* preproc_tmp;
    QAbstractItemModel* aim;
    QStandardItemModel* sim;
    aim = ui->tableViewConstVal->model();
    sim = dynamic_cast<QStandardItemModel*>(aim);

    if (!sim) return;
    if (!mediator->getDh()) return;

    preproc_tmp = new PPreproc();
    *preproc_tmp = mediator->setup_preproc(sim);
    mediator->getPreproc()->copy_const(preproc_tmp);
    // The next line needs updates
    lsystem->update_sub_branches(mediator->check_selection(), preproc_tmp);

    treeviewDlg->getHistoricTreeModel()->generate_tree(lsystem);
    pMainViewer->updateGL();
    delete preproc_tmp;
}

void MainWindow::on_pushButtonReinitialize_clicked()
{
    string s = ui->textEditPreproc->toPlainText().toStdString();
    mediator->copy_preproc(*lsystem->getPreproc());
    mediator->setup_combobox(ui->comboBoxConst, ui->doubleSpinBoxConstValue, lsystem->getPreproc());
}


void MainWindow::on_pushButtonObbCurr_clicked()
{
    PBoundingBox obb;
    std::vector<PShapeIndexer*> vec_idxr_tmp, vec_all_idxr;
    std::vector<CGLA::Vec3f> vec_vertices_incl, vec_vertices_idxr;
    mediator->clear_selection_reviewed();
    mediator->add_selection(mediator->check_selection());
    vec_idxr_tmp = mediator->select_indexer(pMainScene, mediator->getVecSelectionReviewed());

    for (std::vector<PShapeIndexer*>::iterator iter=vec_idxr_tmp.begin(); iter!=vec_idxr_tmp.end(); iter++)
    {
        vec_vertices_idxr = (*iter)->getVertices();
        vec_vertices_incl.insert(vec_vertices_incl.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
    }
    mediator->add_obb(pMainScene, obb, vec_vertices_incl, true);
    pMainViewer->updateGL();

    PMetric metric = mediator->compute_metric(lsystem, pMainScene, obb);
    ui->tableViewMetric->setModel(mediator->setup_table_model(&metric));
    PIORecord io;
    QFile file;
    QString filename("output.csv");
    io.save_to_csv(this, file, filename, metric);
}

void MainWindow::on_pushButtonObbSub_clicked()
{
    PBoundingBox obb;
    std::vector<PShapeIndexer*> vec_idxr_tmp;
    std::vector<CGLA::Vec3f> vec_vertices_incl, vec_vertices_idxr;
    std::vector<unsigned int> vec_slct_tmp;
    mediator->clear_selection_reviewed();
    vec_slct_tmp = lsystem->sub_branches(mediator->check_selection());
    mediator->setVecSelectionReviewed(vec_slct_tmp);
    vec_idxr_tmp = mediator->select_indexer(pMainScene, mediator->getVecSelectionReviewed());

    for (std::vector<PShapeIndexer*>::iterator iter=vec_idxr_tmp.begin(); iter!=vec_idxr_tmp.end(); iter++)
    {
        vec_vertices_idxr = (*iter)->getVertices();
        vec_vertices_incl.insert(vec_vertices_incl.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
    }
    mediator->add_obb(pMainScene, obb, vec_vertices_incl);
    pMainViewer->updateGL();
    PMetric metric = mediator->compute_metric(lsystem, pMainScene, obb);
    ui->tableViewMetric->setModel(mediator->setup_table_model(&metric));
}

void MainWindow::on_pushButtonObbAll_clicked()
{
    PBoundingBox obb;
    std::vector<PShapeIndexer*> vec_idxr_tmp;
    std::vector<CGLA::Vec3f> vec_vertices_incl, vec_vertices_idxr;
    std::vector<unsigned int> vec_slct_tmp = lsystem->sub_branches();
    mediator->clear_selection_reviewed();
    mediator->setVecSelectionReviewed(vec_slct_tmp);
    vec_idxr_tmp = mediator->select_indexer(pMainScene, mediator->getVecSelectionReviewed());
    for (std::vector<PShapeIndexer*>::iterator iter=vec_idxr_tmp.begin(); iter!=vec_idxr_tmp.end(); iter++)
    {
        vec_vertices_idxr = (*iter)->getVertices();
        vec_vertices_incl.insert(vec_vertices_incl.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
    }
    mediator->add_obb(pMainScene, obb, vec_vertices_incl);
    pMainViewer->updateGL();
    PMetric metric = mediator->compute_metric(lsystem, pMainScene, obb);
    ui->tableViewMetric->setModel(mediator->setup_table_model(&metric));
}

int MainWindow::get_snapshot_format()
{
    int value;
    switch(ui->comboBoxImgFormat->currentIndex())
    {
    case 0:
        value = RES_600x400;
        break;
    case 1:
        value = RES_1024x768;
        break;
    case 2:
        value = RES_1200x780;
        break;
    default:
        value = RES_600x400;
    }
    return value;
}

void MainWindow::simConst_allRules(time_t seed)
{

    std::vector<PPreproc> vec_pp, vec_pp_out;
    std::vector<PShapeIndexer*> vec_idxr_tmp;
    std::vector<CGLA::Vec3f> vec_vertices_incl, vec_vertices_idxr;
    std::vector<unsigned int> vec_slct_tmp;
    std::vector<unsigned int> vec_modified_rule;
    std::vector<float> vec_change_rate;
    std::vector<float> eig_val, vec_mean;
    std::vector<std::vector<float> > vec_vec_change_rate;
    std::vector<std::vector<float> > eig_vec, co_mat;
    std::vector<PMetric> vec_mtrc;
    PBoundingBox obb;
    PMetric metric, main_metric;
    PIORecord io;
    PPcaEval pca;
    QFile file_co, file_pca;
    QString fn_cst_ovrvw("constants_overview_ar.csv"), fn_pca("pca_eval_ar.csv"), fn_main_output;
    PLSystemParametric* lp;
    PMediator* med;
    Scene* scn;
    PModelingConfig* mc=0;

    //QDir curr_dir(QDir::currentPath());
    QString work_path(QDir::currentPath());
    work_path += "/";
    work_path += file_basename.c_str();
    work_path +="_ar";
    QString dir_name(file_basename.c_str());
    dir_name+="_ar";
    if (!QDir(dir_name).exists())
    {

        QDir().mkdir(dir_name);
    }
    QDir::setCurrent(work_path);
    std::cout << std::endl << "Current path : " << work_path.toStdString();
    // Main Metric is computed first
    mediator->clear_selection_reviewed();
    vec_slct_tmp = lsystem->sub_branches();
    mediator->setVecSelectionReviewed(vec_slct_tmp);

    if (lsystem->getPreproc()->getVecDefine().empty())
        return;


    vec_pp = lsystem->generate_preproc();
    vec_idxr_tmp = mediator->select_indexer(pMainScene, mediator->getVecSelectionReviewed());
    for (std::vector<PShapeIndexer*>::iterator iter=vec_idxr_tmp.begin(); iter!=vec_idxr_tmp.end(); iter++)
    {
        vec_vertices_idxr = (*iter)->getVertices();
        vec_vertices_incl.insert(vec_vertices_incl.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
    }
    mediator->add_obb(pMainScene, obb, vec_vertices_incl);
    pMainViewer->updateGL();
    main_metric = mediator->compute_metric(lsystem, pMainScene, obb);
    ui->tableViewMetric->setModel(mediator->setup_table_model(&main_metric));
    vec_vec_change_rate.reserve(vec_pp.size()*lsystem->getVecRules().size()*lsystem->getVecRules().size());

    for (std::vector<PPreproc>::iterator iter_pp=vec_pp.begin(); iter_pp!=vec_pp.end(); iter_pp++)
    {

        //MOdification de toutes les regles
        lp = new PLSystemParametric(*lsystem, false);
        med = new PMediator(lp);
        scn = new Scene(lp);
        std::vector<CGLA::Vec3f> vec_vertices_incl_tmp2, vec_vertices_idxr_tmp2;
        PBoundingBox obb_tmp2;
        PPreproc* preproc = new PPreproc(*iter_pp);
        for (unsigned int i=0; i<lsystem->getVecRules().size(); i++)
        {
            lp->rewrite_rule(i, preproc);
        }
        lp->init_dh();
        srand(seed);
        for (int k=0; k<lsystem->getNumDerivation(); k++)
            lp->derive();
        vec_slct_tmp = lp->sub_branches();
        med->clear_selection_reviewed();
        med->setVecSelectionReviewed(vec_slct_tmp);
        scn->draw(false);
        vec_idxr_tmp = med->select_indexer(scn, med->getVecSelectionReviewed());
        for (std::vector<PShapeIndexer*>::iterator iter_si=vec_idxr_tmp.begin(); iter_si!=vec_idxr_tmp.end(); iter_si++)
        {
            vec_vertices_idxr_tmp2 = (*iter_si)->getVertices();
            vec_vertices_incl_tmp2.insert(vec_vertices_incl_tmp2.end(), vec_vertices_idxr_tmp2.begin(),  vec_vertices_idxr_tmp2.end());
        }
        med->add_obb(scn, obb_tmp2, vec_vertices_incl_tmp2, true);
        metric = med->compute_metric(lp, scn, obb_tmp2);
        vec_mtrc.push_back(metric);
        vec_pp_out.push_back(*preproc);
        vec_modified_rule.push_back(10000);
        vec_change_rate = main_metric.compute_diff(metric);
        vec_vec_change_rate.push_back(vec_change_rate);
        //debug:
        cout<<endl<<"Metrics gathered : " << vec_mtrc.size();
        delete scn;
        delete med;
        delete lp;
        //debug:
        static int dbg_csts=0;
        cout<<endl<<"Constants processed : " << ++dbg_csts << endl << "==== ** ==== ** ====";
    }
    vec_mean = pca.mean(vec_vec_change_rate);
    co_mat = pca.covariance_matrix(vec_vec_change_rate, vec_mean);
    pca.compute_eigens(co_mat, eig_vec, eig_val);
    io.save_to_csv(this, file_pca, fn_pca, co_mat, eig_vec, eig_val);
    io.save_to_csv(this, file_co, fn_cst_ovrvw, vec_mtrc, vec_pp_out, vec_modified_rule, vec_vec_change_rate);

    // PCA Compression Process
    QString fn_gap;
    QFile file_gap;
    ublas::matrix<float> mx_feat_vec, mx_feat_vec_t, mx_pop_adjust, mx_pop_adjust_t;
    ublas::matrix<float> mx_final_pop, mx_pop_orig, mx_pop_orig_t;
    std::vector<std::vector<float> > eig_vec_cmprss, pop_adj, pop_orig, gap;
    std::vector<float> vec_avrg_gap, vec_err;

    for (size_t i=1; i<=eig_vec.size(); i++)
    {
        mx_feat_vec.clear();
        mx_feat_vec_t.clear();
        mx_pop_orig.clear();
        mx_pop_orig_t.clear();
        mx_pop_adjust.clear();
        mx_pop_adjust_t.clear();
        eig_vec_cmprss.clear();
        gap.clear();
        pop_adj.clear();
        pop_orig.clear();


        fn_gap = QString("gap_ar_%1.csv").arg(i);
        eig_vec_cmprss = pca.select_eigenvector(i);
        pop_adj = pca.adjust_pop(vec_vec_change_rate);
        Procedural::load_matrix<float>(eig_vec_cmprss, mx_feat_vec);
        Procedural::load_matrix<float>(pop_adj, mx_pop_adjust);
        mx_feat_vec_t   = ublas::trans(mx_feat_vec);
        mx_pop_adjust_t = ublas::trans(mx_pop_adjust);
        // Debug


        // Compression
        mx_final_pop    = ublas::prod(mx_feat_vec, mx_pop_adjust_t);
        mx_pop_orig     = ublas::prod(mx_feat_vec_t, mx_final_pop);
        mx_pop_orig_t   = ublas::trans(mx_pop_orig);
        Procedural::save_matrix<float>(mx_pop_orig_t, pop_orig);
        gap = pca.compute_diff(pop_adj, pop_orig);
        // I/O Recording
        io.generic_save<float>(this, file_gap, fn_gap, gap,  std::vector<std::string>(), std::vector<std::string>(), std::string(""), fn_gap.toStdString());
        vec_err.push_back(pca.compute_error(gap));
        gap = pca.abs(gap);
        vec_avrg_gap.push_back(pca.mean2(gap));
    }
    std::cout << std::endl << "Average Means for every order :" << std::endl;
    for (size_t i=0; i<=vec_avrg_gap.size(); i++)
    {
        std::cout << vec_avrg_gap[i] << "\t";
    }
    std::cout << std::endl << "Error Rating for every order :" << std::endl;
    for (size_t i=0; i<=vec_err.size(); i++)
    {
        std::cout << vec_err[i] << "\t";
    }

    // Output a 3D plot from 3 features vectors
    std::vector< std::vector<float> > compressed_data = pca.compress_data(vec_vec_change_rate, 3);
    pcaViewer->update_pt3d(compressed_data);

    // Wrap the 3D points
    unsigned int nrules;
    const char WRAPPER_FN[] = "wrapper.tmp";
    QFile file_wrapper(WRAPPER_FN);
    std::stringstream ss_wrap;
    std::vector<unsigned int> vec_ruleidx_wrap;
    std::vector<unsigned int> vec_wrapper_idx = pca.wrapper_pt3d(compressed_data);
    for (std::vector<unsigned int>::iterator iter=vec_wrapper_idx.begin(); iter!=vec_wrapper_idx.end(); iter++)
    {
        ss_wrap << (*iter);
        for (std::vector<float>::iterator jter=compressed_data.at(*iter).begin(); jter!=compressed_data.at(*iter).end(); jter++)
        {
            ss_wrap << "," << *jter;
        }
        ss_wrap << std::endl;
    }
    PIORecord::save(file_wrapper, ss_wrap);

    QString fn_png =QString::fromStdString(file_basename);
    fn_png+="_ar";

    QStringList filelist_png  = generate_filename(fn_png.toStdString(), std::string("png"), 27);
    QStringList filelist_lsys = generate_filename(fn_png.toStdString(), std::string("txt"), 27);
    QStringList filelist_pp   = generate_filename(fn_png.toStdString(), std::string("csv"), 27);
    nrules = lsystem->getVecRules().size();
    mediator->select_preproc(vec_pp, nrules, vec_wrapper_idx, vec_pp_wrap, vec_ruleidx_wrap,true);

    unsigned int pp_idx=0;
    std::vector<PPreproc>::iterator iter_pp=vec_pp_wrap.begin();
    std::vector<unsigned int>::iterator iter_rules=vec_ruleidx_wrap.begin();
    pMainViewer->saveStateToFile();

    // calculate every projection points of vertices for the 27 trees

    float htop_min=0, hbot_min=0, vtop_min=0, vbot_min=0;
    Vec3f vert_htop, vert_hbot, vert_vtop, vert_vbot;

    float fovy = pMainViewer->camera()->fieldOfView();
    CGLA::Vec3f vectop_v, vecbot_v, vectop_h, vecbot_h;
    vectop_v[0]=0; //x comp
    vectop_v[1]=-cos(fovy/2); //y comp
    vectop_v[2]=-sin(fovy/2); //z comp

    vecbot_v[0]=0;
    vecbot_v[1]= cos(fovy/2);
    vecbot_v[2]= -sin(fovy/2);

    vectop_h[0]=-cos(fovy/2); //x comp
    vectop_h[1]=0; //y comp
    vectop_h[2]=-sin(fovy/2); //z comp

    vecbot_h[0]= cos(fovy/2);
    vecbot_h[1]= 0;
    vecbot_h[2]= -sin(fovy/2);

    float z_max=0;

    std::vector<std::vector<CGLA::Vec3f>> vec_vec_extremums;

    while(iter_pp!=vec_pp_wrap.end() && iter_rules!=vec_ruleidx_wrap.end() )
    {
        lp = new PLSystemParametric(*lsystem, false);
        med = new PMediator(lp);
        scn = new Scene(lp);
        if(*iter_rules>nrules)
            for(int i=0; i<nrules;i++)
                lp->rewrite_rule(i, &(*iter_pp));
        else lp->rewrite_rule(*iter_rules, &(*iter_pp));

        lp->init_dh();
        srand(seed);
        for (int k=0; k<lsystem->getNumDerivation(); k++)
            lp->derive();
        std::vector<CGLA::Vec3f> vec_vertices_inc;
        vec_slct_tmp = lp->sub_branches();

        med->setVecSelectionReviewed(vec_slct_tmp);
        scn->draw(true);
        vec_idxr_tmp = med->select_indexer(scn, med->getVecSelectionReviewed());
        for (std::vector<PShapeIndexer*>::iterator iter_si=vec_idxr_tmp.begin(); iter_si!=vec_idxr_tmp.end(); iter_si++)
        {
            vec_vertices_idxr = (*iter_si)->getVertices();
            vec_vertices_inc.insert(vec_vertices_inc.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
        }

        float htop_min_tmp=0, hbot_min_tmp=0, vtop_min_tmp=0, vbot_min_tmp=0;
        Vec3f vert_htop_tmp, vert_hbot_tmp, vert_vtop_tmp, vert_vbot_tmp;

        for (std::vector<CGLA::Vec3f>::iterator iter_vert=vec_vertices_inc.begin(); iter_vert!=vec_vertices_inc.end(); iter_vert++)
        {
            Vec3f vertice = *iter_vert;
            float h_top = dot(vertice,vectop_h);
            float h_bot = dot(vertice,vecbot_h);
            float v_top = dot(vertice,vectop_v);
            float v_bot = dot(vertice,vecbot_v);


            //local
            if(h_top<htop_min_tmp)
            {
                vert_htop_tmp=vertice;
                htop_min_tmp=h_top;
            }

            if(v_top<vtop_min_tmp)
            {
                vert_vtop_tmp=vertice;
                vtop_min_tmp=v_top;
            }

            if(h_bot<hbot_min_tmp)
            {
                vert_hbot_tmp=vertice;
                hbot_min_tmp=h_bot;
            }

            if(v_bot<vbot_min_tmp)
            {
                vert_vbot_tmp=vertice;
                vbot_min_tmp=v_bot;
            }



            //global
            if(h_top<htop_min)
            {
                vert_htop=vertice;
                htop_min=h_top;
            }
            if(v_top<vtop_min)
            {
                vert_vtop=vertice;
                vtop_min=v_top;
            }
            if(h_bot<hbot_min)
            {
                vert_hbot=vertice;
                hbot_min=h_bot;
            }
            if(v_bot<vbot_min)
            {
                vert_vbot=vertice;
                vbot_min=v_bot;
            }
            if(vertice[2]>z_max)
                z_max=vertice[2];
        }

        std::vector<Vec3f> vec_extremums;
        vec_extremums.push_back(vert_hbot_tmp);
        vec_extremums.push_back(vert_htop_tmp);
        vec_extremums.push_back(vert_vbot_tmp);
        vec_extremums.push_back(vert_vtop_tmp);
        vec_vec_extremums.push_back(vec_extremums);




        //debug:
        cout<<endl<<"One box Added";
        iter_pp++;
        iter_rules++;
        delete scn;
        delete med;
        delete lp;
    }

    //save pos on Zaxe for camera
    float z_cam=0;
    float yp_max = vert_vtop[1]-tan(fovy/2)*(fabs(z_max-vert_vtop[2]));
    float yp_min = vert_vbot[1]+tan(fovy/2)*(fabs(z_max-vert_vbot[2]));
    float xp_max = vert_htop[0]-tan(fovy/2)*(fabs(z_max-vert_htop[2]));
    float xp_min = vert_hbot[0]+tan(fovy/2)*(fabs(z_max-vert_hbot[2]));

    double pos_h = ((fabs(xp_min-xp_max))/2.0)*1.05/tan(fovy/2)+z_max;
    double pos_v = ((fabs(yp_min-yp_max))/2.0)*1.05/tan(fovy/2)+z_max;
    z_cam = (std::fmax(pos_h,pos_v));





    pp_idx=0;
    iter_pp=vec_pp_wrap.begin();
    std::vector<std::vector<CGLA::Vec3f>>::iterator iter_vec_ext=vec_vec_extremums.begin();

    if (!color_config_fn.empty())
    {
        mc = new PModelingConfig(*mod_config);
    }
    while (iter_pp!=vec_pp_wrap.end() && iter_vec_ext != vec_vec_extremums.end())
    {
        QFile file_tmp(filelist_lsys.at(pp_idx));
        QFile file_pp_tmp(filelist_pp.at(pp_idx));
        std::stringstream ss_tmp, ss_pp_tmp;

        lp = new PLSystemParametric(*lsystem, false);
        scn = new Scene(lp);
        med = new PMediator(lp);
        pMainViewer->setScene(scn);
        if (mc)
        {
            scn->setModConfig(mc);
            mc->smart_save_color(filelist_lsys.at(pp_idx).toStdString());
            if (!shape_config_fn.empty())
                mc->smart_save_shape(filelist_lsys.at(pp_idx).toStdString());
        }

        //mediator->set_img_res(pMainViewer, get_snapshot_format());/*This function needs to be rewritten*/
        if(vec_ruleidx_wrap.at(pp_idx)>nrules)
            for(int i=0; i<nrules;i++)
                lp->rewrite_rule(i, &(*iter_pp));
        else lp->rewrite_rule(vec_ruleidx_wrap.at(pp_idx), &(*iter_pp));

        lp->to_std_stream(ss_tmp);
        (*iter_pp).to_csv_stream(ss_pp_tmp);
        in_out_rec->save(file_tmp, ss_tmp);
        in_out_rec->save(file_pp_tmp, ss_pp_tmp);
        lp->init_dh();
        srand(seed);
        for (int k=0; k<lsystem->getNumDerivation(); k++)
            lp->derive();

        QString sfn = pMainViewer->stateFileName();
        pMainViewer->setStateFileName(sfn);
        pMainViewer->setCamera(pMainViewer->camera());
        pMainViewer->updateGL();

        std::cout << std::endl << "before\n" ;

        if(pMainViewer->restoreStateFromFile())
            std::cout << std::endl << "Failed to load current state!";

        qglviewer::Camera* cam=pMainViewer->camera();
        cam->setPosition(qglviewer::Vec(0.0,0.0,1.0));
        //pMainViewer->updateGL();

        cam->lookAt(pMainViewer->sceneCenter());
        //pMainViewer->updateGL();

        cam->setUpVector(qglviewer::Vec(0.0,1.0,0.0));
        //pMainViewer->updateGL();

        cam->setType(qglviewer::Camera::PERSPECTIVE);
        //pMainViewer->updateGL();

        cam->showEntireScene();
        pMainViewer->setCamera(cam);

        pMainViewer->updateGL();

        qglviewer::Vec cam_pos= pMainViewer->camera()->position();


        std::vector<Vec3f> vec_extremums= (*iter_vec_ext);
        float y_max = vec_extremums.at(3)[1]-tan(fovy/2)*(fabs(z_max-vec_extremums.at(3)[2]));
        float y_min = vec_extremums.at(2)[1]+tan(fovy/2)*(fabs(z_max-vec_extremums.at(2)[2]));
        float x_max = vec_extremums.at(1)[0]-tan(fovy/2)*(fabs(z_max-vec_extremums.at(1)[2]));
        float x_min = vec_extremums.at(0)[0]+tan(fovy/2)*(fabs(z_max-vec_extremums.at(0)[2]));


        //fix position of camera
        cam_pos[2]=z_cam;
        cam_pos[0] = (x_min+x_max)/2.0;
        cam_pos[1] = (y_min+y_max)/2.0;
        pMainViewer->camera()->setPosition(cam_pos);
        pMainViewer->updateGL();

        pMainViewer->setGridIsDrawn(false);
        pMainViewer->setAxisIsDrawn(false);
        pMainViewer->updateGL();
        pMainViewer->take_snapshot(get_snapshot_format(), filelist_png.at(pp_idx));
        iter_pp++;
        pp_idx++;
        iter_vec_ext++;
        delete lp;
        delete scn;
        delete med;
    }

    if (!color_config_fn.empty())
    {
        delete mc;
    }
    fn_main_output = QString(file_basename.c_str());
    fn_main_output+="_ar_main.csv";
    save_pca_output(fn_main_output, filelist_pp, filelist_lsys, filelist_png);
    setup_snapshot_browser(vec_pp_wrap, vec_ruleidx_wrap, lsystem);
    pMainViewer->setScene(pMainScene);
    std::cout << std::endl << "Current path : " << work_path.toStdString();
    QDir::setCurrent(QCoreApplication::applicationDirPath());
}


//PMetric MainWindow::check_metrics()
//{
//    PMetric value;
//    PBoundingBox obb;
//    std::vector<CGLA::Vec3f> vec_vertices_incl, vec_vertices_idxr;
//    std::vector<PShapeIndexer*> vec_idxr_tmp;
//    std::vector<unsigned int> vec_slct_tmp;
//    mediator->clear_selection_reviewed();
//    vec_slct_tmp = lsystem->sub_branches();
//    mediator->setVecSelectionReviewed(vec_slct_tmp);

//    vec_idxr_tmp = mediator->select_indexer(pMainScene, mediator->getVecSelectionReviewed());
//    for (std::vector<PShapeIndexer*>::iterator iter=vec_idxr_tmp.begin(); iter!=vec_idxr_tmp.end(); iter++)
//    {
//        vec_vertices_idxr = (*iter)->getVertices();
//        vec_vertices_incl.insert(vec_vertices_incl.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
//    }
//    mediator->add_obb(pMainScene, obb, vec_vertices_incl);
//    pMainViewer->updateGL();
//    value = mediator->compute_metric(lsystem, pMainScene, obb);

//    return value;
//}

//PMetric MainWindow::check_metrics(PLSystemParametric* _lp, Scene* _scn, PMediator* _pmed)
//{
//    PMetric value;
//    PBoundingBox obb;
//    std::vector<CGLA::Vec3f> vec_vertices_incl, vec_vertices_idxr;
//    std::vector<PShapeIndexer*> vec_idxr_tmp;
//    std::vector<unsigned int> vec_slct_tmp;
//    _pmed->clear_selection_reviewed();
//    vec_slct_tmp = _lp->sub_branches();
//    _pmed->setVecSelectionReviewed(vec_slct_tmp);

//    vec_idxr_tmp = mediator->select_indexer(_scn, _pmed->getVecSelectionReviewed());
//    for (std::vector<PShapeIndexer*>::iterator iter=vec_idxr_tmp.begin(); iter!=vec_idxr_tmp.end(); iter++)
//    {
//        vec_vertices_idxr = (*iter)->getVertices();
//        vec_vertices_incl.insert(vec_vertices_incl.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
//    }
//    _pmed->add_obb(_scn, obb, vec_vertices_incl);
//    pMainViewer->updateGL();
//    value = _pmed->compute_metric(_lp, _scn, obb);


//    return value;
//}

void MainWindow::metrics_stats()
{
    const unsigned int METRICS_NB = 11;
    std::vector<PPreproc> vec_pp, vec_pp_out;
    PLSystemParametric *lp, *sub_lp1, *sub_lp2;
    PMediator *med, *med1, *med2;
    Scene *scn, *scn1, *scn2;
    PMetric metric1, metric2;
    PIORecord io;
    PPcaEval pca;
    unsigned int char_size = 1;
    PTextParser textparser;
    std::string alphabet_buffer, alphabet_set_buffer, current_string;
    std::vector<unsigned int> vec_modified_rule;
    std::vector<unsigned int> vec_slct_tmp1, vec_slct_tmp2;
    std::vector<float> vec_params;
    std::vector<float> vec_change_rate_tmp, vec_change_rate;
    std::vector<float> eig_val, vec_mean;
    std::vector< std::vector<float> > vec_vec_change_rate;
    std::vector< std::vector<float> > eig_vec, co_mat;
    std::vector< std::vector<float> > vec2_params;
    std::vector< std::vector< std::vector<float> > > vec3_params;
    std::vector< std::string > vec_alphabet_str;
    std::vector< std::vector<std::string> > vec2_alphabet_str;
    std::vector<CGLA::Vec3f> vec_vertices_incl1, vec_vertices_idxr1;
    std::vector<CGLA::Vec3f> vec_vertices_incl2, vec_vertices_idxr2;
    std::vector<PShapeIndexer*> vec_idxr_tmp1, vec_idxr_tmp2;

    QFile file_co, file_pca;
    QString fn_cst_ovrvw("constants_overview_extrem.csv"), fn_pca("pca_eval_extrem.csv");
    // The next parameters are unused (but required by some methods)
    vector<float> vec_auxiliary_params;
    vector<string> vec_str_auxiliary_params;

    lp = new PLSystemParametric(*lsystem, false);
    scn = new Scene(lp);
    med = new PMediator(lp, scn);

    lp->init_dh();
    for (int k=0; k<lsystem->getNumDerivation()-1; k++)
        lp->derive();
    textparser.setText(lp->getText());
    current_string = lp->getText();
    vec_vec_change_rate.reserve(vec_pp.size()*lp->getVecRules().size()*lp->getVecRules().size());
    for (unsigned int j=0; j<lp->getVecRules().size(); j++)
    {
        for (unsigned int i=0; i<lp->getText().size(); i+=char_size)
        {
            alphabet_buffer = lp->pick_char_at(i, char_size);
            alphabet_buffer = textparser.clean_final_space(alphabet_buffer);
            alphabet_set_buffer = alphabet_buffer;
            if (i+alphabet_buffer.size()<lp->getText().length())
            {
                if (lp->getText()[i+alphabet_buffer.size()]=='(')
                {
                    unsigned int idx_closer = textparser.find_bracket_closer(lp->getText(), i+alphabet_buffer.size());
                    alphabet_set_buffer += lp->getText().substr(i+alphabet_buffer.size(), idx_closer-i-alphabet_buffer.size()+1);
                }
            }
            try
            {
                vec_params = lp->get_map_params()[i];
            }
            catch (...)
            {
                std::cout << "Requesting for invalid map index...";
                return;
            }

            if (    lp->getVecRules().at(j)->is_candidate(  alphabet_buffer,
                                                            vec_params,
                                                            current_string,
                                                            i,
                                                            (lp->getPreproc())?lp->getPreproc()->getIgnore():0,
                                                            vec_str_auxiliary_params,
                                                            vec_auxiliary_params)
                )
            {
                vec2_params.push_back(vec_params);
                vec_params.clear();
                vec_alphabet_str.push_back(alphabet_set_buffer);
            }
        }
        vec3_params.push_back(vec2_params);
        vec2_params.clear();
        vec2_alphabet_str.push_back(vec_alphabet_str);
        vec_alphabet_str.clear();
    }

    vec_pp = lsystem->generate_preproc();
    for (std::vector<PPreproc>::iterator iter_pp=vec_pp.begin(); iter_pp!=vec_pp.end(); iter_pp++)
    {
        for (unsigned int i=0; i<lsystem->getVecRules().size(); i++)
        {
            for (int m=0; m<METRICS_NB; m++)
                vec_change_rate.push_back(0);
            if (vec2_alphabet_str.at(i).size()>0)
            {
                for (std::vector<std::string>::iterator iter_str = vec2_alphabet_str.at(i).begin(); iter_str != vec2_alphabet_str.at(i).end(); iter_str++)
                {
                    PBoundingBox obb_tmp1, obb_tmp2;
                    std::vector<CGLA::Vec3f> vec_vertices_incl_tmp1, vec_vertices_idxr_tmp1;
                    std::vector<CGLA::Vec3f> vec_vertices_incl_tmp2, vec_vertices_idxr_tmp2;
                    sub_lp1 = new PLSystemParametric(*lp, false);
                    sub_lp2 = new PLSystemParametric(*lp, false);
                    scn1 = new Scene(sub_lp1);
                    med1 = new PMediator(sub_lp1, scn1);
                    scn2 = new Scene(sub_lp2);
                    med2 = new PMediator(sub_lp2, scn2);
                    sub_lp1->setText(*iter_str);
                    sub_lp2->setText(*iter_str);

                    sub_lp2->rewrite_rule(i, &(*iter_pp));

                    sub_lp1->init_dh();
                    sub_lp2->init_dh();
                    sub_lp1->derive();
                    sub_lp2->derive();

                    vec_slct_tmp1 = sub_lp1->sub_branches();
                    vec_slct_tmp2 = sub_lp2->sub_branches();
                    med1->clear_selection_reviewed();
                    med1->setVecSelectionReviewed(vec_slct_tmp1);
                    med2->clear_selection_reviewed();
                    med2->setVecSelectionReviewed(vec_slct_tmp2);
                    scn1->draw(false);
                    scn2->draw(false);

                    vec_idxr_tmp1 = med1->select_indexer(scn1, med1->getVecSelectionReviewed());
                    for (std::vector<PShapeIndexer*>::iterator iter_si=vec_idxr_tmp1.begin(); iter_si!=vec_idxr_tmp1.end(); iter_si++)
                    {
                        vec_vertices_idxr_tmp1 = (*iter_si)->getVertices();
                        vec_vertices_incl_tmp1.insert(vec_vertices_incl_tmp1.end(), vec_vertices_idxr_tmp1.begin(),  vec_vertices_idxr_tmp1.end());
                    }
                    vec_idxr_tmp2 = med2->select_indexer(scn2, med2->getVecSelectionReviewed());
                    for (std::vector<PShapeIndexer*>::iterator iter_si=vec_idxr_tmp2.begin(); iter_si!=vec_idxr_tmp2.end(); iter_si++)
                    {
                        vec_vertices_idxr_tmp2 = (*iter_si)->getVertices();
                        vec_vertices_incl_tmp2.insert(vec_vertices_incl_tmp2.end(), vec_vertices_idxr_tmp2.begin(),  vec_vertices_idxr_tmp2.end());
                    }
                    if (!vec_slct_tmp1.empty()&&!vec_slct_tmp2.empty())
                    {
                        med1->add_obb(scn1, obb_tmp1, vec_vertices_incl_tmp1, true);
                        med2->add_obb(scn2, obb_tmp2, vec_vertices_incl_tmp2, true);
                        metric1 = med1->compute_metric(sub_lp1, scn1, obb_tmp1);
                        metric2 = med2->compute_metric(sub_lp2, scn2, obb_tmp2);
                        vec_change_rate_tmp = metric1.compute_diff(metric2);
                        if (vec_change_rate_tmp.size() == vec_change_rate.size())
                            for (unsigned int k=0; k<vec_change_rate_tmp.size(); k++)
                            {
                                vec_change_rate[k] = vec_change_rate[k] + vec_change_rate_tmp[k];
                            }
                        else
                        {
                            std::cout << std::endl << "WARNING: Metric Vectors have different sizes!";
                        }
                    }


                    delete scn1;
                    delete med1;
                    delete sub_lp1;
                    delete scn2;
                    delete med2;
                    delete sub_lp2;
                }
                for (unsigned int l=0; l<vec_change_rate.size(); l++)
                {
                    vec_change_rate[l] = vec_change_rate[l]/vec2_alphabet_str.at(i).size();
                }
            }
            vec_pp_out.push_back(*iter_pp);
            vec_modified_rule.push_back(i);
            vec_vec_change_rate.push_back(vec_change_rate);
            vec_change_rate.clear();
        }
    }
    io.save_to_csv(this, file_co, fn_cst_ovrvw, vec_pp_out, vec_modified_rule, vec_vec_change_rate);
    vec_mean = pca.mean(vec_vec_change_rate);
    co_mat = pca.covariance_matrix(vec_vec_change_rate, vec_mean);
    pca.compute_eigens(co_mat, eig_vec, eig_val);

    io.save_to_csv(this, file_pca, fn_pca, co_mat, eig_vec, eig_val);


    /*************PCA*****COMPRESSION********/
    QString fn_gap;
    QFile file_gap;
    ublas::matrix<float> mx_feat_vec, mx_feat_vec_t, mx_pop_adjust, mx_pop_adjust_t;
    ublas::matrix<float> mx_final_pop, mx_pop_orig, mx_pop_orig_t;
    std::vector<std::vector<float> > eig_vec_cmprss, pop_adj, pop_orig, gap;
    std::vector<float> vec_avrg_gap, vec_err;

    for (size_t i=1; i<=eig_vec.size(); i++)
    {
        mx_feat_vec.clear();
        mx_feat_vec_t.clear();
        mx_pop_orig.clear();
        mx_pop_orig_t.clear();
        mx_pop_adjust.clear();
        mx_pop_adjust_t.clear();
        eig_vec_cmprss.clear();
        gap.clear();
        pop_adj.clear();
        pop_orig.clear();


        fn_gap = QString("gap_%1.csv").arg(i);
        eig_vec_cmprss = pca.select_eigenvector(i);
        pop_adj = pca.adjust_pop(vec_vec_change_rate);
        Procedural::load_matrix<float>(eig_vec_cmprss, mx_feat_vec);
        Procedural::load_matrix<float>(pop_adj, mx_pop_adjust);
        mx_feat_vec_t   = ublas::trans(mx_feat_vec);
        mx_pop_adjust_t = ublas::trans(mx_pop_adjust);
        // Debug


        // Compression
        mx_final_pop    = ublas::prod(mx_feat_vec, mx_pop_adjust_t);
        mx_pop_orig     = ublas::prod(mx_feat_vec_t, mx_final_pop);
        mx_pop_orig_t   = ublas::trans(mx_pop_orig);
        Procedural::save_matrix<float>(mx_pop_orig_t, pop_orig);
        gap = pca.compute_diff(pop_adj, pop_orig);
        // I/O Recording
        io.generic_save<float>(this, file_gap, fn_gap, gap,  std::vector<std::string>(), std::vector<std::string>(), std::string(""), fn_gap.toStdString());
        vec_err.push_back(pca.compute_error(gap));
        gap = pca.abs(gap);
        vec_avrg_gap.push_back(pca.mean2(gap));
    }
    std::cout << std::endl << "Average Means for every order :" << std::endl;
    for (size_t i=0; i<=vec_avrg_gap.size(); i++)
    {
        std::cout << vec_avrg_gap[i] << "\t";
    }
    std::cout << std::endl << "Error Rating for every order :" << std::endl;
    for (size_t i=0; i<=vec_err.size(); i++)
    {
        std::cout << vec_err[i] << "\t";
    }

    // Output a 3D plot from 3 features vectors
    std::vector< std::vector<float> > compressed_data = pca.compress_data(vec_vec_change_rate, 3);
    pcaViewer->update_pt3d(compressed_data);

    pca.wrapper_pt3d(compressed_data);
}

void MainWindow::on_pushButtonSimConst_clicked()
{
    std::vector<PPreproc> vec_pp, vec_pp_out;
    std::vector<PShapeIndexer*> vec_idxr_tmp;
    std::vector<CGLA::Vec3f> vec_vertices_incl, vec_vertices_idxr;
    std::vector<unsigned int> vec_slct_tmp;
    std::vector<unsigned int> vec_modified_rule;
    std::vector<float> vec_change_rate;
    std::vector<float> eig_val, vec_mean;
    std::vector<std::vector<float> > vec_vec_change_rate;
    std::vector<std::vector<float> > eig_vec, co_mat;
    std::vector<PMetric> vec_mtrc;
    PBoundingBox obb;
    PMetric metric, main_metric;
    PIORecord io;
    PPcaEval pca;
    QFile file_co, file_pca;
    QString fn_cst_ovrvw("constants_overview.csv"), fn_pca("pca_eval.csv"), fn_main_output;
    PLSystemParametric* lp;
    PMediator* med;
    Scene* scn;
    PModelingConfig* mc=0;

    //QDir curr_dir(QDir::currentPath());
    QString work_path(QDir::currentPath());
    work_path += "/";
    work_path += file_basename.c_str();
    if (!QDir(file_basename.c_str()).exists())
    {
        QString dir_name(file_basename.c_str());
        QDir().mkdir(dir_name);
    }
    QDir::setCurrent(work_path);
    std::cout << std::endl << "Current path : " << work_path.toStdString();
    // Main Metric is computed first
    mediator->clear_selection_reviewed();
    vec_slct_tmp = lsystem->sub_branches();
    mediator->setVecSelectionReviewed(vec_slct_tmp);

    if (lsystem->getPreproc()->getVecDefine().empty())
        return;
    time_t seed = lsystem->getPreproc()->getSeed();

    vec_pp = lsystem->generate_preproc();
    vec_idxr_tmp = mediator->select_indexer(pMainScene, mediator->getVecSelectionReviewed());
    for (std::vector<PShapeIndexer*>::iterator iter=vec_idxr_tmp.begin(); iter!=vec_idxr_tmp.end(); iter++)
    {
        vec_vertices_idxr = (*iter)->getVertices();
        vec_vertices_incl.insert(vec_vertices_incl.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
    }
    mediator->add_obb(pMainScene, obb, vec_vertices_incl);
    pMainViewer->updateGL();
    main_metric = mediator->compute_metric(lsystem, pMainScene, obb);
    ui->tableViewMetric->setModel(mediator->setup_table_model(&main_metric));
    vec_vec_change_rate.reserve(vec_pp.size()*lsystem->getVecRules().size()*lsystem->getVecRules().size());

    for (std::vector<PPreproc>::iterator iter_pp=vec_pp.begin(); iter_pp!=vec_pp.end(); iter_pp++)
    {
        for (unsigned int i=0; i<lsystem->getVecRules().size(); i++)
        {
            lp = new PLSystemParametric(*lsystem, false);
            scn = new Scene(lp);
            med = new PMediator(lp, scn);
            PBoundingBox obb_tmp;
            std::vector<CGLA::Vec3f> vec_vertices_incl_tmp, vec_vertices_idxr_tmp;

            lp->rewrite_rule(i, &(*iter_pp));
            lp->init_dh();
            srand(seed);
            for (int k=0; k<lsystem->getNumDerivation(); k++)
                lp->derive();


            vec_slct_tmp = lp->sub_branches();
            med->clear_selection_reviewed();
            med->setVecSelectionReviewed(vec_slct_tmp);
            scn->draw(false);
            vec_idxr_tmp = med->select_indexer(scn, med->getVecSelectionReviewed());
            for (std::vector<PShapeIndexer*>::iterator iter_si=vec_idxr_tmp.begin(); iter_si!=vec_idxr_tmp.end(); iter_si++)
            {
                vec_vertices_idxr_tmp = (*iter_si)->getVertices();
                vec_vertices_incl_tmp.insert(vec_vertices_incl_tmp.end(), vec_vertices_idxr_tmp.begin(),  vec_vertices_idxr_tmp.end());
            }
            med->add_obb(scn, obb_tmp, vec_vertices_incl_tmp, true);
            metric = med->compute_metric(lp, scn, obb_tmp);
            vec_mtrc.push_back(metric);
            vec_pp_out.push_back(*iter_pp);
            vec_modified_rule.push_back(i);
            vec_change_rate = main_metric.compute_diff(metric);
            vec_vec_change_rate.push_back(vec_change_rate);
            //debug:
            cout<<endl<<"Metrics gathered : " << vec_mtrc.size();

            delete scn;
            delete med;
            delete lp;
        }
        //debug:
        static int dbg_csts=0;
        cout<<endl<<"Constants processed : " << ++dbg_csts << endl << "==== ** ==== ** ====";
    }
    vec_mean = pca.mean(vec_vec_change_rate);
    co_mat = pca.covariance_matrix(vec_vec_change_rate, vec_mean);
    pca.compute_eigens(co_mat, eig_vec, eig_val);
    io.save_to_csv(this, file_pca, fn_pca, co_mat, eig_vec, eig_val);
    io.save_to_csv(this, file_co, fn_cst_ovrvw, vec_mtrc, vec_pp_out, vec_modified_rule, vec_vec_change_rate);

    // PCA Compression Process
    QString fn_gap;
    QFile file_gap;
    ublas::matrix<float> mx_feat_vec, mx_feat_vec_t, mx_pop_adjust, mx_pop_adjust_t;
    ublas::matrix<float> mx_final_pop, mx_pop_orig, mx_pop_orig_t;
    std::vector<std::vector<float> > eig_vec_cmprss, pop_adj, pop_orig, gap;
    std::vector<float> vec_avrg_gap, vec_err;

    for (size_t i=1; i<=eig_vec.size(); i++)
    {
        mx_feat_vec.clear();
        mx_feat_vec_t.clear();
        mx_pop_orig.clear();
        mx_pop_orig_t.clear();
        mx_pop_adjust.clear();
        mx_pop_adjust_t.clear();
        eig_vec_cmprss.clear();
        gap.clear();
        pop_adj.clear();
        pop_orig.clear();


        fn_gap = QString("gap_%1.csv").arg(i);
        eig_vec_cmprss = pca.select_eigenvector(i);
        pop_adj = pca.adjust_pop(vec_vec_change_rate);
        Procedural::load_matrix<float>(eig_vec_cmprss, mx_feat_vec);
        Procedural::load_matrix<float>(pop_adj, mx_pop_adjust);
        mx_feat_vec_t   = ublas::trans(mx_feat_vec);
        mx_pop_adjust_t = ublas::trans(mx_pop_adjust);
        // Debug


        // Compression
        mx_final_pop    = ublas::prod(mx_feat_vec, mx_pop_adjust_t);
        mx_pop_orig     = ublas::prod(mx_feat_vec_t, mx_final_pop);
        mx_pop_orig_t   = ublas::trans(mx_pop_orig);
        Procedural::save_matrix<float>(mx_pop_orig_t, pop_orig);
        gap = pca.compute_diff(pop_adj, pop_orig);
        // I/O Recording
        io.generic_save<float>(this, file_gap, fn_gap, gap,  std::vector<std::string>(), std::vector<std::string>(), std::string(""), fn_gap.toStdString());
        vec_err.push_back(pca.compute_error(gap));
        gap = pca.abs(gap);
        vec_avrg_gap.push_back(pca.mean2(gap));
    }
    std::cout << std::endl << "Average Means for every order :" << std::endl;
    for (size_t i=0; i<=vec_avrg_gap.size(); i++)
    {
        std::cout << vec_avrg_gap[i] << "\t";
    }
    std::cout << std::endl << "Error Rating for every order :" << std::endl;
    for (size_t i=0; i<=vec_err.size(); i++)
    {
        std::cout << vec_err[i] << "\t";
    }

    // Output a 3D plot from 3 features vectors
    std::vector< std::vector<float> > compressed_data = pca.compress_data(vec_vec_change_rate, 3);
    pcaViewer->update_pt3d(compressed_data);

    // Wrap the 3D points
    unsigned int nrules;
    const char WRAPPER_FN[] = "wrapper.tmp";
    QFile file_wrapper(WRAPPER_FN);
    std::stringstream ss_wrap;
    std::vector<unsigned int> vec_ruleidx_wrap;
    std::vector<unsigned int> vec_wrapper_idx = pca.wrapper_pt3d(compressed_data);
    for (std::vector<unsigned int>::iterator iter=vec_wrapper_idx.begin(); iter!=vec_wrapper_idx.end(); iter++)
    {
        ss_wrap << (*iter);
        for (std::vector<float>::iterator jter=compressed_data.at(*iter).begin(); jter!=compressed_data.at(*iter).end(); jter++)
        {
            ss_wrap << "," << *jter;
        }
        ss_wrap << std::endl;
    }
    PIORecord::save(file_wrapper, ss_wrap);

    QStringList filelist_png  = generate_filename(file_basename, std::string("png"), 27);
    QStringList filelist_lsys = generate_filename(file_basename, std::string("txt"), 27);
    QStringList filelist_pp   = generate_filename(file_basename, std::string("csv"), 27);
    nrules = lsystem->getVecRules().size();
    mediator->select_preproc(vec_pp, nrules, vec_wrapper_idx, vec_pp_wrap, vec_ruleidx_wrap);

    unsigned int pp_idx=0;
    std::vector<PPreproc>::iterator iter_pp=vec_pp_wrap.begin();
    std::vector<unsigned int>::iterator iter_rules=vec_ruleidx_wrap.begin();
    pMainViewer->saveStateToFile();



    // calculate every projection points of vertices for the 27 trees

    float htop_min=0, hbot_min=0, vtop_min=0, vbot_min=0;
    Vec3f vert_htop, vert_hbot, vert_vtop, vert_vbot;

    float fovy = pMainViewer->camera()->fieldOfView();
    CGLA::Vec3f vectop_v, vecbot_v, vectop_h, vecbot_h;
    vectop_v[0]=0; //x comp
    vectop_v[1]=-cos(fovy/2); //y comp
    vectop_v[2]=-sin(fovy/2); //z comp

    vecbot_v[0]=0;
    vecbot_v[1]= cos(fovy/2);
    vecbot_v[2]= -sin(fovy/2);

    vectop_h[0]=-cos(fovy/2); //x comp
    vectop_h[1]=0; //y comp
    vectop_h[2]=-sin(fovy/2); //z comp

    vecbot_h[0]= cos(fovy/2);
    vecbot_h[1]= 0;
    vecbot_h[2]= -sin(fovy/2);

    float z_max=0;

    std::vector<std::vector<CGLA::Vec3f>> vec_vec_extremums;
    while(iter_pp!=vec_pp_wrap.end() && iter_rules!=vec_ruleidx_wrap.end() )
    {

        lp = new PLSystemParametric(*lsystem, false);
        med = new PMediator(lp);
        scn = new Scene(lp);
        lp->rewrite_rule(*iter_rules, &(*iter_pp));
        lp->init_dh();
        srand(seed);
        for (int k=0; k<lsystem->getNumDerivation(); k++)
            lp->derive();
        std::vector<CGLA::Vec3f> vec_vertices_inc;
        vec_slct_tmp = lp->sub_branches();

        med->setVecSelectionReviewed(vec_slct_tmp);
        scn->draw(true);
        vec_idxr_tmp = med->select_indexer(scn, med->getVecSelectionReviewed());
        for (std::vector<PShapeIndexer*>::iterator iter_si=vec_idxr_tmp.begin(); iter_si!=vec_idxr_tmp.end(); iter_si++)
        {
            vec_vertices_idxr = (*iter_si)->getVertices();
            vec_vertices_inc.insert(vec_vertices_inc.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
        }

        float htop_min_tmp=0, hbot_min_tmp=0, vtop_min_tmp=0, vbot_min_tmp=0;
        Vec3f vert_htop_tmp, vert_hbot_tmp, vert_vtop_tmp, vert_vbot_tmp;

        for (std::vector<CGLA::Vec3f>::iterator iter_vert=vec_vertices_inc.begin(); iter_vert!=vec_vertices_inc.end(); iter_vert++)
        {
            Vec3f vertice = *iter_vert;
            float h_top = dot(vertice,vectop_h);
            float h_bot = dot(vertice,vecbot_h);
            float v_top = dot(vertice,vectop_v);
            float v_bot = dot(vertice,vecbot_v);


            //local
            if(h_top<htop_min_tmp)
            {
                vert_htop_tmp=vertice;
                htop_min_tmp=h_top;
            }

            if(v_top<vtop_min_tmp)
            {
                vert_vtop_tmp=vertice;
                vtop_min_tmp=v_top;
            }

            if(h_bot<hbot_min_tmp)
            {
                vert_hbot_tmp=vertice;
                hbot_min_tmp=h_bot;
            }

            if(v_bot<vbot_min_tmp)
            {
                vert_vbot_tmp=vertice;
                vbot_min_tmp=v_bot;
            }



            //global
            if(h_top<htop_min)
            {
                vert_htop=vertice;
                htop_min=h_top;
            }
            if(v_top<vtop_min)
            {
                vert_vtop=vertice;
                vtop_min=v_top;
            }
            if(h_bot<hbot_min)
            {
                vert_hbot=vertice;
                hbot_min=h_bot;
            }
            if(v_bot<vbot_min)
            {
                vert_vbot=vertice;
                vbot_min=v_bot;
            }
            if(vertice[2]>z_max)
                z_max=vertice[2];
        }

        std::vector<Vec3f> vec_extremums;
        vec_extremums.push_back(vert_hbot_tmp);
        vec_extremums.push_back(vert_htop_tmp);
        vec_extremums.push_back(vert_vbot_tmp);
        vec_extremums.push_back(vert_vtop_tmp);
        vec_vec_extremums.push_back(vec_extremums);




        //debug:
        cout<<endl<<"One box Added";
        iter_pp++;
        iter_rules++;
        delete scn;
        delete med;
        delete lp;
    }

    //save pos on Zaxe for camera
    float z_cam=0;
    float yp_max = vert_vtop[1]-tan(fovy/2)*(fabs(z_max-vert_vtop[2]));
    float yp_min = vert_vbot[1]+tan(fovy/2)*(fabs(z_max-vert_vbot[2]));
    float xp_max = vert_htop[0]-tan(fovy/2)*(fabs(z_max-vert_htop[2]));
    float xp_min = vert_hbot[0]+tan(fovy/2)*(fabs(z_max-vert_hbot[2]));

    double pos_h = ((fabs(xp_min-xp_max))/2.0)*1.05/tan(fovy/2)+z_max;
    double pos_v = ((fabs(yp_min-yp_max))/2.0)*1.05/tan(fovy/2)+z_max;
    z_cam = (std::fmax(pos_h,pos_v));

    pp_idx=0;
    iter_pp=vec_pp_wrap.begin();
    std::vector<std::vector<CGLA::Vec3f>>::iterator iter_vec_ext=vec_vec_extremums.begin();

    // Setup Modeling Configuration
    if ((!color_config_fn.empty())||(!shape_config_fn.empty()))
    {
        mc = new PModelingConfig(*mod_config);
    }
    while (iter_pp!=vec_pp_wrap.end() && iter_vec_ext != vec_vec_extremums.end())
    {

        QFile file_tmp(filelist_lsys.at(pp_idx));
        QFile file_pp_tmp(filelist_pp.at(pp_idx));
        std::stringstream ss_tmp, ss_pp_tmp;

        lp = new PLSystemParametric(*lsystem, false);
        scn = new Scene(lp);
        med = new PMediator(lp);
        pMainViewer->setScene(scn);
        if (mc)
        {
            scn->setModConfig(mc);
            mc->smart_save_color(filelist_lsys.at(pp_idx).toStdString());
            if (!shape_config_fn.empty())
                mc->smart_save_shape(filelist_lsys.at(pp_idx).toStdString());
        }

        //mediator->set_img_res(pMainViewer, get_snapshot_format());/*This function needs to be rewritten*/

        lp->rewrite_rule(vec_ruleidx_wrap.at(pp_idx), &(*iter_pp));
        lp->to_std_stream(ss_tmp);
        (*iter_pp).to_csv_stream(ss_pp_tmp);
        in_out_rec->save(file_tmp, ss_tmp);
        in_out_rec->save(file_pp_tmp, ss_pp_tmp);

        lp->init_dh();
        srand(seed);
        for (int k=0; k<lsystem->getNumDerivation(); k++)
            lp->derive();

        QString sfn = pMainViewer->stateFileName();
        pMainViewer->setStateFileName(sfn);
        pMainViewer->setCamera(pMainViewer->camera());
        pMainViewer->updateGL();

        std::cout << std::endl << "before\n" ;

        if(pMainViewer->restoreStateFromFile())
            std::cout << std::endl << "Failed to load current state!";

        qglviewer::Camera* cam=pMainViewer->camera();
        cam->setPosition(qglviewer::Vec(0.0,0.0,1.0));
        //pMainViewer->updateGL();

        cam->lookAt(pMainViewer->sceneCenter());
        //pMainViewer->updateGL();

        cam->setUpVector(qglviewer::Vec(0.0,1.0,0.0));
        //pMainViewer->updateGL();

        cam->setType(qglviewer::Camera::PERSPECTIVE);
        //pMainViewer->updateGL();

        cam->showEntireScene();
        pMainViewer->setCamera(cam);

        pMainViewer->updateGL();

        qglviewer::Vec cam_pos= pMainViewer->camera()->position();


        std::vector<Vec3f> vec_extremums= (*iter_vec_ext);
        float y_max = vec_extremums.at(3)[1]-tan(fovy/2)*(fabs(z_max-vec_extremums.at(3)[2]));
        float y_min = vec_extremums.at(2)[1]+tan(fovy/2)*(fabs(z_max-vec_extremums.at(2)[2]));
        float x_max = vec_extremums.at(1)[0]-tan(fovy/2)*(fabs(z_max-vec_extremums.at(1)[2]));
        float x_min = vec_extremums.at(0)[0]+tan(fovy/2)*(fabs(z_max-vec_extremums.at(0)[2]));


        //fix position of camera
        cam_pos[2]=z_cam;
        cam_pos[0] = (x_min+x_max)/2.0;
        cam_pos[1] = (y_min+y_max)/2.0;
        pMainViewer->camera()->setPosition(cam_pos);
        pMainViewer->updateGL();

        pMainViewer->setGridIsDrawn(false);
        pMainViewer->setAxisIsDrawn(false);
        pMainViewer->updateGL();
        pMainViewer->take_snapshot(get_snapshot_format(), filelist_png.at(pp_idx));
        iter_pp++;
        pp_idx++;
        iter_vec_ext++;
        delete lp;
        delete scn;
        delete med;
    }

    if (!color_config_fn.empty())
    {
        delete mc;
    }
    fn_main_output = QString(file_basename.c_str());
    fn_main_output+="_main.csv";
    save_pca_output(fn_main_output, filelist_pp, filelist_lsys, filelist_png);
    setup_snapshot_browser(vec_pp_wrap, vec_ruleidx_wrap, lsystem);
    pMainViewer->setScene(pMainScene);
    std::cout << std::endl << "Current path : " << work_path.toStdString();
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    //simConst_allRules(seed);
}

void MainWindow::setup_snapshot_browser(std::vector<PPreproc>& _in_vec_pp, std::vector<unsigned int>& _in_vec_modif_rules_idx, PLSystemParametric* _lp)
{
    imgBrowser->setVecPreproc(_in_vec_pp);
    imgBrowser->setVecModifRulesIdx(_in_vec_modif_rules_idx);
    imgBrowser->setLSystem(_lp);
}

QStringList MainWindow::generate_filename(std::string _prefix, std::string _extension, int _file_number)
{
    QStringList value;
    QString filename_tmp = QString(_prefix.c_str());
    filename_tmp += "_%1.";
    filename_tmp += _extension.c_str();
    for (int i=0; i<_file_number; i++)
    {
        value << filename_tmp.arg(i);
    }
    return value;
}

std::string MainWindow::setup_filename_prefix(std::string _common_root)
{
    QString date_format("MMdd");
    std::string date_part;
    std::string value(_common_root);

    QDateTime dt(QDateTime::currentDateTime());
    date_part = dt.date().toString(date_format).toStdString();
    value+="_";
    value+=date_part;
    return value;
}

void MainWindow::save_pca_output(QString& _file_basename, QStringList& _sl_pp, QStringList& _sl_lsys, QStringList& _sl_png)
{
    QFile file(_file_basename);
    PIORecord io;
    std::stringstream ss;
    if ((_sl_pp.size()!=_sl_lsys.size())||(_sl_lsys.size()!=_sl_png.size()))
        return;
    for (unsigned int i=0; i<_sl_pp.size(); i++)
    {
        ss << i << "," << _sl_lsys.at(i).toStdString() << "," << _sl_pp.at(i).toStdString() << "," << _sl_png.at(i).toStdString() << std::endl;
    }
    io.save(file, ss);
}

void MainWindow::closeEvent(QCloseEvent * event){
    QMainWindow::closeEvent(event);
    exit(0);
}

void MainWindow::reinit()
{
    ui->verticalLayoutViewer->removeWidget(pMainViewer);

    delete lsystem;
    delete in_out_rec;
    delete mediator;
    delete pMainScene;
    delete pMainViewer;
    delete mod_config;

    delete outputDlg;
    delete treeviewDlg;
    delete pcaViewer;
    delete imgBrowser;
    delete dlgViewer;
    delete qstd_imodel;

    lsystem     = new PLSystemParametric();
    in_out_rec  = new PIORecord();
    pMainScene  = new Scene(lsystem);
    mediator    = new PMediator(lsystem, pMainScene);
    pMainViewer = new Viewer(pMainScene, CONSTRAINTS_FREE_VIEW, this);
    mod_config  = new PModelingConfig(*lsystem->getAlphabet());
    outputDlg = new OutputDialog(this);
    treeviewDlg = new TreeViewDialog(this);
    pcaViewer = new PPcaViewer(this);
    imgBrowser = new PImgBrowser(this);
    dlgViewer = new DialogViewer(this);
    qstd_imodel = new QStandardItemModel();

    ui->verticalLayoutViewer->addWidget(pMainViewer);
    pMainViewer->showEntireScene();
    current_filename.clear();
    file_basename.clear();
    color_config_fn.clear();


    ui->tableViewConstVal->reset();


    ui->textEditPreproc->clear();
    ui->textEditProduction->clear();
    ui->lineEditAxiom->clear();
    ui->spinBoxAngle->clear();
    ui->spinBoxDepth->clear();
   }

void MainWindow::update_color_cb()
{
    QPalette palette;
    std::vector<float> vec_color_tmp;

    ui->comboBoxAlphabet->clear();
    for (size_t i=0; i<mod_config->getAlphabet().getVecAlphabet().size(); i++)
    {
        ui->comboBoxAlphabet->addItem(QString(mod_config->getAlphabet().getVecAlphabet().at(i).c_str()));
        vec_color_tmp = mod_config->check_color(i);
    }
    if (!mod_config->getAlphabet().getVecAlphabet().empty())
    {
        vec_color_tmp = mod_config->check_color(0);
        ui->comboBoxAlphabet->setCurrentIndex(0);
        palette.setColor(backgroundRole(), QColor(vec_color_tmp[0]*255, vec_color_tmp[1]*255, vec_color_tmp[2]*255));
        ui->widgetColor->setPalette(palette);
        ui->widgetColor->setAutoFillBackground(true);
        update_shape_line();
    }
}

void MainWindow::update_shape_line()
{
    if (!mod_config->getVecShapeStr().empty())
        ui->comboBoxlineEditShape->setCurrentText(mod_config->check_shape(ui->comboBoxAlphabet->currentIndex()).c_str());
}

void MainWindow::mouseReleaseEvent(QMouseEvent* e)
{
//    if ((e->button() == Qt::LeftButton)
//            &&(e->x()>950)&&(e->x()<1012)
//            &&(e->y()>475)&&(e->y()<492))
    if (    (e->button() == Qt::LeftButton)
            &&(e->x()>this->width() - (1024 - 950))&&(e->x()<this->width() - (1024 - 1012))
            &&(e->y()>this->height() - (530 - 475))&&(e->y()<this->height() - (530 - 492))
       )
    {
        on_widgetColor_clicked();
        //std::cout << std::endl << "Mouse coordinates : " << e->x() << ", " << e->y();
    }
    else QMainWindow::mouseReleaseEvent(e);
}

void MainWindow::on_widgetColor_clicked()
{
    QPalette palette;
    std::vector<float> shape_glcolor;
    std::stringstream my_stream;
    QColor dflt_qcolor(128, 89, 13);
    QColor shape_qcolor = QColorDialog::getColor(dflt_qcolor, this, "Pick Shape Color");

    if (shape_qcolor.isValid())
    {
        size_t idx_tmp = ui->lineEditCurrentSelection->text().toStdString().find_first_of("(");
        if (idx_tmp==std::string::npos)
            idx_tmp = 1;

        shape_glcolor.push_back(Procedural::brute_format((float)shape_qcolor.red()/255.0f, 3));
        shape_glcolor.push_back(Procedural::brute_format((float)shape_qcolor.green()/255.0f, 3));
        shape_glcolor.push_back(Procedural::brute_format((float)shape_qcolor.blue()/255.0f, 3));
        shape_glcolor.push_back(1.0f);
        if(ui->lineEditCurrentSelection->text().length() > 0)
        {
            mod_config->change_color(ui->lineEditCurrentSelection->text().left(idx_tmp).toStdString(), shape_glcolor);
        } else
        {
            mod_config->change_color(ui->comboBoxAlphabet->currentText().toStdString(), shape_glcolor);
        }
        mod_config->color_stream(my_stream);
        ui->textEditConfigContent->setText(my_stream.str().c_str());
        pMainScene->setSelectionChanged(true); // Force 3d scene refresh
        pMainViewer->updateGL();

        palette.setColor(backgroundRole(), shape_qcolor);
        ui->widgetColor->setPalette(palette);
        ui->widgetColor->setAutoFillBackground(true);
    }
}

void MainWindow::on_comboBoxAlphabet_currentIndexChanged(int index)
{
    QPalette palette;
    std::vector<float> alphabet_color = mod_config->check_color(index);
    if ((!alphabet_color.empty())&&(alphabet_color.size()>=3))
    {
        QColor alphabet_qcolor( alphabet_color[0]*255,
                                alphabet_color[1]*255,
                                alphabet_color[2]*255);
        palette.setColor(backgroundRole(), alphabet_qcolor);
        ui->widgetColor->setPalette(palette);
        ui->widgetColor->setAutoFillBackground(true);
        update_shape_line();
    }

}

void MainWindow::on_comboBoxlineEditShape_currentIndexChanged(int index)
{
    std::string new_shape_str = ui->comboBoxlineEditShape->currentText().toStdString();//ui->lineEditShape->text().simplified().toStdString();
    if ((!mod_config->getVecShapeStr().empty())&&(!shape_config_fn.empty()))
    {
        size_t idx_tmp = ui->lineEditCurrentSelection->text().toStdString().find_first_of("(");
        if (idx_tmp==std::string::npos)
            idx_tmp = 1;
        if(ui->lineEditCurrentSelection->text().length() > 0)
        {
            mod_config->change_shape(ui->lineEditCurrentSelection->text().left(idx_tmp).toStdString(), new_shape_str);
        }
        else
        {
            mod_config->change_shape(ui->comboBoxAlphabet->currentText().toStdString(), new_shape_str);
        }
        mod_config->save_shape(shape_config_fn);
    }
    pMainScene->setSelectionChanged(true); // Force 3d scene refresh
    pMainViewer->updateGL();

}

void MainWindow::load_targetqlfr()
{
    std::ifstream filetarget("targetqlfrcoords.csv");
    std::stringstream ss_target;
    std::vector<float> vec_tmp;
    std::vector<std::vector <std::string> > vecs_targetqlfrdatas;
    ss_target << filetarget.rdbuf();
    filetarget.close();
    PIORecord::csv_to_data(vecs_targetqlfrdatas,ss_target);
    vec_targetqlfr.clear();
    vecs_targetqlfrcoords.clear();

    for (std::vector<std::vector <std::string> >::iterator iter = vecs_targetqlfrdatas.begin(); iter != vecs_targetqlfrdatas.end(); iter++)
    {
        if ((*iter).size()>=4)
        {
            vec_tmp.push_back(PTextParser::stdstr_to_float((*iter)[0]));
            vec_tmp.push_back(PTextParser::stdstr_to_float((*iter)[1]));
            vec_tmp.push_back(PTextParser::stdstr_to_float((*iter)[2]));
            vecs_targetqlfrcoords.push_back(vec_tmp);
            std::string qlfrtmp = (*iter)[3];
            size_t strfound = qlfrtmp.find("\r");
            if(strfound!=std::string::npos)
            {
                qlfrtmp.erase(strfound);
                boost::algorithm::to_lower(qlfrtmp);
            }
            vec_targetqlfr.push_back(qlfrtmp);
            vec_tmp.clear();
        }
    }
    ui->comboBoxTargetQualifier->clear();
    ui->comboBoxTargetQualifier->addItem(" ");
    for (std::vector<std::string>::iterator iter = vec_targetqlfr.begin(); iter != vec_targetqlfr.end(); iter++)
    {
        ui->comboBoxTargetQualifier->addItem((*iter).c_str());
    }
}

void MainWindow::partialregen_simul(std::vector<PDerivHistoric*> _vec_pdh_regen, std::string _qlfr, PLSystemParametric* _lsystem)
{
    std::ofstream file("qlfr_timeline.txt", std::ofstream::out|std::ofstream::app);
    std::stringstream ss_buf;
    struct timeval dbg_before, dbg_after, dbg_tmp;

    std::vector<PDerivHistoric*> vec_activregen;
    std::vector<float> vec_target_coords;
    std::vector<size_t> vec_target_idxorder;
    std::vector<unsigned int> vec_nbregen;
    std::string str_exbasename;
    std::stringstream ss_constants, ss_clouds;
    PPreproc* pp;
    unsigned int leafdist;
    QString qs_constantfn, qs_cloudfn;

    for (std::vector<PDerivHistoric*>::iterator iter=_vec_pdh_regen.begin(); iter!=_vec_pdh_regen.end(); iter++)
    {
        leafdist = (*iter)->dist_to_leaf(0);
        if (leafdist!=0)
        {
            vec_activregen.push_back(*iter);
            vec_nbregen.push_back(leafdist);
        }
        (*iter)->del_child();
    }
    _lsystem->update_leaves();

    pp = new PPreproc(*_lsystem->getPreproc());
    //Now we proceed on used cases
    QString qs_constextension("_0912_constants.csv");
    QString qs_cloudextension("_0912_scatterplot.tmp");
    QStringList qsl_ex;

    qsl_ex << "alternative_branch";
    qsl_ex << "honda_monopodial";
    qsl_ex << "honda_sympodial";
    qsl_ex << "ternary_branching";
    qsl_ex << "tree1";
    qsl_ex << "tree2";
    qsl_ex << "tree3";

    // We look from the current file name if the grammar was previously analyzed
    str_exbasename = Procedural::file_basename(current_filename);

    // From here we want to know if we need to look from specific file
    // Existing file will be analyzed whether the sample is known
    QStringListIterator qsli(qsl_ex);
    while (qsli.hasNext())
    {
        QString qs_tmp = qsli.next();
        if (str_exbasename.find(qs_tmp.toStdString())!=std::string::npos)
        {
            qs_constantfn = qs_tmp + qs_constextension;
            qs_cloudfn    = qs_tmp + qs_cloudextension;
        }
    }

    if (qs_constantfn.isEmpty()==false)
    {
        std::vector<std::vector <std::string> > vecs_constdatas, vecs_clouddatas;
        std::vector<std::vector <float> > vecs_cloudcoords;
        std::vector<float> vec_targetdist;
        std::ifstream fileconst(qs_constantfn.toStdString());
        std::ifstream filecloud(qs_cloudfn.toStdString());

        gettimeofday(&dbg_before, NULL);
        ss_constants << fileconst.rdbuf();
        ss_clouds    << filecloud.rdbuf();
        PIORecord::csv_to_data(vecs_constdatas, ss_constants);
        PIORecord::csv_to_data(vecs_clouddatas, ss_clouds);
        gettimeofday(&dbg_after, NULL);
        ss_buf << std::endl << "Loading cloud datas to stack : " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);

        // Constant data file has header
        if (vecs_constdatas.size()==vecs_clouddatas.size()+1)
        {
            gettimeofday(&dbg_before, NULL);
            for (std::vector<std::vector <std::string> >::iterator iter=vecs_clouddatas.begin();iter!=vecs_clouddatas.end();iter++)
            {
                std::vector<float> vec_tmp;
                if ((*iter).size()>=3)
                {
                    vec_tmp.push_back(PTextParser::stdstr_to_float((*iter)[0]));
                    vec_tmp.push_back(PTextParser::stdstr_to_float((*iter)[1]));
                    vec_tmp.push_back(PTextParser::stdstr_to_float((*iter)[2]));
                    vecs_cloudcoords.push_back(vec_tmp);
                }
            }

            vec_target_coords = qlfr_coords(_qlfr);
            for (std::vector<std::vector <float> >::iterator iter=vecs_cloudcoords.begin();iter!=vecs_cloudcoords.end();iter++)
            {
                if (vec_target_coords.size()==(*iter).size())
                    vec_targetdist.push_back(Procedural::euclidist(vec_target_coords,*iter));
            }
            vec_target_idxorder = Procedural::order_by_idx(vec_targetdist);
            gettimeofday(&dbg_after, NULL);
            ss_buf << std::endl << "Computing closest data point : " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);

            if(_lsystem != 0)
            {
                if(val == true)
                {
                    std::vector<std::string> vec_varname;
                    std::vector<float> vec_varval;

                    gettimeofday(&dbg_before, NULL);

                    int modify_rule = (int) PTextParser::stdstr_to_float(vecs_constdatas[vec_target_idxorder[0]+1][0]);
                    PRuleParametric *prule = new PRuleParametric();
                    PPreproc *pptmp = new PPreproc(*_lsystem->getPreproc());
                    --modify_rule;
                    *prule = *_lsystem->getVecRules()[modify_rule];
                    for(int i = 0; i < _lsystem->getPreproc()->getVecDefine().size(); i++)
                    {
                        vec_varname.push_back(vecs_constdatas[0][1+i]);
                        vec_varval.push_back(PTextParser::stdstr_to_float(vecs_constdatas[vec_target_idxorder[0]+1][1+i]));
                    }
                    if(_lsystem->getPreproc()->getVecDefine().size()==vec_varname.size())
                    {
                        for(int i = 0; i < _lsystem->getPreproc()->getVecDefine().size(); i++)
                        {
                            std::string strtmp = _lsystem->getPreproc()->getVecDefine()[i]->getConstantName();
                            for(int j = 0; j < vec_varname.size(); j++)
                            {
                                if(vec_varname[j].compare(strtmp) == 0)
                                    _lsystem->getPreproc()->getVecDefine()[i]->setConstantValue(vec_varval[j]);
                            }
                        }
                    }
                    _lsystem->rewrite_rule(modify_rule, _lsystem->getPreproc());

                    gettimeofday(&dbg_after, NULL);
                    ss_buf << std::endl << "Initializing L-system for regeneration : " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);

                    gettimeofday(&dbg_before, NULL);
                    for (std::vector<PDerivHistoric*>::iterator iter=vec_activregen.begin(); iter!=vec_activregen.end(); iter++)
                    {
                        int deriv_idx = (*iter)->getIdxIntoDeriv();
                        int deriv_nb = vec_nbregen[iter-vec_activregen.begin()];
                        _lsystem->generate(deriv_idx,deriv_nb);
                        _lsystem->update_leaves();
                    }
                    gettimeofday(&dbg_after, NULL);
                    ss_buf << std::endl << "Regenerating parts : " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);

                    *_lsystem->getPreproc() = *pptmp;
                    *_lsystem->getVecRules()[modify_rule] = *prule;

                    //pMainViewer->setFocus();
                    //pMainViewer->updateGL();

    //                ofstream myfile;
    //                myfile.open ("Coordinates.txt");
    //                myfile << PTextParser::stdstr_to_float(vecs_clouddatas[vec_target_idxorder[0]][0]) << ", " << PTextParser::stdstr_to_float(vecs_clouddatas[vec_target_idxorder[0]][1]) << ", " << PTextParser::stdstr_to_float(vecs_clouddatas[vec_target_idxorder[0]][2]) << "\n";
    //                myfile.close();
                    //std::cout << std::endl  << " Coordinates :::: " << PTextParser::stdstr_to_float(vecs_clouddatas[vec_target_idxorder[0]][0]) << "," << PTextParser::stdstr_to_float(vecs_clouddatas[vec_target_idxorder[0]][1]) << "," << PTextParser::stdstr_to_float(vecs_clouddatas[vec_target_idxorder[0]][2]);

                    //pMainViewer->setFocus();
                    //pMainViewer->updateGL();

                    gettimeofday(&dbg_before, NULL);
                    //on_pushButtonSaveImage_clicked();
                    SaveImage( _lsystem);
                    gettimeofday(&dbg_after, NULL);
                    ss_buf << std::endl << "Saving snapshot from L-system: " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);
                } else {

                    std::vector<std::string> vec_varname;
                    std::vector<float> vec_varval;

                    gettimeofday(&dbg_before, NULL);

                    int modify_rule = (int) PTextParser::stdstr_to_float(vecs_constdatas[vec_target_idxorder[0]+1][0]);
                    //PRuleParametric *prule = new PRuleParametric();
                    //PPreproc *pptmp = new PPreproc(*_lsystem->getPreproc());
                    --modify_rule;
                    //*prule = *_lsystem->getVecRules()[modify_rule];
                    for(int i = 0; i < _lsystem->getPreproc()->getVecDefine().size(); i++)
                    {
                        vec_varname.push_back(vecs_constdatas[0][1+i]);
                        vec_varval.push_back(PTextParser::stdstr_to_float(vecs_constdatas[vec_target_idxorder[0]+1][1+i]));
                    }
                    if(_lsystem->getPreproc()->getVecDefine().size()==vec_varname.size())
                    {
                        for(int i = 0; i < _lsystem->getPreproc()->getVecDefine().size(); i++)
                        {
                            std::string strtmp = _lsystem->getPreproc()->getVecDefine()[i]->getConstantName();
                            for(int j = 0; j < vec_varname.size(); j++)
                            {
                                if(vec_varname[j].compare(strtmp) == 0)
                                    _lsystem->getPreproc()->getVecDefine()[i]->setConstantValue(vec_varval[j]);
                            }
                        }
                    }
                    _lsystem->rewrite_rule(modify_rule, _lsystem->getPreproc());

                    gettimeofday(&dbg_after, NULL);
                    ss_buf << std::endl << "Initializing L-system for regeneration : " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);

                    gettimeofday(&dbg_before, NULL);
                    /*for (std::vector<PDerivHistoric*>::iterator iter=vec_activregen.begin(); iter!=vec_activregen.end(); iter++)
                    {
                        int deriv_idx = (*iter)->getIdxIntoDeriv();
                        int deriv_nb = vec_nbregen[iter-vec_activregen.begin()];
                        _lsystem->generate(deriv_idx,deriv_nb);
                        _lsystem->update_leaves();
                    }*/
                    qThreads.push_back(new QlfrThread(_lsystem, modify_rule, vec_activregen, vec_nbregen));
                    qThreads.back()->start();
                    gettimeofday(&dbg_after, NULL);
                    ss_buf << std::endl << "Regenerating parts : " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);

                    //*_lsystem->getPreproc() = *pptmp;
                    //*_lsystem->getVecRules()[modify_rule] = *prule;

                }
           }
        }
    }
    if (file.is_open())
    {
        file << ss_buf.str();
        file.close();
    }
    else
    {
        std::cout << std::endl << "WARNING: Could not open qlfr_timeline.txt...";
    }
}

void MainWindow::on_pushButtonApplyQualifier_clicked()
{
    std::ofstream fileselect("selectfile.txt", std::ofstream::out|std::ofstream::app);
    std::stringstream ss_select;

    std::vector<PDerivHistoric*> vec_pdh_tmp, vec_pdh_slctd, vec_pdh_root, vec_pdh_leavesremoved;

    std::deque<std::shared_ptr<PGraphCreator>> dqgraphcreator;
    std::deque<std::shared_ptr<Node>> dqslctdnodes;
    std::deque<std::shared_ptr<GraphEdge>> dqslctdedges;
    std::vector<std::string> vecgraphfilename, vecgraphslctnfilename, vecstrtmp;
    std::shared_ptr<PGraphCreator> gcreatortmp;
    std::shared_ptr<Node> nodetmp;
    std::shared_ptr<GraphEdge> gedgetmp;
    std::list<float> list_capty;
    std::string targetqlfr;
    QList<int> list_idx_tmp;
    PTextParser textparser;

    targetqlfr = ui->comboBoxTargetQualifier->currentText().toStdString();

    if(targetqlfr.compare(" ") == 0)
    {
        std::cout << std::endl << " Preselection!";
        list_idx_slt = getScene()->getListSelection();

    }
    else
    {
        // BEGIN graph creation process
        if(list_idx_slt.size() > 0 )
        {
            list_idx_tmp = list_idx_slt;
        }
        else
        {
            list_idx_tmp = getScene()->getListSelection();
        }

        for (auto i: list_idx_tmp)
            vec_pdh_slctd.push_back(lsystem->select_historic(i));
        vec_pdh_root = lsystem->getVecVecDhV2().front();
        for (auto pdh: vec_pdh_root)
        {
            gcreatortmp = std::shared_ptr<PGraphCreator>(new PGraphCreator());
            //std::cout << std::endl << lsystem->getAlphabet()->getText() << std::endl;
            nodetmp = gcreatortmp->createntree(pdh, lsystem->getAlphabet());
            if (nodetmp)
            {
                gcreatortmp->settreenoderoot(nodetmp);
                gedgetmp = gcreatortmp->creategraph(nodetmp);
                dqgraphcreator.push_back(gcreatortmp);
            }
        }
        textparser.setText("graph");
        vecstrtmp = textparser.add_digit_suffix(dqgraphcreator.size());
        for (int i=0; i < vecstrtmp.size(); i++)
            vecgraphfilename.push_back(vecstrtmp.at(i)+".xml");
        textparser.setText("graphslctn");
        vecstrtmp = textparser.add_digit_suffix(dqgraphcreator.size());
        for (int i=0; i < vecstrtmp.size(); i++)
            vecgraphslctnfilename.push_back(vecstrtmp.at(i)+".xml");
        for (int i=0; i<dqgraphcreator.size(); i++)
        {
            std::stringstream ssgraph;
            std::ofstream filetmp(vecgraphfilename[i]);

            if (filetmp.is_open())
            {
                dqgraphcreator[i]->createdisplay(dqgraphcreator[i]->getgedgeroot(), ssgraph);
                filetmp << ssgraph.str();
                filetmp.close();
            }
            dqgraphcreator[i]->initcapacity();
        }
        for (auto i:list_idx_tmp)
            vec_pdh_tmp.push_back(lsystem->select_historic(i));
        for (auto gc:dqgraphcreator)
        {
            for (auto pdh:vec_pdh_tmp)
            {
                nodetmp = gc->treenode_from_dh(pdh);
                if (nodetmp)
                {
                    dqslctdnodes.push_back(nodetmp);
                    gedgetmp = gc->edge_from_treenode(nodetmp);
                    if (gedgetmp)
                        dqslctdedges.push_back(gedgetmp);
                }
            }
        }
        list_capty = PGraphCreator::captyprogression(ui->spinBoxSmartSelection->value());
        for (auto edge:dqslctdedges)
            edge->transfercity(list_capty, 2);
        for (int i=0; i<dqgraphcreator.size(); i++)
        {
            std::stringstream ssgraphslctn;
            std::ofstream fileslctntmp(vecgraphslctnfilename[i]);
            dqgraphcreator[i]->graphcut();
            vec_pdh_tmp = dqgraphcreator[i]->selected_derivhistoric();
            vec_pdh_slctd.insert(vec_pdh_slctd.end(), vec_pdh_tmp.begin(), vec_pdh_tmp.end());
            if (fileslctntmp.is_open())
            {
                dqgraphcreator[i]->createdisplay(dqgraphcreator[i]->getgedgeroot(), dqslctdnodes, ssgraphslctn);
                fileslctntmp << ssgraphslctn.str();
                fileslctntmp.close();
            }
        }
        std::cout << std::endl << vec_pdh_slctd.size() << " historic instance(s) selected!";
        for (auto pdh: vec_pdh_slctd)
        {
            if (!pdh->getVecChildren().empty())
                vec_pdh_leavesremoved.push_back(pdh);
        }
        std::cout << std::endl << vec_pdh_leavesremoved.size() << " historic instance(s) selected with leaves removed!";
        // END graph creation process

        //Remove selection color
        pMainScene->clear_list_selection();
        pMainScene->setSelectionChanged(true);


        targetqlfr = ui->comboBoxTargetQualifier->currentText().toStdString();
        boost::algorithm::to_lower(targetqlfr);
        qualifier = targetqlfr;


        ss_select << qualifier << std::endl;
        ss_select << ui->spinBoxSmartSelection->value() << std::endl;
        for (auto i:list_idx_tmp)
            ss_select << i << ",";
        ss_select << std::endl;

        if (fileselect.is_open())
        {
            fileselect << ss_select.str();
            fileselect.close();
        }
        else
        {
            std::cout << std::endl << "WARNING: Could not open selectfile.txt...";
        }

        partialregen_simul(vec_pdh_slctd, qualifier, lsystem);
    }
}

void MainWindow::ApplyQualifier(std::string _qlfr)
{
    std::ofstream file("qlfr_timeline.txt", std::ofstream::out|std::ofstream::app);
    std::stringstream ss_buf;

    struct timeval dbg_before, dbg_after, dbg_tmp;
    std::vector<PDerivHistoric*> vec_pdh_tmp, vec_pdh_slctd, vec_pdh_root, vec_pdh_leavesremoved;

    std::deque<std::shared_ptr<PGraphCreator>> dqgraphcreator;
    std::deque<std::shared_ptr<Node>> dqslctdnodes;
    std::deque<std::shared_ptr<GraphEdge>> dqslctdedges;
    std::vector<std::string> vecgraphfilename, vecgraphslctnfilename, vecstrtmp;
    std::shared_ptr<PGraphCreator> gcreatortmp;
    std::shared_ptr<Node> nodetmp;
    std::shared_ptr<GraphEdge> gedgetmp;
    std::list<float> list_capty;
    std::string targetqlfr;
    QList<int> list_idx_tmp;
    PTextParser textparser;


    //std::string target;

    //target = ui->comboBoxTargetImage->currentText().toStdString();

    targetqlfr = ui->comboBoxTargetQualifier->currentText().toStdString();

    gettimeofday(&dbg_before, NULL);
    //original_copy();
    gettimeofday(&dbg_after, NULL);
    ss_buf << std::endl << "Cloning L-system for " << _qlfr << " : " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);
    //pMainScene->addSelection(list_idx_slt);------***
    //pMainScene->setSelectionChanged(true);------***
    //pMainViewer->setFocus();
    //pMainViewer->updateGL();

        /*for(std::vector<std::string>::iterator iter = vec_targetqlfr.begin(); iter != vec_targetqlfr.end(); iter++)
        {
            //targetqlfr = (*iter).c_str();*/

    // BEGIN graph creation process
            if(list_idx_slt.size() > 0 ){
                   list_idx_tmp = list_idx_slt;
               } else {
                   list_idx_tmp = getScene()->getListSelection();
               }

    for (auto i: list_idx_tmp)
        vec_pdh_slctd.push_back(lsystem_copie->select_historic(i));
    vec_pdh_root = lsystem_copie->getVecVecDhV2().front();

    gettimeofday(&dbg_before, NULL);
    for (auto pdh: vec_pdh_root)
    {
        gcreatortmp = std::shared_ptr<PGraphCreator>(new PGraphCreator());
        //std::cout << std::endl << lsystem_copie->getAlphabet()->getText() << std::endl;
        nodetmp = gcreatortmp->createntree(pdh, lsystem_copie->getAlphabet());
        if (nodetmp)
        {
            gcreatortmp->settreenoderoot(nodetmp);
            gedgetmp = gcreatortmp->creategraph(nodetmp);
            dqgraphcreator.push_back(gcreatortmp);
        }
    }
    gettimeofday(&dbg_tmp, NULL);
    textparser.setText("graph");
    vecstrtmp = textparser.add_digit_suffix(dqgraphcreator.size());
    for (int i=0; i < vecstrtmp.size(); i++)
        vecgraphfilename.push_back(vecstrtmp.at(i)+".xml");
    textparser.setText("graphslctn");
    vecstrtmp = textparser.add_digit_suffix(dqgraphcreator.size());
    for (int i=0; i < vecstrtmp.size(); i++)
        vecgraphslctnfilename.push_back(vecstrtmp.at(i)+".xml");
    for (int i=0; i<dqgraphcreator.size(); i++)
    {
//        std::stringstream ssgraph;
//        std::ofstream filetmp(vecgraphfilename[i]);

//        if (filetmp.is_open())
//        {
//            dqgraphcreator[i]->createdisplay(dqgraphcreator[i]->getgedgeroot(), ssgraph);
//            filetmp << ssgraph.str();
//            filetmp.close();
//        }
        dqgraphcreator[i]->initcapacity();
    }
    for (auto i:list_idx_tmp)
        vec_pdh_tmp.push_back(lsystem_copie->select_historic(i));
    for (auto gc:dqgraphcreator)
    {
        for (auto pdh:vec_pdh_tmp)
        {
            nodetmp = gc->treenode_from_dh(pdh);
            if (nodetmp)
            {
                dqslctdnodes.push_back(nodetmp);
                gedgetmp = gc->edge_from_treenode(nodetmp);
                if (gedgetmp)
                    dqslctdedges.push_back(gedgetmp);
            }
        }
    }
    gettimeofday(&dbg_tmp, NULL);
    list_capty = PGraphCreator::captyprogression(ui->spinBoxSmartSelection->value());
    for (auto edge:dqslctdedges)
        edge->transfercity(list_capty, 2);
    for (int i=0; i<dqgraphcreator.size(); i++)
    {
//        std::stringstream ssgraphslctn;
//        std::ofstream fileslctntmp(vecgraphslctnfilename[i]);
        dqgraphcreator[i]->graphcut();
        vec_pdh_tmp = dqgraphcreator[i]->selected_derivhistoric();
        vec_pdh_slctd.insert(vec_pdh_slctd.end(), vec_pdh_tmp.begin(), vec_pdh_tmp.end());
//        if (fileslctntmp.is_open())
//        {
//            dqgraphcreator[i]->createdisplay(dqgraphcreator[i]->getgedgeroot(), dqslctdnodes, ssgraphslctn);
//            fileslctntmp << ssgraphslctn.str();
//            fileslctntmp.close();
//        }
    }
    gettimeofday(&dbg_after, NULL);
    ss_buf << std::endl << "Graph cut creation for " << _qlfr << " : " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);
    ss_buf << std::endl << "Graph cut computation for " << _qlfr << " : " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_tmp.tv_usec - dbg_tmp.tv_sec*1000000);
    if (file.is_open())
    {
        file << ss_buf.str();
        file.close();
    }
    else
    {
        std::cout << std::endl << "WARNING: Could not open qlfr_timeline.txt...";
    }


    std::cout << std::endl << vec_pdh_slctd.size() << " historic instance(s) selected!";
    for (auto pdh: vec_pdh_slctd)
    {
        if (!pdh->getVecChildren().empty())
            vec_pdh_leavesremoved.push_back(pdh);
    }
    std::cout << std::endl << vec_pdh_leavesremoved.size() << " historic instance(s) selected with leaves removed!";
    // END graph creation process

    //Remove selection color
    //pMainScene->clear_list_selection();----***
    //pMainScene->setSelectionChanged(true);-----***

    qualifier = _qlfr;
    //targetqlfr = ui->comboBoxTargetQualifier->currentText().toStdString();
    //boost::algorithm::to_lower(targetqlfr);

    partialregen_simul(vec_pdh_slctd, qualifier, lsystem_copie);
}

std::vector<float> MainWindow::qlfr_coords(std::string _qlfr)
{
    std::vector<float> value;

    if (vecs_targetqlfrcoords.size()==vec_targetqlfr.size())
    {
        for (size_t i=0; i<vec_targetqlfr.size(); i++)
        {
            if (_qlfr.compare(vec_targetqlfr[i])==0)
            {
                value = vecs_targetqlfrcoords[i];
                break;
            }
        }
    }
    return value;
}

void MainWindow::SaveImage(PLSystemParametric* _lsystem)
{

    //PLSystemParametric* lp;
    //PMediator* med;
    //Scene* scn;
    std::vector<PShapeIndexer*> vec_idxr_tmp;
    std::vector<CGLA::Vec3f> vec_vertices_inc, vec_vertices_idxr;
    std::vector<unsigned int> vec_slct_tmp;

    std::string str_exbasename;
    QString filename_png;
    std::string targetimg;
    std::string targetqlfr;
    //targetimg = ui->comboBoxTargetImage->currentText().toStdString();
    //targetqlfr = ui->comboBoxTargetQualifier->currentText().toStdString();
    targetqlfr = qualifier;

    /*QStringList qsl_ex;

    qsl_ex << "alternative_branch";
    qsl_ex << "honda_monopodial";
    qsl_ex << "honda_sympodial";
    qsl_ex << "ternary_branching";
    qsl_ex << "tree1";
    qsl_ex << "tree2";
    qsl_ex << "tree3";

    // We look from the current file name if the grammar was previously analyzed
    str_exbasename = Procedural::file_basename(current_filename);*/

    // From here we want to know if we need to look from specific file
    // Existing file will be analyzed whether the sample is known
    /*QStringListIterator qsli(qsl_ex);
    while (qsli.hasNext())
    {
        QString qs_tmp = qsli.next();
        if (str_exbasename.find(qs_tmp.toStdString())!=std::string::npos)
        {
            filename_png = qs_tmp + "_" + targetqlfr.c_str() + "_" + targetimg.c_str();
        }
    }*/
    filename_png = targetqlfr.c_str();

    //lp = new PLSystemParametric(*lsystem, false);
    //med = new PMediator(lp);
    //scn = new Scene(lp);

    pMainScene= new Scene(_lsystem);//
    mediator = new PMediator(_lsystem, pMainScene);//

    vec_slct_tmp = _lsystem->sub_branches();

    mediator->setVecSelectionReviewed(vec_slct_tmp);
    pMainScene->setModConfig(mod_config);//
    pMainScene->draw(true);
    pMainViewer->setScene(pMainScene);//

    float htop_min=0, hbot_min=0, vtop_min=0, vbot_min=0;
    Vec3f vert_htop, vert_hbot, vert_vtop, vert_vbot;

    float fovy = pMainViewer->camera()->fieldOfView();
    CGLA::Vec3f vectop_v, vecbot_v, vectop_h, vecbot_h;
    vectop_v[0]=0; //x comp
    vectop_v[1]=-cos(fovy/2); //y comp
    vectop_v[2]=-sin(fovy/2); //z comp

    vecbot_v[0]=0;
    vecbot_v[1]= cos(fovy/2);
    vecbot_v[2]= -sin(fovy/2);

    vectop_h[0]=-cos(fovy/2); //x comp
    vectop_h[1]=0; //y comp
    vectop_h[2]=-sin(fovy/2); //z comp

    vecbot_h[0]= cos(fovy/2);
    vecbot_h[1]= 0;
    vecbot_h[2]= -sin(fovy/2);

    float z_max=0;

        vec_idxr_tmp = mediator->select_indexer(pMainScene, mediator->getVecSelectionReviewed());
        for (std::vector<PShapeIndexer*>::iterator iter_si=vec_idxr_tmp.begin(); iter_si!=vec_idxr_tmp.end(); iter_si++)
        {
            vec_vertices_idxr = (*iter_si)->getVertices();
            vec_vertices_inc.insert(vec_vertices_inc.end(), vec_vertices_idxr.begin(),  vec_vertices_idxr.end());
        }

        float htop_min_tmp=0, hbot_min_tmp=0, vtop_min_tmp=0, vbot_min_tmp=0;
        Vec3f vert_htop_tmp, vert_hbot_tmp, vert_vtop_tmp, vert_vbot_tmp;

        for (std::vector<CGLA::Vec3f>::iterator iter_vert=vec_vertices_inc.begin(); iter_vert!=vec_vertices_inc.end(); iter_vert++)
        {
            Vec3f vertice = *iter_vert;
            float h_top = dot(vertice,vectop_h);
            float h_bot = dot(vertice,vecbot_h);
            float v_top = dot(vertice,vectop_v);
            float v_bot = dot(vertice,vecbot_v);

            //local
            if(h_top<htop_min_tmp)
            {
                vert_htop_tmp=vertice;
                htop_min_tmp=h_top;
            }

            if(v_top<vtop_min_tmp)
            {
                vert_vtop_tmp=vertice;
                vtop_min_tmp=v_top;
            }

            if(h_bot<hbot_min_tmp)
            {
                vert_hbot_tmp=vertice;
                hbot_min_tmp=h_bot;
            }

            if(v_bot<vbot_min_tmp)
            {
                vert_vbot_tmp=vertice;
                vbot_min_tmp=v_bot;
            }

            //global
            if(h_top<htop_min)
            {
                vert_htop=vertice;
                htop_min=h_top;
            }
            if(v_top<vtop_min)
            {
                vert_vtop=vertice;
                vtop_min=v_top;
            }
            if(h_bot<hbot_min)
            {
                vert_hbot=vertice;
                hbot_min=h_bot;
            }
            if(v_bot<vbot_min)
            {
                vert_vbot=vertice;
                vbot_min=v_bot;
            }
            if(vertice[2]>z_max)
                z_max=vertice[2];
        }

        std::vector<Vec3f> vec_extremums;
        vec_extremums.push_back(vert_hbot_tmp);
        vec_extremums.push_back(vert_htop_tmp);
        vec_extremums.push_back(vert_vbot_tmp);
        vec_extremums.push_back(vert_vtop_tmp);

        float z_cam=0;
        float yp_max = vert_vtop[1]-tan(fovy/2)*(fabs(z_max-vert_vtop[2]));
        float yp_min = vert_vbot[1]+tan(fovy/2)*(fabs(z_max-vert_vbot[2]));
        float xp_max = vert_htop[0]-tan(fovy/2)*(fabs(z_max-vert_htop[2]));
        float xp_min = vert_hbot[0]+tan(fovy/2)*(fabs(z_max-vert_hbot[2]));

        double pos_h = ((fabs(xp_min-xp_max))/2.0)*1.05/tan(fovy/2)+z_max;
        double pos_v = ((fabs(yp_min-yp_max))/2.0)*1.05/tan(fovy/2)+z_max;
        z_cam = (std::fmax(pos_h,pos_v));
        //z_cam = 20;

    qglviewer::Camera* cam=pMainViewer->camera();
    cam->setPosition(qglviewer::Vec(0.0,0.0,1.0));
    //pMainViewer->updateGL();

    cam->lookAt(pMainViewer->sceneCenter());
    //pMainViewer->updateGL();

    cam->setUpVector(qglviewer::Vec(0.0,1.0,0.0));
    //pMainViewer->updateGL();

    cam->setType(qglviewer::Camera::PERSPECTIVE);
    //pMainViewer->updateGL();

    cam->showEntireScene();
    pMainViewer->setCamera(cam);

    //pMainViewer->updateGL();

    qglviewer::Vec cam_pos= pMainViewer->camera()->position();

    //std::vector<Vec3f> vec_extremums= (*iter_vec_ext);
    float y_max = vec_extremums.at(3)[1]-tan(fovy/2)*(fabs(z_max-vec_extremums.at(3)[2]));
    float y_min = vec_extremums.at(2)[1]+tan(fovy/2)*(fabs(z_max-vec_extremums.at(2)[2]));
    float x_max = vec_extremums.at(1)[0]-tan(fovy/2)*(fabs(z_max-vec_extremums.at(1)[2]));
    float x_min = vec_extremums.at(0)[0]+tan(fovy/2)*(fabs(z_max-vec_extremums.at(0)[2]));

    //fix position of camera
    cam_pos[2]= z_cam;
    cam_pos[0] = (x_min+x_max)/2.0;
    cam_pos[1] = (y_min+y_max)/2.0;
    pMainViewer->camera()->setPosition(cam_pos);
    //pMainViewer->updateGL();

    pMainViewer->setGridIsDrawn(false);
    pMainViewer->setAxisIsDrawn(false);
    pMainViewer->updateGL();

//    pMainViewer->setSnapshotQuality(100);
//    pMainViewer->setSnapshotFormat("EPS");
//    pMainViewer->saveSnapshot(QString(qualifier.c_str()) + ".eps");

    //pMainViewer->take_snapshot(get_snapshot_format(), filename_png);
    QString folder(QDir::currentPath());
    QImage snapshot = pMainViewer->grabFrameBuffer(true);
    QPixmap pixmap;
    pixmap = pixmap.fromImage(snapshot.scaled(135,135,Qt::IgnoreAspectRatio,Qt::FastTransformation));
    QFile file(folder + "/" + qualifier.c_str() +".jpg");
    file.open(QIODevice::WriteOnly);
    pixmap.save(&file, "jpeg",100);
    file.close();
    pMainViewer->setGridIsDrawn(true);
    //delete lp;
    //delete scn;
    //delete med;
}

void MainWindow::on_pushButtonAllSlt_clicked()
{
    std::vector<unsigned int> vec_slct_tmp;
    vec_slct_tmp = lsystem->sub_branches();
    mediator->setVecSelectionReviewed(vec_slct_tmp);
    pMainViewer->updateGL();
    getScene()->setSelection(vec_slct_tmp);
    getScene()->setSelectionChanged(true);
    getScene()->draw(true);
}

//void MainWindow::on_pushButtonAlphaSlt_clicked()
//{
//    size_t idx_tmp = ui->lineEditCurrentSelection->text().toStdString().find_first_of("(");
//    if (idx_tmp==std::string::npos)
//        idx_tmp = 1;

//    if(ui->lineEditCurrentSelection->text().length() > 0)
//    {

//    std::vector<unsigned int> vec_slct_tmp;
//    vec_slct_tmp = lsystem->sub_branches(ui->lineEditCurrentSelection->text().left(idx_tmp).toStdString());
//    mediator->setVecSelectionReviewed(vec_slct_tmp);
//    pMainViewer->updateGL();
//    getScene()->addAllSelection(vec_slct_tmp);
//    getScene()->setSelectionChanged(true);
//    getScene()->draw(true);

//    }
//}



void MainWindow::on_pushButtonDisplayQlfr_clicked()
{
    val = false;
    //mThreads.clear();
    qThreads.clear();
    std::ofstream file("qlfr_timeline.txt", std::ofstream::out|std::ofstream::app);
    std::stringstream ss_buf;

    pMainViewer->save_camera();

    struct timeval dbg_before, dbg_after, dbg_tmp;
    std::vector<timeval> vec_dbg_timeval;
    ss_buf << std::endl << "====**====**====";
    ss_buf << std::endl << "Initializing qualifier regenerations... :\t";

    gettimeofday(&dbg_before, NULL);
    vec_dbg_timeval.push_back(dbg_before);

    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Cloning All L-system: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    //gettimeofday(&dbg_before, NULL);
    //vec_dbg_timeval.push_back(dbg_before);
    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);


    //lsystem_copie =   mThreads[0]->lsystem;
    ApplyQualifier("petit");
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for PETIT: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);
//    scn_cpy = new Scene(lsystem_copie);
//    vwr_cpy = new Viewer(scn_cpy, CONSTRAINTS_FREE_VIEW, this);
//    vwr_cpy->setSceneRadius(pMainViewer->sceneRadius());
//    vwr_cpy->setFixedSize(pMainViewer->size());
//    scn_cpy->draw(false);
//    vwr_cpy->setSnapshotQuality(100);
//    vwr_cpy->setSnapshotFormat("EPS");
//    vwr_cpy->saveSnapshot("petit.eps");
//    delete scn_cpy;
//    delete vwr_cpy;

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[1]->wait();
    //lsystem_copie = mThreads[1]->lsystem;
    ApplyQualifier("grand");
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for GRAND: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[2]->wait();
    //lsystem_copie = mThreads[2]->lsystem;
    ApplyQualifier("epars");
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for EPARS: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[3]->wait();
    //lsystem_copie = mThreads[3]->lsystem;
    ApplyQualifier("dense");
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for DENSE: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[4]->wait();
    //lsystem_copie = mThreads[4]->lsystem;
    ApplyQualifier("realiste");
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for REALISTE: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[5]->wait();
    //lsystem_copie = mThreads[5]->lsystem;
    ApplyQualifier("irrealiste");
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for IRREALISTE: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[6]->wait();
    //lsystem_copie = mThreads[6]->lsystem;
    ApplyQualifier("normal");
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for NORMAL: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[7]->wait();
    //lsystem_copie = mThreads[7]->lsystem;
    ApplyQualifier("mince");
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for MINCE: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[8]->wait();
    //lsystem_copie = mThreads[8]->lsystem;
    ApplyQualifier("gros");
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for GROS: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[9]->wait();
    //lsystem_copie = mThreads[9]->lsystem;
    ApplyQualifier("rond");
    gettimeofday(&dbg_after, NULL);
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for ROND: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[9]->wait();
    //lsystem_copie = mThreads[9]->lsystem;
    ApplyQualifier("symetrique");
    gettimeofday(&dbg_after, NULL);
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for SYMETRIQUE: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    //mThreads[9]->wait();
    //lsystem_copie = mThreads[9]->lsystem;
    ApplyQualifier("asymetrique");
    gettimeofday(&dbg_after, NULL);
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Regeneration time for ASYMETRIQUE: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);


    std::vector<std::string> names;
    names.push_back ("petit");
    names.push_back ("grand");
    names.push_back ("epars");
    names.push_back ("dense");
    names.push_back ("realiste");
    names.push_back ("irrealiste");
    names.push_back ("normal");
    names.push_back ("mince");
    names.push_back ("gros");
    names.push_back ("rond");
    names.push_back ("symetrique");
    names.push_back ("asymetrique");


    for(int i=0;i<12;i++)
    {
        qThreads[i]->wait();
        qualifier = names[i];
        SaveImage(qThreads[i]->lsystem);
        save_vectorialimg(qThreads[i]->lsystem);
        //save_pnghd(qThreads[i]->lsystem);
    }

    gettimeofday(&dbg_after, NULL);
    gettimeofday(&dbg_tmp, NULL);
    ss_buf << std::endl <<  "Saving snapshot from L-systems: " << 0.000001*(dbg_tmp.tv_usec + dbg_tmp.tv_sec*1000000 - vec_dbg_timeval.back().tv_usec - vec_dbg_timeval.back().tv_sec*1000000);
    vec_dbg_timeval.push_back(dbg_tmp);

    ss_buf << std::endl <<  "All qualifiers regeneration time: " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_after.tv_usec - dbg_before.tv_sec*1000000);

    /*lsystem_copie = new PLSystemParametric(*lsystem,false);
    //lsystem_copie->init_dh();
    lsystem_copie->copie(lsystem);

    lsystem = lsystem_copie;*/

    pMainScene= new Scene(lsystem);
    mediator = new PMediator(lsystem, pMainScene);

    std::vector<unsigned int> vec_slct_tmp;
    vec_slct_tmp = lsystem->sub_branches();

    mediator->setVecSelectionReviewed(vec_slct_tmp);

    pMainScene->setModConfig(mod_config);
    pMainScene->draw(true);
    pMainViewer->setScene(pMainScene);

    pMainScene->setSelection(list_idx_slt);
    pMainScene->setSelectionChanged(true);
    pMainViewer->setFocus();
    pMainViewer->updateGL();

    QString folder(QDir::currentPath());

    /*std::vector<QString> names;
    names.push_back ("petit");
    names.push_back ("grand");
    names.push_back ("epars");
    names.push_back ("dense");
    names.push_back ("realiste");
    names.push_back ("irrealiste");
    names.push_back ("normal");
    names.push_back ("mince");
    names.push_back ("gros");
    names.push_back ("rond");

    gettimeofday(&dbg_before, NULL);
    ss_buf << std::endl <<  "Converting snapshot... :\t";
    for (std::vector<QString>::iterator iter=names.begin(); iter!=names.end(); iter++)
    {
        QImage img(folder + "/" + (*iter));
        QPixmap pixmap;
        pixmap = pixmap.fromImage(img.scaled(135,135,Qt::IgnoreAspectRatio,Qt::FastTransformation));
        QFile file(folder + "/" + (*iter)+".jpg");
        file.open(QIODevice::WriteOnly);
        pixmap.save(&file, "jpeg",100);
        file.close();
    }
    gettimeofday(&dbg_after, NULL);
    ss_buf << std::endl << "Snapshot processing time: " << 0.000001*(dbg_after.tv_usec + dbg_after.tv_sec*1000000 - dbg_before.tv_usec - dbg_before.tv_sec*1000000);*/
    if (file.is_open())
    {
        file << ss_buf.str();
        file.close();
    }
    else
    {
        std::cout << std::endl << "WARNING: Could not open qlfr_timeline.txt...";
    }

    QPixmap pix1(folder + "/petit.jpg");
    QPixmap pix2(folder + "/grand.jpg");
    QPixmap pix3(folder + "/epars.jpg");
    QPixmap pix4(folder + "/dense.jpg");
    QPixmap pix5(folder + "/realiste.jpg");
    QPixmap pix6(folder + "/irrealiste.jpg");
    QPixmap pix7(folder + "/normal.jpg");
    QPixmap pix8(folder + "/mince.jpg");
    QPixmap pix9(folder + "/gros.jpg");
    QPixmap pix10(folder + "/rond.jpg");
    QPixmap pix11(folder + "/symetrique.jpg");
    QPixmap pix12(folder + "/asymetrique.jpg");

    //QImage image(folder + "/petit.jpg");

    // tell the painter to draw on the QImage
    /*QPainter* painter = new QPainter(&image); // sorry i forgot the "&"
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 10));
    // you probably want the to draw the text to the rect of the image
    painter->drawText(image.rect(), Qt::AlignCenter, "Text on Image");

    ui->label_pic1->setPixmap(QPixmap::fromImage(image));*/

    QFont f( "Arial", 12, QFont::Bold);
    if (pix1.isNull())
      {
         ui->label_pic1->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic1->setPixmap(pix1);
        ui->label_text1->setText("petit");
        ui->label_text1->setFont( f);
     }
    if (pix2.isNull())
      {
         ui->label_pic2->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic2->setPixmap(pix2);
        ui->label_text2->setText("grand");
        ui->label_text2->setFont( f);
     }
    if (pix3.isNull())
      {
         ui->label_pic3->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic3->setPixmap(pix3);
        ui->label_text3->setText("epars");
        ui->label_text3->setFont( f);
     }
    if (pix4.isNull())
      {
         ui->label_pic4->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic4->setPixmap(pix4);
        ui->label_text4->setText("dense");
        ui->label_text4->setFont( f);
     }
    if (pix5.isNull())
      {
         ui->label_pic5->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic5->setPixmap(pix5);
        ui->label_text5->setText("realiste");
        ui->label_text5->setFont( f);
     }
    if (pix6.isNull())
      {
         ui->label_pic6->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic6->setPixmap(pix6);
        ui->label_text6->setText("irrealiste");
        ui->label_text6->setFont( f);
     }
    if (pix7.isNull())
      {
         ui->label_pic7->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic7->setPixmap(pix7);
        ui->label_text7->setText("normal");
        ui->label_text7->setFont( f);
     }
    if (pix8.isNull())
      {
         ui->label_pic8->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic8->setPixmap(pix8);
        ui->label_text8->setText("mince");
        ui->label_text8->setFont( f);
     }
    if (pix9.isNull())
      {
         ui->label_pic9->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic9->setPixmap(pix9);
        ui->label_text9->setText("gros");
        ui->label_text9->setFont( f);
     }
    if (pix10.isNull())
      {
         ui->label_pic10->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic10->setPixmap(pix10);
        ui->label_text10->setText("rond");
        ui->label_text10->setFont( f);
     }
    if (pix11.isNull())
      {
         ui->label_pic11->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic11->setPixmap(pix11);
        ui->label_text11->setText("symetrique");
        ui->label_text11->setFont( f);
     }
    if (pix12.isNull())
      {
         ui->label_pic12->setText("Aucun aperçu disponible");
      }
      else
     {
        ui->label_pic12->setPixmap(pix12);
        ui->label_text12->setText("asymetrique");
        ui->label_text12->setFont( f);
     }
    val = true;
}

void MainWindow::save_vectorialimg(PLSystemParametric* _lsystem)
{
    std::string targetqlfr;
    targetqlfr = qualifier;

    pMainScene  = new Scene(_lsystem);//
    mediator    = new PMediator(_lsystem, pMainScene);//

    pMainViewer->load_camera();
    pMainViewer->setGridIsDrawn(false);
    pMainViewer->setAxisIsDrawn(false);
    pMainViewer->updateGL();

    pMainViewer->setSnapshotQuality(100);
    pMainViewer->setSnapshotFormat("EPS");
    pMainViewer->saveSnapshot(QString(qualifier.c_str()) + ".eps");
}

void MainWindow::save_pnghd(PLSystemParametric* _lsystem)
{
    std::string targetqlfr;
    targetqlfr = qualifier;

    pMainScene  = new Scene(_lsystem);//
    mediator    = new PMediator(_lsystem, pMainScene);//

    pMainViewer->load_camera();
    pMainViewer->setGridIsDrawn(false);
    pMainViewer->setAxisIsDrawn(false);
    pMainViewer->updateGL();

    pMainViewer->setSnapshotQuality(100);
    pMainViewer->setSnapshotFormat("PNG");
    pMainViewer->saveSnapshot(QString(qualifier.c_str()) + ".png");
}

void MainWindow::on_horizontalSliderSmartSelection_sliderMoved(int position)
{
    ui->spinBoxSmartSelection->setValue(position);
}

void MainWindow::on_spinBoxSmartSelection_valueChanged(int arg1)
{
    ui->horizontalSliderSmartSelection->setValue(arg1);
}

void MainWindow::on_horizontalSliderSmartSelection_actionTriggered(int action)
{
    ui->spinBoxSmartSelection->setValue(ui->horizontalSliderSmartSelection->sliderPosition());
}

void MainWindow::on_pushButtonExtremSim_clicked()
{
    metrics_stats();
}

void MainWindow::on_actionSave_Camera_triggered()
{
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_K, Qt::ControlModifier);
    QCoreApplication::postEvent (pMainViewer, event);
}

void MainWindow::on_actionLoad_Camera_triggered()
{
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_L, Qt::ControlModifier);
    QCoreApplication::postEvent (pMainViewer, event);
}

void MainWindow::on_actionLoad_Selection_triggered()
{
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_W, Qt::ControlModifier);
    QCoreApplication::postEvent (pMainViewer, event);
}

void MainWindow::on_actionSave_Selection_triggered()
{
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_M, Qt::ControlModifier);
    QCoreApplication::postEvent (pMainViewer, event);
}

void MainWindow::on_actionVRender_triggered()
{
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_P, Qt::ControlModifier);
    QCoreApplication::postEvent (pMainViewer, event);
}

void MainWindow::on_actionBackground_Color_triggered()
{
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_B, Qt::ControlModifier);
    QCoreApplication::postEvent (pMainViewer, event);
}

void MainWindow::on_actionShow_Hide_Gizmo_triggered()
{
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_A, Qt::NoModifier);
    QCoreApplication::postEvent (pMainViewer, event);
}

void MainWindow::on_actionShow_Hide_Grid_triggered()
{
    QKeyEvent *event = new QKeyEvent (QEvent::KeyPress, Qt::Key_G, Qt::NoModifier);
    QCoreApplication::postEvent (pMainViewer, event);
}

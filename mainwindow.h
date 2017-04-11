#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QTimer>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QGridLayout>
#include <QDragEnterEvent>

#include "windowtitlebar.h"
#include "grammar/ppreproc.h"
#include "viewport/pmodelingconfig.h"
#include "inithread.h"
#include "qlfrthread.h"

class Viewer;
class PIORecord;
class PMediator;
class PLSystemParametric;
class PDerivHistoric;
class Scene;
class AboutDialog;
class OutputDialog;
class TreeViewDialog;
class PPcaViewer;
class PImgBrowser;
class DialogViewer;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Viewer*             pMainViewer;
    Scene*              pMainScene;
    Scene*              scn_cpy;
    PModelingConfig*    mod_config;
    PIORecord*          in_out_rec;
    PMediator*          mediator;
    PLSystemParametric* lsystem;
    PLSystemParametric* lsystem_copie;
    QTimer*             timer;
    AboutDialog*        aboutDlg;
    OutputDialog*       outputDlg;
    TreeViewDialog*     treeviewDlg;
    WindowTitleBar*      titleBar;
    QGridLayout*        mainLayout;
    QStandardItemModel* qstd_imodel;
    PPcaViewer*         pcaViewer;
    PImgBrowser*        imgBrowser;
    DialogViewer*       dlgViewer;
    std::string         current_filename, file_basename, color_config_fn, shape_config_fn;
    std::vector<PPreproc> vec_pp_wrap;
    std::vector<std::string> vec_targetqlfr;
    std::vector< std::vector<float> > vecs_targetqlfrcoords;
    QList<int> list_idx_slt;
    std::string qualifier;
    //std::vector<IniThread* > mThreads;
    std::vector<QlfrThread* > qThreads;
    bool val;
    std::vector< std::vector<unsigned int> > vec_rules_count;
    std::vector< std::vector<float> > vec_infl_factor;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    Scene* getScene() const {return pMainScene;}
    Viewer* getViewer() const {return pMainViewer;}
    PLSystemParametric* getLSystem() const {return lsystem;}
    TreeViewDialog* getTreeViewDialog() const {return treeviewDlg;}
    int get_snapshot_format();
    void setup_snapshot_browser(std::vector<PPreproc>& _in_vec_pp, std::vector<unsigned int>& _in_vec_modif_rules_idx, PLSystemParametric* _lp);
    QStringList generate_filename(std::string _prefix, std::string _extension, int _file_number);
    std::string setup_filename_prefix(std::string _common_root);
    void save_pca_output(QString& _file_basename, QStringList& _sl_pp, QStringList& _sl_lsys, QStringList& _sl_png);
    void update_color_cb();
    void update_shape_line();
    void partialregen_simul(std::vector<PDerivHistoric*> _vec_pdh_regen, std::string _qlfr, PLSystemParametric* _lsystem);
    void load_targetqlfr();
    std::vector<float> qlfr_coords(std::string _qlfr);

public:
    void hist_str_dbg();
    void hist_drop_dbg();
    void hist_read_dbg();
public slots:
    void on_actionOutput_Img_Browser_triggered(QString _filename="", QString _id_user="", QString _date="");

private:
    Ui::MainWindow *ui;

private slots:
    void on_actionDerivation_Tree_triggered();
    void on_pushButtonConfigBrowse_clicked();
    void on_pushButtonSaveConfig_clicked();
    void on_actionLoad_triggered();
    void on_actionExecute_triggered();
    void on_actionOutput_triggered();
    void on_actionHistoric_triggered();
    void on_actionAbout_Promo_triggered();
    void on_pushButtonUpdate_clicked();
    void on_actionReset_triggered();
    void timerTimeout();
    void on_pushButtonApply_clicked();
    void on_comboBoxActions_currentIndexChanged(int index);
    void minimize();
    void on_comboBoxConst_currentIndexChanged(const QString &arg1);
    void on_doubleSpinBoxConstValue_valueChanged(double arg);
    void on_pushButtonConstGlobalScope_clicked();
    void on_pushButtonConstLocalScope_clicked();
    void on_pushButtonConstChildScope_clicked();
    void on_pushButtonReinitialize_clicked();
    void on_pushButtonObbCurr_clicked();
    void mouseReleaseEvent(QMouseEvent* e);

    void on_pushButtonObbSub_clicked();
    void on_pushButtonObbAll_clicked();
    void on_pushButtonSimConst_clicked();

    void on_actionPCA_Plot_triggered();
  //  void on_actionOutput_Img_Browser_triggered(QString _filename="", QString _id_user="", QString _date="");

    void on_pushButtonResetConfig_clicked();
    void on_widgetColor_clicked();
    void on_comboBoxAlphabet_currentIndexChanged(int index);
    void on_comboBoxlineEditShape_currentIndexChanged(int index);

    void on_pushButtonApplyQualifier_clicked();
    void on_pushButtonAllSlt_clicked();
    //void on_pushButtonAlphaSlt_clicked();
    void on_pushButtonDisplayQlfr_clicked();

    void on_horizontalSliderSmartSelection_sliderMoved(int position);
    void on_spinBoxSmartSelection_valueChanged(int arg1);

    void on_horizontalSliderSmartSelection_actionTriggered(int action);

    void on_pushButtonExtremSim_clicked();

    void on_actionSave_Camera_triggered();

    void on_actionLoad_Camera_triggered();

    void on_actionLoad_Selection_triggered();

    void on_actionSave_Selection_triggered();

    void on_actionVRender_triggered();

    void on_actionBackground_Color_triggered();

    void on_actionShow_Hide_Gizmo_triggered();

    void on_actionShow_Hide_Grid_triggered();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void LoadProjectFile(QString filename);
    void simConst_allRules(time_t seed);
    void closeEvent(QCloseEvent * event);
    void reinit();
    void original_copy();
    void ApplyQualifier(std::string _qlfr);
    void SaveImage(PLSystemParametric* _lsystem);
    void save_vectorialimg(PLSystemParametric* _lsystem);
    void save_pnghd(PLSystemParametric* _lsystem);

protected:
//    std::vector<PMetric> vec_metrics;
//    PMetric check_metrics();
//    PMetric check_metrics(PLSystemParametric* _lp, Scene* _scn, PMediator* _pmed);
    void metrics_stats();
    //void save_metrics_stats();

};

#endif // MAINWINDOW_H

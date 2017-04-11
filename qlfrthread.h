#ifndef QLFRTHREAD_H
#define QLFRTHREAD_H

#include <QtCore>

#include "viewport/viewer.h";
#include "viewport/scene.h";
#include "engine/pmediator.h";
#include "engine/plsystemparametric.h";

/*class PIORecord;
class PDerivHistoric;
class AboutDialog;
class OutputDialog;
class TreeViewDialog;
class PPcaViewer;
class PImgBrowser;
class DialogViewer;*/

class QlfrThread : public QThread
{
    Q_OBJECT
    private:
        int modify_rule;
        PRuleParametric *prule;
        PPreproc *pptmp;
        std::vector<PDerivHistoric*> vec_activregen;
        std::vector<unsigned int> vec_nbregen;

    public:
        QlfrThread(PLSystemParametric* _lsystem, int _modify_rule, std::vector<PDerivHistoric*> _vec_activregen, std::vector<unsigned int> _vec_nbregen);
        void run();
        PLSystemParametric* lsystem;
};

#endif // QLFRTHREAD_H

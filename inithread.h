#ifndef INITHREAD_H
#define INITHREAD_H

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

class IniThread : public QThread
{
    Q_OBJECT
    private:
        PLSystemParametric* lsystemc;

    public:
        IniThread(PLSystemParametric* _lsystem);
        void run();
        PLSystemParametric* lsystem;
        int nb;
};

#endif // INITHREAD_H

#include "inithread.h"
#include "engine/ptextparser.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <QtCore>
#include <QDebug>

#include "engine/pgraphcreator.h"
#include "engine/pderivhistoric.h"
#include "engine/pruleparametric.h"
#include "engine/GraphEdge.h"
#include "engine/Node.h"

using namespace std;

IniThread::IniThread(PLSystemParametric* _lsystem)
{
    lsystem = new PLSystemParametric(*_lsystem,false);
    nb = _lsystem->getNumDerivation();
    lsystemc = _lsystem;
}

void IniThread::run(){

    lsystem->init_dh();
    for (int k=0; k<nb; k++){
        lsystem->derive();
    }
}


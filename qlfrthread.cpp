#include "qlfrthread.h"
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

QlfrThread::QlfrThread(PLSystemParametric* _lsystem, int _modify_rule, std::vector<PDerivHistoric*> _vec_activregen, std::vector<unsigned int> _vec_nbregen)
{
    lsystem = _lsystem;
    //PRuleParametric *prule = new PRuleParametric();
    //PPreproc *pptmp = new PPreproc(*lsystem->getPreproc());
    //*prule = *lsystem->getVecRules()[_modify_rule];

    modify_rule = _modify_rule;
    vec_activregen = _vec_activregen;
    vec_nbregen = _vec_nbregen;
}

void QlfrThread::run(){


    std::ofstream file("qlfr_timeline.txt", std::ofstream::out|std::ofstream::app);
    std::stringstream ss_buf;

    for (std::vector<PDerivHistoric*>::iterator iter=vec_activregen.begin(); iter!=vec_activregen.end(); iter++)
    {
        int deriv_idx = (*iter)->getIdxIntoDeriv();
        int deriv_nb = vec_nbregen[iter-vec_activregen.begin()];
        lsystem->generate(deriv_idx,deriv_nb);
        lsystem->update_leaves();
    }

    //*lsystem->getPreproc() = *pptmp;
    //*lsystem->getVecRules()[modify_rule] = *prule;

    /*ss_buf << std::endl <<  "Running!!!! ";


    if (file.is_open())
    {
        file << ss_buf.str();
        file.close();
    }
    else
    {
        std::cout << std::endl << "WARNING: Could not open qlfr_timeline.txt...";
    }*/

}


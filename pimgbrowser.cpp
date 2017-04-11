#include <QString>
#include <QBoxLayout>
#include "pimgbrowser.h"
#include "ui_pimgbrowser.h"
#include "engine/plsystemparametric.h"
#include "engine/pmediator.h"

PImgBrowser::PImgBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PImgBrowser)
{
    QImage img;
    QRect rect;
    QCursor cursor(Qt::OpenHandCursor);

    init_file_list();
    mainImg = QImage(":images/viewport.png");
    scene = new Scene();
    viewer = new Viewer(scene, CONSTRAINTS_FREE_VIEW, this);
    current_img_index = 0;
    ui->setupUi(this);
    lbl_width = this->size().width()*0.125;
    lbl_height = this->size().height()*0.17;
    ui->verticalLayoutViewer->addWidget(viewer);
    ui->verticalLayoutViewer->setDirection(QBoxLayout::BottomToTop);
    ui->scrollAreaWidgetContentsImg->setFixedWidth(lbl_width*3 + 5*2);
    ui->scrollAreaWidgetContentsImg->setFixedHeight(lbl_height*9 + 5*9);
    ui->labelSnapshot->setScaledContents(true);
    ui->labelSnapshot->setPixmap(QPixmap::fromImage(mainImg));
    //ui->labelNewGroup->setAcceptDrops(true);
    ui->scrollAreaImg->installEventFilter(this);

    for (int i=0; i<27; i++)
    {
        imgLbl[i] = new QLabel(ui->scrollAreaWidgetContentsImg);
        imgLbl[i]->setFixedSize(lbl_width, lbl_height);
        imgLbl[i]->setScaledContents(true);
        rect = QRect(5*(i%3) + lbl_width*(i%3), 5*((int)(i/3))+5 + lbl_height*((int)(i/3)), lbl_width, lbl_height);
        imgLbl[i]->setGeometry(rect);
        img = QImage(img_file_list.at(i));
        imgLbl[i]->setPixmap(QPixmap::fromImage(img));
        imgLbl[i]->setCursor(cursor);
        imgLbl[i]->installEventFilter(this);
    }
}

PImgBrowser::~PImgBrowser()
{
    for (int i=0; i<27; i++)
        delete imgLbl[i];
    delete ui;
    delete viewer;
}

void PImgBrowser::init_file_list()
{
    img_file_list << "img_0.png" << "img_1.png" << "img_2.png";
    img_file_list << "img_3.png" << "img_4.png" << "img_5.png";
    img_file_list << "img_6.png" << "img_7.png" << "img_8.png";
    img_file_list << "img_9.png" << "img_10.png" << "img_11.png";
    img_file_list << "img_12.png" << "img_13.png" << "img_14.png";
    img_file_list << "img_15.png" << "img_16.png" << "img_17.png";
    img_file_list << "img_18.png" << "img_19.png" << "img_20.png";
    img_file_list << "img_21.png" << "img_22.png" << "img_23.png";
    img_file_list << "img_24.png" << "img_25.png" << "img_26.png";
}

void PImgBrowser::snaphot_resize(int _width, int _height)
{
    QImage img;
    QRect rect;
    QCursor cursor(Qt::OpenHandCursor);
    lbl_width = _width*0.125;
    lbl_height = _height*0.17;
    ui->scrollAreaWidgetContentsImg->setFixedWidth(lbl_width*3 + 5*2);
    ui->scrollAreaWidgetContentsImg->setFixedHeight(lbl_height*9 + 5*9);
    for (int i=0; i<27; i++)
    {
        imgLbl[i]->setFixedSize(lbl_width, lbl_height);
        imgLbl[i]->setScaledContents(true);
        rect = QRect(5*(i%3) + lbl_width*(i%3), 5*((int)(i/3))+5 + lbl_height*((int)(i/3)), lbl_width, lbl_height);
        imgLbl[i]->setGeometry(rect);
        img = QImage(img_file_list.at(i));
        imgLbl[i]->setPixmap(QPixmap::fromImage(img));
        imgLbl[i]->setCursor(cursor);
    }
}

void PImgBrowser::resizeEvent(QResizeEvent* _re)
{
    this->snaphot_resize(_re->size().width(), _re->size().height());
    _re->accept();
}

//void PImgBrowser::mousePressEvent(QMouseEvent* _me)
//{
//    QPixmap pm;
//    QRect rect;
//    QString s("Failed to load ");
//    std::cout << std::endl << "x:" << _me->x() << ", y:" << _me->y();
//    for (int i=0; i<27; i++)
//    {
//        rect = imgLbl[i]->geometry();
//        if (_me->button() == Qt::LeftButton)
//        {

//            if (rect.contains(_me->x(), _me->y()))
//            {
//                std::cout << std::endl << "Label contains: " << i << " [" << rect.x() << "," << rect.y() << "]" << " " << "[w=" << rect.width() << ",h=" << rect.height() << "]";
//                //mainImg = QImage(img_file_list.at(i));
//                //ui->labelSnapshot->setPixmap(QPixmap::fromImage(mainImg));
//                if (!pm.load( img_file_list.at(i)))
//                {
//                    s += img_file_list.at(i);
//                    qWarning(s.toStdString().c_str());
//                }
//                ui->labelSnapshot->setPixmap(pm);

//                ui->labelSnapshot->repaint();
//            }
//            else
//            {
//                mainImg = QImage(":images/viewport.png");
//                ui->labelSnapshot->setPixmap(QPixmap::fromImage(mainImg));
//            }

//            //this->repaint();
//        }
//    }
//}

bool PImgBrowser::eventFilter(QObject *_o, QEvent *_e)
{
    QPixmap pm;
    QRect rect;
    //QMouseEvent* me;
    QString s("Failed to load ");

    //me = static_cast<QMouseEvent*>(_e);

    for (int i=0; i<27; i++)
    {
        if (_o == imgLbl[i] && _e->type() == QEvent::MouseButtonPress)
        {
            //std::cout << std::endl << "Label contains: " << i << " [" << rect.x() << "," << rect.y() << "]" << " " << "[w=" << rect.width() << ",h=" << rect.height() << "]";
            current_img_index = i;
            if (!pm.load( img_file_list.at(i)))
            {
               s += img_file_list.at(i);
               qWarning(s.toStdString().c_str());
            }
            ui->labelSnapshot->setPixmap(pm);
            ui->labelSnapshot->repaint();
            std::cout << std::endl << "Good widget!";
            return true;
        }
    }
//    mainImg = QImage(":images/viewport.png");
//    ui->labelSnapshot->setPixmap(QPixmap::fromImage(mainImg));
    return QWidget::eventFilter(_o, _e);
}

void PImgBrowser::on_pushButtonLoadSnapshot_clicked()
{
    PLSystemParametric* lp = new PLSystemParametric(*lsystem, false);
    scene->setLSystem(lp);
    lp->rewrite_rule(vec_modif_rules_idx.at(current_img_index), &vec_preproc.at(current_img_index));
    lp->init_dh();
    for (int k=0; k<lsystem->getNumDerivation(); k++)
    {
        lp->derive();
        viewer->updateGL();
    }
}

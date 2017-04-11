#include <QGuiApplication>
#include <QApplication>

#include "viewport/viewer.h"
#include "mainwindow.h"
#include "GL/glut.h"

long DBG_NOW;

int main(int argc, char** argv)
{
    int value;

    struct timeval dbg_tv;
    QApplication app(argc, argv);
    glutInit(&argc, argv);
    gettimeofday(&dbg_tv, NULL);
    DBG_NOW = dbg_tv.tv_sec*1000000;

    QFile file(QDir::currentPath().append("/../promo/stylesheets/promo.qss"));
    if (!file.open(QFile::ReadOnly))
    {
        file.setFileName(QDir::currentPath().append("/promo.qss"));
        if (!file.open(QFile::ReadOnly))
        {
            qWarning("\nCannot read stylesheets!\t");
            return 0;
        }
    }
    QString styleSheet = QLatin1String(file.readAll());

    app.setStyleSheet(styleSheet);

    if (argc < 2)
   {
       MainWindow* mainWin = new MainWindow;
       mainWin->show();
       value = app.exec();
       delete mainWin;
       return value;
   }
   else
   {
       QString file_name = argv[1];
       QString id_user = argv[2];
       QString date = argv[3];
       MainWindow* mainWin = new MainWindow;
       mainWin->on_actionOutput_Img_Browser_triggered(file_name , id_user, date);
       app.setQuitOnLastWindowClosed(false);
       value = app.exec();
       delete mainWin;
       return value;
   }
}


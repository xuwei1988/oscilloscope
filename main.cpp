#include <QApplication>
#include "datasource.h"
#include "wareformgenerator.h"
#include "mainview.h"
#include "controllbar.h"
#include "mainwindow.h"
#include <QDebug>
#include <sys/time.h>

#include <QSplashScreen>
#include <QPixmap>
#include <QElapsedTimer>
#include <QDateTime>

QStringList strList;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    strList << ":/images/event.png";
    strList << ":/images/warning.png";
    strList << ":/images/error.png";

    QPixmap pixmap(":/images/screen1.JPG");

    QSplashScreen screen(pixmap);
    screen.show();

    int delayTime = 1500;
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < delayTime)
        app.processEvents();

    MainWindow mw;
    mw.show();

    screen.finish(&mw);
    return app.exec();
}


















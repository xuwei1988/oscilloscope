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

/* 2019.07.03 添加ZLG USB-CAN */
#include "ControlCAN.h"
#include <QLibrary>

QStringList strList;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /* 2019.07.03 添加ZLG USB-CAN */
    QLibrary lib("ControlCAN.dll");
    if(true == lib.load())
        qDebug()<<"Dll load ok";

//    DWORD EQU_Type = VCI_USBCAN_E_U;
//    DWORD EQU_Index = 0;
//    DWORD EQU_Number = 0;
//    VCI_INIT_CONFIG init_config;
//    uint32_t EQU_baud = 0x060007;

//    init_config.AccCode = 0x00000000;
//    init_config.AccMask = 0xFFFFFFFF;

//    int buffLen = 0;
//    int readLen = 0;
//    VCI_CAN_OBJ frames[50];

//    if(VCI_OpenDevice(EQU_Type, EQU_Index, EQU_Number) != STATUS_OK) {
//        qDebug()<<"VCI_OpenDevice Error";
//    }

//    if(VCI_SetReference(EQU_Type, EQU_Index, EQU_Number, 0, (void *)&EQU_baud) != STATUS_OK) {
//        qDebug()<<"VCI_SetReference Error";
//    }

//    if(VCI_InitCAN(EQU_Type, EQU_Index, EQU_Number, &init_config) != STATUS_OK) {
//        qDebug()<<"VCI_InitCAN Error";
//    }

//    if(VCI_StartCAN(EQU_Type, EQU_Index, EQU_Number) != STATUS_OK) {
//        qDebug()<<"VCI_StartCAN Error";
//    }

//    while(1) {
//        buffLen = VCI_GetReceiveNum(EQU_Type, EQU_Index, EQU_Number);
//        if(buffLen > 0) {
//            readLen = VCI_Receive(EQU_Type,EQU_Index,EQU_Number,frames, 50, 20);
//            qDebug()<<"readLen = "<<readLen;
//        }

//    }

    /*************************************************************/

    strList << NULL;
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


















#include "mainwindow.h"
#include "mainview.h"
#include "magnifyview.h"
#include "datasource.h"
#include "wareformgenerator.h"
#include "connectdialog.h"
//#include "eventtable.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QDockWidget>
#include <QWidget>
#include <QDateTime>

#include "scopeimfor.h"
#include "connectdialog.h"
#include "recorddata.h"
#include "eventselectdialog.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

#include <QDebug>
extern QStringList strList;


MainWindow::MainWindow()
{
    createActions();
    createMenus();

    createDockWindows();
    createConnectToolBar();
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(channelSetting()));
    connect(eventSettingsAction, SIGNAL(triggered()), this, SLOT(eventSetting()));
    connect(connectAction, SIGNAL(triggered()), this, SLOT(connectSetting()));
    connect(disconnectAction, SIGNAL(triggered()), this, SLOT(disconnectSetting()));

    connect(eventList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(eventSelected(QModelIndex)));
    connect(eventList, SIGNAL(actionDone(ActionType,int)), this, SLOT(eventActioned(ActionType,int)));
//    setWindowIcon(QIcon(":/images/scope_icon.png"));
    setMinimumSize(1500, 800);
}

MainWindow::MainWindow(DataSource *data, WareformGenerator *wg)
    : dataSource(data), warefirm(wg)
{
}

void MainWindow::createActions()
{
    connectAction = new QAction(QIcon(":/images/connect.png"),
                                tr("&Connect"), this);
    connectAction->setShortcut(tr("Ctrl+C"));

    disconnectAction = new QAction(QIcon(":/images/disconnect.png"),
                                       tr("&Disconnect"), this);
    disconnectAction->setShortcut(tr("Ctrl+D"));
    disconnectAction->setEnabled(false);

    settingsAction = new QAction(QIcon(":/images/settings.png"),
                                        tr("&Settings"), this);
    settingsAction->setShortcut(tr("Ctrl+S"));
    settingsAction->setEnabled(false);

    eventSettingsAction = new QAction(QIcon(":/images/event_setting.png"),
                                      tr("&Event Setting"), this);
    eventSettingsAction->setShortcut(tr("Ctrl+E"));
    eventSettingsAction->setEnabled(false);

    exitAction = new QAction(QIcon(":/images/application-exit.png"),
                             tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);


    QAction *maxAction = new QAction(QIcon(":/images/Max.png"),
                            tr("Max"), this);
    maxAction->setCheckable(true);
    maxAction->setChecked(true);
    maxAction->setStatusTip(tr("Show or hide the Max Value"));
//    connect(maxAction, &QAction::triggered, this, &MainWindow::measureChanged);


    QAction *minAction = new QAction(QIcon(":/images/Min.png"),
                            tr("Min"), this);
    minAction->setCheckable(true);
    minAction->setChecked(true);
    minAction->setStatusTip(tr("Show or hide the Min Value"));

    QAction *rmsAction = new QAction(QIcon(":/images/RMS.png"),
                            tr("RMS"), this);
    rmsAction->setCheckable(true);
    rmsAction->setChecked(true);
    rmsAction->setStatusTip(tr("Show or hide the RMS Value"));

    QAction *avgAction = new QAction(QIcon(":/images/Avg.png"),
                            tr("Avg"), this);
    avgAction->setCheckable(true);
    avgAction->setChecked(false);
    avgAction->setStatusTip(tr("Show or hide the Avg Value"));

    QAction *freqAction = new QAction(QIcon(":/images/Freq.png"),
                             tr("Freq"), this);
    freqAction->setCheckable(true);
    freqAction->setChecked(false);
    freqAction->setStatusTip(tr("Show or hide the Frequency"));

    measureChoiceActions<<maxAction<<minAction<<rmsAction<<avgAction<<freqAction;

}


void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(connectAction);
    fileMenu->addAction(disconnectAction);
    fileMenu->addAction(eventSettingsAction);
    fileMenu->addAction(settingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    optionsMenu = menuBar()->addMenu(tr("&Options"));
    measureSubMenu = optionsMenu->addMenu(tr("&Measure"));
    measureSubMenu->addActions(measureChoiceActions);

    viewMenu = menuBar()->addMenu(tr("&View"));
}

void MainWindow::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("Event List"), this);
    dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    eventList = new EventList(dock);
    eventList->setAlternatingRowColors(true);

    dock->setWidget(eventList);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());
}

void MainWindow::createToolBar()
{
    createConnectToolBar();
    createChannelSetToolBar();

}

void MainWindow::createConnectToolBar()
{
    connectToolBar = addToolBar(tr("Connect"));
    connectToolBar->addAction(connectAction);
    connectToolBar->addAction(disconnectAction);
    connectToolBar->addAction(settingsAction);
    connectToolBar->addAction(eventSettingsAction);
}

void MainWindow::createChannelSetToolBar()
{
    seriesSettingToolBar = addToolBar(tr("Series Setting"));

    channelDeleteAction = new QAction(QIcon(":/images/delete.bmp"),
                                      tr("&Delete"), this);

    curveToolButton = new QToolButton;
    curveToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    curveToolButton->setMenu(createColorMenu(SLOT(channelChanned())));
    channelAction = curveToolButton->menu()->defaultAction();

    curveToolButton->setIcon(createColorToolButtonIcon(
                                 ":/images/curve.png",
                                  mainView->channelColor(0)));
    curveToolButton->setAutoFillBackground(true);

    /* 2018.01.09************************************************************ */
    curveUpAction = new QAction(QIcon(":/images/curveup.bmp"),
                                tr("&Up"), this);
    curveDownAction = new QAction(QIcon(":/images/curvedown.bmp"),
                                tr("&Down"), this);
    curveZoomInAction = new QAction(QIcon(":/images/curveZoomin.bmp"),
                                tr("&In"), this);
    curveZoomOutAction = new QAction(QIcon(":/images/curveZoomout.bmp"),
                                tr("&Out"), this);
    verticalMessureAction = new QAction(QIcon(":/images/verticalMesure.bmp"),
                                        tr("&Up"), this);
    horizonMessureAction = new QAction(QIcon(":/images/horizonMesure.bmp"),
                                        tr("&Up"), this);
    /* end ******************************************************************* */

    seriesSettingToolBar->addWidget(curveToolButton);
    seriesSettingToolBar->addAction(channelDeleteAction);

    /* 2018.01.09************************************************************ */
    seriesSettingToolBar->addAction(curveUpAction);
    seriesSettingToolBar->addAction(curveDownAction);
    seriesSettingToolBar->addAction(curveZoomInAction);
    seriesSettingToolBar->addAction(curveZoomOutAction);
    seriesSettingToolBar->addAction(verticalMessureAction);
    seriesSettingToolBar->addAction(horizonMessureAction);
    /* end ******************************************************************* */
}

void MainWindow::connectSigNSlot()
{
    disconnect(mainView, &MainView::channelSelect, magnifyView, &MagnifyView::channelVisibleSet);
    connect(mainView, &MainView::channelSelect, magnifyView, &MagnifyView::channelVisibleSet);

    disconnect(mainView, &MainView::magnifyViewCursorPos, this, &MainWindow::cursorReplace);
    connect(mainView, &MainView::magnifyViewCursorPos, this, &MainWindow::cursorReplace);

    disconnect(magnifyView, &MagnifyView::cursorMove, this, &MainWindow::magnifyViewCursorPosMove);
    connect(magnifyView, &MagnifyView::cursorMove, this, &MainWindow::magnifyViewCursorPosMove);

    disconnect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    for(int i(0); i < measureChoiceActions.count(); i++){
        disconnect(measureChoiceActions[i], &QAction::triggered, this, &MainWindow::measureChanged);
        connect(measureChoiceActions[i], &QAction::triggered, this, &MainWindow::measureChanged);
    }

    disconnect(channelDeleteAction, SIGNAL(triggered()), this, SLOT(channelDelete()));
    connect(channelDeleteAction, SIGNAL(triggered()), this, SLOT(channelDelete()));

    disconnect(settingsAction, SIGNAL(triggered()), this, SLOT(channelSetting()));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(channelSetting()));

    disconnect(mainView, &MainView::totalYRangeChanged, chanelPickDialog, &ChanelPickDialog::tableScaleAllChanged);
    connect(mainView, &MainView::totalYRangeChanged, chanelPickDialog, &ChanelPickDialog::tableScaleAllChanged);

    disconnect(this, &MainWindow::measureSet, mainView, &MainView::measureOp);
    connect(this, &MainWindow::measureSet, mainView, &MainView::measureOp);

    disconnect(this, &MainWindow::channelOperate, chanelPickDialog, &ChanelPickDialog::tableOperated);
    connect(this, &MainWindow::channelOperate, chanelPickDialog, &ChanelPickDialog::tableOperated);

    disconnect(curveUpAction, &QAction::triggered, this, &MainWindow::channelOp);
    connect(curveUpAction, &QAction::triggered, this, &MainWindow::channelOp);

    disconnect(curveDownAction, &QAction::triggered, this, &MainWindow::channelOp);
    connect(curveDownAction, &QAction::triggered, this, &MainWindow::channelOp);

    disconnect(curveZoomInAction, &QAction::triggered, this, &MainWindow::channelOp);
    connect(curveZoomInAction, &QAction::triggered, this, &MainWindow::channelOp);

    disconnect(curveZoomOutAction, &QAction::triggered, this, &MainWindow::channelOp);
    connect(curveZoomOutAction, &QAction::triggered, this, &MainWindow::channelOp);

    disconnect(verticalMessureAction, &QAction::triggered, this, &MainWindow::messureOp);
    connect(verticalMessureAction, &QAction::triggered, this, &MainWindow::messureOp);
}

QMenu *MainWindow::createColorMenu(const char *slot)
{
    QMenu *colorMenu = new QMenu(this);
    chnImforList imforList = dataSource->getSelectedChannelsImfor();
    for(int i(0); i < imforList.count(); i++){
        QAction *action = new QAction(imforList.at(i).name, this);
        action->setData(mainView->channelColor(i));
        action->setIcon(createColorIcon(mainView->channelColor(i)));
        recentChannelActions << action;
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if(i == 0)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}

QIcon MainWindow::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);
    return QIcon(pixmap);
}


QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(60, 70);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 60, 53);
    QRect source(0, 0, 60, 53);
    painter.fillRect(QRect(0, 50, 60, 70), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}

void MainWindow::channelChanned()
{
    channelAction = qobject_cast<QAction *>(sender());
    curveToolButton->setIcon(createColorToolButtonIcon(
                                 ":/images/curve.png",
                                 qvariant_cast<QColor>(channelAction->data())));

    chnImforList imforList = dataSource->getSelectedChannelsImfor();
    for(int i(0); i < imforList.count(); i++){
        if(channelAction->text() == imforList.at(i).name){
            selectedChannelID = i;
            break;
        }
    }
}

void MainWindow::channelDelete()
{
    if(recentChannelActions.count() == 1)
        return;

    delete recentChannelActions[selectedChannelID];
    recentChannelActions.removeAt(selectedChannelID);
    curveToolButton->setIcon(createColorToolButtonIcon(
                                 ":/images/curve.png",
                                 qvariant_cast<QColor>(recentChannelActions.first()->data())));
    warefirm->wareformDel(selectedChannelID);
    dataSource->removeChannelEx(selectedChannelID);
    mainView->seriesRemove(selectedChannelID);
    magnifyView->seriesRemove(selectedChannelID);

    emit channelOperate(selectedChannelID, Delete, 0.0);

    selectedChannelID = 0;

}

void MainWindow::channelSetting()
{
    if(scopeImfor.isAllNameReceved() == false)
        return;
    if(chanelPickDialog ==nullptr){
        for(int i = 0; i < scopeImfor.totChannel; i++) {
            QPair<QString, QString> pair;
            pair.first = scopeImfor.channelList[i];
            pair.second = pair.first.right(1);
            pair.first = pair.first.left(pair.first.count() - 1);
            communicationPara.channels<<pair;
        }
        communicationPara.bandwidth = scopeImfor.bandWidth;
        communicationPara.sampleRate = scopeImfor.cyclePoints;

        chanelPickDialog = new ChanelPickDialog(&communicationPara, this);
        if(connectDialog)
            connectDialog->setChannelPickDialog(chanelPickDialog);
        connect(chanelPickDialog, SIGNAL(accepted()), this, SLOT(mainWindowMakeUp()));
        connect(chanelPickDialog, SIGNAL(accepted()), connectDialog, SLOT(processChannelPicked()));

    }
    chanelPickDialog->show();
    chanelPickDialog->raise();
    chanelPickDialog->activateWindow();
}

void MainWindow::connectSetting()
{
    if(connectDialog == nullptr){
        connectDialog = new ConnectDialog(this);
        connect(connectDialog, &ConnectDialog::imforGet, this, &MainWindow::channelImforGet);
    }

    connectDialog->show();
    connectDialog->raise();
    connectDialog->activateWindow();
}


void MainWindow::eventSetting()
{
    if(eventSelectTable == nullptr) {
        eventSelectTable = new EventSelectDialog(connectDialog, this);
    }

    eventSelectTable->show();
    eventSelectTable->raise();
    eventSelectTable->activateWindow();
}

void MainWindow::disconnectSetting()
{
    connectDialog->getThread().stopTrans();
    connectAction->setEnabled(true);
    disconnectAction->setDisabled(true);
    settingsAction->setDisabled(true);
    eventSettingsAction->setDisabled(true);
}

void MainWindow::channelImforGet()
{
    connectAction->setDisabled(true);
    disconnectAction->setEnabled(true);
    settingsAction->setEnabled(true);
    eventSettingsAction->setEnabled(true);
}

void MainWindow::mainWindowMakeUp()
{

    qDebug()<<"mainWindowMakeUp";
    ChanelPickDialog::Settings p = chanelPickDialog->settings();
    DataSource::setChannelImfor(p.imfList, p.cycPoints);

    selectedChannelID = 0;

    eventList->clear();

    for(int i(0); i < recordList.count(); i++) {
        delete recordList[i];
    }
    recordList.clear();

    static int cnt = 0;
    if(cnt++){
        delete dataSource;
        delete warefirm;
        delete mainSplitter;

        delete channelDeleteAction;

        if(curveToolButton == nullptr)
            qDebug()<<"nullptr";
        delete curveToolButton;

        for(int i(0); i < recentChannelActions.count(); i++){
            delete recentChannelActions[i];
        }
        recentChannelActions.clear();



        /* 2018.01.09************************************************************ */
        delete curveUpAction;
        delete curveDownAction;
        delete curveZoomInAction;
        delete curveZoomOutAction;
        delete verticalMessureAction;
        delete horizonMessureAction;
        /* end ******************************************************************* */
        delete seriesSettingToolBar;
    }

    dataSource = new DataSource(p.ids, 400);
    dataSource->genarateTransData();

    QList<qreal> scales;
    for(int i(0); i < p.ids.count(); i++){
        scales<<p.imfList.at(p.ids.at(i)).trans.first;
    }
    warefirm = new WareformGenerator(*dataSource, scales);

    disconnect(&connectDialog->getThread(), &SciThread::fresh, warefirm, &WareformGenerator::fresh);
    connect(&connectDialog->getThread(), &SciThread::fresh, warefirm, &WareformGenerator::fresh);
    disconnect(&connectDialog->getThread(), &SciThread::eventGet, this, &MainWindow::eventOp);
    connect(&connectDialog->getThread(), &SciThread::eventGet, this, &MainWindow::eventOp);

    QList<QPair<bool, QString>> measures;
    for(int i(0); i < measureChoiceActions.count(); i++){
        QPair<bool, QString> measure;
        measure.second = measureChoiceActions.at(i)->text();
        measure.first = measureChoiceActions.at(i)->isChecked();
        measures << measure;
    }

    mainView = new MainView(dataSource, measures);
    magnifyView = new MagnifyView(dataSource, mainView);


    mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->addWidget(mainView);
    mainSplitter->addWidget(magnifyView);

    createChannelSetToolBar();
    connectSigNSlot();

    setCentralWidget(mainSplitter);
//    warefirm->start();
}

void MainWindow::channelOp()
{
    QAction *action = qobject_cast<QAction*> (sender());
    Trans newTran = dataSource->getTrans(selectedChannelID);
    int changeType;
    qreal data;

    if(action == curveUpAction){
        newTran.second += 0.1;
        changeType = Position_Change;
        data = newTran.second;
        mainView->refreshSeriesData(selectedChannelID, 0.1, 1);
    }

    if(action == curveDownAction){
        newTran.second -= 0.1;
        changeType = Position_Change;
        data = newTran.second;
        mainView->refreshSeriesData(selectedChannelID, -0.1, 1);
    }

    if(action == curveZoomInAction){
        newTran.first *= (qreal)2;
        changeType = Scale_Change;
        data = newTran.first;
        mainView->setLegendShow(selectedChannelID, newTran);
        mainView->refreshSeriesData(selectedChannelID, 0, 2);
    }

    if(action == curveZoomOutAction){
        newTran.first /= (qreal)2;
        changeType = Scale_Change;
        data = newTran.first;
        mainView->setLegendShow(selectedChannelID, newTran);
        mainView->refreshSeriesData(selectedChannelID, 0, 0.5);
    }


    dataSource->setTrans(selectedChannelID, newTran);
    emit channelOperate(selectedChannelID, changeType, data);
}

void MainWindow::messureOp()
{
    QAction *action = qobject_cast<QAction*> (sender());
    if(action == verticalMessureAction){
        if(mainView->setupMessureCursor()){
            magnifyView->cursorShow();
            emit mainView->magnifyViewCursorPos(mainView->zoomCursorPos().first, mainView->zoomCursorPos().second);
        }else{
            magnifyView->cursorHide();
        }
    }
}

void MainWindow::cursorReplace(qreal p1, qreal p2)
{
    qDebug()<<"corsorReplace: "<<"P1:"<<p1<<"P2:"<<p2;
    magnifyView->setCursorPos(p1, p2);
}

void MainWindow::magnifyViewCursorPosMove(int id, qreal pos)
{
    if(pos >= qreal(0) && pos <= qreal(1)) {
        qreal p = mainView->getMagRectLeft();
        mainView->setCursorPos(id, p + pos * mainView->getMagRectPercent());
        mainView->setCursorInterval();
    }
}

void MainWindow::measureChanged(bool trig)
{
    QAction *action = qobject_cast<QAction*> (sender());
    for(int i(0); i < measureChoiceActions.count(); i++){
        if(action == measureChoiceActions.at(i)){
            emit measureSet(i, trig);
            break;
        }
    }
}

extern RecordData *record;
void MainWindow::eventOp()
{
    recordList<<record;
    record = nullptr;
    QString str1 = strList.at(recordList.last()->eventLevel());
    QString str2 = recordList.last()->imformation();
    str2 += QString("\n");
    str2 += recordList.last()->happenTime;


    eventList->addItem(new QListWidgetItem(QIcon(str1),str2));
}


void MainWindow::eventSelected(QModelIndex index)
{
    eventTable = new EventTable(recordList[index.row()], this);
    eventTable->show();

    eventTable->raise();
    eventTable->activateWindow();
}

void MainWindow::eventActioned(ActionType type, int pos)
{
    qDebug()<<"Action Type:"<<type;
    qDebug()<<"Action Pos:"<<pos;

    switch(type) {
        case deAction: {
            qDebug()<<"recordList:"<<recordList.count();
            RecordData *record = recordList[pos];
            delete record;

            recordList.removeAt(pos);
            qDebug()<<"recordList:"<<recordList.count();
            break;
        }
        case deAllAction: {
            RecordData *record;
            for(int i(0); i < recordList.count(); i++) {
                record = recordList[i];
                delete record;
            }

            recordList.clear();
            break;
        }
        case svAction: {
            QString fileName = QFileDialog::getSaveFileName(this,
                                        tr("Save Event"), ".",
                                        tr("Scope Event files (*.sco)"));
            if(fileName.isEmpty())
                return;

            qDebug()<<fileName;

            QFile file(fileName);
            if(!file.open(QIODevice::WriteOnly)) {
                QMessageBox::warning(this, tr("Scope Event"),
                                     tr("Cannot write file %1:\n%2.")
                                     .arg(file.fileName())
                                     .arg(file.errorString()));

                return;
            }

            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_5_9);

            QApplication::setOverrideCursor(Qt::WaitCursor);

            RecordData *record =  recordList.at(pos);

            out << record->imformation() << record->happenTime << record->channels()
                << record->eventLevel() << record->pointsNum();

            out << record->channelList.count();

            for(int i(0); i < record->channelList.count(); i++) {
                out << record->channelList.at(i);
            }

            for(int i(0); i < record->channelNum(); i++) {
                out << record->records.at(i).first;

                for(int j(0); j < record->pointsNum(); j++) {
                    out << record->records.at(i).second.at(j);
                }
            }

            QApplication::restoreOverrideCursor();

            break;
        }
        case svAllAction: {

            break;
        }

        case opAction: {
            qDebug()<<"opAction";
            QString fileName = QFileDialog::getOpenFileName(this,
                                        tr("Open Event"), ".",
                                        tr("Scope Event files (*.sco)"));

            if(!fileName.isEmpty()) {
                QFile file(fileName);
                if(!file.open(QIODevice::ReadOnly)) {
                    QMessageBox::warning(this, tr("Scope Event"),
                                         tr("Cannot write file %1:\n%2.")
                                         .arg(file.fileName())
                                         .arg(file.errorString()));
                    return;
                }


                QDataStream in(&file);
                in.setVersion(QDataStream::Qt_5_9);

                RecordData *prec = new RecordData;
                QString desc;
                QString time;
                uint32_t selectedChn;
                uint8_t level;
                int perChnNum;

                int n;

                in >> desc >> time >> selectedChn >> level >> perChnNum;

                prec->setDescription(desc);
                prec->happenTime = time;
                prec->setSelectedChannel(selectedChn);
                prec->setLevel(level);
                prec->setChannelPoints(perChnNum);

                in >> n;
                QString channelName;
                for(int i(0); i < n; i++) {
                    in >> channelName;

                    prec->channelList.append(channelName);
                }


                for(int i(0); i < 32; i++) {
                    if(selectedChn & (1 << i)) {
                        QVector<int16_t> vect;
                        vect.resize(prec->pointsNum());
                        prec->records.append(ChnRecord(i, vect));
                    }
                }

                for(int i(0); i < prec->channelNum(); i++) {
                    in >> prec->records[i].first;

                    for(int j(0); j < prec->pointsNum(); j++) {
                        in >> prec->records[i].second[j];
                    }
                }

                recordList<<prec;
                QString str1 = strList.at(recordList.last()->eventLevel());
                QString str2 = recordList.last()->imformation();
                str2 += QString("\n");
                str2 += prec->happenTime;
                eventList->addItem(new QListWidgetItem(QIcon(str1),str2));
            }

            break;
        }

    }
}











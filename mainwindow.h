#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include "chanelpickdialog.h"
#include "communicationpara.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QToolBar;
class QComboBox;
class QCheckBox;
class QLabel;
class QSplitter;
class QToolButton;

class RecordData;
class QListWidget;
QT_END_NAMESPACE


class MagnifyView;
class MainView;
class DataSource;
class WareformGenerator;
class ConnectDialog;
//class EventTable;
#include "eventtable.h"

class EventSelectDialog;
class EventList;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    enum {
        Delete = 0,
        Scale_Change = 1,
        Position_Change = 2
    };
public:
    MainWindow();
    MainWindow(DataSource *data, WareformGenerator *wg);

signals:
    void channelOperate(int id, int changeType, qreal data);
    void measureSet(int id, bool isSet);

private slots:
    void channelChanned();
    void channelDelete();
    void channelSetting();
    void connectSetting();
    void eventSetting();
    void disconnectSetting();
    void mainWindowMakeUp();

     /* 2018.01.09************************************************************ */
    void channelOp();
    void messureOp();

    void cursorReplace(qreal p1, qreal p2);
    void magnifyViewCursorPosMove(int id, qreal pos);

    void measureChanged(bool trig);
    void eventOp();

    void eventSelected(QModelIndex index);
    void eventActioned(ActionType type, int pos);

public slots:
    void channelImforGet();
private:

    void createToolBar();
    void createActions();
    void createMenus();
    void createDockWindows();

    /* 2018.01.08 ----------------------------------------------------------------*/
    void createConnectToolBar();
    void createChannelSetToolBar();
    void connectSigNSlot();
    /* 2018.01.08 ----------------------------------------------------------------*/

    QMenu *createColorMenu(const char *slot);
    QIcon createColorIcon(QColor color);
    QIcon createColorToolButtonIcon(const QString &imageFile, QColor color);



    QAction *connectAction;
    QAction *disconnectAction;
    QAction *settingsAction;
    QAction *eventSettingsAction;
    QAction *exitAction;

    QAction *channelAction;
    QAction *channelDeleteAction;

    QAction *curveUpAction;
    QAction *curveDownAction;
    QAction *curveZoomInAction;
    QAction *curveZoomOutAction;
    QAction *verticalMessureAction;
    QAction *horizonMessureAction;

    QList<QAction *> measureChoiceActions;

    QMenu *fileMenu;
    QMenu *optionsMenu;
    QMenu *measureSubMenu;
    QMenu *viewMenu;

    QToolBar *connectToolBar;
    QToolBar *seriesSettingToolBar;
    QSplitter *mainSplitter;


    QToolButton *curveToolButton;
    DataSource *dataSource;
    WareformGenerator *warefirm = nullptr;
    MainView *mainView;
    MagnifyView *magnifyView;


    int selectedChannelID = 0;
    QList<QAction *> recentChannelActions;

    ChanelPickDialog *chanelPickDialog  = nullptr;
    ConnectDialog *connectDialog = nullptr;
    CommunicationPara communicationPara;

    QList<RecordData *> recordList;
//    QListWidget *eventList;
    EventList *eventList;
    EventTable *eventTable = nullptr;
    EventSelectDialog *eventSelectTable = nullptr;
};

#endif // MAINWINDOW_H

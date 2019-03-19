#ifndef SCITHREAD_H
#define SCITHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
//#include "connectdialog.h"
#include <QtSerialPort/QSerialPort>
#include <QVector>
#include <QTimer>

#include "scopeimfor.h"
class DataSource;
class MainView;

enum ConState {
    Unlink = 0,
    WaitName,
    WaitEvent,
    WaitSetting,
    SendSetting
};

struct SerialSettings {
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};

class SciThread : public QThread
{
    Q_OBJECT

public:
    explicit SciThread(QObject *parent = nullptr);
    ~SciThread();

    void connectToTarget(const SerialSettings &settings, int waitTimeout);
    void post(const QByteArray &data);
    void run();
    void stop();

    void sendNew(uint8_t type,uint8_t *data, uint8_t len)
    {
        newframe.addHi = ADD_HI;
        newframe.addLo = ADD_LO;
        newframe.totLen = len + 1;
        newframe.data[0] = type;
        memcpy(&newframe.data[1], data, len);
        isSendReq = true;

    }

    QVector<int> &selectVector()
    {
        return selectVec;
    }

    void goToSendSettingState();
    void stopTrans();


signals:
    void linked(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);

    void imforGet();
    void fresh(int chnum, int perChannelnum);
    void eventGet();

private:
    void unlinkStateJob(QSerialPort *serial);
    void waitNameStateJob(QSerialPort *serial);
    void waitEventStateJob(QSerialPort *serial);
    void SendSettingStateJob(QSerialPort *serial);
    bool checkRead(QByteArray &response, int state);

    void requireStop(QSerialPort *serial);

    /* 2018.04.10 基本的应答函数 */
    void sendReplyToDevice(QSerialPort *serial);

    SerialSettings settings;
    int waitTimeout;


    QMutex mutex;
    QWaitCondition cond;
    bool quit;
    int state;
	QVector<int> selectVec;
    ScopeFrame newframe;

	bool isEnter = false;
    bool isStop = false;
    bool isSendReq = false;
    qreal cnt = 0;
    QByteArray response;

};

#endif // SCITHREAD_H

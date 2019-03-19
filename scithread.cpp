#include "scithread.h"
#include <QtSerialPort/QSerialPort>
#include "recorddata.h"

#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

QT_USE_NAMESPACE

QList<QVector<QPointF> > newdata;
RecordData *record = nullptr;

//static bool scopedriver_calcChksum(uint8_t *pdata, uint16_t len);

SciThread::SciThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false), state(Unlink)
{

}

SciThread::~SciThread()
{
    qDebug()<<"!SciThread";
    mutex.lock();


    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void SciThread::connectToTarget(const SerialSettings &settings, int waitTimeout)
{
    qDebug()<<"connectToTarget";
    QMutexLocker locker(&mutex);
    this->settings = settings;
    this->waitTimeout = waitTimeout;
    quit = false;
    isStop = false;
    isEnter = false;
    scopeImfor.clear();
    if(!isRunning()){
        start();
    }
}

void SciThread::run()
{
    qDebug()<<"run";

    mutex.lock();
    QSerialPort serial;
    serial.setPortName(settings.name);
    serial.setBaudRate(settings.baudRate);
    serial.setDataBits(settings.dataBits);
    serial.setParity(settings.parity);
    serial.setStopBits(settings.stopBits);
    serial.setFlowControl(settings.flowControl);
    state = Unlink;
    mutex.unlock();



    if(!serial.open(QIODevice::ReadWrite)) {
        emit error(tr("Can't open %1, error code %2")
                   .arg(settings.name).arg(serial.error()));
        return;
    }

    while(!quit){
        switch(state) {
            case Unlink:{
                static int cnt = 0;
                if(cnt == 0){
                    requireStop(&serial);
                    cnt++;
                }
                unlinkStateJob(&serial);
                break;
            }
            case WaitName:{
                waitNameStateJob(&serial);
                break;
            }
            case WaitEvent:{
                waitEventStateJob(&serial);
                break;
            }
            case WaitSetting:{
                break;
            }
            case SendSetting:{
                SendSettingStateJob(&serial);
                break;
            }
            default:
                break;
        }

        if(isStop == true){
            requireStop(&serial);
            isStop = false;
        }

        if(isSendReq == true) {
            int len = 3 + newframe.totLen;
            QByteArray data;
            data.append((char *)&newframe, len);
            serial.write(data, len);
            if(serial.waitForBytesWritten(waitTimeout))
                qDebug()<<"####";
            isSendReq = false;
        }
    }
}


void SciThread::requireStop(QSerialPort *serial)
{
    ScopeFrame frame;
    frame.addHi = ADD_HI;
    frame.addLo = ADD_LO;
    frame.totLen = 1;
    frame.data[0] = disconnecting;

    int len = 3 + frame.totLen;

    QByteArray data;
    data.append((char *)&frame, len);
    qDebug()<<data;
    serial->write(data, len);
    if(serial->waitForBytesWritten(waitTimeout))
        qDebug()<<"####";
}

void SciThread::goToSendSettingState()
{
    state = SendSetting;
    isEnter = false;

    newdata.clear();
    response.clear();

    cnt = 0;
}

void SciThread::stopTrans()
{
    qDebug()<<"stopTrans";
    isStop = true;
}

#include <QPointF>
void SciThread::SendSettingStateJob(QSerialPort *serial)
{

    if(!isEnter){
        isEnter = true;
        qDebug()<<"SendSettingStateJob()";
        qDebug()<<selectVec;


        ScopeFrame frame;
        frame.addHi = ADD_HI;
        frame.addLo = ADD_LO;
        frame.totLen = selectVec.count() + 1;
        frame.data[0] = selecting;
        for(int i = 0; i < selectVec.count(); i++){
            frame.data[i + 1] = (uint8_t)selectVec[i];
        }

        int len = 3 + frame.totLen;

        QByteArray data;
        data.append((char *)&frame, len);



        qDebug()<<data;
        serial->write(data);
        serial->waitForBytesWritten(waitTimeout);
    }


    if(serial->waitForReadyRead(30)) {
        response += serial->readAll();
        QByteArray y;
        y[0] = ADD_HI;
        y[1] = ADD_LO;

        while(response.count()) {
            int p = response.indexOf(y);
            if(p == -1)
                break;

            response = response.mid(p);
            if(response.count() >= 3) {
                uint8_t totCount = (uint8_t)response.at(2) + 3;
                if(response.count() >= totCount){
                    QByteArray part = response.left(totCount);
                    response = response.mid(totCount);


                    unsigned char *data = (unsigned char *)part.data();

                   switch(data[3]) {
                        case frameReal:{
                           for(int i(0); i < data[4]; i++) {
                                QVector<QPointF> points;
                                for(int j = 0; j < data[5]; j++) {
                                    int16_t value = data[6 + 2 * j * data[4] + 2 *i];
                                    value <<= 8;
                                    value += data[6 + 2 * j * data[4] + 2 *i + 1];
                                    points.append(QPointF(qreal(cnt + j), qreal(value)));
                                }
                                newdata.append(points);
                           }
                            cnt += data[5];
                            emit fresh(data[4], data[5]);
                           break;
                        }
                       case frameEvtDiscript:{
                            ScoEvtPCB evtPcb;
                            evtPcb.selectedChn = ((uint32_t)data[4] << 24)
                                    + ((uint32_t)data[5] << 16)
                                    + ((uint32_t)data[6] << 8)
                                    + ((uint32_t)data[7]);
                            evtPcb.level = data[8];
                            evtPcb.chnPoints = (((uint16_t)data[9]) << 8) + (uint16_t)data[10];
                            for(int i(11); i < totCount; i++){
                                evtPcb.discription += data[i];

                            }

                           if(!record) {
                               record = new RecordData(evtPcb);

                           }else{
                               delete record;
                               record = new RecordData(evtPcb);

                           }
                           break;
                       }
                       case frameEvtData:{
                           if(record){
                               ScoEvt scoEvt;
                               RecRes res;
                               scoEvt.chnID = data[4];
                               scoEvt.points = data[5];
                               for(int i(0); i < scoEvt.points; i++) {
                                   scoEvt.data << (((int16_t)data[6 + 2 * i] << 8) + (int16_t)data[6 + 2 * i + 1]);
                               }
                               res = record->receiveChnData(scoEvt);

                               if(res == fullRes) {
                                   QDateTime time = QDateTime::currentDateTime();
                                   QString timerStr = time.toString("yyyy-MM-dd hh:mm:ss dddd");

                                   record->happenTime = timerStr;
                                   record->channelList = scopeImfor.channelList;

                                   emit eventGet();
                               }

                           }
                           break;
                       }
                   }


                }else{
                    break;
                }
            }else{
                break;
            }

        }
    }


}

void SciThread::waitNameStateJob(QSerialPort *serial)
{
    qDebug()<<"waitNameStateJob";
    ScopeFrame frame;
    frame.addHi = ADD_HI;
    frame.addLo = ADD_LO;
    frame.totLen = 2;
    frame.data[0] = nameloading;

    for(int i(0); i < scopeImfor.totChannel; i++) {
        frame.data[1] = i;
        int len = frame.totLen + 3;

        QByteArray data;
        data.append((char *)&frame, len);



        serial->write(data);
        if(serial->waitForBytesWritten(waitTimeout)){
            QByteArray responseData;
            while(serial->waitForReadyRead(100))
               responseData += serial->readAll();
            qDebug()<<"responseData"<<responseData;
            checkRead(responseData, state);
        }

    }

    if(scopeImfor.isAllNameReceved()){
        sendReplyToDevice(serial);
//        emit imforGet();
        state = WaitEvent;
    }else{
        stop();
        emit timeout(tr("Wait Name State Err"));
    }
}

void SciThread::waitEventStateJob(QSerialPort *serial)
{
    qDebug()<<"waitEventStateJob";
    ScopeFrame frame;
    frame.addHi = ADD_HI;
    frame.addLo = ADD_LO;
    frame.totLen = 2;
    frame.data[0] = eventloading;

    for(int i(0); i < scopeImfor.eventNumber; i++) {
        frame.data[1] = i;
        int len = frame.totLen + 3;

        QByteArray data;
        data.append((char *)&frame, len);
        serial->write(data);

        qDebug()<<"write:"<<data;
        if(serial->waitForBytesWritten(waitTimeout)) {
            QByteArray responseData;
            while(serial->waitForReadyRead(100))
               responseData += serial->readAll();
            qDebug()<<"responseData"<<responseData;
            checkRead(responseData, state);
        }

    }
    if(scopeImfor.isAllEventReceved()) {
        sendReplyToDevice(serial);
        emit imforGet();
        state = WaitSetting;
    }else {
        stop();
        emit timeout(tr("Wait Event State Err"));
    }

}

void SciThread::unlinkStateJob(QSerialPort *serial){
    qDebug()<<"unlinkStateJob";

    ScopeFrame frame;
    frame.addHi = 0xAA;
    frame.addLo = 0xAA;
    frame.totLen = 1;
    frame.data[0] = connecting;

    int len = frame.totLen + 3;

    qDebug()<<"len: "<<len;
    QByteArray data;
    data.append((char *)&frame, len);

    qDebug()<<data;

    int writeCnt = 0;
    while(writeCnt < 3) {
        serial->write(data);
        if(serial->waitForBytesWritten(waitTimeout)) {
            QByteArray responseData;
            while(serial->waitForReadyRead(30))
               responseData += serial->readAll();

            qDebug()<<responseData;
            if(checkRead(responseData, state)){
                emit linked(tr("%1").arg(QString(responseData)));
                scopeImfor.bandWidth = serial->baudRate() / 10;

                sendReplyToDevice(serial);

                state = WaitName;
                break;
            }
        }
        writeCnt++;
    }

    if(writeCnt >= 3){
        stop();
        emit timeout(tr("Wait request 3 times with no response %1")
                                .arg(QTime::currentTime().toString()));
    }
}

bool SciThread::checkRead(QByteArray &response, int state)
{
    //step1: 检测数据是否为空
    if(response.isEmpty())
        return false;

    //step3: 根据状态检测数据
    switch(state) {
        case Unlink: {
            //step2: 检测数据整体是否正确
            ScopeFrame *pf = (ScopeFrame *)response.data();
            if(pf->addHi != ADD_HI || pf->addLo != ADD_LO)
                return false;
        #if(SCOPE_CHECKSUM_ENABLE)
            if(!scopedriver_calcChksum(pf->data, pf->totLen)) {
                return false;
            }
        #endif

            if(pf->data[0] != frameBasic)
                return false;

            scopeImfor.cyclePoints = (pf->data[1] << 8) + pf->data[2];
            scopeImfor.totChannel = pf->data[3];
            scopeImfor.eventNumber = pf->data[4];

            scopeImfor.devicCfg.refreshCycle = pf->data[5];
            scopeImfor.devicCfg.eventProFreq = (pf->data[6] << 8) + pf->data[7];

            scopeImfor.devicCfg.realDataBlkSize = (pf->data[8] << 8) + pf->data[9];
            scopeImfor.devicCfg.realDataBlkNum = pf->data[10];

            scopeImfor.devicCfg.eventDataBlkSize = (pf->data[11] << 8) + pf->data[12];
            scopeImfor.devicCfg.eventDataBlkNum = pf->data[13];

            scopeImfor.devicCfg.eventPcbBlkNum = pf->data[14];

            return true;
        }


        case WaitName:{
            ScopeFrame *pf = (ScopeFrame *)response.data();
            if(pf->addHi != ADD_HI || pf->addLo != ADD_LO)
                return false;

            if(pf->data[0] != frameName)
                return false;

            QString str;
            int pos = 0;
            while(pos < pf->totLen - 1) {
                if(pf->data[pos + 1]) {
                    str += pf->data[pos + 1];
                }
                pos++;

            }
            scopeImfor.channelList << str;
            return true;
        }

        case WaitEvent:{
            ScopeFrame *pf = (ScopeFrame *)response.data();
            if(pf->addHi != ADD_HI || pf->addLo != ADD_LO)
                return false;

            if(pf->data[0] != frameEvent)
                return false;

            Event event;
            event.level = pf->data[1];
            event.cycBefor = pf->data[2];
            event.cycAfter = pf->data[3];
            event.selectedChns = ((uint32_t)pf->data[4] << 24)
                    + ((uint32_t)pf->data[5] << 16)
                    + ((uint32_t)pf->data[6] << 8)
                    + (uint32_t)pf->data[7];

            int pos = 8;
            while(pos < pf->totLen - 1) {
                if(pf->data[pos]) {
                    event.description += pf->data[pos];
                }
                pos++;
            }
            scopeImfor.eventList<<event;

            qDebug()<<"event.level:"<<event.level;
            qDebug()<<"event.cycAfter:"<<event.cycAfter;
            qDebug()<<"event.cycBefor:"<<event.cycBefor;
            qDebug()<<"event.selectedChns:"<<event.selectedChns;
            qDebug()<<"event.description:"<<event.description;

            return true;
        }

        default: return false;

    }
}


void SciThread::stop()
{   
    mutex.lock();
    quit = true;
    mutex.unlock();
}


void SciThread::sendReplyToDevice(QSerialPort *serial)
{
    ScopeFrame frame;
    frame.addHi = 0xAA;
    frame.addLo = 0xAA;
    frame.totLen = 1;
    frame.data[0] = ask;

    int len = frame.totLen + 3;

    qDebug()<<"len: "<<len;
    QByteArray data;
    data.append((char *)&frame, len);

    serial->write(data);
    serial->waitForBytesWritten(waitTimeout);
}










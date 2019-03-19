#ifndef SCOPEIMFOR_H
#define SCOPEIMFOR_H

#include <QString>
#include <QStringList>

#define ADD_HI              0xAA
#define ADD_LO              0xAA

#define SCOPE_CHECKSUM_ENABLE       0

enum ScoPCToDevice {
    ask = 0,
    connecting = 1,
    nameloading = 2,
    eventloading = 3,
    selecting = 4,
    disconnecting = 5
};

enum ScoDeviceToPC {
    frameBasic = 0,
    frameName = 1,
    frameEvent = 2,
    frameReal = 3,
    frameEvtDiscript = 4,
    frameEvtData = 5
};

struct ScopeFrame {
    uint8_t addHi;
    uint8_t addLo;
    uint8_t totLen;
    uint8_t data[250];
};

class Event{
public:
   QString description;
   uint8_t level;
   uint8_t cycBefor;
   uint8_t cycAfter;
   uint8_t selectedChns;
};

class ScopeImfor {
public:
    ScopeImfor();
    void clear();

    bool isAllNameReceved() const
    {
        if(totChannel == 0)
            return false;

        return channelList.count() == totChannel;
    }

    bool isAllEventReceved() const
    {
        return eventList.count() == eventNumber;
    }


    QStringList channelList;
    uint16_t totChannel = 0;              //芯片采样总通道数
    uint16_t eventNumber = 0;
    uint16_t cyclePoints = 0;             //芯片单周期采样点数
    qint32 bandWidth = 0;

    struct {
        uint8_t refreshCycle;           //芯片循环采样保留周期数
        uint16_t eventProFreq;          //芯片事件执行频率(1ms,2ms,5ms...)

        uint16_t realDataBlkSize;       //芯片实时量数据块的缓存大小
        uint8_t realDataBlkNum;         //芯片实时量数据块的缓存数量

        uint16_t eventDataBlkSize;      //芯片故障量数据块的缓存大小
        uint8_t eventDataBlkNum;         //芯片故障量数据块的缓存数量

        uint8_t eventPcbBlkNum;          //芯片事件处理器模块数量
    }devicCfg;

    QList<Event> eventList;

//    struct {
//        uint8_t refreshCycle;           //芯片循环采样保留周期数
//        uint16_t eventProFreq;          //芯片事件执行频率(1ms,2ms,5ms...)

//        uint16_t realDataBlkSize;       //芯片实时量数据块的缓存大小
//        uint8_t realDataBlkNum;         //芯片实时量数据块的缓存数量

//        uint16_t eventDataBlkSize;      //芯片故障量数据块的缓存大小
//        uint8_t eventDataBlkNum;         //芯片故障量数据块的缓存数量

//        uint8_t eventPcbBlkNum;          //芯片事件处理器模块数量
//    }devicCfg;
};

extern ScopeImfor scopeImfor;

#endif // SCOPEIMFOR_H

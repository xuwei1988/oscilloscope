#ifndef RECORDDATA_H
#define RECORDDATA_H

#include <QtCore/QVector>

typedef QPair<int, QVector<int16_t>> ChnRecord;

//typedef enum ScoFrameType{
//    frameBasic = 0,
//    frameName = 1,
//    frameReal = 2,
//    frameEvtDiscript = 3,
//    frameEvtData = 4
//}ScoFrame;

typedef enum EvtlevelType {
    customer = 0,
    warning = 1,
    fault = 2
}Evtlevel;

typedef enum RecResType {
    partRes,
    errRes,
    fullRes
}RecRes;

class ScoEvtPCB{
public:
    uint32_t selectedChn;
    uint8_t level;
    uint16_t chnPoints;
    QString discription;
};

class ScoEvt{
public:
    uint8_t chnID;
    uint8_t points;
    QVector<int16_t> data;
};

class RecordData
{
public:
    RecordData();
    RecordData(ScoEvtPCB &scoEvtPcb);

    RecRes receiveChnData(ScoEvt &scoEvt);
    QString imformation() const
    {
        return discription;
    }

    uint32_t channels() const
    {
        return selectedChn;
    }

    uint8_t eventLevel() const
    {
        return level;
    }

    int channelNum() const
    {
        return records.count();
    }

    int pointsNum() const
    {
        return perChnNum;
    }

    int channelID(int num) const
    {
        return records.at(num).first;
    }

    void setDescription(QString &desc)
    {
        discription = desc;
    }

    void setSelectedChannel(uint32_t channels)
    {
        selectedChn = channels;
    }

    void setLevel(uint8_t lv)
    {
        level = lv;
    }

    void setChannelPoints(int number)
    {
        perChnNum = number;
    }

    QList<ChnRecord> records;
    QString happenTime;
    QStringList channelList;

private:
    bool state;                         //接收状态用于描述事件是否接收完整

    QString discription;                //事件描述

    uint32_t selectedChn;               //已选取的通道

    uint8_t level;                     //记录的事件等级
    int perChnNum;                      //每个通道记录的总点数
};

#endif // RECORDDATA_H

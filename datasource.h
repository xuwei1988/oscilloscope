#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QVector>
#include <QtCore/QPair>


/* 类型重命名，增强可读性 --------------------------------------*/
typedef QPair<int, QVector<QPointF>> Channel;
typedef QList<Channel> ChannelList;
typedef QPair<qreal, qreal> Trans;
typedef struct{
    QString name;
    Trans trans;
}ChannelImfor;

typedef QList<ChannelImfor> chnImforList;



class DataSource : public QObject
{
    Q_OBJECT
public:
    DataSource(QVector<int> channelIds,
               int cycs = 1000,                                            //保留周期(1000 = 20S) (1000/50Hz = 20)
               QObject *parent = 0);

    DataSource(ChannelList &data);                                          //注意：传递给此函数的data必须是采样点的整数倍！！

    static void setChannelImfor(chnImforList &imfList, int cycpoints);
    chnImforList &getChannelImfor(){return chnImfList; }
    chnImforList getSelectedChannelsImfor() const;

    bool genarateTransData();
    bool addChannel(int id);
//    bool removeChannel(int id);


    static void setChannelTrans(int id, Trans trans);
    static void setChannelTransNow(DataSource &data, int id, Trans trans);

    int getColumns() const {return columns; }
    int getRows() const {return rows; }
    int getCyclePoints() const {return cyclePoints; }
    int getCycles() const {return cycles; }

    ChannelList & getRawData() {return rawData; }
    ChannelList & getTransData() {return transData; }

    const QVector<QPointF> & getRawVec(int row) const {return rawData.at(row).second; }
    const QVector<QPointF> & getTransVec(int row) const {return transData.at(row).second; }
    QPair<qreal, qreal> getAreaXRange(qreal start, qreal end = 1.0) const;

    Trans getTrans(int id) const;

public slots:
    bool removeChannel(int id);
    bool removeChannelEx(int id);

    bool refreshData(ChannelList &newData);
    bool refreshData(QList<QVector<QPointF> > &newData);

    void setTrans(int id, Trans trans);


private:
    int cycles;                                 //采样保留周期数
    static int cyclePoints;                     //单周期采样点数

    int rows;                                   //采样通道数
    int columns;                                //单通道总点数

    ChannelList rawData;                        //原始采样数据
    ChannelList transData;                      //被转换的采样数据
    static chnImforList chnImfList;             //各通道的信息(包括名称、坐标变换等)
};

#endif // DATASOURCE_H

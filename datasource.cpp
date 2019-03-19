#include "datasource.h"
#include <QtMath>

chnImforList DataSource::chnImfList;
int DataSource::cyclePoints;

DataSource::DataSource(QVector<int> channelIds,
                       int cycs,                                      //单周期采样点数(200 = 10K采样频率)
                       QObject *parent)
    : QObject(parent),
      cycles(cycs)
{
    rows = channelIds.count();
    columns = cycles * cyclePoints;

    for(int i(0); i < rows; i++){
        QVector<QPointF> points;
        for(int j(0); j < columns; j++){
            qreal x(0);
            qreal y(0);

            x = j;
            y = 0;
            points.append(QPointF(x, y));
        }
        rawData.append(Channel(channelIds.at(i), points));
    }
}


DataSource::DataSource(ChannelList &data)
{
    rawData = data;
    rows = rawData.count();
    columns = rawData.at(0).second.count();
    cycles = columns / cyclePoints;


    genarateTransData();
}



void DataSource::setChannelImfor(chnImforList &imfList, int cycpoints)
{
    chnImfList = imfList;
    cyclePoints = cycpoints;
}


bool DataSource::genarateTransData()
{
    if(chnImfList.isEmpty())
        return false;

    for(int i(0); i < rows; i++){
        int pos = rawData.at(i).first;
        Trans trans = chnImfList.at(pos).trans;
//        if(trans.first != 1 || trans.second != 0){
            QVector<QPointF> points;
            for(int j(0); j < columns; j++){
                qreal x = rawData.at(i).second.at(j).x();
                qreal y = rawData.at(i).second.at(j).y() * trans.first + trans.second;
                points.append(QPointF(x, y));
            }
            transData.append(Channel(pos, points));
//        }
    }
    return true;
}

bool DataSource::addChannel(int id)
{
    for(int i(0); i < rows; i++){
        int currId =  rawData.at(i).first;
        if(currId == id)
            return false;

        if(currId > id){
            QVector<QPointF> points;
            QVector<QPointF> points1;
            for(int j(0); j < columns; j++){
                qreal x(0);
                qreal y(0);

                qreal y1(0);

                y = 0;
                y1 = y + chnImfList.at(id).trans.second;
                x = rawData.at(0).second.at(j).x();
                points.append(QPointF(x, y));
                points1.append(QPointF(x, y1));
            }
            rawData.insert(i, Channel(id, points));
            transData.insert(i, Channel(id, points1));
            rows++;
            return true;
        }
    }
    return false;
}

bool DataSource::removeChannel(int id)
{
    if(rows == 1)
        return false;
    for(int i(0); i < rows; i++){
        if(rawData.at(i).first == id){
            rawData.removeAt(i);
            transData.removeAt(i);
            rows--;
            return true;
        }
    }
    return false;
}


bool DataSource::removeChannelEx(int id)
{
    if(rows == 1)
        return false;
    rawData.removeAt(id);
    transData.removeAt(id);
    rows--;
    return true;
}

/* 对于坐标变换得分两种情况:
 * 1: 如果当前屏幕正在运动，那么不改变当前已存储的点的只，但是之后进来的点要用新变换
 * 2: 如果屏幕禁止，那么需要刷新屏幕所有的点
 */
void DataSource::setChannelTrans(int id, Trans trans)
{
    chnImfList[id].trans = trans;
}

void DataSource::setChannelTransNow(DataSource &data, int id, Trans trans)
{
    chnImfList[id].trans = trans;


    for(int i(0); i < data.rows; i++){
        if(data.transData.at(i).first == id){
            for(int j(0); j < data.columns; j++){
                data.transData[i].second[j].ry() =
                        data.rawData[i].second[j].y() / trans.first + trans.second;
            }
            break;
        }
    }
}

QPair<qreal, qreal> DataSource::getAreaXRange(qreal start, qreal end) const
{
    QPair<qreal, qreal> r;
    int begin = columns * start - 1;
    if(begin < 0)
        begin = 0;
    r.first = rawData.first().second.at(begin).x();
    r.second = rawData.first().second.at(columns * end - 1).x();

    return r;
}


bool DataSource::refreshData(ChannelList &newData)
{
    if(newData.count() != rows)
        return false;

    int slideNum = newData.at(0).second.count();

    for(int i(0); i < rows; i++){
        QVector<QPointF> &rRawVec = rawData[i].second;
        QVector<QPointF> &rTransVec = transData[i].second;
        QVector<QPointF> &vec = newData[i].second;
        int a = chnImfList.at(rawData[i].first).trans.first;
        int b = chnImfList.at(rawData[i].first).trans.second;

        if(slideNum < columns){
            rRawVec.remove(0, slideNum);
            rRawVec += vec;

            rTransVec.remove(0, slideNum);
            for(int j(0); j < slideNum; j++){
                QPointF point(vec.at(j).x(),vec.at(j).y() * a + b);
                rTransVec.append(point);
            }
        }else{
            rRawVec = vec.mid(slideNum - columns);
            for(int j(0); j < columns; j++){
                QPointF point(rRawVec.at(j).x(),rRawVec.at(j).y() * a + b);
                rTransVec[i] = point;
            }

        }

    }

    return true;
}

bool DataSource::refreshData(QList<QVector<QPointF> > &newData)
{
    if(newData.count() != rows)
        return false;

    int slideNum = newData.at(0).count();

    for(int i(0); i < rows; i++){
        QVector<QPointF> &rRawVec = rawData[i].second;
        QVector<QPointF> &rTransVec = transData[i].second;
        QVector<QPointF> &vec = newData[i];
        qreal a = chnImfList.at(rawData[i].first).trans.first;
        qreal b = chnImfList.at(rawData[i].first).trans.second;

        if(slideNum < columns){
            rRawVec.remove(0, slideNum);
            rRawVec += vec;

            rTransVec.remove(0, slideNum);
            for(int j(0); j < slideNum; j++){
                QPointF point(vec.at(j).x(),vec.at(j).y() * a + b);
                rTransVec.append(point);
            }

        }else{
            rRawVec = vec.mid(slideNum - columns);
            for(int j(0); j < columns; j++){
                QPointF point(rRawVec.at(j).x(),rRawVec.at(j).y() * a + b);
                rTransVec[j] = point;
            }

        }
        for(int k(0); k < columns; k++){
            rRawVec[k].setX(k);
            rTransVec[k].setX(k);
        }

    }

    return true;
}

chnImforList DataSource::getSelectedChannelsImfor() const
{
    chnImforList list;
    const auto channels = rawData;
    for(Channel channel : channels){
        list<<chnImfList.at(channel.first);
    }

    return list;
}

#include <QDebug>
void DataSource::setTrans(int id, Trans trans)
{
    qDebug()<<"setTrans: "<<trans;
    chnImfList[rawData.at(id).first].trans = trans;
}

Trans DataSource::getTrans(int id) const
{
    return chnImfList[rawData.at(id).first].trans;
}

















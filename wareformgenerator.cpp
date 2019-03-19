#include "wareformgenerator.h"
#include "datasource.h"
#include <QDebug>
#include <QTimerEvent>
#include <QTime>
#include <QtMath>
WareformGenerator::WareformGenerator(int n, DataSource &data, QObject *parent)
    :QObject(parent),
      m_data(data)
{
    timerID = startTimer(n);

    a<<20<<7<<400<<0.5;
}

WareformGenerator::WareformGenerator(DataSource &data, QList<qreal> scales, QObject *parent)
    :QObject(parent),
      m_data(data)
{
    this->scales = scales;
    for(int i(0); i < scales.count(); i++){
        a<<qreal(1) / scales.at(i) * 0.8;
    }
}

void WareformGenerator::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    static QTime dateTime(QTime::currentTime());
    long int currTime = dateTime.elapsed();
    static int prevTime = 0;

    int freq = m_data.getCyclePoints();
     qreal perMsNum = (qreal)freq/(qreal)20;
     int size = (currTime - prevTime) * perMsNum;

     if(size == 0){
         qDebug()<<"size == 0";
         return;
     }
     QList<QVector<QPointF> > newdata;
     for(int i(0); i < m_data.getRows(); i++){
         QVector<QPointF> points;
          for(int j(0); j < size; j++){
              points.append(QPointF(qreal(j + m_data.getColumns() + prevTime * perMsNum),
                                    20 * qCos(3.14 * ((qreal)i/(qreal)3 +
                                                 (qreal)2 * j/(qreal)freq +
                                                 (qreal)2 * prevTime * perMsNum/ (qreal)freq))));

          }
         newdata.append(points);
     }
     m_data.refreshData(newdata);
     prevTime = currTime;
}

void WareformGenerator::wareformDel(int id)
{
    a.removeAt(id);
}

void WareformGenerator::start()
{
    timerID = startTimer(25);
}

void WareformGenerator::stop()
{
    killTimer(timerID);
}

extern QList<QVector<QPointF> >newdata;
void WareformGenerator::fresh(int chNum, int perChNum)
{

    QList<QVector<QPointF> > freshData;

    if(newdata.isEmpty())
        return;

    for(int i = 0; i < chNum; i++) {
        if(newdata[0].count() != perChNum)
            return;

        freshData.append(newdata[0]);
        for(int j = 0; j < perChNum; j++) {
            freshData[i][j].setX(freshData[i][j].x() + m_data.getColumns());
        }
        newdata.removeFirst();
    }
    m_data.refreshData(freshData);
}















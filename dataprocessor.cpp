#include "dataprocessor.h"

DataProcessor dataProcessor;

DataProcessor::DataProcessor()
    : minPoint(QPointF(0, 0)), maxPoint(QPointF(0,0)), avg(0), rms(0), freq(0)
{

}

void DataProcessor::calc(QVector<QPointF> &points)
{
    int cnt = points.count();
    minPoint.setY(points.first().y());
    maxPoint.setY(points.first().y());
    qreal sum = 0;
    qreal pow = 0;


    for(int i(0); i < cnt; i++){
        qreal y = points.at(i).y();
        if(minPoint.y() > y){
            minPoint.setX(i);
            minPoint.setY(y);
        }
        if(maxPoint.y() < y){
            maxPoint.setX(i);
            maxPoint.setY(y);
        }

        sum += y;
        pow += y*y;
    }
    avg = sum / qreal(cnt);
    rms = sqrt(pow/qreal(cnt));
}


void DataProcessor::calc(QVector<QPointF> &points, QPair<qreal, qreal> tran)
{
    int cnt = points.count();
    minPoint.setY((points.first().y()- tran.second) /tran.first);
    maxPoint.setY((points.first().y()- tran.second) /tran.first);

    qreal sum = 0;
    qreal pow = 0;


    for(int i(0); i < cnt; i++){
        qreal y = (points.at(i).y() - tran.second)/tran.first;
        if(minPoint.y() > y){
            minPoint.setX(i);
            minPoint.setY(y);
        }
        if(maxPoint.y() < y){
            maxPoint.setX(i);
            maxPoint.setY(y);
        }

        sum += y;
        pow += y*y;
    }
    avg = sum / qreal(cnt);
    rms = sqrt(pow/qreal(cnt));
}

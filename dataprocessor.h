#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QPointF>
#include <QVector>
//#include <QPair>
class DataProcessor
{
public:
    DataProcessor();
    void calc(QVector<QPointF> &points);
    void calc(QVector<QPointF> &points, QPair<qreal, qreal> tran);

    void clear() {
        minPoint.rx() = 0;
        minPoint.ry() = 0;
        maxPoint.rx() = 0;
        maxPoint.ry() = 0;
        avg = 0;
        rms = 0;
        freq = 0;


    }


    QPointF minPoint;
    QPointF maxPoint;
    qreal avg;
    qreal rms;
    qreal freq;
};

extern DataProcessor dataProcessor;

#endif // DATAPROCESSOR_H

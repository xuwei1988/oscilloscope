#ifndef WAREFORMGENERATOR_H
#define WAREFORMGENERATOR_H

#include <QObject>

class DataSource;

class WareformGenerator : public QObject
{
    Q_OBJECT
public:
    WareformGenerator(int n, DataSource &data, QObject *parent = 0);
    WareformGenerator(DataSource &data, QList<qreal> scales, QObject *parent = 0);

public slots:
    void wareformDel(int id);
    void start();
    void stop();
    void fresh(int chNum, int perChNum);

protected slots:
    void timerEvent(QTimerEvent *event);

private:
    int timerID;
    DataSource &m_data;
    QList<qreal> a;
    QList<qreal> scales;
};

#endif // WAREFORMGENERATOR_H

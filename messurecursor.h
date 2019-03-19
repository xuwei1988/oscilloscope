#ifndef MESSURECURSOR_H
#define MESSURECURSOR_H

#include <QtCharts/QChartGlobal>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QFont>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MessureCursor : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    MessureCursor(QChart *parent,int id, qreal posX);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void updateGeometry();
    void setCenter(qreal posX);
    qreal center() const {return centerX; }

signals:
    void positionChanged(qreal pos);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QChart *chart;
    qreal centerX;
    int id;
    QRectF textRect;
};

#endif // MESSURECURSOR_H

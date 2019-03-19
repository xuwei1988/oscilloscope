#ifndef MAGNIFYRECT_H
#define MAGNIFYRECT_H

#include <QtCharts/QChartGlobal>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QFont>
//#include <QObject>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
QT_END_NAMESPACE


QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MagnifyRect : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    MagnifyRect(QChart *parent);
    void setPercent(qreal percentX);
    void setCenter(qreal posX);
    qreal percent() const {return rectPercentX; }
    qreal left() const {return rectCenterX - rectPercentX / (qreal)2; }
    qreal right() const {return rectCenterX + rectPercentX / (qreal)2; }

    void setRect();
    void updateGeometry();

    QPair<qreal, qreal> getArea() const
    {
        return QPair<qreal, qreal>(rectCenterX - rectPercentX/(qreal)2, rectPercentX);
    }


    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);



signals:
    void MagnifyChanged(qreal left, qreal right);

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);

private:
    QChart *chart;                      //父对象
    QRectF rect;                        //需描绘矩形
    qreal rectCenterX;                  //矩形中心在X轴的"坐标"
    qreal rectPercentX;                 //矩形X轴占比
};

#endif // MAGNIFYRECT_H

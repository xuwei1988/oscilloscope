#include "magnifyrect.h"
#include "datasource.h"
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QChart>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

#include <QDebug>

MagnifyRect::MagnifyRect(QChart *parent)
    : QObject(0),
      QGraphicsItem(parent)

{
    chart = parent;

    setCursor(Qt::PointingHandCursor);
    setFlags(ItemIsMovable | ItemIsSelectable);
}


QRectF MagnifyRect::boundingRect() const
{
    return rect;
}

void MagnifyRect::setRect()
{
    qreal width, height;
    height = chart->plotArea().height();
    width = chart->plotArea().width() * rectPercentX;
    rect.setRect(0, 0, width, height);
}


void MagnifyRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    QPen pen(Qt::white);
    pen.setWidth(2);
    if(option->state & QStyle::State_Selected){
        pen.setWidth(3);
        setCursor(Qt::SizeHorCursor);
    }else{
       setCursor(Qt::PointingHandCursor);
    }
    painter->setPen(pen);

    painter->drawRoundedRect(rect, 0, 0);
}

void MagnifyRect::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton){
        QPointF move;
        move.setX(event->pos().x() - event->buttonDownPos(Qt::LeftButton).x());
        move.setY(0);
        //注意： 这里返回的mapToParent(move))是item坐标原点的坐标
        setCenter(QPointF(mapToParent(move)).x() + rect.width()/2.0 );
        updateGeometry();
        emit MagnifyChanged(left(), right());
        event->setAccepted(true);
    } else {
        event->setAccepted(false);
    }
}

void MagnifyRect::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    //防止和MainView的wheelEvent重叠！
    if(event->modifiers() == Qt::AltModifier)
        return;
    int numDegrees = event->delta() / 8;
    int numTicks =numDegrees / 15;

    qDebug()<<numTicks;
    if(numTicks < 0){
        rectPercentX /= (qreal)(abs(numTicks) * 2);
    }
    if(numTicks > 0){
        rectPercentX *= (qreal)(abs(numTicks) * 2);
    }

    qreal maxPer;
    //以下代码防止画出的矩形越界！
    if(rectCenterX >= 0.5)
        maxPer = 2*(1 - rectCenterX);
    else
        maxPer = 2 * rectCenterX;
    if(rectPercentX < 0.02)
        rectPercentX = 0.02;
    if(rectPercentX >= maxPer)
        rectPercentX = maxPer;
    setRect();
    updateGeometry();
    emit MagnifyChanged(left(), right());
}


void MagnifyRect::updateGeometry()
{
    prepareGeometryChange();
    qreal x, y;
    x = rectCenterX * chart->plotArea().width() -
            chart->plotArea().width()*rectPercentX/(qreal)2 +
            chart->plotArea().x();
    y = chart->plotArea().y();
    setPos(QPointF(x, y));
}

void MagnifyRect::setPercent(qreal percentX)
{
    rectPercentX = percentX;
}

void MagnifyRect::setCenter(qreal posX)
{
    rectCenterX = (posX - chart->plotArea().x())/chart->plotArea().width();

    //以下代码防止画出的矩形越界！
    if(rectCenterX < rectPercentX/2)
        rectCenterX = rectPercentX/2;

    if(rectCenterX >= (1 - rectPercentX/2))
        rectCenterX = 1 - rectPercentX/2;

    qDebug()<<"center: "<< rectCenterX;
}





























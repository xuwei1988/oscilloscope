#include "messurecursor.h"

#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QChart>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QDebug>


MessureCursor::MessureCursor(QChart *parent, int id, qreal posX)
    : QObject(0),
      QGraphicsItem(parent)

{
    chart = parent;
    centerX = posX;
    this->id = id;

    QFont font;
    QFontMetrics metrics(font);
    textRect = metrics.boundingRect(
                QRect(0, 0, 15, 15), Qt::AlignLeft, QString::number(id,'f',0));
    textRect.translate(-15, 5);

    setCursor(Qt::PointingHandCursor);
    setFlags(ItemIsMovable | ItemIsSelectable);
}


QRectF MessureCursor::boundingRect() const
{
    qreal width, height;
    height = chart->plotArea().height();
    width = 30;

    return QRectF(-15, 0, width, height);
}

void MessureCursor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
//    Q_UNUSED(option)

    QPen pen(QColor(0xD8, 0xBF, 0xD8), 2, Qt::DashDotDotLine);
    if(option->state & QStyle::State_Selected){
        pen.setWidth(3);
    }


    QPointF p1, p2;
    p1.setX(0);
    p1.setY(1.5);

    p2.setX(0);
    p2.setY(chart->plotArea().height() - 0.5);

    painter->setPen(pen);
    painter->drawLine(p1, p2);
    painter->drawText(textRect, QString("%1 ").arg(id));

//    pen.setStyle(Qt::SolidLine);
//    painter->setPen(pen);
//    painter->setBrush(QBrush(Qt::NoBrush));
//    painter->drawEllipse(QPointF(-17, 28), 10, 10);
}


void MessureCursor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF move;
    move.setX(event->pos().x() - event->buttonDownPos(Qt::LeftButton).x());
    move.setY(0);
    qreal posX;
    posX = (QPointF(mapToParent(move)).x() -
            chart->plotArea().x()) / chart->plotArea().width();

    emit positionChanged(posX);
    setCenter(posX);
    updateGeometry();
    event->setAccepted(true);
}

void MessureCursor::updateGeometry()
{
    prepareGeometryChange();
    qreal x, y;
    x = centerX * chart->plotArea().width()
            + chart->plotArea().x();
    y = chart->plotArea().y();
    setPos(x, y);
}

void MessureCursor::setCenter(qreal posX)
{
    if(posX <= 0)
        posX = 0;
    if(posX >= 1)
        posX = 1;
    centerX = posX;
}



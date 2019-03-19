#include "magnifyview.h"
#include "datasource.h"
#include "mainview.h"
#include "messurecursor.h"
#include "callout.h"

#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QValueAxis>

#include <QDebug>

MagnifyView::MagnifyView(DataSource *pdata, MainView *mainview, QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      data(pdata),
      mainView(mainview),
      m_tooltip(0)
{

    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    chart = new QChart;
    chart->setMinimumSize(640, 280);

    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setTheme(QChart::ChartThemeDark);
    chart->legend()->hide();

    timediv = new QGraphicsSimpleTextItem(chart);
    timediv->setPen(QPen(Qt::lightGray));

    screenCycles = new QGraphicsSimpleTextItem(chart);
    screenCycles->setPen(QPen(Qt::yellow));

    screenCycles = new QGraphicsSimpleTextItem(chart);
    screenCycles->setPen(QPen(Qt::white));

    //series
    int cnt = data->getRows();
    seriesList.reserve(cnt);
    for(int i(0); i < cnt; i++){
        QLineSeries *series = new QLineSeries;
        seriesList.append(series);
        chart->addSeries(seriesList.at(i));

        series->setUseOpenGL(true);
        series->replace(data->getTransVec(i));
        series->setName(QString("line " + QString::number(seriesList.count())));

        connect(series, &QLineSeries::hovered, this, &MagnifyView::tooltip);
        connect(series, &QLineSeries::clicked, this, &MagnifyView::keepCallout);
    }

    chart->setAcceptHoverEvents(true);


    cursors<<new MessureCursor(chart, 1, 0.48)
          <<new MessureCursor(chart, 2, 0.52);
    for(int i(0); i < cursors.count(); i++){
        connect(cursors[i], &MessureCursor::positionChanged, this, &MagnifyView::cursorPosChanged);
        cursors[i]->setZValue(12);
        cursors[i]->updateGeometry();
        cursors[i]->hide();
    }



    setRenderHint(QPainter::Antialiasing);
    scene()->addItem(chart);
    hide();
    connect(mainView, &MainView::magnifyRectIsShow, this, &MagnifyView::setupMagnifyView);
    connect(mainView, &MainView::axisRangeChanged, this, &MagnifyView::updateAxis);
    connect(mainView, &MainView::timerStart, this, &MagnifyView::deleteCallouts);

    this->setMouseTracking(true);
    setMinimumSize(800, 300);
}

MagnifyView::~MagnifyView()
{
    qDebug()<<"~MagnifyView";
    delete chart;
    if(setUpInvokeCnt){
        delete axisX;
        delete axisY;
    }
    for(int i(0); i < seriesList.count(); i++){
        seriesRemove(i);
    }

    deleteCallouts();
}

void MagnifyView::setCursorPos(qreal pos1, qreal pos2)
{
    if(pos1 > 0 && pos1 < 1){
        cursors[0]->setCenter(pos1);
        cursors[0]->updateGeometry();
        cursors[0]->show();
    }else{
        cursors[0]->hide();
    }

    if(pos2 > 0 && pos2 < 1){
        cursors[1]->setCenter(pos2);
        cursors[1]->updateGeometry();
        cursors[1]->show();
    }else{
        cursors[1]->hide();
    }
}

void MagnifyView::cursorShow()
{
        cursors[0]->show();
        cursors[1]->show();
}

void MagnifyView::cursorHide()
{
    cursors[0]->hide();
    cursors[1]->hide();
}

void MagnifyView::deleteCallouts()
{
    m_tooltip = 0;
    while(m_callouts.count()){
        m_callouts.first()->hide();
        delete m_callouts.first();
        m_callouts.removeAt(0);
    }
}

void MagnifyView::resizeEvent(QResizeEvent *event)
{
    if(scene()){
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        chart->resize(event->size());
        QRectF rec = chart->plotArea();
        timediv->setPos(rec.x() + rec.width() - 120, rec.y() + 5);
        screenCycles->setPos(rec.x(), rec.y() - 20);

        if(cursors.count() && cursors.first()->isVisible()){
            for(int i(0); i < cursors.count(); i++){
                cursors[i]->updateGeometry();
            }
        }
        //随着程序的复杂，可能需要其他方面的扩展！
        //
        //
        //
    }
}


void MagnifyView::setupMagnifyView(bool isShow)
{
    qDebug()<<"setupMagnifyView";
    qDebug()<<"setUpInvokeCnt:"<<setUpInvokeCnt;

    qreal div = mainView->getTimeDiv() * mainView->getMagRectPercent();
    QString sdiv = QString::number(div, 'f', 2);
    timediv->setText(QString("[%1ms/div]").
                     arg(sdiv));
    QString scyc = QString::number(div * 10 / 20, 'f', 0);
    screenCycles->setText(QString("Number of Cycles:%1   ").
                          arg(scyc));
    if(setUpInvokeCnt++ == 0){
        QPair<qreal, qreal> pair;
        pair = mainView->getAxisRange(Qt::Horizontal);
        axisX = new QValueAxis;
        axisX->setLabelsVisible(false);
        axisX->setTickCount(11);
        axisX->setRange(pair.first, pair.second);

        pair = mainView->getAxisRange(Qt::Vertical);
        axisY = new QValueAxis;
        axisY->setLabelsVisible(false);
        axisY->setTickCount(11);
        axisY->setRange(pair.first, pair.second);

        QPen pen;
        pen.setStyle(Qt::DashDotLine);
        pen.setWidth(1);
        pen.setBrush(Qt::lightGray);
        axisX->setGridLinePen(pen);
        axisY->setGridLinePen(pen);

        for(int i(0); i < seriesList.count(); i++){
            chart->setAxisX(axisX, seriesList.at(i));
            chart->setAxisY(axisY, seriesList.at(i));
        }


    }
    setVisible(isShow);
}


void MagnifyView:: updateAxis()
{
    QPair<qreal, qreal> pair;

    pair = mainView->getAxisRange(Qt::Horizontal);
    axisX->setRange(pair.first, pair.second);

    pair = mainView->getAxisRange(Qt::Vertical);
    axisY->setRange(pair.first, pair.second);

    for(int i(0); i < data->getRows(); i++){
        seriesList.at(i)->replace(mainView->getChartVec(i));
    }

    qreal div = mainView->getTimeDiv() * mainView->getMagRectPercent();
    QString sdiv = QString::number(div, 'f', 2);
    timediv->setText(QString("[%1ms/div]").
                     arg(sdiv));

    QString scyc = QString::number(div * 10 / 20, 'f', 0);
    screenCycles->setText(QString("Number of Cycles:%1   ").
                          arg(scyc));
}


void MagnifyView::channelVisibleSet(int i)
{
    chart->series()[i]->setVisible(!chart->series()[i]->isVisible());
}

void MagnifyView::seriesRemove(int id)
{
    seriesList[id]->hide();
    delete seriesList[id];
    seriesList.removeAt(id);
}

void MagnifyView::cursorPosChanged(qreal pos)
{
    MessureCursor *cursor = qobject_cast<MessureCursor*> (sender());
    if(cursor == cursors.at(0)){
        emit cursorMove(0, pos);
    }

    if(cursor == cursors.at(1)){
        emit cursorMove(1, pos);
    }
}

void MagnifyView::keepCallout()
{
    m_callouts.append(m_tooltip);
    m_tooltip = new Callout(chart);
}

void MagnifyView::tooltip(QPointF point, bool state)
{
    QLineSeries *series = qobject_cast<QLineSeries *> (sender());
    QColor color = series->color();
    Trans tran;
    for(int i(0); i < seriesList.count(); i++){
        if(series == seriesList.at(i)){
            tran = data->getTrans(i);
        }
    }

    if(m_tooltip == 0)
        m_tooltip = new Callout(chart);

    if (state) {
        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x() - axisX->min()).
                           arg((point.y() - tran.second) / tran.first), color);
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(15);
        m_tooltip->updateGeometry();
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

























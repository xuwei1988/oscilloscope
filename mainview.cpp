#include "mainview.h"
#include "magnifyrect.h"
#include "messurecursor.h"
#include "datasource.h"

#include "dataprocessor.h"
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGraphicsScene>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtGui/QMouseEvent>
#include <QValueAxis>
#include <QTimer>
#include <QDebug>

#include <QColor>

MainView::MainView(DataSource *pdata, QList<QPair<bool, QString>> &measures, qreal zm, QWidget *parent)
    : QGraphicsView(new QGraphicsScene, parent),
      data(pdata),
      magnifyRect(0),
      zoom(zm)

{
    setDragMode(QGraphicsView::NoDrag);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //chart
    chart = new QChart();
    chart->setMinimumSize(640, 280);
    chart->legend()->setAlignment(Qt::AlignTop);
    chart->setTheme(QChart::ChartThemeDark);

    //axis
    axisX = new QValueAxis;
    axisX->setRange(data->getAreaXRange(0).first, data->getAreaXRange(0).second);
    axisX->setLabelFormat("%g");
    axisX->setLabelsVisible(false);
    axisX->setTickCount(11);
    connect(axisX, &QValueAxis::rangeChanged, this, &MainView::axisXRangeChanged);

    axisY = new QValueAxis;
    axisY->setRange(-1.0, 1.0);
    axisY->setLabelFormat("%g");
    axisY->setLabelsVisible(false);
    axisY->setTickCount(11);

    timediv = new QGraphicsSimpleTextItem(chart);
    timediv->setPen(QPen(Qt::lightGray));
    timediv->setText(QString("[%1ms/div]").
                     arg((qreal)data->getCycles() * qreal(20)/(qreal)10));

    sampleInterval = new QGraphicsSimpleTextItem(chart);
    sampleInterval->setPen(QPen(Qt::yellow));
    sampleInterval->setText(QString("Sampling Interval:%1us").arg(qreal(20 * 1000) / qreal(data->getCyclePoints())));

    cursorsInterval = new QGraphicsSimpleTextItem(chart);
    cursorsInterval->setPen(QPen(Qt::white));
    cursorsInterval->hide();


    QPen pen;
    pen.setStyle(Qt::DashDotLine);
    pen.setWidth(1);
    pen.setBrush(Qt::lightGray);
    axisX->setGridLinePen(pen);
    axisY->setGridLinePen(pen);

    //series
    int cnt = data->getRows();
    chnImforList imforList = data->getSelectedChannelsImfor();
    seriesList.reserve(cnt);
    for(int i(0); i < cnt; i++){
        QLineSeries *series = new QLineSeries;
        seriesList.append(series);
        chart->addSeries(seriesList.at(i));

        series->setUseOpenGL(true);
        series->replace(data->getTransVec(i));

        QString name;
        name = imforList.at(i).name;
        qreal div = data->getTrans(i).first;
        name += QString("%1 V/div   ").arg((qreal)0.2/div);
        series->setName(name);

        chart->setAxisX(axisX, series);
        chart->setAxisY(axisY, series);
    }
    connectMarkers();

    QList<MeasureText> measureTexts;
    for(int j(0); j < seriesList.count(); j++){
        for(int i(0); i < measures.count(); i++){
            if(measures.at(i).first){
                QGraphicsSimpleTextItem *item = new QGraphicsSimpleTextItem(chart);
                item->setPen(QPen(Qt::white));
                item->setText(QString("%1:").arg(measures.at(i).second));
                item->hide();
                measureTexts<<MeasureText(i, item);
            }
        }

        measureTextsList<<measureTexts;
        measureTexts.clear();
    }


    setRenderHint(QPainter::Antialiasing);
    scene()->addItem(chart);

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerJob()));

    chart->setAcceptHoverEvents(true);
    connect(seriesList.at(0), &QLineSeries::hovered, this, &MainView::showPoint);
    timer->start(30);
}

MainView::~MainView()
{
    qDebug()<<"~MainView";
    for(int i(0); i < seriesList.count(); i++){
        seriesRemove(i);
    }

    for(int i(0); i < cursors.count(); i++){
        delete cursors[i];
        cursors.removeAt(i);
    }

    delete chart;
    delete axisX;
    delete axisY;

}

void MainView::setLegend()
{
    QLegend *legend = chart->legend();
    legend->detachFromChart();
    chart->legend()->setBackgroundVisible(true);
    chart->legend()->setBrush(QBrush(QColor(128, 128, 128, 128)));
    chart->legend()->setPen(QPen(QColor(192, 192, 192, 192)));

    QRectF rect = chart->plotArea();

    qDebug()<<"setLegend: "<<rect;
    chart->legend()->setGeometry(QRectF(rect.x() + 60,
                                 rect.y() + 10,
                                 350,
                                 50));
    qDebug()<<"chart->legend: "<<chart->legend()->geometry();

    chart->legend()->update();
}

void MainView::timerJob()
{
    if(isVisible()){
        qreal minX = data->getAreaXRange((1 - zoom)).first;
        qreal maxX = data->getAreaXRange((1 - zoom)).second;
        axisX->setRange(minX, maxX);
        for(int i(0); i < data->getRows(); i++){
            seriesList.at(i)->replace(data->getTransVec(i));
        }
        if(magnifyRect && magnifyRect->isVisible())
            emit axisRangeChanged();
    }
}

void MainView::showPoint(QPointF point, bool state)
{
    if(state)
        qDebug()<<"Show Point"<<point;
}

/* 2018.01.11 *************************************************************/
void MainView::magnifyRectChanged(qreal left, qreal right)
{
    Q_UNUSED(left)
    Q_UNUSED(right)
    qDebug()<<"magnifyRectChanged";
    if(magnifyRect && magnifyRect->isVisible()){
        emit axisRangeChanged();
    }

    if(!cursors.isEmpty() && cursors.at(0)->isVisible()){
        qreal p1, p2;
        p1 = (cursors.at(0)->center() - left) / (right - left);
        p2 = (cursors.at(1)->center() - left) / (right - left);
        emit magnifyViewCursorPos(p1, p2);
    }
}

void MainView::cursorPosChanged(qreal pos)
{
    MessureCursor *cursor = qobject_cast<MessureCursor*> (sender());
    qreal p1, p2;
    if(cursor == cursors.at(0) && magnifyRect){
        qDebug()<<"1:"<<pos;
        p1 = (pos - magnifyRect->left()) / (magnifyRect->right() - magnifyRect->left());
        p2 = (cursors.at(1)->center() - magnifyRect->left()) / (magnifyRect->right() - magnifyRect->left());

    }
    if(cursor == cursors.at(1) && magnifyRect){
        qDebug()<<"2:"<<pos;
        p1 = (cursors.at(0)->center() - magnifyRect->left()) / (magnifyRect->right() - magnifyRect->left());
        p2 = (pos - magnifyRect->left()) / (magnifyRect->right() - magnifyRect->left());
    }
    emit magnifyViewCursorPos(p1, p2);

    setCursorInterval();
}

void MainView::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<"keyPress";
    static int kill = 0;
    switch (event->key()) {
    case Qt::Key_Space:{
        kill++;

        if(kill%2 == 1){
            qDebug()<<"Stop";
            timer->stop();                        
            hideNshowAllMeasure(true);
            measureCursorsValues();
        }else{
            qDebug()<<"Start";
            timer->start(25);
            hideNshowAllMeasure(false);
            emit timerStart();
        }
    }

    }
}

void MainView::resizeEvent(QResizeEvent *event)
{
    if(scene()){
        scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        chart->resize(event->size());

        QRectF rec = chart->plotArea();
        timediv->setPos(rec.x() + rec.width() - 120, rec.y() + 5);
        sampleInterval->setPos(rec.x() + rec.width() - 200, rec.y() - 20);
        cursorsInterval->setPos(rec.x(), rec.y() - 20);

        replaceMeasureText();


        if(magnifyRect && magnifyRect->isVisible()){
            magnifyRect->setRect();
            magnifyRect->updateGeometry();
        }

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


/* 利用2分法进行视野缩放 ----------------------------------------------------------------*/
void MainView::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numTicks =numDegrees / 15;
    qDebug()<<numTicks;
    if(event->modifiers() == Qt::AltModifier){
        int n = data->getColumns();
        qreal zoomlist = data->getCyclePoints()/(qreal)n;
        if(numTicks > 0){
            zoom /= (qreal)(abs(numTicks) * 2);
        }
        if(numTicks < 0){
            zoom *= (qreal)(abs(numTicks) * 2);
        }

        if(zoom < zoomlist)
            zoom = zoomlist;
        if(zoom > 1)
            zoom = 1;

        if(!timer->isActive()){
            qreal minX = axisX->min();
//            if(numTicks < 0 && zoom < 1)
//                minX = axisX->min() + numTicks * (axisX->max() - axisX->min());
//            else if(numTicks > 0 && zoom > zoomlist)
//                minX = axisX->min() + (axisX->max() - axisX->min())/2;
//            axisX->setMin(minX);
            if(numTicks < 0)
               minX = axisX->min() + numTicks * (axisX->max() - axisX->min());
            else
               minX = axisX->min() + (axisX->max() - axisX->min())/2;
            axisX->setMin(minX);
        }

        timediv->setText(QString("[%1ms/div]").
                         arg((qreal)data->getCycles() * qreal(20) * zoom/(qreal)10));
        if(magnifyRect && magnifyRect->isVisible())
            emit axisRangeChanged();
        qDebug()<<"zoom: "<<zoom;
    }else if(event->modifiers() == Qt::ControlModifier){
        qDebug()<<"ControlModifier";
        Trans newTran;
        qreal a(0);
        if(numTicks > 0){
            a = (qreal)(abs(numTicks) * 2);
        }else{
            a = (qreal)1 / (qreal)(abs(numTicks) * 2);
        }
        emit totalYRangeChanged(a);
        for(int i(0); i < data->getRows(); i++){
            newTran = data->getTrans(i);
            newTran.first *= a;
            data->setTrans(i, newTran);
            setLegendShow(i, newTran);

            refreshSeriesData(i, 0, a);
        }
    }
//    qDebug()<<"QChart:"<<chart->plotArea();
//    qDebug()<<"chart->geometry()"<<chart->geometry();
//    qDebug()<<"chart->size:"<<chart->size();
//    qDebug()<<"Scene:"<<scene()->sceneRect();
    QGraphicsView::wheelEvent(event);

}


void MainView::mouseDoubleClickEvent(QMouseEvent *event)
{
//    QVector<QPointF> vec = seriesList[0]->pointsVector();

    if(event->button() == Qt::LeftButton){
        if(magnifyRect == 0){
            magnifyRect = new MagnifyRect(chart);
            magnifyRect->setPercent(0.1);
            magnifyRect->setCenter((qreal)event->pos().x());
            magnifyRect->setRect();
            magnifyRect->setZValue(11);
            magnifyRect->updateGeometry();
            magnifyRect->show();
            connect(magnifyRect, &MagnifyRect::MagnifyChanged, this, &MainView::magnifyRectChanged);

            emit magnifyRectIsShow(true);
        }else{
            magnifyRect->setPercent(0.1);
            magnifyRect->setCenter((qreal)event->pos().x());
            magnifyRect->setRect();
            magnifyRect->setZValue(11);
            magnifyRect->updateGeometry();
            magnifyRect->show();
            emit magnifyRectIsShow(true);
        }
        emit magnifyRect->MagnifyChanged(magnifyRect->left(), magnifyRect->right());
    }else{
        if(magnifyRect){
            magnifyRect->hide();
            emit magnifyRectIsShow(false);
        }
    }
}

QPair<qreal, qreal> MainView::getAxisRange(Qt::Orientation x)
{
    if(x == Qt::Horizontal){
        qreal min = axisX->min() +
                (axisX->max() - axisX->min()) * magnifyRect->getArea().first;

        qreal max = min + (axisX->max() - axisX->min()) *
                magnifyRect->getArea().second;

        return QPair<qreal, qreal>(min, max);
    }else{
        return QPair<qreal, qreal>(axisY->min(), axisY->max());
    }
}

QVector<QPointF> MainView::getChartVec(int i)
{
    return seriesList[i]->pointsVector();
}


QColor MainView::channelColor(int channel) const
{
    return seriesList.at(channel)->color();
}

#include <QtCharts/QLegendMarker>
void MainView::connectMarkers()
{
    const auto markers = chart->legend()->markers();
    for(QLegendMarker *marker : markers){
        QObject::disconnect(marker, &QLegendMarker::clicked,
                            this, &MainView::handleMarkerClicked);
        QObject::connect(marker, &QLegendMarker::clicked, this, &MainView::handleMarkerClicked);
    }
}

void MainView::handleMarkerClicked()
{
    qDebug()<<"handleMarkerClicked";

    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(marker);

    for(int i(0); i < seriesList.count(); i++){
        if(seriesList.at(i) == marker->series()){
            qDebug()<<"Series: "<<i;
            emit channelSelect(i);
            hideNshowMeasure(i, !marker->series()->isVisible());

            break;
        }
    }

    marker->series()->setVisible(!marker->series()->isVisible());
    marker->setVisible(true);
    qreal alpha = 1.0;

    if (!marker->series()->isVisible())
        alpha = 0.5;

    QColor color;
    QBrush brush = marker->labelBrush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setLabelBrush(brush);

    brush = marker->brush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setBrush(brush);

    QPen pen = marker->pen();
    color = pen.color();
    color.setAlphaF(alpha);
    pen.setColor(color);
    marker->setPen(pen);


}


void MainView::setLegendShow(int id, QPair<qreal, qreal>trans)
{
    qDebug()<<"setLegendShow: "<<id;

    QString name = data->getSelectedChannelsImfor().at(id).name;
    name += QString("%1 V/div   ").arg((qreal)0.2/trans.first);
    seriesList[id]->setName(name);
}


void MainView::seriesRemove(int id)
{


    seriesList[id]->hide();
    delete seriesList[id];
    seriesList.removeAt(id);

    QMutableListIterator<MeasureText> i(measureTextsList[id]);
    while(i.hasNext()){
        delete i.next().second;
    }
    measureTextsList.removeAt(id);

    replaceMeasureText();
}

void MainView::refreshSeriesData(int id, qreal deltaPos, qreal deltaScale)
{
    if(timer->isActive() == false) {
        QVector<QPointF > vec = seriesList.at(id)->pointsVector();
        if(deltaScale == 1){
            for(int i(0); i < vec.count(); i++){
                vec[i].setY(vec[i].y() + deltaPos);
    //            vec[i].setY((vec[i].y()  - + deltaPos) * deltaScale);
            }
        }else{
            for(int i(0); i < vec.count(); i++){
                vec[i].setY((vec[i].y()  - data->getTrans(id).second) * deltaScale + data->getTrans(id).second);
            }
        }
        seriesList.at(id)->replace(vec);
    }

    if(magnifyRect && magnifyRect->isVisible())
        emit axisRangeChanged();
}


qreal MainView::getTimeDiv() const
{
    int i = timediv->text().indexOf("ms");
    return timediv->text().mid(1, i - 1).toDouble();
}

qreal MainView::getMagRectPercent() const
{
    return magnifyRect->percent();
}

qreal MainView::getMagRectLeft() const
{
    return magnifyRect->left();
}


bool MainView::setupMessureCursor()
{
    bool r;
    if(!cursors.isEmpty()){
        if(cursors.first()->isVisible()){
            for(int i(0); i < cursors.count(); i++){
                cursors[i]->setVisible(false);
            }
            cursorsInterval->hide();
            hideNshowAllMeasure(false);
            r = false;
        }else{
            for(int i(0); i < cursors.count(); i++){
                cursors[i]->setVisible(true);
            }
            cursorsInterval->show();
            if(!timer->isActive())
                hideNshowAllMeasure(true);
            r = true;
        }
    }

    if(cursors.isEmpty()){
        cursors<<new MessureCursor(chart, 1, 0.48)
              <<new MessureCursor(chart, 2, 0.52);
        for(int i(0); i < cursors.count(); i++){
            connect(cursors[i], &MessureCursor::positionChanged, this, &MainView::cursorPosChanged);
            cursors[i]->setZValue(12);
            cursors[i]->updateGeometry();
            cursors[i]->show();
        }
        emit cursors[0]->positionChanged(0.48);
        emit cursors[1]->positionChanged(0.52);

        setCursorInterval();
        cursorsInterval->show();
        if(!timer->isActive())
            hideNshowAllMeasure(true);
        r = true;
    }

    return r;
}

void MainView::setCursorPos(int id, qreal pos)
{
    if(pos >= qreal(0) && pos <= qreal(1)){
        qDebug()<<"True";
        cursors[id]->setCenter(pos);
        cursors[id]->updateGeometry();
    }
}

qreal MainView::getCursorPos(int id)
{
    return cursors.at(id)->center();
}

QPair<qreal, qreal> MainView::zoomCursorPos() const
{
    QPair<qreal, qreal> pair(0, 0);
    if(magnifyRect){

        pair.first = (cursors.at(0)->center() - magnifyRect->left()) / (magnifyRect->right() - magnifyRect->left());
        pair.second = (cursors.at(1)->center() - magnifyRect->left()) / (magnifyRect->right() - magnifyRect->left());
    }
    return pair;
}


void MainView::axisXRangeChanged(qreal min, qreal max)
{
    qDebug()<<"Min:"<<min<<"Max:"<<max;
    setCursorInterval();
}


void MainView::setCursorInterval()
{
    if(!cursors.isEmpty()){
        qreal percent = cursors.at(1)->center() - cursors.at(0)->center();
        cursorsInterval->setText(QString("Cursor Interval:%1").
                                arg(QString::number((axisX->max() - axisX->min()) * percent, 'f', 0)));

        if(!timer->isActive()){
            measureCursorsValues();
        }
    }
}


void MainView::replaceMeasureText()
{
    QRectF rec = chart->plotArea();
    qreal deltaX = rec.width()/qreal(10);
    for(int k(0); k < measureTextsList.count(); k++){
        int cnt = 0;
        QListIterator<MeasureText> i(measureTextsList[k]);
        i.toBack();
        while(i.hasPrevious()){
            cnt++;
            QGraphicsSimpleTextItem *item = i.previous().second;
            qreal xbase = rec.x() + 5;
            qreal ybase = rec.y() + rec.height() - 20 * cnt;

            item->setPen(QPen(seriesList.at(k)->color()));
            item->setPos(xbase + deltaX * k, ybase);
        }
    }
}

void MainView::hideNshowMeasure(int column, bool isShow)
{
    QMutableListIterator<MeasureText> i(measureTextsList[column]);
    while(i.hasNext()){
        i.next().second->setVisible(isShow);
    }
}

void MainView::hideNshowAllMeasure(bool isShow)
{
    if(cursors.isEmpty())
        return;
    else if(cursors.first()->isVisible() == false)
        isShow = false;

    for(int j(0); j < measureTextsList.count(); j++){
        QMutableListIterator<MeasureText> i(measureTextsList[j]);
        while(i.hasNext()){
            i.next().second->setVisible(isShow);
        }
    }
}

void MainView::measureOp(int id, bool isSet)
{
    qDebug()<<"Measure Op:"<<id<<":"<<isSet;

    if(isSet == true){
        MeasureText measureText;
        measureText.first = id;

        for(int i(0); i < measureTextsList.count(); i++){
            measureText.second = new QGraphicsSimpleTextItem(chart);
            QList<MeasureText>::iterator iter = measureTextsList[i].begin();
            while(iter != measureTextsList[i].end()){
                if(iter->first > id){
                    measureTextsList[i].insert(iter, measureText);
                    break;
                }
                iter++;
            }

            if(iter == measureTextsList[i].end()){
                qDebug()<<"end"<<i;
                measureTextsList[i].insert(iter, measureText);
            }
            measureCursorsValues();
        }
    }else{
        for(int i(0); i < measureTextsList.count(); i++){
            QMutableListIterator<MeasureText> iter(measureTextsList[i]);
            while(iter.hasNext()){
                MeasureText &mt = iter.next();
                if(mt.first == id){
                    delete mt.second;
                    iter.remove();
                    break;
                }
            }
        }
    }

    replaceMeasureText();

}

void MainView::measureCursorsValues()
{
    if(cursors.isEmpty())
        return;
    qDebug()<<"measureCursorsValues";
    qreal percent = cursors.at(1)->center() - cursors.at(0)->center();
    qreal xrange = axisX->max() - axisX->min();
    int first, second;
    if(percent >= 0){
        first = 0;
        second = 1;
    }else{
        first = 1;
        second = 0;
    }

    int start, cnt;
    cnt = cursors.at(second)->center() * xrange - cursors.at(first)->center() * xrange;
    start = cursors.at(first)->center() * xrange + axisX->min();

    qDebug() << cursors.at(first)->center();
    qDebug() << cursors.at(second)->center();

    qDebug() << "xrange" << xrange;

    qDebug() << cursors.at(first)->center() * xrange;
    qDebug() << cursors.at(second)->center() * xrange;

    if(!cnt)
        return;

    for(int i(0); i < seriesList.count(); i++) {
        Trans tran = data->getTrans(i);
        QVector<QPointF> vec = seriesList[i]->pointsVector().mid(start, cnt);
//        qDebug()<<"calc:"<<vec;
//        qDebug()<<"tran"<<tran;

        dataProcessor.calc(vec, tran);
        QMutableListIterator<MeasureText> iter(measureTextsList[i]);
        while(iter.hasNext()){
            MeasureText &mt = iter.next();
            switch(mt.first){
                case MaxAction_Pos:{
                    mt.second->setText(QString("Max:%1").arg(dataProcessor.maxPoint.y()));
                    break;
                }
                case MinAction_Pos:{
                    mt.second->setText(QString("Min:%1").arg(dataProcessor.minPoint.y()));
                    break;
                }
                case RmsAction_Pos:{
                    mt.second->setText(QString("Rms:%1").arg(dataProcessor.rms));
                    break;
                }

                case AvgAction_Pos:{
                    mt.second->setText(QString("Avg:%1").arg(dataProcessor.avg));
                    break;
                }
                case FreqAction_Pos:{
                    mt.second->setText(QString("Freq"));
                    break;
                }
            }

        }
        dataProcessor.clear();
    }
}

















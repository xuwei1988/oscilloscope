#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QtWidgets/QGraphicsView>
#include <QtCharts/QChartGlobal>

//typedef QPair<int, QGraphicsSimpleTextItem *> MeasureText;

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QMouseEvent;
class QResizeEvent;
class QWheelEvent;
class QKeyEvent;
class QTimer;
class QColor;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class DataSource;
class MagnifyRect;
class MessureCursor;


class MainView : public QGraphicsView
{
    Q_OBJECT

    enum {
        MaxAction_Pos = 0,
        MinAction_Pos = 1,
        RmsAction_Pos = 2,
        AvgAction_Pos = 3,
        FreqAction_Pos = 4
    };


public:
    typedef QPair<int, QGraphicsSimpleTextItem *> MeasureText;

    MainView(DataSource *pdata, QList<QPair<bool, QString>> &measures, qreal zm = 1, QWidget *parent = 0);
    ~MainView();
    qreal getZoom() {return zoom; }
    QPair<qreal, qreal> getAxisRange(Qt::Orientation x);
    QVector<QPointF> getChartVec(int i);
    QColor channelColor(int channel) const;

    void setLegendShow(int id, QPair<qreal, qreal>trans);
    void refreshSeriesData(int id, qreal deltaPos, qreal deltaScale);

    qreal getTimeDiv() const;
    qreal getMagRectPercent() const;
    qreal getMagRectLeft() const;

    bool setupMessureCursor();
    void setCursorPos(int id, qreal pos);
    qreal getCursorPos(int id);

    QPair<qreal, qreal> zoomCursorPos() const;
    void setCursorInterval();

public slots:

    void seriesRemove(int id);
    void measureOp(int id, bool isSet);


protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void magnifyRectIsShow(bool isShow);
    void axisRangeChanged();
    void channelSelect(int channel);
    void totalYRangeChanged(qreal multiple);
    void magnifyViewCursorPos(qreal pos1, qreal pos2);

    void timerStart();

private slots:
    void timerJob();
    void magnifyRectChanged(qreal left, qreal right);
    void cursorPosChanged(qreal pos);

    void showPoint(QPointF point, bool state);
    void handleMarkerClicked();

    void axisXRangeChanged(qreal min, qreal max);

private:
    void connectMarkers();
    void setLegend();
    void replaceMeasureText();
    void hideNshowMeasure(int column, bool isShow);
    void hideNshowAllMeasure(bool isShow);

    void measureCursorsValues();

    /* 2018.01.18简单数学算法，以后可能会有一个专门的算法类来处理
     * 当前为了简单先这么做
     */
//    void calculate(QVector<QPointF > &points, QList<QPair<int, qreal> > &values) const;



    QChart *chart;
    DataSource *data;
    MagnifyRect *magnifyRect;

    QList<MessureCursor *> cursors;

    QList<QLineSeries *> seriesList;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QGraphicsSimpleTextItem *timediv;
    QGraphicsSimpleTextItem *sampleInterval;
    QGraphicsSimpleTextItem *cursorsInterval;


    QList<QList<MeasureText> > measureTextsList;

    qreal zoom;
    int timerID;

    QTimer *timer;
};

#endif // MAINVIEW_H



























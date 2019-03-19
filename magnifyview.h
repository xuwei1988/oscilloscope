#ifndef MAGNIFYVIEW_H
#define MAGNIFYVIEW_H

#include <QtWidgets/QGraphicsView>
#include <QtCharts/QChartGlobal>

QT_BEGIN_NAMESPACE
class QResizeEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class DataSource;
class MainView;
class MessureCursor;
class Callout;


class MagnifyView : public QGraphicsView
{
    Q_OBJECT
public:
    MagnifyView(DataSource *pdata, MainView *mainview, QWidget *parent = 0);
    ~MagnifyView();

    void setCursorPos(qreal pos1, qreal pos2);
    void cursorShow();
    void cursorHide();

public slots:
    void setupMagnifyView(bool isShow);
    void updateAxis();
    void channelVisibleSet(int i);
    void seriesRemove(int id);

private slots:
    void cursorPosChanged(qreal pos);
    void keepCallout();
    void tooltip(QPointF point, bool state);

    void deleteCallouts();

signals:
    void cursorMove(int id, qreal pos);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    DataSource *data;
    MainView *mainView;             //传递mainView指针用来获取其内部信息
    int setUpInvokeCnt = 0;

    QChart *chart;
    Callout *m_tooltip;
    QList<Callout *> m_callouts;

    QList<MessureCursor *> cursors;
    QList<QLineSeries *> seriesList;
    QValueAxis *axisX;
    QValueAxis *axisY;

    QGraphicsSimpleTextItem *timediv;
    QGraphicsSimpleTextItem *screenCycles;
};

#endif // MAGNIFYVIEW_H



























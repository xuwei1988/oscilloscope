#include "eventtable.h"
#include "customtablemodel.h"
#include "recorddata.h"
#include "scopeimfor.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTableView>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QVXYModelMapper>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QAction>
#include <QIcon>
#include <QMenu>


QT_CHARTS_USE_NAMESPACE


EventTable::EventTable(RecordData *recordData, QWidget *parent)
    :QDialog(parent)
{
    m_recordData = recordData;

    setAttribute(Qt::WA_DeleteOnClose);
    CustomTableModel *model = new CustomTableModel(recordData);

    QTableView *tableView = new QTableView;
    tableView->setModel(model);

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setColumnHidden(0, true);

    QChart *chart = new QChart;
    chart->setAnimationOptions(QChart::AllAnimations);

    for(int i(0); i < m_recordData->channelNum(); i++){
        QLineSeries *series = new QLineSeries;
        int id = m_recordData->channelID(i);
//        series->setName(scopeImfor.channelList.at(id));
        series->setName(m_recordData->channelList.at(id));

        QVXYModelMapper *mapper = new QVXYModelMapper(this);
        mapper->setXColumn(0);
        mapper->setYColumn(i + 1);
        mapper->setSeries(series);
        mapper->setModel(model);
        chart->addSeries(series);
        QString seriesColorHex = "#000000";
        seriesColorHex = "#" + QString::number(series->pen().color().rgb(), 16).right(6).toUpper();
        model->addMapping(seriesColorHex, QRect(i + 1, 0, 1, model->rowCount()));
    }

    chart->createDefaultAxes();
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(640, 480);

    tableView->horizontalHeader()->
            setStyleSheet("QHeaderView::section {background-color:lightblue;color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");
    tableView->verticalHeader()->
            setStyleSheet("QHeaderView::section {background-color:lightblue;color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(tableView, 1, 0);
    mainLayout->addWidget(chartView, 1, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(0, 0);

    setWindowTitle(m_recordData->imformation());
    setWindowIcon(QIcon(":/images/event_icon.png"));
    setLayout(mainLayout);
}


#include <QDebug>
EventList::EventList(QWidget *parent)
    :QListWidget(parent)
{
    saveAction = new QAction(QIcon(":/images/save_current.png"),
                             tr("保存"), this);

    delAllAction = new QAction(QIcon(":/images/delete_all.png"),
                               tr("删除全部"), this);
    delAction = new QAction(QIcon(":/images/delete_current.png"),
                            tr("删除"), this);

    openAction = new QAction(QIcon(":/images/open_file.png"),
                             tr("打开"), this);


    connect(saveAction, SIGNAL(triggered()), this, SLOT(actionTrigered()));
    connect(delAction, SIGNAL(triggered()), this, SLOT(actionTrigered()));
    connect(delAllAction, SIGNAL(triggered()), this, SLOT(actionTrigered()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(actionTrigered()));

}

void EventList::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    QMenu *menu = new QMenu(this);
    menu->addAction(openAction);
    menu->addAction(saveAction);
    menu->addAction(delAllAction);
    menu->addAction(delAction);

    menu->exec(cursor().pos());
}

void EventList::actionTrigered()
{
    QAction *action = qobject_cast<QAction*> (sender());
    if(action == delAction) {
        int row = currentRow();
        qDebug()<<"contextMenuEvent"<<currentRow();
        QListWidgetItem *item = takeItem(row);
        delete item;
        emit actionDone(deAction,row);
    }

    if(action == delAllAction) {
       QListWidgetItem *item;
       int n = count();
       for(int i(0); i < n; i++) {
           item = takeItem(0);
           delete item;
       }
       emit actionDone(deAllAction, 0);
    }

    if(action == saveAction) {
        qDebug()<<"saveAction";
        int row = currentRow();
        emit actionDone(svAction, row);
    }

    if(action == openAction) {
        emit actionDone(opAction, 0);
    }

}














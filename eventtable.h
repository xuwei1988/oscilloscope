#ifndef EVENTTABLE_H
#define EVENTTABLE_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
class RecordData;
QT_USE_NAMESPACE
class QContextMenuEvent;
class QAction;
QT_END_NAMESPACE

class EventTable : public QDialog
{
    Q_OBJECT

public:
    EventTable(RecordData *recordData, QWidget *parent = 0);

private:
    RecordData *m_recordData;
};


#include <QtWidgets/QListWidget>
typedef enum {
    svAction = 0,
    svAllAction,
    deAction,
    deAllAction,
    opAction
}ActionType;

class EventList : public QListWidget
{
    Q_OBJECT

public:
    EventList(QWidget *parent = 0);

protected slots:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void actionTrigered();

signals:
    void actionDone(ActionType action, int pos);

private:
    QAction *delAction;
    QAction *delAllAction;
    QAction *saveAction;
    QAction *openAction;
};



#endif // EVENTTABLE_H

#ifndef EVENTSELECTDIALOG_H
#define EVENTSELECTDIALOG_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QItemDelegate>
#include "scopeimfor.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QListWidget;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QLineEdit;
class QResizeEvent;
class QTableWidgetItem;
QT_END_NAMESPACE

class ConnectDialog;

class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    ChannelWidget(QStringList &channelNames, uint32_t selectedChannels, QWidget *parent = 0);
    QString comboText() const;
    QVector<bool> checkList() const;


private slots:
//    void onCurrentIndexChanged(int);
    void stateChanged(int);
    void textChanged(const QString &);

private:
    QComboBox *comboBox;
    QListWidget *listWidget;
    QLineEdit *pLineEdit;
    QStringList channelNames;
    uint32_t selectedChannels;

    bool bSelected = false;
    QString strSelectedText;
};


class EventTableDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    enum {
        nameColumn = 0,
        levelColumn,
        cycBfColumn,
        cycAfColumn,
        chnColumn,
        setColumn
    };
    EventTableDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
//    void setEditorData(QWidget *editor, const QModelIndex &index) const;
//    void setModelData(QWidget *editor, QAbstractItemModel *model,
//                      const QModelIndex &index) const;
};

class EventSelectDialog : public QDialog
{
    Q_OBJECT

public:
    EventSelectDialog(ConnectDialog *connectDialog, QWidget *parent = 0);

protected slots:
    void resizeEvent(QResizeEvent *event);


private slots:
    void buttonClicked(void);

private:
    QList<QPushButton *> m_buttonlist;
    QTableWidget *m_tableWidget;
    ConnectDialog *connectDialog;
};

#endif // EVENTSELECTDIALOG_H


















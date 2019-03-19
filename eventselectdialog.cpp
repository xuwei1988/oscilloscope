#include "eventselectdialog.h"
#include "connectdialog.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QDebug>

#include <QtWidgets/QSpinBox>
#include <QPixmap>
#include <QApplication>

#include <QtWidgets/QTableWidget>
#include <QIcon>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>


ChannelWidget::ChannelWidget(QStringList &channelNames, uint32_t selectedChannels, QWidget *parent)
    : QWidget(parent)
{
    this->channelNames = channelNames;
    this->selectedChannels = selectedChannels;
    comboBox = new QComboBox(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(comboBox);
    setLayout(layout);

    listWidget = new QListWidget;
    pLineEdit = new QLineEdit(this);
    QString text;
    for(int i(0); i < channelNames.count(); i++){
        QListWidgetItem *pItem = new QListWidgetItem(listWidget);
        listWidget->addItem(pItem);
        pItem->setData(Qt::UserRole, i);
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(channelNames.at(i));

        if(selectedChannels & (1<<i)) {
            pCheckBox->setCheckState(Qt::Checked);
            text.append(channelNames.at(i) + ";");
        }else{
            pCheckBox->setCheckState(Qt::Unchecked);
        }



        listWidget->setItemWidget(pItem, pCheckBox);
        connect(pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    }

    comboBox->setModel(listWidget->model());
    comboBox->setView(listWidget);
    comboBox->setLineEdit(pLineEdit);
    pLineEdit->setText(text);
    pLineEdit->setReadOnly(true);
    connect(pLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged(const QString &)));
}

void ChannelWidget::stateChanged(int state)
{
    Q_UNUSED(state);
    bSelected = true;
    QString strSelectedData("");
    strSelectedText.clear();
    QObject *object = QObject::sender();
    QCheckBox *pSenderCheckBox = static_cast<QCheckBox *>(object);

    for(int i(0); i < listWidget->count(); i++){
        QListWidgetItem *pItem = listWidget->item(i);
        QWidget *pWidget = listWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if(pCheckBox->isChecked()) {
            QString strText = pCheckBox->text();
            strSelectedData.append(strText).append(";");
        }

        if(pSenderCheckBox == pCheckBox) {
            int nData = pItem->data(Qt::UserRole).toInt();
            qDebug() << QString("I am sender...id : %1").arg(nData);
        }
    }

    if (strSelectedData.endsWith(";"))
        strSelectedData.remove(strSelectedData.count() - 1, 1);

    if (!strSelectedData.isEmpty()){
        //ui.comboBox->setEditText(strSelectedData);
        strSelectedText = strSelectedData;
        pLineEdit->setText(strSelectedData);
        pLineEdit->setToolTip(strSelectedData);
    }else{
        pLineEdit->clear();
    }

    bSelected = false;
}

void ChannelWidget::textChanged(const QString &str)
{
    Q_UNUSED(str);
    if (!bSelected)
            pLineEdit->setText(strSelectedText);
}

QString ChannelWidget::comboText() const
{
    return comboBox->lineEdit()->text();
}


QVector<bool> ChannelWidget::checkList() const
{
    QVector<bool> vec;
    for(int i(0); i < listWidget->count(); i++) {
        QListWidgetItem *pItem = listWidget->item(i);
        QWidget *pWidget = listWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        vec<<pCheckBox->isChecked();
    }
    return vec;
}



/**********************************************************************************************/
extern QStringList strList;
EventTableDelegate::EventTableDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}


void EventTableDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    if(index.column() == levelColumn) {
        int level = index.model()->data(index, Qt::DisplayRole).toInt();
        QPixmap pixmap = QPixmap(strList.at(level)).scaled(30, 30);
        qApp->style()->drawItemPixmap(painter, option.rect,  Qt::AlignCenter, QPixmap(pixmap));
    }else  if(index.column() == cycBfColumn || index.column() == cycAfColumn) {
        int cyc = index.model()->data(index, Qt::DisplayRole).toInt();
        QString text = QString("%1").arg(cyc);
        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);

    }else{
        QItemDelegate::paint(painter, option, index);
    }
}

QWidget *EventTableDelegate::createEditor(QWidget *parent,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    if(index.column() == cycBfColumn || index.column() == cycAfColumn) {
        QSpinBox *editor = new QSpinBox(parent);
        editor->setFrame(false);
        editor->setMinimum(0);
        editor->setMaximum(5);
        editor->setSingleStep(1);
        return editor;
    }else{
        return QItemDelegate::createEditor(parent, option, index);
    }
}

/**********************************************************************************/
EventSelectDialog::EventSelectDialog(ConnectDialog *connectDialog, QWidget *parent)
    :  QDialog(parent)
{
    this->connectDialog = connectDialog;

//    qDebug()<<scopeImfor.eventList.count();

    m_tableWidget = new QTableWidget(scopeImfor.eventList.count(), 6);
    m_tableWidget->setColumnWidth(4,200);
    m_tableWidget->setColumnWidth(1, 100);
    m_tableWidget->setColumnWidth(2, 100);
    m_tableWidget->setColumnWidth(3, 100);
    m_tableWidget->setColumnWidth(5, 100);

    EventTableDelegate *delegate = new EventTableDelegate;
    m_tableWidget->setItemDelegateForColumn(1, delegate);
    m_tableWidget->setItemDelegateForColumn(2, delegate);
    m_tableWidget->setItemDelegateForColumn(3, delegate);
    m_tableWidget->setItemDelegateForColumn(4, delegate);

    m_tableWidget->setHorizontalHeaderLabels(
                QStringList()<<tr("名称")<<"等级"<<"前周期"<<"后周期"<<"通道选择"<<"确认");
    QFont font = m_tableWidget->horizontalHeader()->font();
    font.setBold(true);
    m_tableWidget->horizontalHeader()->setFont(font);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:lightblue;color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");

    for(int row(0); row < scopeImfor.eventList.count(); row++) {
        Event event = scopeImfor.eventList.at(row);
        QTableWidgetItem *item0 = new QTableWidgetItem(event.description);
        m_tableWidget->setItem(row, 0, item0);
        m_tableWidget->item(row, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(event.level));
        m_tableWidget->setItem(row, 1, item1);


        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(event.cycBefor));
        m_tableWidget->setItem(row, 2, item2);
        m_tableWidget->item(row, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(event.cycAfter));
        m_tableWidget->setItem(row, 3, item3);
        m_tableWidget->item(row, 3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);


        ChannelWidget *widget = new ChannelWidget(scopeImfor.channelList, event.selectedChns,parent);
        m_tableWidget->setCellWidget(row, 4, widget);

        QPushButton *btn = new QPushButton(tr("OK"), this);
        connect(btn, QPushButton::clicked, this, &EventSelectDialog::buttonClicked);
        m_buttonlist<<btn;
        m_tableWidget->setCellWidget(row, 5, btn);

    }

    m_tableWidget->resizeColumnToContents(0);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_tableWidget);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(layout);

    setWindowTitle(tr("事件设置表"));
    setWindowIcon(QIcon(":/images/event_setting.png"));
    setMinimumSize(880, 300);

    m_tableWidget->setColumnWidth(4,width() - 450 - m_tableWidget->columnWidth(0));
}

#include <QDebug>

void EventSelectDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_tableWidget->setColumnWidth(4,width() - 450 - m_tableWidget->columnWidth(0));
}

void EventSelectDialog::buttonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());

    for(int i(0); i < m_buttonlist.count(); i++) {
        if(m_buttonlist.at(i) == btn) {

            qDebug()<<"buttonClicked:"<<i;
            ChannelWidget *widget = qobject_cast<ChannelWidget *> (m_tableWidget->cellWidget(i, 4));
            qDebug()<<widget->comboText();
            QVector<bool> vec = widget->checkList();
            qDebug()<<vec;

            uint8_t data[10];
            uint32_t channel = 0;
            data[0] = i;
            data[1] = m_tableWidget->item(i, 2)->data(Qt::DisplayRole).toInt();
            data[2] = m_tableWidget->item(i, 3)->data(Qt::DisplayRole).toInt();

            if(data[1] + data[2] > scopeImfor.devicCfg.refreshCycle) {
                QMessageBox::warning(this, tr("事件设置"),
                                     tr("上传周期选择过多。\n"
                                        "请先减少一些选择！"),
                                     QMessageBox::Yes);

                return;
            }

            int n = 0;

            for(int loop(0); loop < vec.count(); loop++) {
                channel += (((uint32_t)vec.at(loop)) << loop);
                if(vec.at(loop) == true) {
                    n++;
                }
            }

            if((data[1] + data[2]) * n * scopeImfor.cyclePoints >
                    scopeImfor.devicCfg.eventDataBlkNum * scopeImfor.devicCfg.eventDataBlkSize) {
                QMessageBox::warning(this, tr("事件设置"),
                                     tr("上传通道选择过多。\n"
                                        "请先减少一些通道，或减少一些周期！"),
                                     QMessageBox::Yes);

                return;
            }

            data[3] = ((channel & 0xFF000000) >> 24);
            data[4] = ((channel & 0x00FF0000) >> 16);
            data[5] = ((channel & 0x0000FF00) >> 8);
            data[6] = ((channel & 0x000000FF));

            connectDialog->getThread().sendNew(6, data, 7);

        }
    }
}




















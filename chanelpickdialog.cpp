#include "chanelpickdialog.h"
#include "ui_chanelpickdialog.h"

#include "channelimfordelegate.h"
#include "checkboxdelegate.h"
#include "communicationpara.h"
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDebug>


ChanelPickDialog::ChanelPickDialog(CommunicationPara *communicationPara, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ChanelPickDialog)
{
    m_ui->setupUi(this);
    this->communicationPara = communicationPara;


    ChannelImforDelegate *delegate1 = new ChannelImforDelegate(communicationPara->units(), 1, 2);
    CheckBoxDelegate *delegate2 = new CheckBoxDelegate;

    connect(m_ui->okButton, &QPushButton::clicked, this, &ChanelPickDialog::ok);
    connect(m_ui->cancelButton, &QPushButton::clicked, this, &ChanelPickDialog::cancel);

    m_ui->tableWidget->setRowCount(communicationPara->count());
    m_ui->tableWidget->setColumnCount(4);
    m_ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->tableWidget->setItemDelegateForColumn(1, delegate1);
    m_ui->tableWidget->setItemDelegateForColumn(2, delegate1);
    m_ui->tableWidget->setItemDelegateForColumn(3, delegate2);
    m_ui->tableWidget->setHorizontalHeaderLabels(
                QStringList()<<"Channel Name"<<"Scale"<<"Position"<<"Select");


    for(int row = 0; row < communicationPara->count(); row++){
        QTableWidgetItem *item0 = new QTableWidgetItem(communicationPara->nameAt(row));
        m_ui->tableWidget->setItem(row, 0, item0);


        QTableWidgetItem *item1
             = new QTableWidgetItem(QString::number(1.0));
        m_ui->tableWidget->setItem(row, 1, item1);

        QTableWidgetItem *item2
             = new QTableWidgetItem(QString::number(0));
        m_ui->tableWidget->setItem(row, 2, item2);

        QTableWidgetItem *item3
             = new QTableWidgetItem(QString::number(0));
        m_ui->tableWidget->setItem(row, 3, item3);

    }

    m_ui->bandwidthLabel->setText(QString("%1Kb/s").arg(communicationPara->bandwidth / 1000));
    m_ui->samplerateLabel->setText(QString("%1/Cycle").arg(communicationPara->sampleRate));
    m_ui->uploadSpeedLabel->setText(QString("%1%").arg(100));

    maxChannel = communicationPara->bandwidth /
            communicationPara->sampleRate / (2 * 50 * 1.2);
    m_ui->maxChannelLabel->setText(QString("%1个").arg(maxChannel));



    connect(m_ui->tableWidget, &QTableWidget::itemChanged,
            this, &ChanelPickDialog::tableChanged);

    connect(m_ui->setButton, &QPushButton::clicked,
            this, &ChanelPickDialog::uploadCountChanged);


}

void ChanelPickDialog::tableChanged(QTableWidgetItem *item)
{
    qDebug()<<item->data(Qt::EditRole).toInt();
    if(item->column() == Select_Column){
        if(item->data(Qt::EditRole).toBool())
            selectedChannel++;
        else
            selectedChannel--;
        if(selectedChannel > maxChannel){
            QMessageBox::warning(this, tr("通道选择"),
                                 tr("通道选择过多。\n"
                                    "请先取消一些选择！"),
                                 QMessageBox::Yes);
            item->setData(Qt::EditRole, false);

        }
        m_ui->pickedChannelLabel->setText(QString("%1个").arg(selectedChannel));

    }
}

void ChanelPickDialog::tableOperated(int selectedId, int changeType, qreal data)
{
    QTableWidgetItem *item = nullptr;
    /* step1: 查找当前操作的通道在表格中的行 */
    int id = 0, row = 0;
    for(row = 0; row < m_ui->tableWidget->colorCount(); row++){
        item = m_ui->tableWidget->item(row, Select_Column);
        if(item->data(Qt::EditRole).toBool()){
            if(id++ == selectedId){
                break;
            }
        }
    }

    /* step2: 根据信号类型， 取出所在列，并修改内容*/
    switch(changeType){
        case Delete:{
            Q_UNUSED(data);

            item->setData(Qt::EditRole, false);
            break;
        }
        case Scale_Change:{
            qDebug()<<data;
            item = m_ui->tableWidget->item(row, Scale_Column);
            item->setData(Qt::EditRole, qreal(1)/data/(qreal)5);
            break;
        }

        case Position_Change:{
            item = m_ui->tableWidget->item(row, Position_Column);
            item->setData(Qt::EditRole, data * (qreal)5);
            break;
        }
        default:break;
    }
}


void ChanelPickDialog::uploadCountChanged(/*QString text*/)
{
    int n = 0, value;
    value = m_ui->lineEdit->text().toInt();
    for(; value > 1; value--){
        if(((int)communicationPara->sampleRate%value) == 0) {
            n = (int)communicationPara->sampleRate/value;
            m_ui->lineEdit->setText(QString("%1").arg(value));
            break;
        }
    }

    maxChannel = communicationPara->bandwidth / (1.2 * value * 100);
    m_ui->maxChannelLabel->setText(QString("%1个").arg(maxChannel));
    m_ui->uploadSpeedLabel->setText(QString("%1%").arg((qreal)value * 100/communicationPara->sampleRate));

    if(currentSettings.interval != n){
        currentSettings.interval = n;
        for(int row(0); row < m_ui->tableWidget->rowCount(); row++){
            QTableWidgetItem *item = m_ui->tableWidget->item(row, Select_Column);
            if(item->data(Qt::EditRole).toBool()){
                item->setData(Qt::EditRole, false);
            }
        }
    }

}



void ChanelPickDialog::ok()
{
    if(updateSettings()){
        accept();
    }else{
        QMessageBox::warning(this, tr("通道选择"),
                             tr("尚未选择任何通道。\n"
                                "请至少选择一个通道！"),
                             QMessageBox::Yes);
    }
}

void ChanelPickDialog::cancel()
{
    reject();
}

void ChanelPickDialog::tableScaleAllChanged(qreal multiple)
{
    for(int row(0); row < m_ui->tableWidget->rowCount(); row++){
        QTableWidgetItem *item = m_ui->tableWidget->item(row, Scale_Column);
        item->setData(Qt::EditRole, item->data(Qt::EditRole).toReal() / multiple);
    }
}

bool ChanelPickDialog::updateSettings()
{
    currentSettings.ids.clear();
    currentSettings.imfList.clear();

    ChannelImfor imf;
    for(int row(0); row < m_ui->tableWidget->rowCount(); row++){
        imf.name = m_ui->tableWidget->item(row, 0)
                ->data(Qt::DisplayRole).toString();
        imf.trans.first = qreal(1) / m_ui->tableWidget->item(row, 1)
                ->data(Qt::DisplayRole).toReal() / qreal(5);
        imf.trans.second = m_ui->tableWidget->item(row, 2)
                ->data(Qt::DisplayRole).toReal() / qreal(5);
        currentSettings.imfList<<imf;

        if(m_ui->tableWidget->item(row, 3)->data(Qt::EditRole).toBool()){
            currentSettings.ids<<row;
        }
    }
    currentSettings.cycPoints = communicationPara->sampleRate/currentSettings.interval;

    if(currentSettings.ids.count() == 0)
        return false;
    return true;

}

ChanelPickDialog::~ChanelPickDialog()
{
    delete m_ui;
}




















#include "connectdialog.h"
#include "ui_connectdialog.h"

#include <QtSerialPort/QSerialPortInfo>
//#include <QtSerialPort/QSerialPort>
#include <QIntValidator>
#include <QLineEdit>

#include <QDebug>

QT_USE_NAMESPACE

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

    intValidator = new QIntValidator(0, 4000000, this);

    connect(ui->baudRateBox,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ConnectDialog::checkCustomBaudRatePolicy);
    connect(ui->serialPortInfoListBox,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ConnectDialog::checkCustomDevicePathPolicy);
    connect(ui->serialConnectButton, &QPushButton::clicked,
            this, &ConnectDialog::apply);

    connect(&thread, &SciThread::linked, this, &ConnectDialog::processLinked);
    connect(&thread, &SciThread::error, this, &ConnectDialog::processError);
    connect(&thread, &SciThread::timeout, this, &ConnectDialog::processTimeout);
    connect(&thread, &SciThread::imforGet, this, &ConnectDialog::processImforGet);



    fillPortsParameters();
    fillPortsInfo();
}

ConnectDialog::~ConnectDialog()
{
    qDebug()<<"~ConnectDialog()";
//    if(thread.isRunning())
//        thread.stop();
//    thread.wait();

    delete ui;
}

void ConnectDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void ConnectDialog::checkCustomDevicePathPolicy(int idx)
{
    bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
    ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        ui->serialPortInfoListBox->clearEditText();
}

void ConnectDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos) {
        QStringList list;
        list << info.portName();
        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
    ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void ConnectDialog::updateSerialSettings()
{
    serialSettings.name = ui->serialPortInfoListBox->currentText();

    if (ui->baudRateBox->currentIndex() == 4) {
        serialSettings.baudRate = ui->baudRateBox->currentText().toInt();
    } else {
        serialSettings.baudRate = static_cast<QSerialPort::BaudRate>(
        ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    serialSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    serialSettings.parity = static_cast<QSerialPort::Parity>(
                ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    serialSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    serialSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());

}

void ConnectDialog::apply()
{
    if(ui->serialConnectButton->text() == QString("disconnect")){
        ui->label->clear();
        thread.stop();
        ui->serialConnectButton->setText(tr("connect"));
        setControlsEnable(true);
    }else{
        ui->serialConnectButton->setText(tr("Connecting..."));
        setControlsEnable(false);
        updateSerialSettings();
        thread.connectToTarget(serialSettings, 3000);
    }
}


void ConnectDialog::closeEvent(QCloseEvent *event)
{
    qDebug()<<"closeEvent";
    ui->label->clear();
    thread.stop();
    QWidget::closeEvent(event);
}

void ConnectDialog::setControlsEnable(bool enable)
{
    ui->serialPortInfoListBox->setEnabled(enable);
    ui->baudRateBox->setEnabled(enable);
    ui->dataBitsBox->setEnabled(enable);
    ui->parityBox->setEnabled(enable);
    ui->stopBitsBox->setEnabled(enable);
    ui->flowControlBox->setEnabled(enable);

    ui->serialConnectButton->setEnabled(enable);

}

void ConnectDialog::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(tr("Custom"));

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}


void ConnectDialog::processError(const QString &s)
{
    thread.stop();
    ui->serialConnectButton->setText(tr("connect"));
    setControlsEnable(true);
    showResponse(s);
}

void ConnectDialog::processLinked(const QString &s)
{
    ui->serialConnectButton->setText(tr("disconnect"));
    ui->serialConnectButton->setEnabled(true);
    showResponse(s);
    hide();
}

void ConnectDialog::processTimeout(const QString &s)
{
    thread.stop();
    ui->serialConnectButton->setText(tr("connect"));
    setControlsEnable(true);
    showResponse(s);
}

void ConnectDialog::processImforGet()
{
    emit imforGet();
}

void ConnectDialog::processChannelPicked()
{
    QVector<int> &vec = thread.selectVector();
    vec.clear();
//    vec << pickDialog->settings().interval;
    vec += pickDialog->settings().ids;
    vec << pickDialog->settings().interval;

    thread.goToSendSettingState();
}

void ConnectDialog::showResponse(const QString &s)
{
    ui->label->setText(tr("%1").arg(s));
}

















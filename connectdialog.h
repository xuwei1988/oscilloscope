#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>

#include "scithread.h"
#include "chanelpickdialog.h"

/*2019.07.03 */
#include "canthread.h"

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class ConnectDialog;
}

class QIntValidator;

QT_END_NAMESPACE

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:

    explicit ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();

    void setChannelPickDialog(ChanelPickDialog *dialog)
    {
        pickDialog = dialog;
    }

    SciThread &getThread()
    {
        return thread;
    }


private slots:
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void processError(const QString &s);
    void processLinked(const QString &s);
    void processTimeout(const QString &s);
    void processImforGet();

/*2019.07.03 */
private slots:
    void canApply();
    void canProcessError(int error);
    void canProcessMsg(int number);

public slots:
    void processChannelPicked();

signals:
    void imforGet();

private:
    void setControlsEnable(bool enable);
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSerialSettings();

    void showResponse(const QString &s);
//    void processError(const QString &s);
//    void processTimeout(const QString &s);

private:
    Ui::ConnectDialog *ui;
    QIntValidator *intValidator;
    SerialSettings serialSettings;
    SciThread thread;
    /*2019.07.03 */
    CanThread canThread;

    ChanelPickDialog *pickDialog = nullptr;
};

#endif // CONNECTDIALOG_H

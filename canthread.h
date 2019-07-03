#ifndef CANTHREAD_H
#define CANTHREAD_H

#include <QThread>
#include <QList>
#include "ControlCAN.h"

class CanThread : public QThread
{
    Q_OBJECT
    enum CanError {
        CAN_OK = 0,
        CAN_OPEN_ERROR,
        CAN_BAUD_ERROR,
        CAN_INIT_ERROR,
        CAN_START_ERROR,
        CAN_RUNTIME_ERROR
    };
public:
    CanThread(QObject *parent = nullptr);
    ~CanThread();
    void run();
    void close_Device();
    void init(DWORD type, DWORD index, DWORD number,uint32_t id, uint32_t baudrate);
    QList<VCI_CAN_OBJ> frameList;

signals:
    void signal_CanData(int number);
    void signal_CanError(int error);

private:
    DWORD EQU_Type = VCI_USBCAN_E_U;
    DWORD EQU_Index = 0;
    DWORD EQU_Number = 0;
    uint32_t EQU_id;
    uint32_t EQU_baud = 0x060007;
    VCI_INIT_CONFIG init_config;

    VCI_CAN_OBJ frames[50];
    VCI_ERR_INFO errinfo;
    bool quit = false;

};

#endif // CANTHREAD_H

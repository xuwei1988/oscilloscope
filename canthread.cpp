#include "canthread.h"
#include <QDebug>

CanThread::CanThread(QObject *parent)
    : QThread(parent)
{
    close_Device();
}

CanThread::~CanThread()
{
    qDebug()<<"~CanThread";
    close_Device();
}

void CanThread::init(DWORD type, DWORD index, DWORD number, uint32_t id, uint32_t baudrate)
{
    EQU_Type = type;
    EQU_Index = index;
    EQU_Number = number;

    EQU_id = id;
    EQU_baud = baudrate;

    frameList.clear();
    quit = false;

    if(!isRunning()) {
        start();
    }
}

void CanThread::close_Device()
{
    quit = true;
    VCI_CloseDevice(EQU_Type,EQU_Index);
}

void CanThread::run()
{
    bool isOK = true;
    ULONG buffLen = 0;
    ULONG readLen = 0;
    uint8_t i = 0;

    init_config.AccCode = 0x00000000;
    init_config.AccMask = 0xFFFFFFFF;

    if(VCI_OpenDevice(EQU_Type, EQU_Index, EQU_Number) != STATUS_OK) {
        isOK = false;
        emit signal_CanError(CAN_OPEN_ERROR);
    }

    if(VCI_SetReference(EQU_Type, EQU_Index, EQU_Number, 0, (void *)&EQU_baud) != STATUS_OK) {
        isOK = false;
        emit signal_CanError(CAN_BAUD_ERROR);
    }

    if(VCI_InitCAN(EQU_Type, EQU_Index, EQU_Number, &init_config) != STATUS_OK) {
        isOK = false;
        emit signal_CanError(CAN_INIT_ERROR);
    }

    if(VCI_StartCAN(EQU_Type, EQU_Index, EQU_Number) != STATUS_OK) {
        isOK = false;
        emit signal_CanError(CAN_START_ERROR);
    }

    if(isOK) {
        while(!quit) {
            msleep(50);
            buffLen = VCI_GetReceiveNum(EQU_Type, EQU_Index, EQU_Number);
            if(buffLen > 0) {
                readLen = VCI_Receive(EQU_Type,EQU_Index,EQU_Number,frames, 50, 20);
                if(readLen == 0xFFFFFFFF) {
                    VCI_ReadErrInfo(EQU_Type,EQU_Index,EQU_Number,&errinfo);
                    emit signal_CanError(CAN_RUNTIME_ERROR);
                }else {
                    for(i = 0; i < readLen; i++) {
                        frameList.append(frames[i]);
                    }
                    emit signal_CanData(readLen);
                }
            }
        }

    }
}

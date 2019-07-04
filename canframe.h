#ifndef CANFRAME_H
#define CANFRAME_H

#include "ControlCAN.h"
#include <QList>

#define CAN_FRAME_BUFF_MAXNUM           60
#define CAN_FRAME_DYNAMIC_MAXNUM        5



class CanFrame : public QObject
{
    Q_OBJECT
    enum CanShowMode{
        FRAME_STATIC_SHOW = 0,
        FRAME_DYNAMIC_SHOW,
        FRAME_DIFF_SHOW
    };
public:
    CanFrame();
    void setShowMode(CanShowMode mode)
    {
        showMode = mode;
    }

    CanShowMode getShowMode() const
    {
        return showMode;
    }

    void refreshFrame(QList<VCI_CAN_OBJ> &newFrames);
private:
    CanShowMode showMode = FRAME_DYNAMIC_SHOW;

    QList<VCI_CAN_OBJ> frames;              /* 无过滤接收到的所有数据帧 */
    QList<VCI_CAN_OBJ> dynamicFrames;       /* 动态显示的数据帧 */
    QList<VCI_CAN_OBJ> diffDynamicFrames;    /* 经过过滤的数据帧 */
};

#endif // CANFRAME_H

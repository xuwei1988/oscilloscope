#include "canframe.h"
#include <QDebug>

CanFrame::CanFrame()
{
    frames.clear();
    dynamicFrames.clear();
    diffDynamicFrames.clear();
}


void CanFrame::refreshFrame(QList<VCI_CAN_OBJ> &newFrames)
{
    int len = 0;
    frames.append(newFrames);
    len = frames.count();
    /* step1: 判断此次刷新是否需要去掉原有旧数据*/
    if(len > CAN_FRAME_BUFF_MAXNUM){
        /* 去掉超范围的数据 */
        frames = frames.mid(len - CAN_FRAME_BUFF_MAXNUM);
    }

    dynamicFrames = frames.mid(CAN_FRAME_BUFF_MAXNUM - CAN_FRAME_DYNAMIC_MAXNUM);

    /* Debug 测试计算正确性 */
    qDebug()<<"refreshFrame";
    QList<int> a;
    for(int i = 0; i < frames.count(); i++) {
        for(int j = 0; j < frames.at(i).DataLen; j++) {
           a<<frames.at(i).Data[j];
        }
        qDebug()<<a;
        a.clear();
    }
}

//qDebug()<<frames.at(i).Data[0]<<frames.at(i).Data[1]
//        <<frames.at(i).Data[2]<<frames.at(i).Data[3]
//        <<frames.at(i).Data[4]<<frames.at(i).Data[5]
//        <<frames.at(i).Data[6]<<frames.at(i).Data[7];

#include "scopeimfor.h"

ScopeImfor scopeImfor;

ScopeImfor::ScopeImfor()
{
}

void ScopeImfor::clear()
{
    channelList.clear();
    eventList.clear();
    totChannel = 0;              //芯片采样总通道数
    cyclePoints = 0;             //芯片单周期采样点数
    eventNumber = 0;
    bandWidth = 0;

    devicCfg.eventDataBlkNum = 0;
    devicCfg.eventDataBlkSize = 0;
    devicCfg.eventPcbBlkNum = 0;
    devicCfg.eventProFreq = 0;
    devicCfg.realDataBlkNum = 0;
    devicCfg.realDataBlkSize = 0;
    devicCfg.refreshCycle = 0;
}


#include "recorddata.h"

RecordData::RecordData()
{
    records.clear();
}

RecordData::RecordData(ScoEvtPCB &scoEvtPcb)
{
    records.clear();

    state = false;
    discription = scoEvtPcb.discription;
    selectedChn = scoEvtPcb.selectedChn;
    level = scoEvtPcb.level;
    perChnNum= scoEvtPcb.chnPoints;

    for(int i(0); i < 32; i++) {
        if((1<<i) & selectedChn){
            QVector<int16_t> vect;
            records.append(ChnRecord(i, vect));
        }
    }
}

RecRes RecordData::receiveChnData(ScoEvt &scoEvt)
{
    uint8_t id = scoEvt.chnID;

    for(int i = 0; i < records.count(); i++) {
        if(records[i].first < id){
            if(records[i].second.count() != perChnNum)
                return errRes;
        }

        if(records[i].first == id) {
            records[i].second.append(scoEvt.data);

            if(id == records.last().first &&
                    records[i].second.count() == perChnNum){
                state = true;
                return fullRes;
            }else {
                return partRes;
            }
        }
    }

    return errRes;
}




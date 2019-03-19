#ifndef COMMUNICATIONPARA_H
#define COMMUNICATIONPARA_H

#include <QStringList>


class CommunicationPara
{
public:
    int count() const {return channels.count(); }
    QString nameAt(int i) const {return channels.at(i).first; }
    QString unitAt(int i) const {return channels.at(i).second; }
    QStringList units() const {
        QStringList list;
        for(int i(0); i < count(); i++)
            list<<channels.at(i).second;
        return list;
    }



    QList<QPair<QString, QString>> channels;

    qreal bandwidth;
    qreal sampleRate;
};

#endif // COMMUNICATIONPARA_H

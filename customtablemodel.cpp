#include "customtablemodel.h"

#include "customtablemodel.h"
#include "recorddata.h"
#include <QtCore/QVector>
#include <QtCore/QRect>
#include <QtGui/QColor>

CustomTableModel::CustomTableModel(RecordData *recordData, QObject *parent)
    : QAbstractTableModel(parent)
{
    m_recordData = recordData;
    m_columnCount = recordData->channelNum() + 1;
    m_rowCount = recordData->pointsNum();
}

int CustomTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_rowCount;
}

int CustomTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_columnCount;
}


QVariant CustomTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if(section == 0){
            return QString("");
        }else{
            return QString("CH%1").arg(m_recordData->channelID(section - 1));
        }
    } else {
        return QString("%1").arg(section + 1);
    }
}

QVariant CustomTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if(index.column() == 0){
            return index.row();
        }else{
            return m_recordData->records[index.column() - 1].second.at(index.row());
        }
    } else if (role == Qt::EditRole) {
        if(index.column() == 0){
            return index.row();
        }else{
            return m_recordData->records[index.column() - 1].second.at(index.row());
        }
    } else if (role == Qt::BackgroundRole) {
        for (const QRect &rect : m_mapping) {
            if (rect.contains(index.column(), index.row()))
                return QColor(m_mapping.key(rect));
        }
        // cell not mapped return white color
        return QColor(Qt::white);
    }
    return QVariant();
}

Qt::ItemFlags CustomTableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

void CustomTableModel::addMapping(QString color, QRect area)
{
    m_mapping.insertMulti(color, area);
}

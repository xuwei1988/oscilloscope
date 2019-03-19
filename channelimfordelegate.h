#ifndef CHANNELIMFORDELEGATE_H
#define CHANNELIMFORDELEGATE_H

#include <QItemDelegate>

class ChannelImforDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ChannelImforDelegate(QStringList uints, int scaleColumn, int positionColumn, QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
private slots:
//    void commitAndCloseEditor();

private:
    int scaleColumn;
    int positionColumn;

    QStringList uints;
};

#endif // CHANNELIMFORDELEGATE_H

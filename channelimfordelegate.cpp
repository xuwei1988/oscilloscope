#include "channelimfordelegate.h"
#include <QtWidgets/QDoubleSpinBox>

ChannelImforDelegate::ChannelImforDelegate(QStringList uints, int scaleColumn, int positionColumn, QObject *parent)
    : QItemDelegate(parent)
{
    this->scaleColumn = scaleColumn;
    this->positionColumn = positionColumn;
    this->uints = uints;
}

void ChannelImforDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    if(index.column() == scaleColumn){
        double scale = index.model()->data(index, Qt::DisplayRole).toDouble();
        QString text = QString("%1 %2")
                        .arg(scale)
                        .arg(uints.at(index.row()));
        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
    }else if(index.column() == positionColumn){
        double position = index.model()->data(index, Qt::DisplayRole).toDouble();
        QString text = QString("%1 格")
                        .arg(position);
        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);
    }else{
        QItemDelegate::paint(painter, option, index);
    }
}

QWidget *ChannelImforDelegate::createEditor(QWidget *parent,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    if(index.column() == scaleColumn){
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setFrame(false);
        editor->setMinimum(0);
        editor->setMaximum(1000);
        editor->setSingleStep(5);
        return editor;
    }else if(index.column() == positionColumn){
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setFrame(false);
        editor->setMinimum(-5);
        editor->setMaximum(5);
        editor->setSingleStep(0.5);
        return editor;
    }else{
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void ChannelImforDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == scaleColumn || index.column() == positionColumn){

        double value = index.model()->data(index, Qt::EditRole).toDouble();

        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->setValue(value);
    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}

void ChannelImforDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == scaleColumn || index.column() == positionColumn){
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>(editor);
        double value = spinBox->value();

        model->setData(index, value, Qt::EditRole);
    }else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

//void ChannelImforDelegate::commitAndCloseEditor()
//{
//     QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox *>
//}
























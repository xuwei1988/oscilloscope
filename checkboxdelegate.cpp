#include "checkboxdelegate.h"
#include <QStyleOption>
#include <QMouseEvent>
#include <QPainter>
#include <QApplication>
#include <QDebug>
//#include <QStyleOptionViewItem>

static QRect CheckBoxRect(const QStyleOptionViewItem &viewItemStyleOptions)
{
    QStyleOptionButton checkBoxStyleOption;

    QRect checkBoxRect = QApplication::style()->subElementRect(
                QStyle::SE_CheckBoxIndicator, &checkBoxStyleOption);

    QPoint checkBoxPoint(viewItemStyleOptions.rect.x() + viewItemStyleOptions.rect.width() / 2 - checkBoxRect.width() / 2,
                         viewItemStyleOptions.rect.y() + viewItemStyleOptions.rect.height() / 2 - checkBoxRect.height() / 2);

    return QRect(checkBoxPoint, checkBoxRect.size());
}

CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

void CheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,const QModelIndex& index)const
{
    bool checked = index.model()->data(index, Qt::DisplayRole).toBool();

    QStyleOptionButton checkBoxStyleOption;
    checkBoxStyleOption.state |= QStyle::State_Enabled;
    checkBoxStyleOption.state |= checked? QStyle::State_On : QStyle::State_Off;
    checkBoxStyleOption.rect = CheckBoxRect(option);

    QApplication::style()->drawControl(QStyle::CE_CheckBox,&checkBoxStyleOption,painter);
}

bool CheckBoxDelegate::editorEvent(QEvent *event,
                                QAbstractItemModel *model,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) {
    if((event->type() == QEvent::MouseButtonRelease) ||
            (event->type() == QEvent::MouseButtonDblClick)){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() != Qt::LeftButton ||
                !CheckBoxRect(option).contains(mouseEvent->pos())){
            return true;
        }
        if(event->type() == QEvent::MouseButtonDblClick){
            return true;
        }
    }else if(event->type() == QEvent::KeyPress){
        if(static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
                static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select){
            return false;
        }
    }else{
        return false;
    }

    bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
    return model->setData(index, !checked, Qt::EditRole);

}



























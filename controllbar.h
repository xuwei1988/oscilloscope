#ifndef CONTROLLBAR_H
#define CONTROLLBAR_H

#include <QWidget>
#include "datasource.h"

QT_BEGIN_NAMESPACE
class QPushButton;
class QComboBox;
class QDoubleSpinBox;
class QCheckBox;
//class QGroupBox;
QT_END_NAMESPACE

class ControllBar : public QWidget
{
    Q_OBJECT
public:
    ControllBar(QWidget *parent = 0);
    ControllBar(chnImforList &imforList, QWidget *parent = 0);

signals:
    void channelTrans(int channel, QPair<qreal, qreal> trans);

private slots:
    void channelChanged(int channel);
    void channelTransChanged(double value);

private:
    chnImforList imfList;
    QComboBox *channelCombo;

    QDoubleSpinBox *aSpinBox;
    QDoubleSpinBox *bSpinBox;

};

#endif // CONTROLLBAR_H

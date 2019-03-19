#include "controllbar.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QCheckBox>
#include <QHBoxLayout>


#include <QDebug>

ControllBar::ControllBar(QWidget *parent)
    : QWidget(parent)
{
    channelCombo = new QComboBox;
    QLabel *aLabel = new QLabel(tr("%/div"));
    QLabel *bLabel = new QLabel(tr("Y Base Line"));

    aSpinBox = new QDoubleSpinBox;
    aSpinBox->setValue(1.0);
    aSpinBox->setRange(0.1, 10);
    aSpinBox->setSingleStep(0.1);

    bSpinBox = new QDoubleSpinBox;
    bSpinBox->setValue(0.0);
    bSpinBox->setSingleStep(0.1);
    bSpinBox->setRange(-1, 1);


    QHBoxLayout *grouplayout = new QHBoxLayout;
    grouplayout->addStretch();
    grouplayout->addWidget(channelCombo);
    grouplayout->addWidget(aSpinBox);
    grouplayout->addWidget(aLabel);

    grouplayout->addWidget(bSpinBox);
    grouplayout->addWidget(bLabel);

    setLayout(grouplayout);
}

ControllBar::ControllBar(chnImforList &imforList, QWidget *parent)
    : QWidget(parent)
{
    channelCombo = new QComboBox;
    channelCombo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel *aLabel = new QLabel(tr("%/div"));
    aLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel *bLabel = new QLabel(tr("Y Base Line"));
    bLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    aSpinBox = new QDoubleSpinBox;
    aSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    aSpinBox->setValue(1.0);
//    aSpinBox->setRange(0.1, 10);
    aSpinBox->setSingleStep(0.1);

    bSpinBox = new QDoubleSpinBox;
    bSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    bSpinBox->setValue(0.0);
    bSpinBox->setSingleStep(0.1);
//    bSpinBox->setRange(-1, 1);

    QHBoxLayout *chnlayout = new QHBoxLayout;
    chnlayout->addWidget(channelCombo);
    chnlayout->addWidget(aSpinBox);
    chnlayout->addWidget(aLabel);
    chnlayout->addWidget(bSpinBox);
    chnlayout->addWidget(bLabel);
    chnlayout->addStretch(1);
    chnlayout->setSpacing(10);


    const auto imfors = imforList;
    for(ChannelImfor imfor : imfors) {
        channelCombo->addItem(imfor.name);
    }
    imfList = imforList;

    aSpinBox->setValue(imforList.first().trans.first);
    bSpinBox->setValue(imforList.first().trans.second);

    connect(channelCombo,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ControllBar::channelChanged);

    connect(aSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &ControllBar::channelTransChanged);

    connect(bSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &ControllBar::channelTransChanged);

    setLayout(chnlayout);
}


void ControllBar::channelChanged(int channel)
{
    qDebug()<<"channelChanged: "<<channel;

    disconnect(aSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &ControllBar::channelTransChanged);

    disconnect(bSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &ControllBar::channelTransChanged);

    aSpinBox->setValue(imfList.at(channel).trans.first);
    bSpinBox->setValue(imfList.at(channel).trans.second);

    connect(aSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &ControllBar::channelTransChanged);

    connect(bSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &ControllBar::channelTransChanged);
}

void ControllBar::channelTransChanged(double value)
{
    qDebug()<<"channelTransChanged: "<<value;
    imfList[channelCombo->currentIndex()].trans = QPair<qreal, qreal>(aSpinBox->value(), bSpinBox->value());

    emit channelTrans(channelCombo->currentIndex(), QPair<qreal, qreal>(aSpinBox->value(), bSpinBox->value()));
}




















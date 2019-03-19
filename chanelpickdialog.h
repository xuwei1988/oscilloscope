#ifndef CHANELPICKDIALOG_H
#define CHANELPICKDIALOG_H

#include <QDialog>
#include "datasource.h"

class CommunicationPara;



namespace Ui {
class ChanelPickDialog;
}

QT_BEGIN_NAMESPACE
class QTableWidgetItem;
QT_END_NAMESPACE

class ChanelPickDialog : public QDialog
{
    Q_OBJECT
    enum {Scale_Column = 1,
          Position_Column = 2,
          Select_Column = 3};

    enum {
        Delete = 0,
        Scale_Change = 1,
        Position_Change = 2
    };
public:
    struct Settings{
        chnImforList imfList;
        QVector<int> ids;
        int interval = 1;
        int cycPoints = 0;
    };

    explicit ChanelPickDialog(CommunicationPara *communicationPara, QWidget *parent = 0);
    ~ChanelPickDialog();

    Settings settings() const {return currentSettings; }

public slots:
    void tableScaleAllChanged(qreal multiple);
    void tableOperated(int selectedId, int changeType, qreal data);

private slots:
    void tableChanged(QTableWidgetItem *item);
    void uploadCountChanged();

    void ok();
    void cancel();



private:
    bool updateSettings();

    Ui::ChanelPickDialog *m_ui = nullptr;

    CommunicationPara *communicationPara;
    int maxChannel = 0;
    int selectedChannel = 0;

    Settings currentSettings;
};

#endif // CHANELPICKDIALOG_H

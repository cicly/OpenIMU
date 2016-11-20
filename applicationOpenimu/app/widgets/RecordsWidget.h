#ifndef RECORDSWIDGET_H
#define RECORDSWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <string>
#include <QLabel>

#include "acquisition/WimuAcquisition.h"
#include "acquisition/WimuRecord.h"
#include "AccDataDisplay.h"
#include "dialogs/FullGraphDialog.h"

class RecordsWidget: public QWidget
{
    Q_OBJECT

    public:
    RecordsWidget();
    RecordsWidget(QWidget *parent,const WimuAcquisition& data, RecordInfo record);

    ~RecordsWidget();

    public slots:
    void openFullGraphSlot();
    void renameRecord();

    private:
    QGridLayout* layout;
    WimuAcquisition acceleroData;
    RecordInfo record;
    QLineEdit* recordNameEdit;
    QLabel* recordTitle;
    QLabel* recordDate;
    QLabel* imuType;
    QLabel* positionImu;
    QLabel* detailsRecord;
    QPushButton* deleteBtn;
    QPushButton* seeFullGraphBtn;
    QPushButton* goToNextStep;
    FullGraphDialog *fDialog;
    bool renameRecordClicked;
    QWidget* m_parent;
    QPushButton * editRecord;
};

#endif

#include <QtGui>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#define SETTING_USE_ACCELEROMETER "UseAccelerometer"
#define SETTING_ROWS "Rows"
#define SETTING_COLS "Columns"

SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    setFixedHeight(180);
    setFixedWidth(parentWidget()->width());

    // This ensures the correct look on Maemo 5 too
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    ui->btnClose->setDefault(true);
    buttonBox->addButton(ui->btnClose, QDialogButtonBox::ActionRole);
    layout()->addWidget(buttonBox);
#else
    setFixedHeight(50);
    setFixedWidth(450);
    ui->chkAccelerometer->hide();
#endif
    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(this, SIGNAL(rejected()), this, SLOT(saveSettings()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveSettings()
{
    // Saving settings
    QSettings s;
    int i;
    bool ok;
    if (!((i = ui->txtColumns->text().toInt(&ok)) > 1 && ok))
    {
        QMessageBox::information(this, "Invalid value", "The column value is invalid", QMessageBox::Ok);
        ui->txtColumns->setText("4");
        i = 4;
    }
    s.setValue(SETTING_COLS, i);
    if (!((i = ui->txtRows->text().toInt(&ok)) > 1 && ok))
    {
        QMessageBox::information(this, "Invalid value", "The row value is invalid", QMessageBox::Ok);
        ui->txtRows->setText("3");
        i = 3;
    }
    s.setValue(SETTING_ROWS, i);
    s.setValue(SETTING_USE_ACCELEROMETER, ui->chkAccelerometer->isChecked());
}

void SettingsDialog::on_btnClose_clicked()
{
    saveSettings();
    accept();
}

void SettingsDialog::showEvent(QShowEvent *e)
{
    ui->chkAccelerometer->setChecked(useAccelerometer());
    ui->txtRows->setText(QString::number(rows()));
    ui->txtColumns->setText(QString::number(columns()));

    QDialog::showEvent(e);
}

bool SettingsDialog::useAccelerometer()
{
    QSettings s;
    return s.value(SETTING_USE_ACCELEROMETER,
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
                   true
#else
                   false
#endif
                   ).toBool();
}

int SettingsDialog::rows()
{
    QSettings s;
    return s.value(SETTING_ROWS, 4).toInt();
}


int SettingsDialog::columns()
{
    QSettings s;
    return s.value(SETTING_COLS, 3).toInt();
}


#include <QtGui>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define SETTING_USE_ACCELEROMETER "UseAccelerometer"
#define SETTING_USE_DROPSHADOW "UseDropshadow"
#define SETTING_BOARDBACKGROUND "BoardBackground"
#define SETTING_TOLERANCE "Tolerance"
#define SETTING_ROWS "Rows"
#define SETTING_COLS "Columns"

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60) || defined(Q_WS_WINCE)
#define DEFAULT_TOLERANCE 10
#else
#define DEFAULT_TOLERANCE 5
#endif

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    _boardBackground(boardBackground())
{
    ui->setupUi(this);
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    setFixedWidth(parentWidget()->width());

    // This ensures the correct look on Maemo 5 too
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    ui->btnClose->setDefault(true);
    buttonBox->addButton(ui->btnClose, QDialogButtonBox::ActionRole);
    layout()->addWidget(buttonBox);
#else
    setFixedHeight(height() - ui->chkAccelerometer->height());
    setFixedWidth(450);
    ui->chkAccelerometer->deleteLater();
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
    s.setValue(SETTING_COLS, ui->cbColumns->currentIndex() + 2);
    s.setValue(SETTING_ROWS, ui->cbRows->currentIndex() + 2);
    s.setValue(SETTING_USE_DROPSHADOW, ui->chkUseDropshadow->isChecked());
    s.setValue(SETTING_BOARDBACKGROUND, _boardBackground);
    s.setValue(SETTING_TOLERANCE, (ui->cbTolerance->currentIndex() + 1) * 5);
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    s.setValue(SETTING_USE_ACCELEROMETER, ui->chkAccelerometer->isChecked());
#endif
}

void SettingsDialog::on_btnClose_clicked()
{
    saveSettings();
    accept();
}

void SettingsDialog::showEvent(QShowEvent *e)
{
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    ui->chkAccelerometer->setChecked(useAccelerometer());
#endif
    ui->chkUseDropshadow->setChecked(useDropShadow());
    int r = CLAMP(rows(), 2, 15), c = CLAMP(columns(), 2, 15);
    ui->cbRows->setCurrentIndex(r - 2);
    ui->cbColumns->setCurrentIndex(c - 2);

    QPixmap pm(20, 20);
    pm.fill(_boardBackground);
    ui->btnBoardColor->setIcon(QIcon(pm));
    int t = CLAMP(tolerance(), 5, 15);
    ui->cbTolerance->setCurrentIndex(t / 5 - 1);

    QDialog::showEvent(e);
}

void SettingsDialog::on_btnBoardColor_clicked()
{
    QColor newColor = QColorDialog::getColor(_boardBackground, this);
    if (newColor != _boardBackground)
    {
        _boardBackground = newColor;
        QPixmap pm(20, 20);
        pm.fill(_boardBackground);
        ui->btnBoardColor->setIcon(QIcon(pm));
    }
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

bool SettingsDialog::useDropShadow()
{
    QSettings s;
    return s.value(SETTING_USE_DROPSHADOW, true).toBool();
}

QColor SettingsDialog::boardBackground()
{
    QSettings s;
    return s.value(SETTING_BOARDBACKGROUND, QColor(Qt::white)).value<QColor>();
}

int SettingsDialog::tolerance()
{
    QSettings s;
    return s.value(SETTING_TOLERANCE, DEFAULT_TOLERANCE).toInt();
}

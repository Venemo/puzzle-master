#include <QtGui>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "util.h"

#if defined(Q_WS_MAEMO_5)
#include <hildon-extras-1/hildon-extras/qt-he-wrapper.h>
#endif

#define SETTING_USE_ACCELEROMETER "UseAccelerometer"
#define SETTING_USE_DROPSHADOW "UseDropshadow"
#define SETTING_ENABLE_SCALING "EnableScaling"
#define SETTING_BOARDBACKGROUND "BoardBackground"
#define SETTING_TOLERANCE "Tolerance"
#define SETTING_ROWS "Rows"
#define SETTING_COLS "Columns"

#if defined(MOBILE)
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
#if defined(Q_WS_MAEMO_5)
    // This ensures the correct look on Maemo 5 too
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    ui->btnClose->setDefault(true);
    buttonBox->addButton(ui->btnClose, QDialogButtonBox::ActionRole);
    layout()->addWidget(buttonBox);
#else
    setFixedWidth(450);
#endif

#if defined(HAVE_QACCELEROMETER)
#else
    // Hiding the relevant UI if we don't have an accelerometer
    ui->chkAccelerometer->hide();
#endif

#if defined(MOBILE)
    ui->chkEnableScaling->hide();
#endif

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
#else
    // Hiding the relevant UI if we don't have Qt 4.6 at least
    ui->chkUseDropshadow->hide();
#endif

    layout()->setSizeConstraint(QLayout::SetFixedSize);

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
    setColumns(ui->cbColumns->currentIndex() + 2);
    setRows(ui->cbRows->currentIndex() + 2);
    setBoardBackground(_boardBackground);
    setTolerance((ui->cbTolerance->currentIndex() + 1) * 5);
    setUseDropShadow(ui->chkUseDropshadow->isChecked());
    setUseAccelerometer(ui->chkAccelerometer->isChecked());
    setEnableScaling(ui->chkEnableScaling->isChecked());
}

void SettingsDialog::on_btnClose_clicked()
{
    saveSettings();
    accept();
}

void SettingsDialog::showEvent(QShowEvent *e)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    ui->chkUseDropshadow->setChecked(useDropShadow());
#endif
#if defined(HAVE_QACCELEROMETER)
    ui->chkAccelerometer->setChecked(useAccelerometer());
#endif
    ui->chkEnableScaling->setChecked(enableScaling());
    int r = CLAMP(rows(), 2, 15), c = CLAMP(columns(), 2, 15);
    ui->cbRows->setCurrentIndex(r - 2);
    ui->cbColumns->setCurrentIndex(c - 2);

    QPixmap pm(80, 30);
    pm.fill(_boardBackground);
    ui->btnBoardColor->setIcon(QIcon(pm));
    int t = CLAMP(tolerance(), 5, 15);
    ui->cbTolerance->setCurrentIndex(t / 5 - 1);

    QDialog::showEvent(e);
}

void SettingsDialog::on_btnBoardColor_clicked()
{
#if defined(Q_WS_MAEMO_5)
    QColor newColor = QtHeWrapper::showHeSimpleColorDialog(this, _boardBackground, true);
#else
    QColor newColor = QColorDialog::getColor(_boardBackground, this);
#endif
    if (newColor.isValid() && newColor != _boardBackground)
    {
        _boardBackground = newColor;
        QPixmap pm(80, 30);
        pm.fill(_boardBackground);
        ui->btnBoardColor->setIcon(QIcon(pm));
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
bool SettingsDialog::useDropShadow()
{
    return settings.value(SETTING_USE_DROPSHADOW, true).toBool();
}

void SettingsDialog::setUseDropShadow(bool value)
{
    settings.setValue(SETTING_USE_DROPSHADOW, value);
}
#endif

bool SettingsDialog::useAccelerometer()
{
    return settings.value(SETTING_USE_ACCELEROMETER, false).toBool();
}

void SettingsDialog::setUseAccelerometer(bool value)
{
    settings.setValue(SETTING_USE_ACCELEROMETER, value);
}

int SettingsDialog::rows()
{
    return settings.value(SETTING_ROWS, 4).toInt();
}

void SettingsDialog::setRows(int value)
{
    settings.setValue(SETTING_ROWS, value);
}

int SettingsDialog::columns()
{
    return settings.value(SETTING_COLS, 3).toInt();
}

void SettingsDialog::setColumns(int value)
{
    settings.setValue(SETTING_COLS, value);
}

QColor SettingsDialog::boardBackground()
{
    return settings.value(SETTING_BOARDBACKGROUND, QColor(Qt::white)).value<QColor>();
}

void SettingsDialog::setBoardBackground(QColor value)
{
    settings.setValue(SETTING_BOARDBACKGROUND, value);
}

int SettingsDialog::tolerance()
{
    return settings.value(SETTING_TOLERANCE, DEFAULT_TOLERANCE).toInt();
}

void SettingsDialog::setTolerance(int value)
{
    settings.setValue(SETTING_TOLERANCE, value);
}

bool SettingsDialog::enableScaling()
{
    return settings.value(SETTING_ENABLE_SCALING, true).toBool();
}

void SettingsDialog::setEnableScaling(bool value)
{
    settings.setValue(SETTING_ENABLE_SCALING, value);
}

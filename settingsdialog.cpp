#include <QtGui>
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "util.h"

#if defined(Q_WS_MAEMO_5)
#include <hildon-extras-1/hildon-extras/qt-he-wrapper.h>
#endif

#define SETTING_USE_ACCELEROMETER "UseAccelerometer"
#define SETTING_USE_OPENGL "UseOpenGL"
#define SETTING_USE_DROPSHADOW "UseDropshadow"
#define SETTING_ENABLE_SCALING "EnableScaling"
#define SETTING_BOARDBACKGROUND "BoardBackground"
#define SETTING_TOLERANCE "Tolerance"
#define SETTING_LAST_IMAGE "LastImage"
#define SETTING_ROWS "Rows"
#define SETTING_COLS "Columns"
#define SETTING_START_IN_FULLSCREEN "StartInFullscreen"

#if defined(MOBILE)
#define DEFAULT_TOLERANCE 10
#else
#define DEFAULT_TOLERANCE 5
#endif

extern QSettings *settings;

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    _boardBackground(boardBackground())
{
    ui->setupUi(this);

#if defined(MOBILE)
    ui->enableScalingBox->hide();
#endif

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
#else
    // Hiding the relevant UI if we don't have Qt 4.6 at least
    ui->useDropshadowBox->hide();
#endif

#if defined(HAVE_QACCELEROMETER)
#else
    // Hiding the relevant UI if we don't have an accelerometer
    ui->accelerometerBox->hide();
#endif

#if defined(HAVE_OPENGL)
#else
    // Hiding the relevant UI if we don't have an accelerometer
    ui->useOpenGlBox->hide();
#endif

#if defined(Q_OS_SYMBIAN)
    QAction *closeAction = new QAction("Close", this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    addAction(closeAction);
    closeButton->hide();
#endif

#if defined(Q_WS_MAEMO_5)
    closeButton->hide();
#endif

    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveSettings()
{
    // Saving settings
    setBoardBackground(_boardBackground);
    setTolerance((ui->toleranceBox->currentIndex() + 1) * 5);
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    setUseDropShadow(ui->useDropshadowBox->isChecked());
#endif
    setEnableScaling(ui->enableScalingBox->isChecked());
    setUseAccelerometer(ui->accelerometerBox->isChecked());
    setUseOpenGl(ui->useOpenGlBox->isChecked());
}

void SettingsDialog::showEvent(QShowEvent *e)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    ui->useDropshadowBox->setChecked(useDropShadow());
#endif

    ui->enableScalingBox->setChecked(enableScaling());
    ui->useOpenGlBox->setChecked(useOpenGl());


    QPixmap pm(80, 30);
    pm.fill(_boardBackground);
    ui->boardColorButton->setIcon(QIcon(pm));
    int t = CLAMP(tolerance(), 5, 15);
    ui->toleranceBox->setCurrentIndex(t / 5 - 1);

    QDialog::showEvent(e);
}

void SettingsDialog::hideEvent(QHideEvent *e)
{
    saveSettings();
    QDialog::hideEvent(e);
}

void SettingsDialog::on_boardColorButton_clicked()
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
        ui->boardColorButton->setIcon(QIcon(pm));
    }
}

bool SettingsDialog::useOpenGl()
{
    return settings->value(SETTING_USE_OPENGL, true).toBool();
}

void SettingsDialog::setUseOpenGl(bool value)
{
    settings->setValue(SETTING_USE_OPENGL, value);
}

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
bool SettingsDialog::useDropShadow()
{
    return settings->value(SETTING_USE_DROPSHADOW,
                       #if defined(Q_OS_SYMBIAN)
                           QSysInfo::symbianVersion() >= QSysInfo::SV_SF_3
                       #else
                           true
                       #endif
                           ).toBool();
}

void SettingsDialog::setUseDropShadow(bool value)
{
    settings->setValue(SETTING_USE_DROPSHADOW, value);
}
#endif

bool SettingsDialog::useAccelerometer()
{
    return settings->value(SETTING_USE_ACCELEROMETER, false).toBool();
}

void SettingsDialog::setUseAccelerometer(bool value)
{
    settings->setValue(SETTING_USE_ACCELEROMETER, value);
}

int SettingsDialog::rows()
{
    return settings->value(SETTING_ROWS, 4).toInt();
}

void SettingsDialog::setRows(int value)
{
    settings->setValue(SETTING_ROWS, value);
}

int SettingsDialog::columns()
{
    return settings->value(SETTING_COLS, 3).toInt();
}

void SettingsDialog::setColumns(int value)
{
    settings->setValue(SETTING_COLS, value);
}

QColor SettingsDialog::boardBackground()
{
    return settings->value(SETTING_BOARDBACKGROUND, QColor(Qt::white)).value<QColor>();
}

void SettingsDialog::setBoardBackground(QColor value)
{
    settings->setValue(SETTING_BOARDBACKGROUND, value);
}

int SettingsDialog::tolerance()
{
    return settings->value(SETTING_TOLERANCE, DEFAULT_TOLERANCE).toInt();
}

void SettingsDialog::setTolerance(int value)
{
    settings->setValue(SETTING_TOLERANCE, value);
}

bool SettingsDialog::enableScaling()
{
    return settings->value(SETTING_ENABLE_SCALING, true).toBool();
}

void SettingsDialog::setEnableScaling(bool value)
{
    settings->setValue(SETTING_ENABLE_SCALING, value);
}

QString SettingsDialog::lastImage()
{
    return settings->value(SETTING_LAST_IMAGE, QString(":/image1.jpg")).toString();
}

void SettingsDialog::setLastImage(const QString &value)
{
    settings->setValue(SETTING_LAST_IMAGE, value);
}

bool SettingsDialog::startInFullscreen()
{
    return settings->value(SETTING_START_IN_FULLSCREEN,
                       #if defined(MOBILE) && !defined(Q_OS_SYMBIAN)
                           true
                       #else
                           false
                       #endif
                           ).toBool();
}

void SettingsDialog::setStartInFullscreen(bool value)
{
    settings->setValue(SETTING_START_IN_FULLSCREEN, value);
}

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
    // Deleting the relevant UI if we don't have an accelerometer
    ui->chkAccelerometer->deleteLater();
#endif

#if defined(MOBILE)
    ui->chkEnableScaling->hide();
#endif

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
#else
    // Deleting the relevant UI if we don't have Qt 4.6 at least
    setFixedHeight(height() - ui->chkUseDropshadow->height());
    ui->chkUseDropshadow->deleteLater();
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
    QSettings s;
    s.setValue(SETTING_COLS, ui->cbColumns->currentIndex() + 2);
    s.setValue(SETTING_ROWS, ui->cbRows->currentIndex() + 2);
    s.setValue(SETTING_BOARDBACKGROUND, _boardBackground);
    s.setValue(SETTING_TOLERANCE, (ui->cbTolerance->currentIndex() + 1) * 5);
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    s.setValue(SETTING_USE_DROPSHADOW, ui->chkUseDropshadow->isChecked());
#endif
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    s.setValue(SETTING_USE_ACCELEROMETER, ui->chkAccelerometer->isChecked());
#endif
    s.setValue(SETTING_ENABLE_SCALING, ui->chkEnableScaling->isChecked());
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
    QSettings s;
    return s.value(SETTING_USE_DROPSHADOW, true).toBool();
}
#endif

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

bool SettingsDialog::enableScaling()
{
    QSettings s;
#if defined (MOBILE)
    return s.value(SETTING_ENABLE_SCALING, true).toBool();
#else
    return s.value(SETTING_ENABLE_SCALING, false).toBool();
#endif
}
